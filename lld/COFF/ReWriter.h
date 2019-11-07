
#ifndef LLD_REWRITER_H
#define LLD_REWRITER_H

#include "InputFiles.h"

using namespace llvm;

namespace lld {
namespace coff {

void rewriteFile(ObjFile *file);
void rewriteTextSection(ObjFile *file);
void rewriteDataSection(ObjFile *file);
void doNothing();

static std::list<std::function<void()>> rewriteQueue;
void enqueueTask(std::function<void()> task);

void rewriteResult();

} // namespace coff
} // namespace lld

#endif // LLD_REWRITER_H
