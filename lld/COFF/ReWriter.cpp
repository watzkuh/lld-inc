
#include "ReWriter.h"
#include "Chunks.h"
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

void rewriteTextSection(ObjFile *file) {
  outs() << "Rewriting .text section for file " << file->getName() << "\n";

  auto &secInfo =
      incrementalLinkFile->objFiles[file->getName()].sections[".text"];
  auto &outputTextSection = incrementalLinkFile->outputSections[".text"];
  auto offset = outputTextSection.rawAddress + secInfo.virtualAddress -
                outputTextSection.virtualAddress;

  uint8_t *buf = binary->getBufferStart() + offset;
  uint32_t chunkStart = incrementalLinkFile->objFiles[file->getName()]
                            .sections[".text"]
                            .virtualAddress;

  for (Chunk *c : file->getChunks()) {
    auto *sc = dyn_cast<SectionChunk>(c);

    if (sc->getSectionName() != ".text") {
      continue;
    }
    const size_t paddedSize =
        alignTo(sc->getSize(), incrementalLinkFile->paddedAlignment);

    memset(buf, 0xCC, paddedSize);
    memcpy(buf, sc->getContents().data(), sc->getSize());
    outs() << "Patched " << paddedSize << " bytes \n";

    for (size_t j = 0, e = sc->getRelocs().size(); j < e; j++) {
      const coff_relocation &rel = sc->getRelocs()[j];
      auto *sym = file->getSymbol(rel.SymbolTableIndex);
      auto *definedSym = dyn_cast_or_null<Defined>(sym);
      uint64_t s = 0;
      if (definedSym != nullptr) {
        // The target of the relocation
        s = incrementalLinkFile->objFiles[definedSym->getFile()->getName()]
                .sections[definedSym->getChunk()->getSectionName()]
                .virtualAddress +
            definedSym->getRVA(); //  only returns offset within the same chunk
      }
      // Fallback to symbol table if we either have no information
      // about the chunk or the symbol
      if (definedSym == nullptr || s == 0 || definedSym->getRVA() == 0) {
        s = incrementalLinkFile->definedSymbols[sym->getName()];
      }
      uint8_t *off = buf + rel.VirtualAddress;

      // Compute the RVA of the relocation for relative relocations.
      uint64_t p = chunkStart + rel.VirtualAddress;
      OutputSection *os = nullptr; // not that interesting at the moment TODO:
      switch (config->machine) {
      case AMD64:
        sc->applyRelX64(off, rel.Type, os, s, p);
        break;
      case I386:
        sc->applyRelX86(off, rel.Type, os, s, p);
        break;
      case ARMNT:
        sc->applyRelARM(off, rel.Type, os, s, p);
        break;
      case ARM64:
        sc->applyRelARM64(off, rel.Type, os, s, p);
        break;
      default:
        llvm_unreachable("unknown machine type");
      }
    }
    chunkStart += paddedSize;
    buf += paddedSize;
  }
}

void rewriteDataSection(ObjFile *file) {
  outs() << "Rewriting .data section for file " << file->getName() << "\n";
  auto &secInfo =
      incrementalLinkFile->objFiles[file->getName()].sections[".data"];
  auto &outputDataSection = incrementalLinkFile->outputSections[".data"];
  auto offset = outputDataSection.rawAddress + secInfo.virtualAddress -
                outputDataSection.virtualAddress;
  for (Chunk *c : file->getChunks()) {
    auto *sc = dyn_cast<SectionChunk>(c);
    if (sc->getSectionName() == ".data") {
      if (secInfo.size != sc->getSize()) {
        outs() << "New data section is not the same size \n";
        outs() << "New: " << sc->getSize() << "\tOld: " << secInfo.size << "\n";
      }
      c->writeTo(binary->getBufferStart() + offset);
    }
  }
  outs() << "Patched " << secInfo.size << " bytes \n";
}

void coff::rewriteFile(coff::ObjFile *file) {
  rewriteTextSection(file);
  rewriteDataSection(file);
}

void coff::doNothing() {}

void coff::rewriteResult() {
  ScopedTimer t(patchTimer);
  binary = CHECK(FileOutputBuffer::create(incrementalLinkFile->outputFile, -1,
                                          llvm::FileOutputBuffer::F_modify),
                 "failed to open " + incrementalLinkFile->outputFile);
  while (!rewriteQueue.empty()) {
    rewriteQueue.front()();
    rewriteQueue.pop_front();
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
