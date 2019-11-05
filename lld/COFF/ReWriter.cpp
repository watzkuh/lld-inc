
#include "ReWriter.h"
#include "Chunks.h"
#include "IncrementalLinkFile.h"
#include "InputFiles.h"
#include "Writer.h"
#include <lld/Common/ErrorHandler.h>
#include <lld/Common/Timer.h>
#include <llvm/Support/Error.h>
#include <llvm/Support/FileOutputBuffer.h>
#include <llvm/Support/xxhash.h>

using namespace llvm;
using namespace llvm::support;
using namespace lld;

static Timer patchTimer("Binary Patching", Timer::root());
std::unique_ptr<FileOutputBuffer> binary;

void coff::rewriteDataSection(ObjFile *file) {
  outs() << "Rewriting .data section for file " << file->getName() << "\n";
  auto &secData = incrementalLinkFile->objFiles[file->getName()].sectionData;
  auto offset = incrementalLinkFile->outputDataSectionRaw +
                secData.virtualAddress -
                incrementalLinkFile->outputDataSectionRVA;
  for (Chunk *c : file->getChunks()) {
    auto *sc = dyn_cast<SectionChunk>(c);
    if (sc->getSectionName() == ".data") {
      auto contents = sc->getContents();
      int sizeDiff = secData.size != contents.size();
      if (sizeDiff) {
        outs() << "New data section is not the same size \n";
      }
      memcpy(binary->getBufferStart() + offset, contents.data(), secData.size);
    }
  }

  outs() << "Patched " << secData.size << " bytes \n";
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
