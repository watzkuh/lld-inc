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

//TODO: Restructure this

struct IncrementalLinkFile {

  struct SectionData {
    std::string name;
    uint32_t virtualAddress;
    uint32_t rawAddress;
    size_t size;
  };

  struct ObjectFile {
    uint64_t hash;
    SectionData sectionData;
  };


public:
  IncrementalLinkFile() = default;
  IncrementalLinkFile(std::vector<std::string> args,
                      std::map<std::string, ObjectFile>  obj, std::string of,
                      uint64_t oh)
      : arguments(std::move((args))), objFiles(std::move(obj)), outputFile(std::move(of)),
        outputHash(oh) {}

  std::vector<std::string> arguments;
  std::vector<std::string> input;
  std::map<std::string, ObjectFile> objFiles;
  std::string outputFile;
  uint64_t outputHash;
  bool rewritePossible = false;

  constexpr static const char *fileEnding = {".ilk.yaml"};
};

extern IncrementalLinkFile *incrementalLinkFile;

class OutputSection;
void writeIlfSectionData(llvm::ArrayRef<OutputSection *> outputSections);

bool initializeIlf(ArrayRef<const char *> argsArr);

} // namespace coff
} // namespace lld

using namespace llvm;
using lld::coff::IncrementalLinkFile;
using yaml::MappingTraits;

struct NormalizedFileMap {
  NormalizedFileMap() {}
  NormalizedFileMap(std::string n, uint64_t h, lld::coff::IncrementalLinkFile::SectionData o)
      : name(std::move(n)), hashValue(h), sectionData(o)  {}
  std::string name;
  uint64_t hashValue;
  lld::coff::IncrementalLinkFile::SectionData sectionData;
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
    io.mapOptional("section-name", file.sectionData.name);
    io.mapOptional("virtual-address", file.sectionData.virtualAddress);
    io.mapOptional("raw-address", file.sectionData.rawAddress);
    io.mapOptional("size", file.sectionData.size);
  }
};

template <> struct MappingTraits<IncrementalLinkFile> {
  struct NormalizedIlf {
  public:
    NormalizedIlf(IO &io){};
    NormalizedIlf(IO &, IncrementalLinkFile &ilf) {
      arguments = ilf.arguments;
      input = ilf.input;
      outputFile = ilf.outputFile;
      outputHash = ilf.outputHash;
      for (const auto &p : ilf.objFiles) {
        NormalizedFileMap a(p.first, p.second.hash, p.second.sectionData);
        files.push_back(a);
      }
    }

    IncrementalLinkFile denormalize(IO &) {
      std::map<std::string, lld::coff::IncrementalLinkFile::ObjectFile> objFiles;
      for (auto &f : files) {
        lld::coff::IncrementalLinkFile::ObjectFile obj;
        obj.hash = f.hashValue;
        obj.sectionData = f.sectionData;
        objFiles[f.name] = obj;
      }
      return IncrementalLinkFile(arguments, objFiles, outputFile, outputHash);
    }

    std::vector<NormalizedFileMap> files;
    std::vector<std::string> arguments;
    std::vector<std::string> input;
    std::string outputFile;
    uint64_t outputHash;
  };

  static void mapping(IO &io, IncrementalLinkFile &ilf) {
    MappingNormalization<NormalizedIlf, IncrementalLinkFile> keys(io, ilf);
    io.mapRequired("linker-arguments", keys->arguments);
    io.mapRequired("input", keys->input);
    io.mapRequired("files", keys->files);
    io.mapRequired("output-file", keys->outputFile);
    io.mapRequired("output-hash", keys->outputHash);
  }
};

#endif // LLD_INCREMENTALLINKFILE_H
