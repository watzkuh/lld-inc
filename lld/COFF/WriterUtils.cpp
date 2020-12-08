#include "WriterUtils.h"
#include "lld/Common/ErrorHandler.h"

using namespace llvm;

namespace lld {
namespace coff {

StringRef getOutputSectionName(StringRef name) {
  StringRef s = name.split('$').first;

  // Treat a later period as a separator for MinGW, for sections like
  // ".ctors.01234".
  return s.substr(0, s.find('.', 1));
}

void sortCRTSectionChunks(std::vector<Chunk *> &chunks) {
  auto sectionChunkOrder = [](const Chunk *a, const Chunk *b) {
    auto sa = dyn_cast<SectionChunk>(a);
    auto sb = dyn_cast<SectionChunk>(b);
    assert(sa && sb && "Non-section chunks in CRT section!");

    StringRef sAObj = sa->file->mb.getBufferIdentifier();
    StringRef sBObj = sb->file->mb.getBufferIdentifier();

    return sAObj == sBObj && sa->getSectionNumber() < sb->getSectionNumber();
  };
  llvm::stable_sort(chunks, sectionChunkOrder);

  if (config->verbose) {
    for (auto &c : chunks) {
      auto sc = dyn_cast<SectionChunk>(c);
      log("  " + sc->file->mb.getBufferIdentifier().str() +
          ", SectionID: " + Twine(sc->getSectionNumber()));
    }
  }
}
} // namespace coff
} // namespace lld
