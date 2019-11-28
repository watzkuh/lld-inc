
#include "ReWriter.h"
#include "Chunks.h"
#include "Driver.h"
#include "IncrementalLinkFile.h"
#include "InputFiles.h"
#include "SymbolTable.h"
#include "Symbols.h"
#include "Writer.h"
#include <lld/Common/ErrorHandler.h>
#include <lld/Common/Threads.h>
#include <lld/Common/Timer.h>
#include <llvm/Support/Error.h>
#include <llvm/Support/FileOutputBuffer.h>
#include <llvm/Support/xxhash.h>

using namespace llvm;
using namespace llvm::support;
using namespace lld;
using namespace lld::coff;

static Timer patchTimer("Binary Patching", Timer::root());
std::unique_ptr<FileOutputBuffer> binary;

std::vector<ObjFile *> changedFiles;
std::set<StringRef> dependentFileNames;

void abortIncrementalLink() {
  outs() << "Incremental link aborted, starting clean link...\n";
  binary->discard();
  symtab->clear();
  incrementalLinkFile->rewriteAborted = true;
  driver->clearVisitedFiles();
  std::vector<char *> args;
  for (auto &a : incrementalLinkFile->arguments)
    args.push_back(&a[0]);
  driver->link(ArrayRef<char *>(args));
}

void assignAddresses(ObjFile *file) {
  std::map<StringRef, uint32_t> rvas;
  for (auto &s : incrementalLinkFile->objFiles[file->getName()].sections) {
    rvas[s.first] = s.second.virtualAddress;
  }
  for (Chunk *c : file->getChunks()) {
    auto *sc = dyn_cast<SectionChunk>(c);
    sc->setRVA(rvas[sc->getSectionName()]);
    rvas[sc->getSectionName()] +=
        alignTo(sc->getSize(), incrementalLinkFile->paddedAlignment);
  }
}

void markDependentFiles(ObjFile *file) {
  auto &f = incrementalLinkFile->objFiles[file->getName()];
  for (auto &dep : f.dependentFiles)
    dependentFileNames.insert(dep);
}

void applyRelocation(SectionChunk sc, uint8_t *off, support::ulittle16_t type,
                     uint64_t s, uint64_t p) {
  OutputSection *os = nullptr; // not that interesting at the moment TODO:
  switch (config->machine) {
  case AMD64:
    sc.applyRelX64(off, type, os, s, p);
    break;
  case I386:
    sc.applyRelX86(off, type, os, s, p);
    break;
  case ARMNT:
    sc.applyRelARM(off, type, os, s, p);
    break;
  case ARM64:
    sc.applyRelARM64(off, type, os, s, p);
    break;
  default:
    llvm_unreachable("unknown machine type");
  }
}

void reapplyRelocations(const StringRef &fileName) {
  outs() << "Applying relocations for file " << fileName << "\n";
  auto &secInfo = incrementalLinkFile->objFiles[fileName].sections[".text"];
  auto &outputTextSection = incrementalLinkFile->outputSections[".text"];
  auto offset = outputTextSection.rawAddress + secInfo.virtualAddress -
                outputTextSection.virtualAddress;
  uint8_t *buf = binary->getBufferStart() + offset;
  for (const auto &c : secInfo.chunks) {
    uint32_t chunkStart = c.virtualAddress;
    for (const auto &sym : c.symbols) {
      uint64_t s =
          incrementalLinkFile->definedSymbols[sym.first].definitionAddress;
      for (const auto &rel : sym.second.relocations) {
        uint8_t *off = buf + rel.virtualAddress;
        memset(off, 0x0,
               4); // Delete the old address TODO: 16/64 bit relocation
        uint64_t p = chunkStart + rel.virtualAddress;
        SectionChunk sc;
        applyRelocation(sc, off, rel.type, s, p);
      }
    }
    buf += c.size;
  }
}

void rewriteTextSection(SectionChunk *sc, uint8_t *buf, uint32_t chunkStart) {

  memcpy(buf, sc->getContents().data(), sc->getSize());

  for (size_t j = 0, e = sc->getRelocs().size(); j < e; j++) {
    const coff_relocation &rel = sc->getRelocs()[j];
    auto *sym = sc->file->getSymbol(rel.SymbolTableIndex);
    auto *definedSym = dyn_cast_or_null<Defined>(sym);
    uint64_t s = 0;
    if (definedSym != nullptr) {
      // The target of the relocation
      s = definedSym->getRVA();
    }
    // Fallback to symbol table if we either have no information
    // about the chunk or the symbol
    if (definedSym == nullptr || s == 0) {
      s = incrementalLinkFile->definedSymbols[sym->getName()].definitionAddress;
    }
    uint8_t *off = buf + rel.VirtualAddress;

    // Compute the RVA of the relocation for relative relocations.
    uint64_t p = chunkStart + rel.VirtualAddress;

    applyRelocation(*sc, off, rel.Type, s, p);
  }
}

void rewriteSection(const std::vector<SectionChunk *> &chunks,
                    StringRef fileName, StringRef secName) {

  // All currently supported sections
  if (secName != ".text" && secName != ".data" && secName != ".rdata")
    return;

  outs() << "Rewriting " << secName << " section for file " << fileName << "\n";

  auto &secInfo = incrementalLinkFile->objFiles[fileName].sections[secName];
  auto &outputSectionInfo = incrementalLinkFile->outputSections[secName];
  auto offset =
      outputSectionInfo.rawAddress +
      (secInfo.virtualAddress < outputSectionInfo.virtualAddress
           ? 0
           : secInfo.virtualAddress - outputSectionInfo.virtualAddress);

  uint8_t *buf = binary->getBufferStart() + offset;
  uint32_t contribSize = 0;

  for (SectionChunk *sc : chunks) {
    const size_t paddedSize =
        alignTo(sc->getSize(), incrementalLinkFile->paddedAlignment);
    if (secName == ".text") {
      memset(buf, 0xCC, paddedSize);
      rewriteTextSection(sc, buf, (secInfo.virtualAddress + contribSize));
    } else if (secName == ".data" || secName == ".rdata") {
      memset(buf, 0x0, paddedSize);
      sc->writeTo(buf);
    }

    contribSize += paddedSize;
    buf += paddedSize;
  }

  if (secInfo.size != contribSize) {
    outs() << "New " << secName << " section is not the same size \n";
    outs() << "New: " << contribSize << "\tOld: " << secInfo.size << "\n";
    incrementalLinkFile->rewriteAborted = true;
    return;
  }
}

void updateSymbolTable(ObjFile *file) {
  for (const auto &sym : file->getSymbols()) {
    auto *definedSym = dyn_cast_or_null<Defined>(sym);
    if (definedSym && definedSym->getRVA() != 0) {
      incrementalLinkFile->definedSymbols[sym->getName()].definitionAddress =
          definedSym->getRVA();
    }
  }
}

void rewriteFile(ObjFile *file) {
  std::map<StringRef, std::vector<SectionChunk *>> chunks;
  for (Chunk *c : file->getChunks()) {
    auto *sc = dyn_cast<SectionChunk>(c);
    chunks[sc->getSectionName()].push_back(sc);
  }
  for (auto &sec : chunks) {
    rewriteSection(sec.second, file->getName(), sec.first);
  }
  updateSymbolTable(file);
}

void coff::markForReWrite(coff::ObjFile *file) {
  markDependentFiles(file);
  assignAddresses(file);
  changedFiles.push_back(file);
}

void coff::doNothing() {}

void coff::rewriteResult() {
  ScopedTimer t(patchTimer);
  binary = CHECK(FileOutputBuffer::create(incrementalLinkFile->outputFile, -1,
                                          llvm::FileOutputBuffer::F_modify),
                 "failed to open " + incrementalLinkFile->outputFile);
  while (!rewriteQueue.empty() && !incrementalLinkFile->rewriteAborted) {
    rewriteQueue.front()();
    rewriteQueue.pop_front();
  }

  for (auto &f : changedFiles) {
    rewriteFile(f);
    if (incrementalLinkFile->rewriteAborted) {
      t.stop();
      abortIncrementalLink();
      return;
    }
  }

  for (auto &f : dependentFileNames)
    reapplyRelocations(f);

  StringRef binaryFileData(
      reinterpret_cast<const char *>(binary->getBufferStart()),
      binary->getBufferSize());
  incrementalLinkFile->outputHash = xxHash64(binaryFileData);

  if (auto e = binary->commit())
    fatal("failed to write the output file: " + toString(std::move(e)));

  t.stop();
}
void coff::enqueueTask(std::function<void()> task) {
  rewriteQueue.push_back(std::move(task));
}
