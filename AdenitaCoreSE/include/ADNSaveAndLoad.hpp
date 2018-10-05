#pragma once

#include "ADNNanorobot.hpp"
#include "ADNBasicOperations.hpp"
#include "ADNNeighbors.hpp"
#include "rapidjson/document.h"
#include "rapidjson/pointer.h"
#include "rapidjson/writer.h"
#include "rapidjson/filereadstream.h"
#include "boost/algorithm/string.hpp"


namespace ADNLoader {
  // json
  ADNPointer<ADNPart> LoadPartFromJson(std::string filename);
  ADNPointer<ADNPart> LoadPartFromJsonLegacy(std::string filename);
  ADNNanorobot* LoadNanorobotFromJson(std::string filename);
  void SavePartToJson(ADNPointer<ADNPart> p, std::string filename);
  //! Writes a part to a string buffer for rapidjson
  void SavePartToJson(ADNPointer<ADNPart> p, rapidjson::Writer<StringBuffer>& s);
  void SaveNanorobotToJson(ADNNanorobot* nr, std::string filename);

  // pdb
  ADNPointer<ADNPart> LoadPartFromPDB(std::string filename, int id = -1);

  // oxdna
  void OutputToOxDNA(ADNPointer<ADNPart> part, std::string folder, ADNAuxiliary::OxDNAOptions options);
  void OutputToOxDNA(ADNNanorobot* nanorobot, std::string folder, ADNAuxiliary::OxDNAOptions options);
  void SingleStrandsToOxDNA(CollectionMap<ADNSingleStrand> singleStrands, std::ofstream& outConf, std::ofstream& outTopo, ADNAuxiliary::OxDNAOptions options);
  std::ofstream CreateOutputFile(std::string fname, std::string folder, bool sign = false);
  std::pair<bool, ADNPointer<ADNPart>> InputFromOxDNA(std::string topoFile, std::string configFile);

  // sequence list
  void OutputToCSV(CollectionMap<ADNPart> parts, std::string fname, std::string folder);

  // generic functions
  //! Populates base segments and double strands from nucleotides and single strands
  void BuildTopScales(ADNPointer<ADNPart> part);

  struct NucleotideWrap {
    ADNPointer<ADNNucleotide> nt_;
    bool paired_ = false;
    int ntId_ = -1;
    int strandId_ = -1;
  };

}