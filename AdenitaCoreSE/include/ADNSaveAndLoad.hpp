#pragma once

#include "ADNNanorobot.hpp"
#include "rapidjson/document.h"
#include "rapidjson/pointer.h"
#include "rapidjson/writer.h"
#include "rapidjson/filereadstream.h"


namespace ADNLoader {
  // json
  ADNPointer<ADNPart> LoadPartFromJson(std::string filename);
  ADNPointer<ADNPart> LoadPartFromJsonLegacy(std::string filename);
  void SavePartToJson(ADNPointer<ADNPart> p, std::string filename);

  // pdb
  ADNPointer<ADNPart> LoadPartFromPDB(std::string filename, int id = -1);

  // oxdna
  //  void OutputToOxDNA(std::string folder, ANTAuxiliary::OxDNAOptions options);
  //  void CreateOxDNAInputFile(std::string folder, ANTAuxiliary::OxDNAOptions options);
  //  void CreateOxDNAConfAndTopoFile(std::string folder, ANTAuxiliary::OxDNAOptions options);
  //  std::ofstream CreateOutputFile(std::string fname, std::string folder);

  // generic functions
  ADNPointer<ADNPart> GeneratePartFromAtomic();
}