#include "IncrementalLinkFile.h"

void yaml::MappingTraits<NormalizedOutputSectionMap>::mapping(
    yaml::IO &io, NormalizedOutputSectionMap &sec) {
  io.mapRequired("name", sec.name);
  io.mapRequired("raw-address", sec.rawAddress);
  io.mapRequired("virtual-address", sec.virtualAddress);
  io.mapOptional("size", sec.size);
}

void yaml::MappingTraits<NormalizedSymbolInfo>::mapping(
    yaml::IO &io, NormalizedSymbolInfo &sym) {
  io.mapRequired("name", sym.name);
  io.mapRequired("address", sym.definitionAddress);
}

void yaml::MappingTraits<NormalizedChunkInfo>::mapping(yaml::IO &io,
                                                       NormalizedChunkInfo &c) {
  io.mapRequired("address", c.virtualAddress);
  io.mapRequired("size", c.size);
}

void yaml::MappingTraits<NormalizedMergeInfo>::mapping(IO &io,
                                                       NormalizedMergeInfo &m) {
  io.mapRequired("from", m.from);
  io.mapRequired("to", m.to);
}

void yaml::MappingTraits<NormalizedSectionMap>::mapping(
    yaml::IO &io, NormalizedSectionMap &sec) {
  io.mapRequired("name", sec.name);
  io.mapRequired("start-address", sec.virtualAddress);
  io.mapOptional("total-size", sec.size);
  io.mapOptional("chunks", sec.chunks);
}

void yaml::MappingTraits<NormalizedFileMap>::mapping(yaml::IO &io,
                                                     NormalizedFileMap &file) {
  io.mapRequired("name", file.name);
  io.mapRequired("last-modified", file.modTime);
  io.mapRequired("position", file.pos);
  io.mapOptional("dependent-files", file.dependentFiles);
  io.mapOptional("sections", file.sections);
  io.mapOptional("defined-symbols", file.definedSymbols);
}

void MappingTraits<IncrementalLinkFile>::mapping(IO &io,
                                                 IncrementalLinkFile &ilf) {
  MappingNormalization<NormalizedIlf, IncrementalLinkFile> keys(io, ilf);
  io.mapRequired("linker-arguments", keys->arguments);
  io.mapRequired("files", keys->files);
  io.mapRequired("output-file", keys->outputFile);
  io.mapRequired("output-hash", keys->outputHash);
  io.mapRequired("output-sections", keys->outputSections);
  io.mapRequired("merged-sections", keys->mergedSections);
}

MappingTraits<IncrementalLinkFile>::NormalizedIlf::NormalizedIlf(
    IO &, IncrementalLinkFile &ilf) {

  arguments = ilf.arguments;
  outputFile = ilf.outputFile;
  outputHash = ilf.outputHash;
  for (const auto &s : ilf.outputSections) {
    NormalizedOutputSectionMap outSection(
        s.first, s.second.rawAddress, s.second.virtualAddress, s.second.size);
    outputSections.push_back(outSection);
  }
  for (const auto &m : ilf.mergedSections) {
    NormalizedMergeInfo mergeInfo{m.first, m.second};
    mergedSections.push_back(mergeInfo);
  }
  for (const auto &f : ilf.objFiles) {
    std::vector<NormalizedSectionMap> sections;
    for (const auto &sec : f.second.sections) {
      std::vector<NormalizedChunkInfo> chunks;
      for (const auto &c : sec.second.chunks) {
        NormalizedChunkInfo chunkInfo(c.virtualAddress, c.size);
        chunks.push_back(chunkInfo);
      }
      NormalizedSectionMap sectionMap(sec.first, sec.second.virtualAddress,
                                      sec.second.size, chunks);
      sections.push_back(sectionMap);
    }

    std::vector<std::string> dependentFiles;
    for (const auto &dep : f.second.dependentFiles)
      dependentFiles.push_back(dep);

    std::vector<NormalizedSymbolInfo> definedSymbols;
    for (const auto &s : f.second.definedSymbols) {
      NormalizedSymbolInfo symInfo{s.first, s.second};
      definedSymbols.push_back(symInfo);
    }
    NormalizedFileMap fileMap(f.first, f.second.modTime, f.second.position,
                              dependentFiles, sections, mergedSections,
                              definedSymbols);
    files.push_back(fileMap);
  }
}

IncrementalLinkFile
MappingTraits<IncrementalLinkFile>::NormalizedIlf::denormalize(IO &) {
  std::unordered_map<std::string, IncrementalLinkFile::ObjectFileInfo> objFiles;
  std::unordered_map<std::string, IncrementalLinkFile::OutputSectionInfo>
      outSections;
  std::unordered_map<std::string, std::string> merged;
  for (auto &s : outputSections) {
    lld::coff::IncrementalLinkFile::OutputSectionInfo sec{
        s.rawAddress, s.virtualAddress, s.size};
    outSections[s.name] = sec;
  }
  for (auto &m : mergedSections) {
    merged[m.from] = m.to;
  }
  for (auto &f : files) {
    lld::coff::IncrementalLinkFile::ObjectFileInfo obj;
    obj.modTime = f.modTime;
    obj.position = f.pos;
    std::set<std::string> dependentFiles;
    for (auto &dep : f.dependentFiles)
      dependentFiles.insert(dep);
    obj.dependentFiles = dependentFiles;
    for (auto &sec : f.sections) {
      lld::coff::IncrementalLinkFile::SectionInfo sectionData;
      sectionData.size = sec.size;
      sectionData.virtualAddress = sec.virtualAddress;
      for (auto &c : sec.chunks) {
        lld::coff::IncrementalLinkFile::ChunkInfo chunkInfo{c.virtualAddress,
                                                            c.size};
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
                             outSections, merged);
}
