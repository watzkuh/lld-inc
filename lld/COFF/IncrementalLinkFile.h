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

  struct ChunkInfo {
    uint32_t virtualAddress;
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
    std::map<std::string, SectionInfo> sections;
  };

public:
  IncrementalLinkFile() = default;
  IncrementalLinkFile(std::vector<std::string> args,
                      std::map<std::string, ObjectFile> obj, std::string of,
                      uint64_t oh,
                      std::map<std::string, OutputSectionInfo> outSections,
                      std::map<std::string, uint64_t> defSyms)
      : arguments(std::move((args))), objFiles(std::move(obj)),
        outputFile(std::move(of)), outputHash(oh),
        outputSections(std::move(outSections)),
        definedSymbols(std::move(defSyms)) {}

  std::vector<std::string> arguments;
  std::vector<std::string> input;
  std::map<std::string, ObjectFile> objFiles;
  std::string outputFile;
  uint64_t outputHash;

  std::map<std::string, OutputSectionInfo> outputSections;
  std::map<std::string, uint64_t> definedSymbols;
  bool rewritePossible = false;

  static void writeToDisk();
  static std::string getFileName();
};

extern IncrementalLinkFile *incrementalLinkFile;

class OutputSection;
void writeIlfSections(llvm::ArrayRef<OutputSection *> outputSections);

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
struct llvm::yaml::SequenceTraits<std::vector<NormalizedOutputSectionMap>> {
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

struct NormalizedSectionMap {
  NormalizedSectionMap() {}
  NormalizedSectionMap(std::string n, uint32_t a, size_t s,
                       std::vector<lld::coff::IncrementalLinkFile::ChunkInfo> c)
      : name(std::move(n)), virtualAddress(a), size(s), chunks(std::move(c)) {}
  std::string name;
  uint32_t virtualAddress;
  size_t size;
  std::vector<lld::coff::IncrementalLinkFile::ChunkInfo> chunks;
};

template <>
struct llvm::yaml::SequenceTraits<std::vector<NormalizedSectionMap>> {
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

template <>
struct llvm::yaml::SequenceTraits<
    std::vector<lld::coff::IncrementalLinkFile::ChunkInfo>> {
  static size_t
  size(IO &io, std::vector<lld::coff::IncrementalLinkFile::ChunkInfo> &seq) {
    return seq.size();
  }
  static lld::coff::IncrementalLinkFile::ChunkInfo &
  element(IO &io, std::vector<lld::coff::IncrementalLinkFile::ChunkInfo> &seq,
          size_t index) {
    if (index >= seq.size())
      seq.resize(index + 1);
    return seq[index];
  }
};

template <>
struct yaml::MappingTraits<lld::coff::IncrementalLinkFile::ChunkInfo> {
  static void mapping(IO &io, lld::coff::IncrementalLinkFile::ChunkInfo &c) {
    io.mapRequired("address", c.virtualAddress);
  }
};

template <> struct yaml::MappingTraits<NormalizedSectionMap> {
  static void mapping(IO &io, NormalizedSectionMap &file) {
    io.mapRequired("name", file.name);
    io.mapRequired("start-address", file.virtualAddress);
    io.mapOptional("size", file.size);
    io.mapOptional("chunks", file.chunks);
  }
};

struct NormalizedFileMap {
  NormalizedFileMap() {}
  NormalizedFileMap(std::string n, uint64_t h,
                    std::vector<NormalizedSectionMap> s)
      : name(std::move(n)), hashValue(h), sections(std::move(s)) {}
  std::string name;
  uint64_t hashValue;
  std::vector<NormalizedSectionMap> sections;
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
    io.mapOptional("sections", file.sections);
  }
};

struct NormalizedSymbolMap {
  NormalizedSymbolMap() {}
  NormalizedSymbolMap(std::string n, uint64_t a)
      : name(std::move(n)), relativeAddress(a) {}
  std::string name;
  uint64_t relativeAddress;
};

template <> struct yaml::MappingTraits<NormalizedSymbolMap> {
  static void mapping(IO &io, NormalizedSymbolMap &symbol) {
    io.mapRequired("name", symbol.name);
    io.mapRequired("address", symbol.relativeAddress);
  }
};

template <>
struct llvm::yaml::SequenceTraits<std::vector<NormalizedSymbolMap>> {
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

template <> struct MappingTraits<IncrementalLinkFile> {
  struct NormalizedIlf {
  public:
    NormalizedIlf(IO &io){};
    NormalizedIlf(IO &, IncrementalLinkFile &ilf) {
      arguments = ilf.arguments;
      input = ilf.input;
      outputFile = ilf.outputFile;
      outputHash = ilf.outputHash;
      for (const auto &s : ilf.outputSections) {
        NormalizedOutputSectionMap outSection(s.first, s.second.rawAddress,
                                              s.second.virtualAddress,
                                              s.second.size);
        outputSections.push_back(outSection);
      }
      for (const auto &f : ilf.objFiles) {
        std::vector<NormalizedSectionMap> sections;
        for (const auto &s : f.second.sections) {
          NormalizedSectionMap sectionMap(s.first, s.second.virtualAddress,
                                          s.second.size, s.second.chunks);
          sections.push_back(sectionMap);
        }
        NormalizedFileMap fileMap(f.first, f.second.hash, sections);
        files.push_back(fileMap);
      }
      for (const auto &s : ilf.definedSymbols) {
        NormalizedSymbolMap symMap(s.first, s.second);
        definedSymbols.push_back(symMap);
      }
    }

    IncrementalLinkFile denormalize(IO &) {
      std::map<std::string, lld::coff::IncrementalLinkFile::ObjectFile>
          objFiles;
      std::map<std::string, lld::coff::IncrementalLinkFile::OutputSectionInfo>
          outSections;
      for (auto &s : outputSections) {
        lld::coff::IncrementalLinkFile::OutputSectionInfo sec{
            s.rawAddress, s.virtualAddress, s.size};
        outSections[s.name] = sec;
      }
      for (auto &f : files) {
        lld::coff::IncrementalLinkFile::ObjectFile obj;
        obj.hash = f.hashValue;
        for (auto &s : f.sections) {
          lld::coff::IncrementalLinkFile::SectionInfo sectionData;
          sectionData.size = s.size;
          sectionData.virtualAddress = s.virtualAddress;
          sectionData.chunks = s.chunks;
          obj.sections[s.name] = sectionData;
        }
        objFiles[f.name] = obj;
      }
      std::map<std::string, uint64_t> definedSymbols;
      for (auto &s : this->definedSymbols) {
        definedSymbols[s.name] = s.relativeAddress;
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
