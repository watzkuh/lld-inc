#ifndef LLD_INCREMENTALLINKFILE_H
#define LLD_INCREMENTALLINKFILE_H

#include "llvm/Support/YAMLParser.h"
#include "llvm/Support/YAMLTraits.h"
#include <llvm/ADT/DenseSet.h>
#include <set>
#include <string>

using namespace llvm;

namespace lld {
namespace coff {

struct IncrementalLinkFile {

  struct RelocationInfo {
    support::ulittle32_t virtualAddress;
    support::ulittle16_t type;
  };

  struct ChunkInfo {
    uint32_t virtualAddress;
    size_t size;
    StringMap<std::vector<RelocationInfo>> symbols;
  };

  struct SectionInfo {
    uint32_t virtualAddress;
    size_t size;
    std::vector<ChunkInfo> chunks;
  };

  struct OutputSectionInfo {
    uint64_t rawAddress;
    uint64_t virtualAddress;
    size_t size;
  };

  struct ObjectFileInfo {
    uint64_t modTime;
    std::set<std::string> dependentFiles;
    StringMap<SectionInfo> sections;
    StringMap<uint64_t> definedSymbols;
  };

public:
  IncrementalLinkFile() = default;
  IncrementalLinkFile(std::vector<std::string> args,
                      StringMap<ObjectFileInfo> obj, std::string of,
                      uint64_t oh, StringMap<OutputSectionInfo> outSections)
      : arguments(std::move((args))), objFiles(std::move(obj)),
        outputFile(std::move(of)), outputHash(oh),
        outputSections(std::move(outSections)) {}

  std::vector<std::string> arguments;
  DenseSet<StringRef> input;
  // input objects/archives + objects extracted from archives in input
  DenseSet<StringRef> rewritableFileNames;
  StringMap<ObjectFileInfo> objFiles;
  std::string outputFile;
  uint64_t outputHash{};

  StringMap<OutputSectionInfo> outputSections;

  bool rewritePossible = false;
  bool rewriteAborted = false;
  size_t paddedAlignment = 64;

  static void writeToDisk();
  static std::string getFileName();
};

extern IncrementalLinkFile *incrementalLinkFile;

class OutputSection;
void writeIlfSections(ArrayRef<OutputSection *> outputSections);

bool initializeIlf(ArrayRef<const char *> argsArr, std::string possibleOutput);

} // namespace coff
} // namespace lld

using namespace llvm;
using lld::coff::IncrementalLinkFile;
using yaml::MappingTraits;

struct NormalizedOutputSectionMap {
  NormalizedOutputSectionMap() {}
  NormalizedOutputSectionMap(std::string n, uint64_t raw, uint64_t rva,
                             size_t s)
      : name(std::move(n)), rawAddress(raw), virtualAddress(rva), size(s) {}
  std::string name;
  yaml::Hex64 rawAddress;
  yaml::Hex64 virtualAddress;
  size_t size;
};

LLVM_YAML_IS_SEQUENCE_VECTOR(NormalizedOutputSectionMap)

template <> struct yaml::MappingTraits<NormalizedOutputSectionMap> {
  static void mapping(IO &io, NormalizedOutputSectionMap &sec);
};

struct NormalizedRelocationInfo {
  std::string symbolName;
  support::ulittle32_t virtualAddress;
  support::ulittle16_t type;
};

LLVM_YAML_IS_SEQUENCE_VECTOR(NormalizedRelocationInfo)

template <> struct yaml::MappingTraits<NormalizedRelocationInfo> {
  static void mapping(IO &io, NormalizedRelocationInfo &rel);
};

struct NormalizedSymbolInfo {
  NormalizedSymbolInfo() {}
  NormalizedSymbolInfo(std::string n, uint64_t d)
      : name(std::move(n)), definitionAddress(d) {}
  std::string name;
  yaml::Hex64 definitionAddress;
};

LLVM_YAML_IS_SEQUENCE_VECTOR(NormalizedSymbolInfo)

template <> struct yaml::MappingTraits<NormalizedSymbolInfo> {
  static void mapping(IO &io, NormalizedSymbolInfo &sym);
};

struct NormalizedChunkInfo {
  NormalizedChunkInfo() {}
  NormalizedChunkInfo(uint32_t a, size_t s,
                      std::vector<NormalizedRelocationInfo> rels)
      : virtualAddress(a), size(s), relocations(std::move(rels)) {}
  yaml::Hex32 virtualAddress;
  size_t size;
  std::vector<NormalizedRelocationInfo> relocations;
};

LLVM_YAML_IS_SEQUENCE_VECTOR(NormalizedChunkInfo);

template <> struct yaml::MappingTraits<NormalizedChunkInfo> {
  static void mapping(IO &io, NormalizedChunkInfo &c);
};

struct NormalizedSectionMap {
  NormalizedSectionMap() {}
  NormalizedSectionMap(std::string n, uint32_t a, size_t s,
                       std::vector<NormalizedChunkInfo> c)
      : name(std::move(n)), virtualAddress(a), size(s), chunks(std::move(c)) {}
  std::string name;
  yaml::Hex32 virtualAddress;
  size_t size;
  std::vector<NormalizedChunkInfo> chunks;
};

LLVM_YAML_IS_SEQUENCE_VECTOR(NormalizedSectionMap)

template <> struct yaml::MappingTraits<NormalizedSectionMap> {
  static void mapping(IO &io, NormalizedSectionMap &sec);
};

struct NormalizedFileMap {
  NormalizedFileMap() {}
  NormalizedFileMap(std::string n, uint64_t t, std::vector<std::string> files,
                    std::vector<NormalizedSectionMap> s,
                    std::vector<NormalizedSymbolInfo> syms)
      : name(std::move(n)), modTime(t), dependentFiles(std::move(files)),
        sections(std::move(s)), definedSymbols(std::move(syms)) {}
  std::string name;
  uint64_t modTime;
  std::vector<std::string> dependentFiles;
  std::vector<NormalizedSectionMap> sections;
  std::vector<NormalizedSymbolInfo> definedSymbols;
};

LLVM_YAML_IS_SEQUENCE_VECTOR(NormalizedFileMap)

template <> struct yaml::MappingTraits<NormalizedFileMap> {
  static void mapping(IO &io, NormalizedFileMap &file);
};

template <> struct MappingTraits<IncrementalLinkFile> {
  struct NormalizedIlf {
  public:
    NormalizedIlf(IO &io){};
    NormalizedIlf(IO &, IncrementalLinkFile &ilf);

    IncrementalLinkFile denormalize(IO &);

    std::vector<NormalizedFileMap> files;
    std::vector<std::string> arguments;
    std::vector<std::string> input;
    std::string outputFile;
    uint64_t outputHash;
    std::vector<NormalizedOutputSectionMap> outputSections;
  };

  static void mapping(IO &io, IncrementalLinkFile &ilf);
};

#endif // LLD_INCREMENTALLINKFILE_H
