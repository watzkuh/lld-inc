#ifndef LLD_INCREMENTALLINKFILE_H
#define LLD_INCREMENTALLINKFILE_H

#include "llvm/Support/YAMLParser.h"
#include "llvm/Support/YAMLTraits.h"
#include "llvm/Support/raw_ostream.h"
#include <string>

using namespace llvm;
using yaml::MappingTraits;
using yaml::Output;

class IncrementalLinkFile {
public:
  std::vector<llvm::StringRef> arguments;
  std::vector<llvm::StringRef> objects;
  constexpr static const char *fileEnding = {"-ilk.yaml"};
};

template <> struct yaml::MappingTraits<IncrementalLinkFile> {
  static void mapping(IO &io, IncrementalLinkFile &info) {
    io.mapRequired("linker-arguments", info.arguments);
    io.mapRequired("object-files", info.objects);
  }
};

extern IncrementalLinkFile *incrementalLinkFile;

#endif // LLD_INCREMENTALLINKFILE_H
