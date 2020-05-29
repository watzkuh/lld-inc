#include "ReWriter.h"
#include "Chunks.h"
#include "Driver.h"
#include "IncrementalLinkFile.h"
#include "InputFiles.h"
#include "SymbolTable.h"
#include "Symbols.h"
#include "Writer.h"
#include <lld/Common/ErrorHandler.h>
#include <lld/Common/Timer.h>
#include <llvm/Support/Error.h>
#include <llvm/Support/FileOutputBuffer.h>
#include <llvm/Support/xxhash.h>

using namespace llvm;
using namespace llvm::support;
using namespace llvm::COFF;
using namespace lld;
using namespace lld::coff;

static Timer patchTimer("Binary Patching", Timer::root());
std::unique_ptr<FileOutputBuffer> binary;

std::vector<ObjFile *> changedFiles;
std::vector<InputFile *> unchangedFiles;
std::set<StringRef> dependentFileNames;
llvm::StringMap<std::pair<uint64_t, uint64_t>> updatedSymbols;
SmallDenseSet<StringRef> sectionNames = {".text", ".data", ".rdata", ".xdata"};

void abortIncrementalLink() {
  lld::outs() << "Incremental link aborted, starting clean link...\n";
  binary->discard();
  symtab->clear();
  incrementalLinkFile->rewriteAborted = true;
  incrementalLinkFile->outputHash = 0;
  driver->clearVisitedFiles();
  std::vector<char *> args;
  for (auto &a : incrementalLinkFile->arguments)
    args.push_back(&a[0]);
  driver->link(ArrayRef<char *>(args));
}

bool isDiscardedCOMDAT(SectionChunk *sc, StringRef fileName) {
  if (!sc->isCOMDAT() || !sc->sym)
    return false;
  // A section was discarded if its leader symbol is defined in another file
  bool discarded =
      incrementalLinkFile->objFiles[fileName.str()].definedSymbols.count(
          sc->sym->getName().str()) == 0 &&
      incrementalLinkFile->globalSymbols.count(sc->sym->getName()) != 0;
  if (discarded)
    // Mark all associated children
    for (auto &it : sc->children()) {
      it.sym = sc->sym;
    }
  return discarded;
}

void assignAddresses(ObjFile *file) {
  std::map<StringRef, uint32_t> rvas;
  for (auto &s :
       incrementalLinkFile->objFiles[file->getName().str()].sections) {
    rvas[s.first] = s.second.virtualAddress;
  }
  for (Chunk *c : file->getChunks()) {
    auto *sc = dyn_cast<SectionChunk>(c);
    if (!sc || sc->getSize() == 0 || isDiscardedCOMDAT(sc, file->getName()))
      continue;

    sc->setRVA(rvas[sc->getSectionName()]);
    rvas[sc->getSectionName()] +=
        alignTo(sc->getSize(), incrementalLinkFile->paddedAlignment);
  }
}

void markDependentFiles(ObjFile *file) {
  auto &f = incrementalLinkFile->objFiles[file->getName().str()];
  for (const auto &dep : f.dependentFiles)
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

void reapplyRelocations(ObjFile *file) {
  lld::outs() << "Reapplying relocations for file " << file->getName() << "\n";
  auto &secInfo =
      incrementalLinkFile->objFiles[file->getName().str()].sections[".text"];
  auto &outputTextSection = incrementalLinkFile->outputSections[".text"];
  auto offset = outputTextSection.rawAddress + secInfo.virtualAddress -
                outputTextSection.virtualAddress;
  uint8_t *buf = binary->getBufferStart() + offset;

  for (auto &c : file->getChunks()) {

    auto *sc = dyn_cast_or_null<SectionChunk>(c);
    if (!sc || sc->getSectionName() != ".text")
      continue;
    if (isDiscardedCOMDAT(sc, file->getName()))
      continue;

    for (size_t j = 0, e = sc->getRelocs().size(); j < e; j++) {
      const coff_relocation &rel = sc->getRelocs()[j];
      auto *sym = sc->file->getSymbol(rel.SymbolTableIndex);

      auto it = updatedSymbols.find(sym->getName());
      if (it == updatedSymbols.end()) {
        continue;
      }

      if (it->second.first == it->second.second &&
          it->second.first == INT64_MAX) {
        lld::outs() << "MISSING: " << sym->getName() << "\n";
        incrementalLinkFile->rewriteAborted = true;
      }
      lld::outs() << sym->getName()
                  << " changed address; old: " << it->second.first
                  << " new: " << it->second.second << "\n";

      uint64_t s = it->second.second;
      uint64_t invertS = -it->second.first;

      uint8_t *off = buf + rel.VirtualAddress;
      uint64_t p = sc->getRVA() + rel.VirtualAddress;
      uint8_t typeOff = 0;
      // Only AMD64 support at the moment
      switch (rel.Type) {
      case COFF::IMAGE_REL_AMD64_REL32:
        typeOff = 4;
        break;
      case COFF::IMAGE_REL_AMD64_REL32_1:
        typeOff = 5;
        break;
      case COFF::IMAGE_REL_AMD64_REL32_2:
        typeOff = 6;
        break;
      case COFF::IMAGE_REL_AMD64_REL32_3:
        typeOff = 7;
        break;
      case COFF::IMAGE_REL_AMD64_REL32_4:
        typeOff = 8;
        break;
      case COFF::IMAGE_REL_AMD64_REL32_5:
        typeOff = 9;
        break;
      }

      // First, undo the original relocation
      applyRelocation(*sc, off, (support::ulittle16_t)rel.Type,
                      invertS + 2 * (p + typeOff), p);
      applyRelocation(*sc, off, (support::ulittle16_t)rel.Type, s, p);
    }
    buf += c->getSize();
  }
}

IncrementalLinkFile::ChunkInfo rewriteTextSection(SectionChunk *sc,
                                                  uint8_t *buf,
                                                  uint32_t chunkStart,
                                                  size_t paddedSize) {
  memset(buf, 0xCC, paddedSize);
  memcpy(buf, sc->getContents().data(), sc->getSize());

  IncrementalLinkFile::ChunkInfo chunkInfo;
  chunkInfo.virtualAddress = sc->getRVA();
  chunkInfo.size = paddedSize;

  StringRef fileName = sc->file->getName();
  // Reset dependencies
  auto backRefs = incrementalLinkFile->objFiles[fileName.str()].dependentOn;
  for (const auto &ref : backRefs) {
    incrementalLinkFile->objFiles[ref].dependentFiles.erase(fileName.str());
  }

  for (size_t j = 0, e = sc->getRelocs().size(); j < e; j++) {
    const coff_relocation &rel = sc->getRelocs()[j];
    auto *sym = sc->file->getSymbol(rel.SymbolTableIndex);
    auto *definedSym = dyn_cast_or_null<Defined>(sym);
    // The target of the relocation
    uint64_t s = 0;
    std::string definedIn = "";
    if (definedSym != nullptr) {
      s = definedSym->getRVA();
      definedIn = definedSym->getFile()->getName().str();
    }
    // Fall back to symbol table if we either have no information
    // about the chunk or the symbol
    if (definedSym == nullptr || s == 0) {
      auto sf = incrementalLinkFile->globalSymbols[sym->getName().str()];
      s = sf.first;
      definedIn = sf.second;
    }
    // Check if a new dependency was introduced
    if (definedIn != fileName && incrementalLinkFile->input.count(definedIn)) {
      incrementalLinkFile->objFiles[definedIn].dependentFiles.insert(
          fileName.str());
    }
    uint8_t *off = buf + rel.VirtualAddress;

    // Compute the RVA of the relocation for relative relocations.
    uint64_t p = chunkStart + rel.VirtualAddress;

    applyRelocation(*sc, off, rel.Type, s, p);
  }
  return chunkInfo;
}

void rewriteSection(const std::vector<SectionChunk *> &chunks,
                    const std::string &fileName, const std::string &secName) {

  // All currently supported sections for incremental links
  if (sectionNames.count(secName) == 0) {
    lld::outs() << "Ignored: " << secName << " section\n";
    return;
  }

  lld::outs() << "Rewriting " << secName << " section for file " << fileName
              << "\n";

  auto secIt = incrementalLinkFile->objFiles[fileName].sections.find(secName);
  if (secIt == incrementalLinkFile->objFiles[fileName].sections.end())
    return;
  auto &secInfo = secIt->second;

  std::string outSecName(secName);
  for (auto &p : config->merge) {
    if (secName == p.first) {
      outSecName = p.second.str();
      break;
    }
  }

  auto &outputSectionInfo = incrementalLinkFile->outputSections[outSecName];

  auto offset =
      outputSectionInfo.rawAddress +
      (secInfo.virtualAddress < outputSectionInfo.virtualAddress
           ? 0
           : secInfo.virtualAddress - outputSectionInfo.virtualAddress);

  uint8_t *buf = binary->getBufferStart() + offset;
  uint32_t contribSize = 0;
  uint32_t num = 0;

  std::vector<IncrementalLinkFile::ChunkInfo> newChunks;
  for (SectionChunk *sc : chunks) {
    if (!sc || sc->getSize() == 0 || isDiscardedCOMDAT(sc, fileName))
      continue;

    const size_t paddedSize =
        alignTo(sc->getSize(), incrementalLinkFile->paddedAlignment);
    IncrementalLinkFile::ChunkInfo chunkInfo;
    if (secName == ".text") {
      chunkInfo = rewriteTextSection(
          sc, buf, (secInfo.virtualAddress + contribSize), paddedSize);
    } else {
      memset(buf, 0x0, paddedSize);
      memcpy(buf, sc->getContents().data(), sc->getSize());
      chunkInfo.size = paddedSize;
      chunkInfo.virtualAddress = sc->getRVA();
    }
    newChunks.push_back(chunkInfo);
    contribSize += paddedSize;
    num++;
    buf += paddedSize;
  }
  secInfo.chunks = newChunks;
  if (num != secInfo.chunks.size()) {
    lld::outs() << num << " new section vs old " << secInfo.chunks.size()
                << "\n";
  }

  if (secInfo.size != contribSize) {
    lld::outs() << "New " << secName << " section in " << fileName
                << " is not the same size \n";
    lld::outs() << "New: " << contribSize << "\tOld: " << secInfo.size << "\n";
    incrementalLinkFile->rewriteAborted = true;
    return;
  }
}

std::pair<uint64_t, std::string> getFileInfoIfDuplicate(StringRef symbol,
                                                        StringRef fileName) {
  auto symLookup = incrementalLinkFile->globalSymbols[symbol];
  if (symLookup.second != fileName) {
    return {incrementalLinkFile->objFiles[symLookup.second].position,
            symLookup.second};
  }
  return {0, ""};
}

void updateSymbolTable(ObjFile *file) {
  auto &oldSyms =
      incrementalLinkFile->objFiles[file->getName().str()].definedSymbols;
  StringMap<bool> newSyms;
  for (const auto &sym : file->getSymbols()) {
    auto *definedSym = dyn_cast_or_null<Defined>(sym);
    if (!definedSym || !definedSym->isLive() || !definedSym->isExternal
        // symbol not defined in this file, but the declaring file also changed
        || definedSym->getFile()->getName() != file->getName())
      continue;
    newSyms[definedSym->getName()] = true;
    auto it = oldSyms.find(definedSym->getName().str());
    if (it == oldSyms.end()) {
      // New symbol was introduced, check if it already exists in another file
      auto fileInfo = getFileInfoIfDuplicate(definedSym->getName(),
                                             sym->getFile()->getName());
      if (fileInfo.first != 0) {
        // If it's a comdat symbol that was defined elsewhere it might not
        // necessarily be an error
        if (definedSym->isCOMDAT) {
          auto *sc = dyn_cast_or_null<SectionChunk>(definedSym->getChunk());
          switch (sc->selection) {
          case IMAGE_COMDAT_SELECT_NODUPLICATES:
            // This is an error, leave diagnostics to the full link
            break;
          case IMAGE_COMDAT_SELECT_ANY:
            // The file that defines the symbol has to appear before the current
            // file to ensure output reproducibility
            if (incrementalLinkFile->objFiles[file->getName().str()].position >
                fileInfo.first)
              continue;
            break;
          case IMAGE_COMDAT_SELECT_LARGEST:
          case IMAGE_COMDAT_SELECT_EXACT_MATCH: {
            auto &chunks = incrementalLinkFile->objFiles[fileInfo.second]
                               .sections[".text"]
                               .chunks;
            // TODO: Chunks are ordered by address, use binary search or
            // different data structure to avoid O(m*n) complexity
            IncrementalLinkFile::ChunkInfo chunkInfo;
            for (auto &c : chunks) {
              if (c.virtualAddress == definedSym->getRVA()) {
                chunkInfo = c;
                break;
              }
            }
            // If the comdat section in the other file is larger, it fine to
            // keep it
            if (sc->selection == IMAGE_COMDAT_SELECT_LARGEST &&
                sc->getSize() >= chunkInfo.size)
              continue;
            // If the comdat section in the other file the same size and appears
            // earlier it is fine to keep it
            if (sc->selection == IMAGE_COMDAT_SELECT_EXACT_MATCH &&
                sc->getSize() == chunkInfo.size &&
                incrementalLinkFile->objFiles[file->getName().str()].position >
                    fileInfo.first)
              continue;
            break;
          }
          case IMAGE_COMDAT_SELECT_ASSOCIATIVE:
            // Associative comdats should not be extern in practice and
            // therefore not be stored in the ilk file
            llvm_unreachable("multiple definitions of associative comdat");
          default:; // NO-OP
          }
        }
        lld::outs() << "Duplicate symbol: " << definedSym->getName() << "\n";
        incrementalLinkFile->rewriteAborted = true;
      }
      updatedSymbols[definedSym->getName()] =
          std::make_pair(0, definedSym->getRVA());
      oldSyms[definedSym->getName().str()] = definedSym->getRVA();
      lld::outs() << "ADDED: " << sym->getName() << "\n";
    } else {
      if (it->second != definedSym->getRVA()) {
        updatedSymbols[definedSym->getName()] =
            std::make_pair(it->second, definedSym->getRVA());
        oldSyms[it->first] = definedSym->getRVA();
      }
    }
  }
  for (auto &sym : oldSyms) {
    if (sym.second == 0)
      continue;
    if (!newSyms[sym.first]) {
      lld::outs() << "REMOVED: " << sym.first << "\n";
      // Kind of hackish way to encode a removed symbol to check for errors
      // later during traversal through dependent files
      updatedSymbols[sym.first] = std::make_pair(INT64_MAX, INT64_MAX);
    }
  }
  for (auto &p : updatedSymbols) {
    if (p.second.first == p.second.second && p.second.first == INT64_MAX)
      // Remove symbols from the incremental link file as well
      oldSyms.erase(p.first().str());
  }
}

void rewriteFile(ObjFile *file) {
  std::map<StringRef, std::vector<SectionChunk *>> chunks;
  for (Chunk *c : file->getChunks()) {
    auto *sc = dyn_cast<SectionChunk>(c);
    chunks[sc->getSectionName()].push_back(sc);
  }
  for (auto &sec : chunks) {
    rewriteSection(sec.second, file->getName().str(), sec.first.str());
  }
}

void coff::markForReWrite(coff::ObjFile *file) {
  markDependentFiles(file);
  assignAddresses(file);
  changedFiles.push_back(file);
}

void coff::defer(InputFile *file) { unchangedFiles.push_back(file); }

void coff::rewriteResult() {
  ScopedTimer t(patchTimer);
  binary = CHECK(FileOutputBuffer::create(incrementalLinkFile->outputFile, -1,
                                          llvm::FileOutputBuffer::F_modify),
                 "failed to open " + incrementalLinkFile->outputFile);
  while (!rewriteQueue.empty() && !incrementalLinkFile->rewriteAborted) {
    rewriteQueue.front()();
    rewriteQueue.pop_front();
  }

  for (auto &f : changedFiles)
    updateSymbolTable(f);
  for (auto &f : changedFiles)
    rewriteFile(f);
  for (auto &f : unchangedFiles) {
    if (dependentFileNames.count(f->getName()) == 1) {
      symtab->addFile(f);
      if (auto *obj = dyn_cast<ObjFile>(f)) {
        assignAddresses(obj);
        reapplyRelocations(obj);
      }
    }
  }

  if (incrementalLinkFile->rewriteAborted) {
    t.stop();
    abortIncrementalLink();
    return;
  }

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
