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
#include "ADNConformations.hpp"
#include "DASLattices.hpp"


using namespace std;
using namespace rapidjson;

double const bp_rise_ = ADNConstants::BP_RISE; // nm
double const dh_diameter_ = ADNConstants::DH_DIAMETER; // nm
double const bp_cadnano_dist_ = ADNConstants::BP_CADNANO_DIST; //0.25 * dh_diameter_; //nm

//using LatticeType = ADNConstants::LatticeType;

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

struct Vstrand {
  int totalLength_;  // total length, including positions without nothing
  int num_;  // identification
  Vec4List scaf_;  // key is position that scaffold base occupies in the vhelix
  Vec4List stap_;
  int col_;  // row and column
  int row_;
  std::map<int, int> loops_;
  std::map<int, int> skips_;
};

struct CadnanoJSONFile {
  string name_;
  std::map<int, Vstrand> vstrands_;  // key is vStrand num
  LatticeType lType_;
  std::pair<int, int> scaffoldStartPosition_;  // first is vhelix num, second is position  within it
  std::vector<vec2> stapleStarts_;  // list of staple starts
};

struct VTube {
  int vStrandId_;
  int initPos_;
  int endPos_;
};

struct VGrid {
  std::vector<VTube> vDoubleStrands_; // = vstrands
  DASLattice lattice_;

  void CreateLattice(LatticeType lType);

  void AddTube(VTube tube);

  SBPosition3 GetGridCellPos3D(int z, unsigned int row, unsigned int column);
  SBPosition3 GetGridCellPos2D(int vStrandId, int z, bool isScaffold);
  SBPosition3 GetGridCellPos1D(int vStrandId, int ntId);
};

class DASCadnano {

private:
  CadnanoJSONFile json_;
  VGrid vGrid_;
  std::map<Vstrand*, std::map<std::pair<int, int>, ADNPointer<ADNBaseSegment>>> cellBsMap_;
  //! To speed up calculation of 1D conformation we keep track to relative position of nt whithin the single strand
  std::map<ADNNucleotide*, int> ntPositions_;
  std::map<ADNSingleStrand*, int> ssId_;
  int lastKey = -1;

  SBPointer<SBMStructuralModelConformation> conformation3D_;
  SBPointer<SBMStructuralModelConformation> conformation2D_;
  SBPointer<SBMStructuralModelConformation> conformation1D_;

  void ParseJSON(std::string filename);
  void ParseCadnanoFormat3(Document& d);
  void ParseCadnanoLegacy(Document& d);

  ADNPointer<ADNPart> CreateCadnanoModel();
  void CreateEdgeMap(ADNPointer<ADNPart> nanorobot);
  void CreateScaffold(ADNPointer<ADNPart> nanorobot);
  void CreateStaples(ADNPointer<ADNPart> nanorobot);
  void TraceSingleStrand(int startVStrand, int startVStrandPos, ADNPointer<ADNSingleStrand> ss, ADNPointer<ADNPart> nanorobot, bool left = true);

  static DNABlocks GetComplementaryBase(DNABlocks type);
  bool IsThereBase(vec4 data);
  void AddSingleStrandToMap(ADNPointer<ADNSingleStrand> ss);

public:
  DASCadnano() = default;
  ~DASCadnano() = default;

  SBPointer<SBMStructuralModelConformation> Get3DConformation();
  SBPointer<SBMStructuralModelConformation> Get2DConformation();
  SBPointer<SBMStructuralModelConformation> Get1DConformation();

  ADNPointer<ADNPart> CreateCadnanoPart(std::string file);

  //! once 3D model has been created, set 2D and 1D positions
  void CreateConformations(ADNPointer<ADNPart> nanorobot);
};