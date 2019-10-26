
#include "IncrementalLinkFile.h"
#include "Symbols.h"
#include "Writer.h"
#include <llvm/Support/xxhash.h>

using namespace lld;
using namespace lld::coff;

bool coff::initializeIlf(ArrayRef<char const *> argsArr) {
  incrementalLinkFile = make<IncrementalLinkFile>();

  std::vector<std::string> mArgs;
  for (auto arg : argsArr) {
    mArgs.push_back(arg);
  }
  incrementalLinkFile->outputFile = config->outputFile;
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
  // bool outputUntouched = xxHash64(outputOrError->get()->getBuffer()) ==
  //                       incrementalLinkFile->outputHash;
  incrementalLinkFile->rewritePossible = sameArgs;
  return incrementalLinkFile->rewritePossible;
}

void coff::writeIlfSectionData(llvm::ArrayRef<OutputSection *> outputSections) {
  if (!config->incrementalLink)
    return;
  for (OutputSection *sec : outputSections) {
    for (Chunk *c : sec->chunks) {
      auto *sc = dyn_cast<SectionChunk>(c);
      if (!sc)
        continue;

      if (sc->getSectionName() == ".data") {
        auto &sec =
            incrementalLinkFile->objFiles[sc->file->getName()].sectionData;
        sec.name = ".data";
        sec.size = sc->getSize();
        sec.virtualAddress = sc->getRVA();
      }
    }
  }
}
std::string IncrementalLinkFile::getFileName() {
  return incrementalLinkFile->outputFile + ".ilk.yaml";
}

void IncrementalLinkFile::writeToFile() {
  std::error_code code;
  raw_fd_ostream out(IncrementalLinkFile::getFileName(), code);
  llvm::yaml::Output yout(out);
  yout << *incrementalLinkFile;
  out.close();
}
