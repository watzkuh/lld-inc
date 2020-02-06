#ifndef LLD_INCREMENTALLINKFILE_H
#define LLD_INCREMENTALLINKFILE_H

#include "llvm/Support/YAMLParser.h"
#include "llvm/Support/YAMLTraits.h"
#include <llvm/ADT/DenseSet.h>
#include <set>
#include <string>
#include <utility>

using namespace llvm;

namespace lld {
namespace coff {

class IncrementalLinkFile {
public:
  struct RelocationInfo {
    uint32_t offset;
    uint16_t type;
    template <class Archive> void serialize(Archive &archive) {
      archive(offset, type);
    }
  };

  struct ChunkInfo {
    uint32_t virtualAddress;
    size_t size;
    std::map<std::string, std::vector<RelocationInfo>> symbols;
    template <class Archive> void serialize(Archive &archive) {
      archive(virtualAddress, size, symbols);
    }
  };

  struct SectionInfo {
    uint32_t virtualAddress;
    size_t size;
    std::vector<ChunkInfo> chunks;
    template <class Archive> void serialize(Archive &archive) {
      archive(virtualAddress, size, chunks);
    }
  };

  struct OutputSectionInfo {
    uint64_t rawAddress;
    uint64_t virtualAddress;
    size_t size;
    template <class Archive> void serialize(Archive &archive) {
      archive(rawAddress, virtualAddress, size);
    }
  };

  struct ObjectFileInfo {
    uint64_t modTime;
    uint64_t position;
    std::set<std::string> dependentFiles;
    std::map<std::string, SectionInfo> sections;
    std::map<std::string, uint64_t> definedSymbols;
    template <class Archive> void serialize(Archive &archive) {
      archive(modTime, position, dependentFiles, sections, definedSymbols);
    }
  };

  IncrementalLinkFile() = default;
  IncrementalLinkFile(std::vector<std::string> args,
                      std::map<std::string, ObjectFileInfo> obj, std::string of,
                      uint64_t oh,
                      std::map<std::string, OutputSectionInfo> outSections,
                      StringMap<uint64_t> syms)
      : arguments(std::move((args))), objFiles(std::move(obj)),
        outputFile(std::move(of)), outputHash(oh),
        outputSections(std::move(outSections)), globalSymbols(std::move(syms)) {
  }

  std::vector<std::string> arguments;
  DenseSet<StringRef> input;
  // input objects/archives + objects extracted from archives in input
  DenseSet<StringRef> rewritableFileNames;
  std::map<std::string, ObjectFileInfo> objFiles;
  std::string outputFile;
  uint64_t outputHash{};

  std::map<std::string, OutputSectionInfo> outputSections;
  StringMap<uint64_t> globalSymbols;

  bool rewritePossible = false;
  bool rewriteAborted = false;
  size_t paddedAlignment = 64;
  uint64_t fileIndex = 0;

  static void writeToDisk();
  static std::string getFileName();

  template <class Archive> void serialize(Archive &archive) {
    archive(arguments, outputFile, outputHash, outputSections, objFiles);
  }
};

extern std::unique_ptr<IncrementalLinkFile> incrementalLinkFile;

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
  uint32_t virtualAddress;
  uint16_t type;
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
  NormalizedFileMap(std::string n, uint64_t t, uint64_t i,
                    std::vector<std::string> files,
                    std::vector<NormalizedSectionMap> s,
                    std::vector<NormalizedSymbolInfo> syms)
      : name(std::move(n)), modTime(t), pos(i),
        dependentFiles(std::move(files)), sections(std::move(s)),
        definedSymbols(std::move(syms)) {}
  std::string name;
  uint64_t modTime;
  uint64_t pos;
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
