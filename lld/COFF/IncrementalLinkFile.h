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

  // TODO: Something like SectionInfo would probably be a more fitting name
  struct SectionData {
    uint32_t virtualAddress;
    size_t size;
  };

  struct ObjectFile {
    uint64_t hash;
    std::map<std::string, SectionData> sections;
  };

public:
  IncrementalLinkFile() = default;
  IncrementalLinkFile(std::vector<std::string> args,
                      std::map<std::string, ObjectFile> obj, std::string of,
                      uint64_t oh, uint32_t outDataRaw, uint32_t outDataRVA,
                      uint32_t outTextRaw, uint32_t outTextRVA)
      : arguments(std::move((args))), objFiles(std::move(obj)),
        outputFile(std::move(of)), outputHash(oh),
        outputDataSectionRaw(outDataRaw), outputDataSectionRVA(outDataRVA),
        outputTextSectionRaw(outTextRaw), outputTextSectionRVA(outTextRVA) {}

  std::vector<std::string> arguments;
  std::vector<std::string> input;
  std::map<std::string, ObjectFile> objFiles;
  std::string outputFile;
  uint64_t outputHash;

  // TODO: Restructure this; Probably move it to SectionData
  uint32_t outputDataSectionRaw;
  uint32_t outputDataSectionRVA;
  uint32_t outputTextSectionRaw;
  uint32_t outputTextSectionRVA;
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

struct NormalizedSectionMap {
  NormalizedSectionMap() {}
  NormalizedSectionMap(std::string n, uint32_t a, size_t s)
      : name(std::move(n)), virtualAddress(a), size(s) {}
  std::string name;
  uint32_t virtualAddress;
  size_t size;
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

template <> struct yaml::MappingTraits<NormalizedSectionMap> {
  static void mapping(IO &io, NormalizedSectionMap &file) {
    io.mapRequired("name", file.name);
    io.mapRequired("virtual-address", file.virtualAddress);
    io.mapOptional("size", file.size);
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

template <> struct MappingTraits<IncrementalLinkFile> {
  struct NormalizedIlf {
  public:
    NormalizedIlf(IO &io){};
    NormalizedIlf(IO &, IncrementalLinkFile &ilf) {
      arguments = ilf.arguments;
      input = ilf.input;
      outputFile = ilf.outputFile;
      outputHash = ilf.outputHash;
      outputDataSectionRaw = ilf.outputDataSectionRaw;
      outputDataSectionRVA = ilf.outputDataSectionRVA;
      outputTextSectionRaw = ilf.outputTextSectionRaw;
      outputTextSectionRVA = ilf.outputTextSectionRVA;
      for (const auto &f : ilf.objFiles) {
        std::vector<NormalizedSectionMap> sections;
        for (const auto &s : f.second.sections) {
          NormalizedSectionMap sectionMap(s.first, s.second.virtualAddress,
                                          s.second.size);
          sections.push_back(sectionMap);
        }
        NormalizedFileMap fileMap(f.first, f.second.hash, sections);
        files.push_back(fileMap);
      }
    }

    IncrementalLinkFile denormalize(IO &) {
      std::map<std::string, lld::coff::IncrementalLinkFile::ObjectFile>
          objFiles;
      for (auto &f : files) {
        lld::coff::IncrementalLinkFile::ObjectFile obj;
        obj.hash = f.hashValue;
        for (auto &s : f.sections) {
          lld::coff::IncrementalLinkFile::SectionData sectionData;
          sectionData.size = s.size;
          sectionData.virtualAddress = s.virtualAddress;
          obj.sections[s.name] = sectionData;
        }
        objFiles[f.name] = obj;
      }
      return IncrementalLinkFile(arguments, objFiles, outputFile, outputHash,
                                 outputDataSectionRaw, outputDataSectionRVA,
                                 outputTextSectionRaw, outputTextSectionRVA);
    }

    std::vector<NormalizedFileMap> files;
    std::vector<std::string> arguments;
    std::vector<std::string> input;
    std::string outputFile;
    uint64_t outputHash;
    uint32_t outputDataSectionRaw;
    uint32_t outputDataSectionRVA;
    uint32_t outputTextSectionRaw;
    uint32_t outputTextSectionRVA;
  };

  static void mapping(IO &io, IncrementalLinkFile &ilf) {
    MappingNormalization<NormalizedIlf, IncrementalLinkFile> keys(io, ilf);
    io.mapRequired("linker-arguments", keys->arguments);
    io.mapRequired("input", keys->input);
    io.mapRequired("files", keys->files);
    io.mapRequired("output-file", keys->outputFile);
    io.mapRequired("output-hash", keys->outputHash);
    io.mapRequired("output-data-section-raw", keys->outputDataSectionRaw);
    io.mapRequired("output-data-section-rva", keys->outputDataSectionRVA);
    io.mapRequired("output-text-section-raw", keys->outputTextSectionRaw);
    io.mapRequired("output-text-section-rva", keys->outputTextSectionRVA);
  }
};

#endif // LLD_INCREMENTALLINKFILE_H
