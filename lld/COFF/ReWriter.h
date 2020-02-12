
#ifndef LLD_REWRITER_H
#define LLD_REWRITER_H

#include "InputFiles.h"
#include "llvm/Option/ArgList.h"

using namespace llvm;

namespace lld {
namespace coff {

void markForReWrite(ObjFile *file);
void defer(InputFile *file);

static std::list<std::function<void()>> rewriteQueue;
void enqueueTask(std::function<void()> task);

void rewriteResult();

} // namespace coff
} // namespace lld

#endif // LLD_REWRITER_H
