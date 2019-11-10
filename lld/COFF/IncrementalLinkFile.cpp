
#include "IncrementalLinkFile.h"
#include "Driver.h"
#include "Symbols.h"
#include "Writer.h"
#include "lld/Common/ErrorHandler.h"
#include <llvm/Support/xxhash.h>

using namespace lld;
using namespace lld::coff;

bool coff::initializeIlf(ArrayRef<char const *> argsArr,
                         std::string possibleOutput) {
  incrementalLinkFile = make<IncrementalLinkFile>();

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

  /* Some debug output how many sectionChunks there are
  for (OutputSection *sec : outputSections) {
    outs() << sec->name << ": ";
    int nrOfChunks = 0;
    for (Chunk *c : sec->chunks) {
      auto *sc = dyn_cast<SectionChunk>(c);
      if (!sc)
        continue;

      nrOfChunks++;
    }
    outs() << nrOfChunks << " Chunks\n";
  } */

  for (OutputSection *sec : outputSections) {
    StringRef const section = sec->name;
    if (section == ".text" || section == ".data") {
      if (section == ".text") {
        incrementalLinkFile->outputTextSectionRaw = sec->getFileOff();
        incrementalLinkFile->outputTextSectionRVA = sec->getRVA();
      }
      if (section == ".data") {
        incrementalLinkFile->outputDataSectionRaw = sec->getFileOff();
        incrementalLinkFile->outputDataSectionRVA = sec->getRVA();
      }
      for (Chunk *c : sec->chunks) {
        auto *sc = dyn_cast<SectionChunk>(c);
        if (!sc)
          continue;
        StringRef const name = sc->file->getName();
        auto &sec = incrementalLinkFile->objFiles[name].sections[section];
        IncrementalLinkFile::ChunkInfo chunkInfo = {sc->getRVA()};
        sec.chunks.push_back(chunkInfo);
        // The contribution of one object file to one of the sections of the
        // output file can consist of n OutputChunks. However, they seem to
        // always be directly after each other, so storing the lowest address
        // and the sum of the sizes could work
        if (sec.virtualAddress == 0 || sec.virtualAddress > sc->getRVA())
          sec.virtualAddress = sc->getRVA();
        sec.size += sc->getSize();
      }
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
