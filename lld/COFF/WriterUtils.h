
#ifndef LLD_WRITERUTILS_H
#define LLD_WRITERUTILS_H

#include "Chunks.h"
#include "llvm/ADT/StringRef.h"
#include <vector>

using namespace llvm;

namespace lld {
namespace coff {

StringRef getOutputSectionName(StringRef name);

// The CRT section contains, among other things, the array of function
// pointers that initialize every global variable that is not trivially
// constructed. The CRT calls them one after the other prior to invoking
// main().
//
// As per C++ spec, 3.6.2/2.3,
// "Variables with ordered initialization defined within a single
// translation unit shall be initialized in the order of their definitions
// in the translation unit"
//
// It is therefore critical to sort the chunks containing the function
// pointers in the order that they are listed in the object file (top to
// bottom), otherwise global objects might not be initialized in the
// correct order.
void sortCRTSectionChunks(std::vector<Chunk *> &chunks);

} // namespace coff
} // namespace lld

#endif // LLD_WRITERUTILS_H
