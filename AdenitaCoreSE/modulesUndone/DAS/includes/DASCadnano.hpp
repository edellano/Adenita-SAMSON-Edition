#ifndef CADNANO_H
#define CADNANO_H

#include "ANTNanorobot.hpp"
#include "rapidjson/reader.h"
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "ANTAuxiliary.hpp"
#include "ANTConstants.hpp"
#include <iostream>
#include "rapidjson/filereadstream.h"
#include <boost/numeric/ublas/matrix.hpp>
#include <string>

// include Daedalus header for useful functions
// todo:
// put this functions outside module and into core or DASHelpers
// #include "DASDaedalus.hpp" // can't be included because boost foreach conflicts

using namespace std;
using namespace rapidjson;

double const bp_rise_ = ANTConstants::BP_RISE; // nm
double const bp_rot_ = ANTConstants::BP_ROT; // rotation in degrees of nts around Z axis
double const dh_diameter_ = ANTConstants::DH_DIAMETER; // nm
double const bp_cadnano_dist_ = ANTConstants::BP_CADNANO_DIST; //0.25 * dh_diameter_; //nm

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


//Grid from caDNAno
namespace ublas = boost::numeric::ublas;

enum LatticeType {
  Honeycomb = 0,
  Square = 1,
  LatticeFree = 2
};

struct LatticeCell {
  //positions by taking LatticeType into account 

  double x_;
  double y_;
};

struct Lattice {
  ublas::matrix<LatticeCell> mat_;
  
  void CreateSquareLattice() {
    mat_ = ublas::matrix<LatticeCell>(55, 55); // in cadnano 2.0 square lattice is 50x50, honeycomb is 30 x 32

    for (unsigned row = 0; row < mat_.size1(); ++row) {
      for (unsigned column = 0; column < mat_.size2(); ++column) {
        //todo
        LatticeCell lc;
        lc.x_ = row * dh_diameter_;
        lc.y_ = column * dh_diameter_;
        mat_(row, column) = lc;
      }
    }
  }

  void CreateHoneycombLattice() {
    mat_ = ublas::matrix<LatticeCell>(30, 32); // in cadnano 2.0 square lattice is 50x50, honeycomb is 30 x 32
    double angle = 120.0;  // deg
    double delt_y = dh_diameter_ * sin(ANTVectorMath::DegToRad(angle - 90)) * 0.5;
    double delt_x = dh_diameter_ * cos(ANTVectorMath::DegToRad(angle - 90));
    double offset = delt_y;

    for (unsigned int row = 0; row < mat_.size1(); ++row) {
      for (unsigned int column = 0; column < mat_.size2(); ++column) {
        double off = offset;
        if ((row + column) % 2 == 0) off *= -1;
        LatticeCell lc;
        lc.x_ = column * delt_x;
        lc.y_ = row * (dh_diameter_ + 2 * delt_y) + off;
        mat_(row, column) = lc;
      }
    }
  }

  LatticeCell GetLatticeCell(unsigned int row, unsigned int column) {
    size_t rSize = mat_.size1();
    size_t cSize = mat_.size2();
    if (row >= rSize || column >= cSize) {
      std::string msg = "Lattice overflow. Probably worng lattice type was selected.";
      ANTAuxiliary::log(msg);
    }
    return mat_(row, column);
  }

  size_t GetNumberRows() {
    return mat_.size1();
  }

  size_t GetNumberCols() {
    return mat_.size2();
  }
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
    SBPosition3 pos = SBPosition3(SBQuantity::nanometer(cPos.z_), SBQuantity::nanometer(lc.x_ + 0.5f), SBQuantity::nanometer(lc.y_));
    return pos;
  }

  //this is not part of cadnano anymore
  SBPosition3 GetGridScaffoldPos1D(int nucleotideId) {
    SBPosition3 pos = SBPosition3(SBQuantity::nanometer(0), SBQuantity::nanometer(-nucleotideId / 2.0f), SBQuantity::nanometer(0));
    return pos;
  }
   
  SBPosition3 GetGridStaplePos1D(int nucleotideId, int strandId) {
    SBPosition3 pos = SBPosition3(SBQuantity::nanometer(strandId * ANTConstants::DH_DIAMETER), SBQuantity::nanometer(-nucleotideId / 2.0f), SBQuantity::nanometer(0));
    return pos;
  }
};

class DASCadnano {

private:

  SEConfig* configuration_;

  CadnanoJSONFile json_;
  std::map<int, ANTSingleStrand*> chains_;
  VGrid vGrid_;
  std::map<Vstrand*, std::map<std::pair<int, int>, ANTBaseSegment*>> cellBsMap_;
  //std::tuple<int, int, int> triple;
  std::map<std::tuple<int, int, int>, ANTNucleotide*> scaffold_nucleotides_;
  // copied from Daedalus
  static DNABlocks GetComplementaryBase(DNABlocks type);
  bool IsThereBase(vec4 data);

public:
  DASCadnano() { configuration_ = new SEConfig(); };
  ~DASCadnano() = default;

  void ParseJSON(string file);
  void WriteJSON();

  void CreateModel(ANTPart & nanorobot, string seq, LatticeType lattice);
  void CreateEdgeMap(ANTPart& nanorobot);
  void CreateScaffold(ANTSingleStrand & scaffold, string seq, ANTPart& nanorobot);
  void CreateStaples(ANTPart & nanorobot);
  void ShiftStructure(ANTPart & nanorobot);
  void CylinderToCadnano(ANTPart & nanorobot);
};

#endif