#ifndef LLD_INCREMENTALLINKFILE_H
#define LLD_INCREMENTALLINKFILE_H

#include "llvm/Support/YAMLParser.h"
#include "llvm/Support/YAMLTraits.h"
#include "llvm/Support/raw_ostream.h"
#include <map>
#include <string>
#include <utility>

using namespace llvm;

namespace lld {
namespace coff {

class IncrementalLinkFile {
public:
  IncrementalLinkFile() = default;
  IncrementalLinkFile(std::vector<std::string> arguments,
                      std::vector<std::string> objects,
                      std::vector<std::string> fileNames,
                      std::vector<uint64_t> fileHashes) {
    this->arguments = arguments;
    this->objects = objects;
    for (unsigned long i = 0; i < fileNames.size(); i++) {
      this->fileHashes[fileNames[i]] = fileHashes[i];
    }
  }
  std::vector<std::string> arguments;
  std::vector<std::string> objects;
  std::map<std::string, uint64_t> fileHashes;
  constexpr static const char *fileEnding = {".ilk.yaml"};
};

extern IncrementalLinkFile *incrementalLinkFile;

} // namespace coff
} // namespace lld

using namespace llvm;
using lld::coff::IncrementalLinkFile;
using yaml::MappingTraits;

template <> struct MappingTraits<IncrementalLinkFile> {
  class NormalizedIlf {
  public:
    NormalizedIlf(IO &io){};
    NormalizedIlf(IO &, IncrementalLinkFile &ilf) {
      arguments = ilf.arguments;
      objects = ilf.objects;
      for (auto &p : ilf.fileHashes) {
        fileNames.push_back(p.first);
        fileHashes.push_back(p.second);
      }
    }

    IncrementalLinkFile denormalize(IO &) {
      return IncrementalLinkFile(arguments, objects, fileNames,
                                 fileHashes);
    }
    std::vector<std::string> arguments;
    std::vector<std::string> objects;

    std::vector<std::string> fileNames;
    std::vector<uint64_t> fileHashes;
  };

  static void mapping(IO &io, IncrementalLinkFile &ilf) {
    MappingNormalization<NormalizedIlf, IncrementalLinkFile> keys(io, ilf);
    io.mapRequired("linker-arguments", keys->arguments);
    io.mapRequired("object-files", keys->objects);
    io.mapRequired("file-names", keys->fileNames);
    io.mapRequired("file-hashes", keys->fileHashes);
  }
};

#endif // LLD_INCREMENTALLINKFILE_H
