#ifndef LLD_INCREMENTALLINKFILE_H
#define LLD_INCREMENTALLINKFILE_H

#include "llvm/Support/YAMLParser.h"
#include "llvm/Support/YAMLTraits.h"
#include "llvm/Support/raw_ostream.h"
#include <llvm/ADT/DenseSet.h>
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
  uint64_t outputHash;

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
  static void mapping(IO &io, NormalizedOutputSectionMap &sec) {
    io.mapRequired("name", sec.name);
    io.mapRequired("raw-address", sec.rawAddress);
    io.mapRequired("virtual-address", sec.virtualAddress);
    io.mapOptional("size", sec.size);
  }
};

struct NormalizedRelocationInfo {
  std::string symbolName;
  support::ulittle32_t virtualAddress;
  support::ulittle16_t type;
};

LLVM_YAML_IS_SEQUENCE_VECTOR(NormalizedRelocationInfo)

template <> struct yaml::MappingTraits<NormalizedRelocationInfo> {
  static void mapping(IO &io, NormalizedRelocationInfo &rel) {
    io.mapRequired("symbol-name", rel.symbolName);
    io.mapRequired("address", rel.virtualAddress);
    io.mapRequired("type", rel.type);
  }
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
  static void mapping(IO &io, NormalizedSymbolInfo &sym) {
    io.mapRequired("name", sym.name);
    io.mapRequired("address", sym.definitionAddress);
  }
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
  static void mapping(IO &io, NormalizedChunkInfo &c) {
    io.mapRequired("address", c.virtualAddress);
    io.mapRequired("size", c.size);
    io.mapOptional("relocations", c.relocations);
  }
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
  static void mapping(IO &io, NormalizedSectionMap &sec) {
    io.mapRequired("name", sec.name);
    io.mapRequired("start-address", sec.virtualAddress);
    io.mapOptional("total-size", sec.size);
    io.mapOptional("chunks", sec.chunks);
  }
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
  static void mapping(IO &io, NormalizedFileMap &file) {
    io.mapRequired("name", file.name);
    io.mapRequired("last-modified", file.modTime);
    io.mapOptional("dependent-files", file.dependentFiles);
    io.mapOptional("sections", file.sections);
    io.mapOptional("defined-symbols", file.definedSymbols);
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
            std::vector<NormalizedRelocationInfo> symbols;
            for (const auto &sym : c.symbols) {
              for (const auto &rel : sym.second) {
                NormalizedRelocationInfo relocationInfo{
                    sym.first(), rel.virtualAddress, rel.type};
                symbols.push_back(relocationInfo);
              }
            }
            NormalizedChunkInfo chunkInfo(c.virtualAddress, c.size, symbols);
            chunks.push_back(chunkInfo);
          }
          NormalizedSectionMap sectionMap(
              sec.getKey(), sec.second.virtualAddress, sec.second.size, chunks);
          sections.push_back(sectionMap);
        }

        std::vector<std::string> dependentFiles;
        for (auto &dep : f.second.dependentFiles)
          dependentFiles.push_back(dep);

        std::vector<NormalizedSymbolInfo> definedSymbols;
        for (auto &s : f.second.definedSymbols) {
          NormalizedSymbolInfo symInfo{s.first(), s.second};
          definedSymbols.push_back(symInfo);
        }
        NormalizedFileMap fileMap(f.getKey(), f.second.modTime, dependentFiles,
                                  sections, definedSymbols);
        files.push_back(fileMap);
      }
    }

    IncrementalLinkFile denormalize(IO &) {
      StringMap<lld::coff::IncrementalLinkFile::ObjectFileInfo> objFiles;
      StringMap<lld::coff::IncrementalLinkFile::OutputSectionInfo> outSections;
      for (auto &s : outputSections) {
        lld::coff::IncrementalLinkFile::OutputSectionInfo sec{
            s.rawAddress, s.virtualAddress, s.size};
        outSections[s.name] = sec;
      }
      for (auto &f : files) {
        lld::coff::IncrementalLinkFile::ObjectFileInfo obj;
        obj.modTime = f.modTime;
        std::set<std::string> dependentFiles;
        for (auto &dep : f.dependentFiles)
          dependentFiles.insert(dep);
        obj.dependentFiles = dependentFiles;
        for (auto &sec : f.sections) {
          lld::coff::IncrementalLinkFile::SectionInfo sectionData;
          sectionData.size = sec.size;
          sectionData.virtualAddress = sec.virtualAddress;
          for (auto &c : sec.chunks) {
            StringMap<std::vector<IncrementalLinkFile::RelocationInfo>> symbols;
            for (auto &rel : c.relocations) {
              IncrementalLinkFile::RelocationInfo relInfo{rel.virtualAddress,
                                                          rel.type};
              symbols[rel.symbolName].push_back(relInfo);
            }
            lld::coff::IncrementalLinkFile::ChunkInfo chunkInfo{
                c.virtualAddress, c.size, symbols};
            sectionData.chunks.push_back(chunkInfo);
          }
          obj.sections[sec.name] = sectionData;
        }
        for (auto &s : f.definedSymbols) {
          obj.definedSymbols[s.name] = s.definitionAddress;
        }
        objFiles[f.name] = obj;
      }

      return IncrementalLinkFile(arguments, objFiles, outputFile, outputHash,
                                 outSections);
    }

    std::vector<NormalizedFileMap> files;
    std::vector<std::string> arguments;
    std::vector<std::string> input;
    std::string outputFile;
    uint64_t outputHash;
    std::vector<NormalizedOutputSectionMap> outputSections;
  };

  static void mapping(IO &io, IncrementalLinkFile &ilf) {
    MappingNormalization<NormalizedIlf, IncrementalLinkFile> keys(io, ilf);
    io.mapRequired("linker-arguments", keys->arguments);
    io.mapRequired("input", keys->input);
    io.mapRequired("files", keys->files);
    io.mapRequired("output-file", keys->outputFile);
    io.mapRequired("output-hash", keys->outputHash);
    io.mapRequired("output-sections", keys->outputSections);
  }
};

#endif // LLD_INCREMENTALLINKFILE_H
