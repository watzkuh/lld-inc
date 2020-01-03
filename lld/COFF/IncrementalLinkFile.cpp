
#include "IncrementalLinkFile.h"
#include "Driver.h"
#include "Symbols.h"
#include "Writer.h"
#include "lld/Common/ErrorHandler.h"
#include <llvm/Support/xxhash.h>

using namespace lld;
using namespace lld::coff;

// Copied from MapFile.cpp
// Returns a list of all symbols that we want to print out.
// TODO:
static std::vector<DefinedRegular *> getSymbols() {
  std::vector<DefinedRegular *> v;
  for (ObjFile *file : ObjFile::instances)
    for (Symbol *b : file->getSymbols()) {
      if (auto *sym = dyn_cast_or_null<DefinedRegular>(b))
        if (sym && !sym->getCOFFSymbol().isSectionDefinition())
          v.push_back(sym);
    }
  return v;
}

bool coff::initializeIlf(ArrayRef<char const *> argsArr,
                         std::string possibleOutput) {
  if (incrementalLinkFile->rewriteAborted) {
    incrementalLinkFile->rewritePossible = false;
    return incrementalLinkFile->rewritePossible;
  }
  std::vector<std::string> mArgs;
  for (auto arg : argsArr) {
    mArgs.push_back(arg);
  }
  incrementalLinkFile->outputFile = config->outputFile;
  if (incrementalLinkFile->outputFile.empty())
    incrementalLinkFile->outputFile = std::move(possibleOutput);
  ErrorOr<std::unique_ptr<MemoryBuffer>> ilkOrError =
      MemoryBuffer::getFile(IncrementalLinkFile::getFileName());
  if (!ilkOrError) {
    // Add the new arguments anyway
    incrementalLinkFile->arguments = mArgs;
    incrementalLinkFile->rewritePossible = false;
    return incrementalLinkFile->rewritePossible;
  }
  yaml::Input yin(ilkOrError->get()->getBuffer());
  yin >> *incrementalLinkFile;
  bool sameArgs = (mArgs == incrementalLinkFile->arguments);
  incrementalLinkFile->arguments = mArgs;
  ErrorOr<std::unique_ptr<MemoryBuffer>> outputOrError =
      MemoryBuffer::getFile(incrementalLinkFile->outputFile);
  if (!outputOrError) {
    incrementalLinkFile->rewritePossible = false;
    return incrementalLinkFile->rewritePossible;
  }
  bool outputUntouched = xxHash64(outputOrError->get()->getBuffer()) ==
                         incrementalLinkFile->outputHash;
  incrementalLinkFile->rewritePossible = sameArgs && outputUntouched;
  return incrementalLinkFile->rewritePossible;
}

void coff::writeIlfSections(llvm::ArrayRef<OutputSection *> outputSections) {
  if (!config->incrementalLink)
    return;

  for (OutputSection *sec : outputSections) {
    StringRef const secName = sec->name;
    IncrementalLinkFile::OutputSectionInfo outputSectionInfo{
        sec->getFileOff(), sec->getRVA(), sec->getRawSize()};
    incrementalLinkFile->outputSections[secName] = outputSectionInfo;
    for (Chunk *c : sec->chunks) {
      auto *sc = dyn_cast<SectionChunk>(c);
      if (!sc || !sc->getSize())
        continue;
      StringRef const name = sc->file->getName();
      if (!incrementalLinkFile->rewritableFileNames.count(name))
        continue;

      // Important! We want to save the section type as its defined in the COFF
      // header, not in the OutputSection object as Writer.cpp merges different
      // sections types into one output section. We do not do this when linking
      // incrementally, so we have to remember the location based on its
      // original section type.
      auto &sec =
          incrementalLinkFile->objFiles[name].sections[sc->header->Name];

      IncrementalLinkFile::ChunkInfo chunkInfo;
      chunkInfo.checksum = sc->checksum;
      chunkInfo.virtualAddress = sc->getRVA();
      chunkInfo.size =
          alignTo(sc->getSize(), incrementalLinkFile->paddedAlignment);

      // Assumption: Only the .text sections has interesting relocations
      if (secName == ".text") {
        for (size_t j = 0, e = sc->getRelocs().size(); j < e; j++) {
          const coff_relocation &rel = sc->getRelocs()[j];
          auto *sym = sc->file->getSymbol(rel.SymbolTableIndex);
          // Non external symbol can be resolved by only parsing the file they
          // are defined in, so we do not have to save them
          if (!sym->isExternal)
            continue;
          auto *definedSym = dyn_cast_or_null<Defined>(sym);
          if (definedSym) {
            IncrementalLinkFile::SymbolInfo symbolInfo;
            symbolInfo.definitionAddress = definedSym->getRVA();
            IncrementalLinkFile::RelocationInfo relInfo{rel.VirtualAddress,
                                                        rel.Type};
            if (!chunkInfo.symbols.count(definedSym->getName()))
              chunkInfo.symbols[definedSym->getName()] = symbolInfo;
            chunkInfo.symbols[definedSym->getName()].relocations.push_back(
                relInfo);
          }
        }
      }
      sec.chunks.push_back(chunkInfo);
      // The contribution of one object file to one of the sections of the
      // output file can consist of n OutputChunks. However, they seem to
      // always be directly after each other, so storing the lowest address
      // and the sum of the sizes could work
      if (sec.virtualAddress == 0 || sec.virtualAddress > sc->getRVA())
        sec.virtualAddress = sc->getRVA();
      sec.size += alignTo(sc->getSize(), incrementalLinkFile->paddedAlignment);
    }
  }
  // TODO: Create own function for writing symbol list
  for (auto &sym : getSymbols()) {
    if (sym->getRVA() == 0 || !sym->isLive() || !sym->isExternal) {
      continue;
    }
    auto &s = incrementalLinkFile->objFiles[sym->file->getName()]
                  .definedSymbols[sym->getName()];
    if (!sym->getFile()->getName().empty())
      s.fileDefinedIn = sym->getFile()->getName();
    s.definitionAddress = sym->getRVA();
    if (s.filesUsedIn.size() <= 1)
      continue;
    // TODO: This way of getting file dependencies is too inefficient
    for (auto &used : s.filesUsedIn) {
      if (used != sym->getFile()->getName())
        incrementalLinkFile->objFiles[sym->getFile()->getName()]
            .dependentFiles.insert(used);
    }
  }
}

std::string IncrementalLinkFile::getFileName() {
  return incrementalLinkFile->outputFile + ".ilk.yaml";
}

void IncrementalLinkFile::writeToDisk() {
  std::error_code code;
  raw_fd_ostream out(IncrementalLinkFile::getFileName(), code);
  llvm::yaml::Output yout(out);
  yout << *incrementalLinkFile;
  out.close();
}
