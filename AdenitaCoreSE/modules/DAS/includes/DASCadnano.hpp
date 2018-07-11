#pragma once

#include "rapidjson/reader.h"
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/pointer.h"
#include "rapidjson/filereadstream.h"
#include <iostream>
#include <boost/numeric/ublas/matrix.hpp>
#include <string>
#include "ADNNanorobot.hpp"
#include "ADNAuxiliary.hpp"
#include "ADNConstants.hpp"


using namespace std;
using namespace rapidjson;

double const bp_rise_ = ADNConstants::BP_RISE; // nm
double const bp_rot_ = ADNConstants::BP_ROT; // rotation in degrees of nts around Z axis
double const dh_diameter_ = ADNConstants::DH_DIAMETER; // nm
double const bp_cadnano_dist_ = ADNConstants::BP_CADNANO_DIST; //0.25 * dh_diameter_; //nm

using LatticeType = ADNConstants::CadnanoLatticeType;

namespace ublas = boost::numeric::ublas;

//JSON Parsing
struct vec2 {
  int n0;
  int n1;
};

struct vec4 {
  int n0;
  int n1;
  int n2;
  int n3;
};

using Vec4List = std::map<int, vec4>;

struct Vstrand2 {
  int totalLength_;  // total length, including positions without nothing
  int num_;  // identification
  Vec4List scaf_;  // key is position that scaffold base occupies in the vhelix
  Vec4List stap_;
  int col_;  // row and column
  int row_;
  std::map<int, int> loops_;
  std::map<int, int> skips_;
};

struct CadnanoJSONFile2 {
  string name_;
  std::map<int, Vstrand2> vstrands_;  // key is vStrand num
  LatticeType lType_;
  std::pair<int, int> scaffoldStartPosition_;  // first is vhelix num, second is position  within it
};

//Grid from caDNAno
struct LatticeCell {
  double x_;
  double y_;
};

struct Lattice {
  ublas::matrix<LatticeCell> mat_;

  void CreateSquareLattice();
  void CreateHoneycombLattice();

  LatticeCell GetLatticeCell(unsigned int row, unsigned int column);

  size_t GetNumberRows();
  size_t GetNumberCols();
};


struct Vstrand {
  //order is the same as in the cadnano 2.0 json format
  std::vector<vec2> stap_colors_;
  int num_;
  int scafLoop_; // todo
  std::vector<vec4> stap_; //size is num of basepairs per vstrand 
  std::vector<int> skip_;
  std::vector<vec4> scaf_;
  int stapLoop_; //todo
  int col_;
  std::vector<int> loop_;
  int row_;
};

struct CadnanoJSONFile {
  string name_;
  std::vector<Vstrand> vstrands_;
  std::map<int, int> numToIdMap_; //id != num in vstrand. the mapping is saved to speed up the lookup
};

struct VTube {
  int vStrandId_;
  int initPos_;
  int endPos_;
};

struct VGrid2 {
  std::vector<VTube> vDoubleStrands_; // = vstrands
  Lattice lattice_;

  void CreateLattice(LatticeType lType);

  void AddTube(VTube tube);

  SBPosition3 GetGridCellPos3D(int vStrandId, int z, unsigned int row, unsigned int column);
};


struct VCellPos {
  double h_;
  double z_;
};

struct VCell { //represents one base
  int id_;
  int vstrandNum_;
  int vstrandPos_;
  VCellPos pos_;
};

struct VBasePair {
  VCell scaffoldCell;
  VCell stapleCell;
};

struct VDoubleStrand {
  std::vector<VBasePair> vBasePairs_;
};

struct VGrid {
  std::vector<VDoubleStrand> vDoubleStrands_; // = vstrands
  Lattice lattice_;

  void ConstructGridAndLattice(size_t numVStrands, size_t vStrandsLength, LatticeType latticeType, CadnanoJSONFile parsedJson) {

    lattice_ = Lattice();
    if (latticeType == LatticeType::Honeycomb) {
      lattice_.CreateHoneycombLattice();
    }
    else {
      lattice_.CreateSquareLattice();
    }

    int id = 0;
    for (int vStrandId = 0; vStrandId < numVStrands; ++vStrandId) {

      VDoubleStrand vDoubleStrand;
      int num = parsedJson.vstrands_[vStrandId].num_;

      for (int z = 0; z < vStrandsLength; ++z) {
        VBasePair bp;
        bp.scaffoldCell.id_ = id;
        bp.scaffoldCell.vstrandNum_ = vStrandId;
        bp.scaffoldCell.vstrandPos_ = z;
        bp.stapleCell.id_ = id;
        bp.stapleCell.vstrandNum_ = vStrandId;
        bp.stapleCell.vstrandPos_ = z;

        double hPos = vStrandId * dh_diameter_;
        double zPos = z * bp_rise_;

        //set 2D positions
        if (num % 2 == 0) { //alternating top/bottom positioning of scaffold strand
          bp.scaffoldCell.pos_.h_ = hPos;
          bp.scaffoldCell.pos_.z_ = zPos;

          bp.stapleCell.pos_.h_ = hPos + bp_cadnano_dist_;
          bp.stapleCell.pos_.z_ = zPos;
        }
        else {
          bp.scaffoldCell.pos_.h_ = hPos + bp_cadnano_dist_;
          bp.scaffoldCell.pos_.z_ = zPos;

          bp.stapleCell.pos_.h_ = hPos;
          bp.stapleCell.pos_.z_ = zPos;
        }

        vDoubleStrand.vBasePairs_.push_back(bp);
        
        ++id;
      }

      vDoubleStrands_.push_back(vDoubleStrand);
    }
  }

  VCell GetGridScaffoldCell(int vStrandId, int z) {
    return vDoubleStrands_[vStrandId].vBasePairs_[z].scaffoldCell;
  }

  VCell GetGridStapleCell(int vStrandId, int z) {
    return vDoubleStrands_[vStrandId].vBasePairs_[z].stapleCell;
  }

  SBPosition3 GetGridStapleCellPos2D(int vStrandId, int z) {
    VCellPos cPos = vDoubleStrands_[vStrandId].vBasePairs_[z].stapleCell.pos_;
    //SBPosition3 pos = SBPosition3(SBQuantity::nanometer(cPos.h_), SBQuantity::nanometer(cPos.z_), SBQuantity::nanometer(0));
    //SBPosition3 pos = SBPosition3(SBQuantity::nanometer(cPos.z_), SBQuantity::nanometer(-cPos.h_), SBQuantity::nanometer(0));
    SBPosition3 pos = SBPosition3(SBQuantity::nanometer(cPos.z_), SBQuantity::nanometer(cPos.h_), SBQuantity::nanometer(0));
    return pos;
  }

  SBPosition3 GetGridScaffoldCellPos2D(int vStrandId, int z) {
    VCellPos cPos = vDoubleStrands_[vStrandId].vBasePairs_[z].scaffoldCell.pos_;
    //SBPosition3 pos = SBPosition3(SBQuantity::nanometer(cPos.h_), SBQuantity::nanometer(cPos.z_), SBQuantity::nanometer(0));
    //SBPosition3 pos = SBPosition3(SBQuantity::nanometer(cPos.z_), SBQuantity::nanometer(-cPos.h_), SBQuantity::nanometer(0));
    SBPosition3 pos = SBPosition3(SBQuantity::nanometer(cPos.z_), SBQuantity::nanometer(cPos.h_), SBQuantity::nanometer(0));
    return pos;
  }

  SBPosition3 GetGridScaffoldCellPos3D(int vStrandId, int z, unsigned int row, unsigned int column) {
    VCellPos cPos = vDoubleStrands_[vStrandId].vBasePairs_[z].scaffoldCell.pos_;
    LatticeCell lc = lattice_.GetLatticeCell(row, column);
    //SBPosition3 pos = SBPosition3(SBQuantity::nanometer(cPos.z_), SBQuantity::nanometer(-lc.x_), SBQuantity::nanometer(lc.y_));
    SBPosition3 pos = SBPosition3(SBQuantity::nanometer(cPos.z_), SBQuantity::nanometer(lc.x_), SBQuantity::nanometer(lc.y_));
    return pos;
  }

  SBPosition3 GetGridStapleCellPos3D(int vStrandId, int z, unsigned int row, unsigned int column) {
    VCellPos cPos = vDoubleStrands_[vStrandId].vBasePairs_[z].stapleCell.pos_;
    LatticeCell lc = lattice_.GetLatticeCell(row, column);
    //SBPosition3 pos = SBPosition3(SBQuantity::nanometer(cPos.z_), SBQuantity::nanometer(-lc.x_ + 0.5f), SBQuantity::nanometer(lc.y_));
    SBPosition3 pos = SBPosition3(SBQuantity::nanometer(cPos.z_), SBQuantity::nanometer(lc.x_), SBQuantity::nanometer(lc.y_));
    return pos;
  }

  //this is not part of cadnano anymore
  SBPosition3 GetGridScaffoldPos1D(int nucleotideId) {
    SBPosition3 pos = SBPosition3(SBQuantity::nanometer(0), SBQuantity::nanometer(-nucleotideId / 2.0f), SBQuantity::nanometer(0));
    return pos;
  }
   
  SBPosition3 GetGridStaplePos1D(int nucleotideId, int strandId) {
    SBPosition3 pos = SBPosition3(SBQuantity::nanometer(strandId * ADNConstants::DH_DIAMETER), SBQuantity::nanometer(-nucleotideId / 2.0f), SBQuantity::nanometer(0));
    return pos;
  }
};

class DASCadnano {

private:
  // refactoring members
  CadnanoJSONFile2 json2_;
  VGrid2 vGrid2_;
  std::map<Vstrand2*, std::map<std::pair<int, int>, ADNPointer<ADNBaseSegment>>> cellBsMap2_;


  // refactoring methods
  void ParseJSON2(std::string filename);
  void ParseCadnanoFormat3(Document& d);
  void ParseCadnanoLegacy(Document& d);

  ADNPointer<ADNPart> CreateCadnanoModel();
  void CreateEdgeMap2(ADNPointer<ADNPart> nanorobot);
  void CreateScaffold2(ADNPointer<ADNPart> nanorobot);
  void CreateStaples2(ADNPointer<ADNPart> nanorobot);

  CadnanoJSONFile json_;
  VGrid vGrid_;
  std::map<Vstrand*, std::map<std::pair<int, int>, ADNPointer<ADNBaseSegment>>> cellBsMap_;
  //std::tuple<int, int, int> triple;
  std::map<std::tuple<int, int, int>, ADNPointer<ADNNucleotide>> scaffold_nucleotides_;
  // copied from Daedalus
  static DNABlocks GetComplementaryBase(DNABlocks type);
  bool IsThereBase(vec4 data);

public:
  DASCadnano() = default;
  ~DASCadnano() = default;

  // refactoring methods
  ADNPointer<ADNPart> CreateCadnanoPart(std::string file);

  void ParseJSON(string file);

  void CreateModel(ADNPointer<ADNPart> nanorobot, string seq, LatticeType lattice);
  void CreateEdgeMap(ADNPointer<ADNPart> nanorobot);
  void CreateScaffold(string seq, ADNPointer<ADNPart> nanorobot);
  void CreateStaples(ADNPointer<ADNPart> nanorobot);
  //void ShiftStructure(ADNPointer<ADNPart> nanorobot);
  //void CylinderToCadnano(ADNPointer<ADNPart> nanorobot);
};