
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
    for (Symbol *b : file->getSymbols())
      if (auto *sym = dyn_cast_or_null<DefinedRegular>(b))
        if (sym && !sym->getCOFFSymbol().isSectionDefinition())
          v.push_back(sym);
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
      if (!sc || sc->getSectionName() != secName || !sc->getSize())
        continue;
      StringRef const name = sc->file->getName();
      auto &sec = incrementalLinkFile->objFiles[name].sections[secName];

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
    if (sym->getRVA() != 0 && sym->isLive()) {
      incrementalLinkFile->definedSymbols[sym->getName()] = sym->getRVA();
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
