#ifndef LLD_INCREMENTALLINKFILE_H
#define LLD_INCREMENTALLINKFILE_H

#include "llvm/Support/YAMLParser.h"
#include "llvm/Support/YAMLTraits.h"
#include "llvm/Support/raw_ostream.h"
#include <map>
#include <set>
#include <string>
#include <utility>

using namespace llvm;

namespace lld {
namespace coff {

struct IncrementalLinkFile {

  struct RelocationInfo {
    support::ulittle32_t virtualAddress;
    support::ulittle16_t type;
  };

  struct SymbolInfo {
    uint64_t definitionAddress;
    std::set<std::string> filesUsedIn;
    std::vector<RelocationInfo> relocations;
  };

  struct ChunkInfo {
    uint32_t virtualAddress;
    size_t size;
    StringMap<SymbolInfo> symbols;
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

  struct ObjectFile {
    uint64_t hash;
    std::set<std::string> dependentFiles;
    StringMap<SectionInfo> sections;
  };

public:
  IncrementalLinkFile() = default;
  IncrementalLinkFile(std::vector<std::string> args,
                      StringMap<ObjectFile> obj, std::string of,
                      uint64_t oh,
                      StringMap<OutputSectionInfo> outSections,
                      StringMap<SymbolInfo> defSyms)
      : arguments(std::move((args))), objFiles(std::move(obj)),
        outputFile(std::move(of)), outputHash(oh),
        outputSections(std::move(outSections)),
        definedSymbols(std::move(defSyms)) {}

  std::vector<std::string> arguments;
  std::set<std::string> input;
  StringMap<ObjectFile> objFiles;
  std::string outputFile;
  uint64_t outputHash;

  StringMap<OutputSectionInfo> outputSections;
  StringMap<SymbolInfo> definedSymbols;
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
  uint64_t rawAddress;
  uint64_t virtualAddress;
  size_t size;
};

template <> struct yaml::MappingTraits<NormalizedOutputSectionMap> {
  static void mapping(IO &io, NormalizedOutputSectionMap &sec) {
    io.mapRequired("name", sec.name);
    io.mapRequired("raw-address", sec.rawAddress);
    io.mapRequired("virtual-address", sec.virtualAddress);
    io.mapOptional("size", sec.size);
  }
};

template <>
struct yaml::SequenceTraits<std::vector<NormalizedOutputSectionMap>> {
  static size_t size(IO &io, std::vector<NormalizedOutputSectionMap> &seq) {
    return seq.size();
  }
  static NormalizedOutputSectionMap &
  element(IO &io, std::vector<NormalizedOutputSectionMap> &seq, size_t index) {
    if (index >= seq.size())
      seq.resize(index + 1);
    return seq[index];
  }
};

template <> struct yaml::MappingTraits<IncrementalLinkFile::RelocationInfo> {
  static void mapping(IO &io, IncrementalLinkFile::RelocationInfo &rel) {
    io.mapRequired("address", rel.virtualAddress);
    io.mapRequired("type", rel.type);
  }
};

template <>
struct yaml::SequenceTraits<
    std::vector<IncrementalLinkFile::RelocationInfo>> {
  static size_t size(IO &io,
                     std::vector<IncrementalLinkFile::RelocationInfo> &seq) {
    return seq.size();
  }
  static IncrementalLinkFile::RelocationInfo &
  element(IO &io, std::vector<IncrementalLinkFile::RelocationInfo> &seq,
          size_t index) {
    if (index >= seq.size())
      seq.resize(index + 1);
    return seq[index];
  }
};

struct NormalizedSymbolInfo {
  NormalizedSymbolInfo() {}
  NormalizedSymbolInfo(uint64_t d,
                       std::vector<IncrementalLinkFile::RelocationInfo> rels)
      : definitionAddress(d), relocations(std::move(rels)) {}
  uint64_t definitionAddress;
  std::vector<IncrementalLinkFile::RelocationInfo> relocations;
};

template <> struct yaml::MappingTraits<NormalizedSymbolInfo> {
  static void mapping(IO &io, NormalizedSymbolInfo &sym) {
    io.mapRequired("address", sym.definitionAddress);
    io.mapRequired("relocations", sym.relocations);
  }
};

struct NormalizedSymbolMap {
  NormalizedSymbolMap() {}
  NormalizedSymbolMap(std::string n, NormalizedSymbolInfo s)
      : name(std::move(n)), symInfo(std::move(s)) {}
  std::string name;
  NormalizedSymbolInfo symInfo;
};

template <>
struct yaml::SequenceTraits<std::vector<NormalizedSymbolMap>> {
  static size_t size(IO &io, std::vector<NormalizedSymbolMap> &seq) {
    return seq.size();
  }
  static NormalizedSymbolMap &
  element(IO &io, std::vector<NormalizedSymbolMap> &seq, size_t index) {
    if (index >= seq.size())
      seq.resize(index + 1);
    return seq[index];
  }
};

template <> struct yaml::MappingTraits<NormalizedSymbolMap> {
  static void mapping(IO &io, NormalizedSymbolMap &symbol) {
    io.mapRequired("name", symbol.name);
    io.mapRequired("address", symbol.symInfo.definitionAddress);
    io.mapOptional("relocations", symbol.symInfo.relocations);
  }
};

struct NormalizedChunkInfo {
  NormalizedChunkInfo() {}
  NormalizedChunkInfo(uint32_t a, size_t s,
                      std::vector<NormalizedSymbolMap> sym)
      : virtualAddress(a), size(s), symbols(std::move(sym)) {}
  uint32_t virtualAddress;
  size_t size;
  std::vector<NormalizedSymbolMap> symbols;
};

template <> struct yaml::MappingTraits<NormalizedChunkInfo> {
  static void mapping(IO &io, NormalizedChunkInfo &c) {
    io.mapRequired("address", c.virtualAddress);
    io.mapRequired("size", c.size);
    io.mapOptional("symbols", c.symbols);
  }
};

template <>
struct yaml::SequenceTraits<std::vector<NormalizedChunkInfo>> {
  static size_t size(IO &io, std::vector<NormalizedChunkInfo> &seq) {
    return seq.size();
  }
  static NormalizedChunkInfo &
  element(IO &io, std::vector<NormalizedChunkInfo> &seq, size_t index) {
    if (index >= seq.size())
      seq.resize(index + 1);
    return seq[index];
  }
};

struct NormalizedSectionMap {
  NormalizedSectionMap() {}
  NormalizedSectionMap(std::string n, uint32_t a, size_t s,
                       std::vector<NormalizedChunkInfo> c)
      : name(std::move(n)), virtualAddress(a), size(s), chunks(std::move(c)) {}
  std::string name;
  uint32_t virtualAddress;
  size_t size;
  std::vector<NormalizedChunkInfo> chunks;
};

template <>
struct yaml::SequenceTraits<std::vector<NormalizedSectionMap>> {
  static size_t size(IO &io, std::vector<NormalizedSectionMap> &seq) {
    return seq.size();
  }
  static NormalizedSectionMap &
  element(IO &io, std::vector<NormalizedSectionMap> &seq, size_t index) {
    if (index >= seq.size())
      seq.resize(index + 1);
    return seq[index];
  }
};

template <> struct yaml::MappingTraits<NormalizedSectionMap> {
  static void mapping(IO &io, NormalizedSectionMap &sec) {
    io.mapRequired("name", sec.name);
    io.mapRequired("start-address", sec.virtualAddress);
    io.mapOptional("total-size", sec.size);
    io.mapOptional("chunks", sec.chunks);
  }
};

struct NormalizedFileMap {
  NormalizedFileMap() {}
  NormalizedFileMap(std::string n, uint64_t h, std::vector<std::string> files,
                    std::vector<NormalizedSectionMap> s)
      : name(std::move(n)), hashValue(h), dependentFiles(std::move(files)),
        sections(std::move(s)) {}
  std::string name;
  uint64_t hashValue;
  std::vector<std::string> dependentFiles;
  std::vector<NormalizedSectionMap> sections;
};

template <> struct yaml::SequenceTraits<std::vector<NormalizedFileMap>> {
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
    io.mapOptional("dependent-files", file.dependentFiles);
    io.mapOptional("sections", file.sections);
  }
};

template <> struct MappingTraits<IncrementalLinkFile> {
  struct NormalizedIlf {
  public:
    NormalizedIlf(IO &io){};
    NormalizedIlf(IO &, IncrementalLinkFile &ilf) {
      arguments = ilf.arguments;
      std::vector<std::string> inputVector(ilf.input.begin(), ilf.input.end());
      input = inputVector;
      outputFile = ilf.outputFile;
      outputHash = ilf.outputHash;
      for (const auto &s : ilf.outputSections) {
        NormalizedOutputSectionMap outSection(s.getKey(), s.second.rawAddress,
                                              s.second.virtualAddress,
                                              s.second.size);
        outputSections.push_back(outSection);
      }
      for (const auto &f : ilf.objFiles) {
        std::vector<NormalizedSectionMap> sections;
        for (const auto &sec : f.second.sections) {
          std::vector<NormalizedChunkInfo> chunks;
          for (const auto &c : sec.second.chunks) {
            std::vector<NormalizedSymbolMap> symbols;
            for (const auto &sym : c.symbols) {
              NormalizedSymbolInfo symbolInfo(sym.second.definitionAddress,
                                              sym.second.relocations);
              NormalizedSymbolMap normalizedSymbolMap(sym.getKey(), symbolInfo);
              symbols.push_back(normalizedSymbolMap);
            }
            NormalizedChunkInfo chunkInfo(c.virtualAddress, c.size, symbols);
            chunks.push_back(chunkInfo);
          }
          NormalizedSectionMap sectionMap(sec.getKey(), sec.second.virtualAddress,
                                          sec.second.size, chunks);
          sections.push_back(sectionMap);
        }

        std::vector<std::string> dependentFiles;
        for (auto &dep : f.second.dependentFiles)
          dependentFiles.push_back(dep);
        NormalizedFileMap fileMap(f.getKey(), f.second.hash, dependentFiles,
                                  sections);
        files.push_back(fileMap);
      }
      for (const auto &s : ilf.definedSymbols) {
        NormalizedSymbolInfo symInfo{s.second.definitionAddress,
                                     s.second.relocations};
        NormalizedSymbolMap symMap{s.getKey(), symInfo};
        definedSymbols.push_back(symMap);
      }
    }

    IncrementalLinkFile denormalize(IO &) {
      StringMap<lld::coff::IncrementalLinkFile::ObjectFile>
          objFiles;
      StringMap<lld::coff::IncrementalLinkFile::OutputSectionInfo>
          outSections;
      for (auto &s : outputSections) {
        lld::coff::IncrementalLinkFile::OutputSectionInfo sec{
            s.rawAddress, s.virtualAddress, s.size};
        outSections[s.name] = sec;
      }
      for (auto &f : files) {
        lld::coff::IncrementalLinkFile::ObjectFile obj;
        obj.hash = f.hashValue;
        std::set<std::string> dependentFiles;
        for (auto &dep : f.dependentFiles)
          dependentFiles.insert(dep);
        obj.dependentFiles = dependentFiles;
        for (auto &sec : f.sections) {
          lld::coff::IncrementalLinkFile::SectionInfo sectionData;
          sectionData.size = sec.size;
          sectionData.virtualAddress = sec.virtualAddress;
          for (auto &c : sec.chunks) {
            StringMap<lld::coff::IncrementalLinkFile::SymbolInfo>
                symbols;
            for (auto &sym : c.symbols) {
              lld::coff::IncrementalLinkFile::SymbolInfo symbolInfo;
              symbolInfo.definitionAddress = sym.symInfo.definitionAddress;
              symbolInfo.relocations = sym.symInfo.relocations;
              symbols[sym.name] = symbolInfo;
            }
            lld::coff::IncrementalLinkFile::ChunkInfo chunkInfo{
                c.virtualAddress, c.size, symbols};
            sectionData.chunks.push_back(chunkInfo);
          }
          obj.sections[sec.name] = sectionData;
        }
        objFiles[f.name] = obj;
      }
      StringMap<lld::coff::IncrementalLinkFile::SymbolInfo> definedSymbols;
      for (auto &s : this->definedSymbols) {
        lld::coff::IncrementalLinkFile::SymbolInfo symInfo;
        symInfo.definitionAddress = s.symInfo.definitionAddress;
        definedSymbols[s.name] = symInfo;
      }

      return IncrementalLinkFile(arguments, objFiles, outputFile, outputHash,
                                 outSections, definedSymbols);
    }

    std::vector<NormalizedFileMap> files;
    std::vector<std::string> arguments;
    std::vector<std::string> input;
    std::string outputFile;
    uint64_t outputHash;
    std::vector<NormalizedOutputSectionMap> outputSections;
    std::vector<NormalizedSymbolMap> definedSymbols;
  };

  static void mapping(IO &io, IncrementalLinkFile &ilf) {
    MappingNormalization<NormalizedIlf, IncrementalLinkFile> keys(io, ilf);
    io.mapRequired("linker-arguments", keys->arguments);
    io.mapRequired("input", keys->input);
    io.mapRequired("files", keys->files);
    io.mapRequired("output-file", keys->outputFile);
    io.mapRequired("output-hash", keys->outputHash);
    io.mapRequired("output-sections", keys->outputSections);
    io.mapOptional("defined-symbols", keys->definedSymbols);
  }
};

#endif // LLD_INCREMENTALLINKFILE_H
