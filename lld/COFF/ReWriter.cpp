#include "ReWriter.h"
#include "IncrementalLinkFile.h"
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

void coff::rewriteDataSection(InputFile *file) {
  outs() << "Rewriting .data section for file " << file->getName() << "\n";
  auto &secData = incrementalLinkFile->objFiles[file->getName()].sectionData;
  auto start = incrementalLinkFile->outputDataSectionRaw +
               secData.virtualAddress -
               incrementalLinkFile->outputDataSectionRVA;
  // TODO: Actual rewrite is just dummy data at the moment
  for (uint64_t i = start; i < start + secData.size - 1; i++) {
    binary->getBufferStart()[i] = '!';
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
