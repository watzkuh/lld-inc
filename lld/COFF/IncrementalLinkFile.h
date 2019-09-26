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

struct IncrementalLinkFile {
public:
  IncrementalLinkFile() = default;
  IncrementalLinkFile(std::vector<std::string> args,
                      std::map<std::string, uint64_t> fh, std::string of,
                      uint64_t oh)
      : arguments(std::move((args))), fileHashes(std::move(fh)), outputFile(std::move(of)),
        outputHash(oh) {}

  std::vector<std::string> arguments;
  std::vector<std::string> objects;
  std::map<std::string, uint64_t> fileHashes;
  std::string outputFile;
  uint64_t outputHash;
  constexpr static const char *fileEnding = {".ilk.yaml"};
};

extern IncrementalLinkFile *incrementalLinkFile;

} // namespace coff
} // namespace lld

using namespace llvm;
using lld::coff::IncrementalLinkFile;
using yaml::MappingTraits;

struct NormalizedFileMap {
  NormalizedFileMap() {}
  NormalizedFileMap(std::string n, uint64_t h)
      : name(std::move(n)), hashValue(h) {}
  std::string name;
  uint64_t hashValue;
};

template <> struct llvm::yaml::SequenceTraits<std::vector<NormalizedFileMap>> {
  static size_t size(IO &io, std::vector<NormalizedFileMap> &seq) {
    return seq.size();
  }
  static NormalizedFileMap &element(IO &io, std::vector<NormalizedFileMap> &seq,
                                    size_t index) {
    if (index >= seq.size())
      seq.resize(index + 1);
    return seq[index];
  }
};

template <> struct yaml::MappingTraits<NormalizedFileMap> {
  static void mapping(IO &io, NormalizedFileMap &file) {
    io.mapRequired("name", file.name);
    io.mapRequired("hash", file.hashValue);
  }
};

template <> struct MappingTraits<IncrementalLinkFile> {
  struct NormalizedIlf {
  public:
    NormalizedIlf(IO &io){};
    NormalizedIlf(IO &, IncrementalLinkFile &ilf) {
      arguments = ilf.arguments;
      objects = ilf.objects;
      outputFile = ilf.outputFile;
      outputHash = ilf.outputHash;
      for (const auto &p : ilf.fileHashes) {
        NormalizedFileMap a(p.first, p.second);
        files.push_back(a);
      }
    }

    IncrementalLinkFile denormalize(IO &) {
      std::map<std::string, uint64_t> fileHashes;
      for (auto &f : files) {
        fileHashes[f.name] = f.hashValue;
      }
      return IncrementalLinkFile(arguments, fileHashes, outputFile, outputHash);
    }

    std::vector<NormalizedFileMap> files;
    std::vector<std::string> arguments;
    std::vector<std::string> objects;
    std::string outputFile;
    uint64_t outputHash;
  };

  static void mapping(IO &io, IncrementalLinkFile &ilf) {
    MappingNormalization<NormalizedIlf, IncrementalLinkFile> keys(io, ilf);
    io.mapRequired("linker-arguments", keys->arguments);
    io.mapRequired("object-files", keys->objects);
    io.mapRequired("files", keys->files);
    io.mapRequired("output-file", keys->outputFile);
    io.mapRequired("output-hash", keys->outputHash);
  }
};

#endif // LLD_INCREMENTALLINKFILE_H
