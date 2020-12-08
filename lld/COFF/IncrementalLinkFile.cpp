#include "IncrementalLinkFile.h"
#include "Driver.h"
#include "Symbols.h"
#include "Writer.h"
#include "cereal/cereal/archives/binary.hpp"
#include "cereal/cereal/cereal.hpp"
#include "cereal/cereal/types/map.hpp"
#include "cereal/cereal/types/set.hpp"
#include "cereal/cereal/types/string.hpp"
#include "cereal/cereal/types/vector.hpp"
#include "lld/Common/ErrorHandler.h"
#include "llvm/Support/raw_ostream.h"
#include <fstream>
#include <lld/Common/Timer.h>
#include <llvm/Support/xxhash.h>

using namespace lld;
using namespace llvm::COFF;
using namespace lld::coff;

static Timer sectionWriter("Writing Output Sections", Timer::root());
static Timer symbolWriter("Writing Symbols", Timer::root());

// Copied from MapFile.cpp
// Returns a list of all symbols that we want to print out.
// TODO:
static std::vector<DefinedRegular *> getSymbols() {
  std::vector<DefinedRegular *> v;
  for (ObjFile *file : ObjFile::instances)
    for (Symbol *b : file->getSymbols()) {
      if (auto *sym = dyn_cast_or_null<DefinedRegular>(b))
        if (!sym->getCOFFSymbol().isSectionDefinition())
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
  for (const auto &arg : argsArr) {
    mArgs.push_back(arg);
  }
  incrementalLinkFile->outputFile = config->outputFile;
  if (incrementalLinkFile->outputFile.empty())
    incrementalLinkFile->outputFile = std::move(possibleOutput);
  if (config->benchmark) {
    {
      std::ifstream file(IncrementalLinkFile::getFileName(),
                         std::ios::out | std::ios::binary);
      if (file.is_open()) {
        cereal::BinaryInputArchive inputArchive(file);
        inputArchive(
            incrementalLinkFile->arguments, incrementalLinkFile->outputFile,
            incrementalLinkFile->outputHash,
            incrementalLinkFile->outputSections, incrementalLinkFile->objFiles,
            incrementalLinkFile->mergedSections);
      }
      file.close();
    }
  } else {
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
  }
  for (const auto &f : incrementalLinkFile->objFiles) {
    for (const auto &s : f.second.definedSymbols) {
      incrementalLinkFile->globalSymbols[s.first] = {s.second, f.first};
    }
    for (const auto &dep : f.second.dependentFiles) {
      incrementalLinkFile->objFiles[dep].dependentOn.insert(f.first);
    }
  }
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
  ScopedTimer t1(sectionWriter);
  lld::outs() << "Writing ilk \n";
  for (OutputSection *os : outputSections) {
    std::string const secName = os->name.str();
    IncrementalLinkFile::OutputSectionInfo outputSectionInfo{
        os->getFileOff(), os->getRVA(), os->getRawSize()};
    incrementalLinkFile->outputSections[secName] = outputSectionInfo;
    for (Chunk *c : os->chunks) {
      auto *sc = dyn_cast<SectionChunk>(c);
      if (!sc || !sc->getSize())
        continue;
      std::string const fileName = sc->file->getName().str();
      if (!incrementalLinkFile->rewritableFileNames.count(fileName))
        continue;

      // We want to save the section type as it is defined in the COFF header,
      // not in the OutputSection object
      auto origSecName = StringRef(sc->header->Name).split('$').first;
      auto &sec =
          incrementalLinkFile->objFiles[fileName].sections[origSecName.str()];

      IncrementalLinkFile::ChunkInfo chunkInfo;
      chunkInfo.virtualAddress = sc->getRVA();
      chunkInfo.size = sc->getSize();

      const bool isCodeSection =
          (os->header.Characteristics & IMAGE_SCN_CNT_CODE) &&
          (os->header.Characteristics & IMAGE_SCN_MEM_READ) &&
          (os->header.Characteristics & IMAGE_SCN_MEM_EXECUTE);
      if (isCodeSection) {
        for (size_t j = 0, e = sc->getRelocs().size(); j < e; j++) {
          const coff_relocation &rel = sc->getRelocs()[j];
          auto *sym = sc->file->getSymbol(rel.SymbolTableIndex);
          auto *definedSym = dyn_cast_or_null<Defined>(sym);
          if (definedSym) {
            // Non external symbol can be resolved by only parsing the file they
            // are defined in, so we do not have to save them
            if (!definedSym->getFile() || !definedSym->isExternal ||
                !definedSym->isLive())
              continue;
            if (definedSym->getFile()->getName() != fileName &&
                incrementalLinkFile->input.count(
                    definedSym->getFile()->getName())) {
              incrementalLinkFile
                  ->objFiles[definedSym->getFile()->getName().str()]
                  .dependentFiles.insert(fileName);
            }
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
      if (c->hasData)
        sec.size += alignTo(sc->getSize(), sc->getAlignment());
    }
  }
  for (auto &f : incrementalLinkFile->rewritableFileNames) {
    for (auto &s : incrementalLinkFile->objFiles[f.str()].sections) {
      s.second.size =
          alignTo(s.second.size, incrementalLinkFile->paddedAlignment);
    }
  }
  t1.stop();

  // TODO: Create own function for writing symbol list
  ScopedTimer t(symbolWriter);
  for (auto &sym : getSymbols()) {
    if (sym->getRVA() == 0 || !sym->isLive() || !sym->isExternal) {
      continue;
    }
    incrementalLinkFile->objFiles[sym->file->getName().str()]
        .definedSymbols[sym->getName().str()] = sym->getRVA();
  }
  t.stop();
}

std::string IncrementalLinkFile::getFileName() {
  std::string end = config->benchmark ? ".bin" : ".yaml";
  return incrementalLinkFile->outputFile + ".ilk" + end;
}

void IncrementalLinkFile::writeToDisk() {
  if (config->benchmark) {
    std::ofstream file(IncrementalLinkFile::getFileName(),
                       std::ios::out | std::ios::binary);
    {
      cereal::BinaryOutputArchive outputArchive(file);
      outputArchive(
          incrementalLinkFile->arguments, incrementalLinkFile->outputFile,
          incrementalLinkFile->outputHash, incrementalLinkFile->outputSections,
          incrementalLinkFile->objFiles, incrementalLinkFile->mergedSections);
    }
    file.close();
  } else {
    std::error_code code;
    raw_fd_ostream out(IncrementalLinkFile::getFileName(), code);
    llvm::yaml::Output yout(out);
    yout << *incrementalLinkFile;
    out.close();
  }
}
