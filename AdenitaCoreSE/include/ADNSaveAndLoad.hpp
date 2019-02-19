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

  // samson
  ADNPointer<ADNPart> GenerateModelFromDatagraph(SBNode* sn);

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

  template <typename T>
  struct Wrap {
    ADNPointer<T> elem_;
    int id_ = -1;
    int strandId_ = -1;
  };

  using NucleotideWrap = Wrap<ADNNucleotide>;

  template <class T>
  class ElementMap {
  public:
    std::pair<bool, ADNPointer<T>> Get(int idx) {
      ADNPointer<T> nt = nullptr;
      bool success = false;
      if (ids_.find(idx) != ids_.end()) {
        nt = ids_.at(idx).elem_;
        success = true;
      }
      return std::make_pair(success, nt);
    };

    int GetIndex(ADNPointer<T> nt, int sId = -1) {
      int idx = -1;
      if (pointers_.find(nt()) != pointers_.end()) {
        Wrap<T> w = pointers_.at(nt());
        idx = w.id_;
      }
      else {
        idx = Insert(nt, sId);
      }
      return idx;
    };

    int Store(ADNPointer<T> elem, int id = -1, int sId = -1) {
      return Insert(elem, sId, id);
    }

  private:
    int Insert(ADNPointer<T> nt, int sId, int id = -1) {
      auto key = GetNextKey();
      if (id != -1) key = id;
      Wrap<T> w;
      w.elem_ = nt;
      w.id_ = key;
      w.strandId_ = sId;
      ids_[key] = w;
      pointers_.insert(std::make_pair(nt(), w));
      return key;
    };

    int GetNextKey() {
      int lkey = 0;
      if (!ids_.empty()) lkey = ids_.rbegin()->first + 1;
      return lkey;
    };

    std::map<int, Wrap<T>> ids_;
    std::map<T*, Wrap<T>> pointers_;

  };
}