#pragma once

#define _USE_MATH_DEFINES

#include "SBMStructuralModelNodeAtom.hpp"
#include "SBMStructuralModelNodeBackbone.hpp"
#include "SBMStructuralModelNodeSidechain.hpp"
#include "SBMStructuralModelNodeResidue.hpp"
#include "SBMStructuralModelNodeChain.hpp"
#include "SBMStructuralModelNodeGroup.hpp"
#include "SBMStructuralModelNode.hpp"
#include <boost/bimap.hpp>
#include <boost/assign/list_of.hpp>
#include "ADNAuxiliary.hpp"
#include "ADNVectorMath.hpp"
#include "ADNLogger.hpp"
#include "ADNMixins.hpp"
#include <cmath>

/* Physical info and maps */

//enum DNABlocks {
//  DA,
//  DT,
//  DC,
//  DG,
//  DN,
//};
using DNABlocks = SBResidue::ResidueType;

enum End {
  ThreePrime = 3,
  FivePrime = 5,
  NotEnd = 0
};

enum CellType {
  BasePair = 0,
  SkipPair = 1,
  LoopPair = 2,
  ALL = 99,
};

//enum NucleotideGroup {
//  Backbone = 0,
//  Sidechain = 1,
//};
using NucleotideGroup = SBNode::Type;

static std::vector<std::string> backbone_names_ = std::vector<std::string>{ "P", "OP1", "OP2", "O5'", "C5'", "C4'",
"O4'", "C3'", "O3'", "C2'", "C1'" };

using DNAPairsToString = boost::bimap<std::pair<DNABlocks, DNABlocks>, std::string>;
const DNAPairsToString nt_pairs_names_ = boost::assign::list_of<DNAPairsToString::relation>
(std::make_pair(DNABlocks::DA, DNABlocks::DT), "AT")(std::make_pair(DNABlocks::DT, DNABlocks::DA), "TA")
(std::make_pair(DNABlocks::DC, DNABlocks::DG), "CG")(std::make_pair(DNABlocks::DG, DNABlocks::DC), "GC")(std::make_pair(DNABlocks::DI, DNABlocks::DI), "NN");

/* Namespace with static functions */
namespace ADNModel {
  DNABlocks GetComplementaryBase(DNABlocks base);
  char GetResidueName(DNABlocks t);
  DNABlocks ResidueNameToType(char n);
}

/* Classes */

class ADNAtom;
class ADNBaseSegment;
class ADNNucleotideGroup;
class ADNNucleotide;
class ADNSingleStrand;

class ADNAtom: public SBAtom, public Identifiable {
public:
  ADNAtom() : SBAtom(), Identifiable() {};
  ADNAtom(const ADNAtom& other);
  ~ADNAtom() = default;

  ADNAtom& operator=(const ADNAtom& other);

  std::string const & GetName() const;
  void SetName(const std::string &name);
  Position3D const & GetPosition() const;
  void SetPosition(Position3D const &newPosition);

  bool IsInBackbone();
};

// mixin needs ADNAtom completely defined, hence defined here
class PositionableSB {
public:
  PositionableSB() = default;
  ~PositionableSB() = default;
  PositionableSB(const PositionableSB& other);

  PositionableSB& operator=(const PositionableSB& other);

  void SetPosition(Position3D pos);
  Position3D GetPosition() const;

  ADNPointer<ADNAtom> GetCenterAtom() const;
  void SetCenterAtom(ADNPointer<ADNAtom> centerAtom);

private:
  ADNPointer<ADNAtom> centerAtom_;
};

class ADNBackbone: public SBBackbone, public PositionableSB {
public:
  ADNBackbone() : PositionableSB(), SBBackbone() {};
  ADNBackbone(const ADNBackbone& other);
  ~ADNBackbone() = default;

  ADNBackbone& operator=(const ADNBackbone& other);

  bool AddAtom(ADNPointer<ADNAtom> atom);
  bool DeleteAtom(ADNPointer<ADNAtom> atom);
  CollectionMap<ADNAtom> GetAtoms() const;

  NucleotideGroup GetGroupType();

  ADNPointer<ADNNucleotide> GetNucleotide() const;
};

class ADNSidechain: public PositionableSB, public SBSideChain {
public:
  ADNSidechain() : PositionableSB(), SBSideChain() {};
  ADNSidechain(const ADNSidechain& other);
  ~ADNSidechain() = default;

  ADNSidechain& operator=(const ADNSidechain& other);

  bool AddAtom(ADNPointer<ADNAtom> atom);
  bool DeleteAtom(ADNPointer<ADNAtom> atom);
  CollectionMap<ADNAtom> GetAtoms();

  NucleotideGroup GetGroupType();

  ADNPointer<ADNNucleotide> GetNucleotide() const;
};

class ADNNucleotide: public PositionableSB, public SBResidue, public Identifiable, public Orientable {
public:
  ADNNucleotide() : PositionableSB(), SBResidue(), Identifiable(), Orientable() {};
  ADNNucleotide(const ADNNucleotide& other);
  ~ADNNucleotide() = default;

  ADNNucleotide& operator=(const ADNNucleotide& other);

  void SetType(DNABlocks t);
  DNABlocks GetType();
  void SetPair(ADNPointer<ADNNucleotide> nt);
  ADNPointer<ADNNucleotide> GetPair();

  ADNPointer<ADNNucleotide> GetPrev();
  ADNPointer<ADNNucleotide> GetNext();
  ADNPointer<ADNSingleStrand> GetStrand();

  void SetBaseSegment(ADNPointer<ADNBaseSegment> bs);
  ADNPointer<ADNBaseSegment> GetBaseSegment();

  End GetEnd();
  void SetEnd(End e);

  ADNPointer<ADNBackbone> GetBackbone();
  ADNPointer<ADNSidechain> GetSidechain();
  void SetSidechainPosition(Position3D pos);
  Position3D GetSidechainPosition();
  void SetBackbonePosition(Position3D pos);
  Position3D GetBackbonePosition();

  std::string const & GetName() const;
  void SetName(const std::string &name);

  void AddAtom(NucleotideGroup g, ADNPointer<ADNAtom> a);
  void ADNNucleotide::DeleteAtom(NucleotideGroup g, ADNPointer<ADNAtom> a);
  CollectionMap<ADNAtom> GetAtoms();

  // Local base is always the standard basis */
  ublas::matrix<double> GetGlobalBasisTransformation();
  bool GlobalBaseIsSet();
  /**
  * Copy atoms into ANTNucleotide target.
  */
  //void CopyAtoms(ADNPointer<ADNNucleotide> target);

private:
  ADNWeakPointer<ADNNucleotide> pair_;
  ADNWeakPointer<ADNBaseSegment> bs_;  // base segment to which the nucleotide belongs to

  End end_;
};

class ADNSingleStrand: public SBChain, public Identifiable {
public:
  ADNSingleStrand() : SBChain(), Identifiable() {};
  //ADNSingleStrand(int numNts);
  //ADNSingleStrand(std::vector<ADNPointer<ADNNucleotide>> nts);
  ADNSingleStrand(const ADNSingleStrand& other);
  ~ADNSingleStrand() = default;

  ADNSingleStrand& operator=(const ADNSingleStrand& other);

  std::string const & GetName() const;

  ADNPointer<ADNNucleotide> GetFivePrime();
  ADNPointer<ADNNucleotide> GetThreePrime();

  // if using these functions, make sure nucleotides are properly linked
  void SetFivePrime(int ntId);
  void SetThreePrime(int ntId);

  void IsScaffold(bool b);
  bool IsScaffold();
  CollectionMap<ADNNucleotide> GetNucleotides() const;
  ADNPointer<ADNNucleotide> GetNucleotide(unsigned int id) const;
  void AddNucleotideThreePrime(ADNPointer<ADNNucleotide> nt);  // add nucleotide to the three prime end
  void AddNucleotideFivePrime(ADNPointer<ADNNucleotide> nt);  // add nucleotide to the five prime end

  /**
  * Shift start of the strand to the selected nucleotide and sequence.
  * Positions of backbone and sidechain are not recalculated.
  * \param the selected nucleotide
  */
  void ShiftStart(ADNPointer<ADNNucleotide> nt, bool shiftSeq = false);
  /**
  * Returns the sequence of the strand
  * \param sequence from 5' to 3'
  */
  std::string GetSequence();
  /**
  * Returns GC content as a % in this strand
  */
  double GetGCContent();

  void SetSequence(std::string seq);
  void SetDefaultName();

private:
  bool isScaffold_ = false;
  ADNPointer<ADNNucleotide> fivePrime_;
  ADNPointer<ADNNucleotide> threePrime_;
};

class ADNCell : public SBStructuralGroup {
public:
  ADNCell() : SBStructuralGroup() {};
  virtual ~ADNCell() {};
  virtual CellType GetType() = 0;
  virtual void RemoveNucleotide(ADNPointer<ADNNucleotide> nt) = 0;
};

class ADNBasePair : public ADNCell {
public:
  ADNBasePair() = default;
  ~ADNBasePair() = default;

  CellType GetType() { return CellType::BasePair; };

  ADNPointer<ADNNucleotide> GetLeftNucleotide();
  void SetLeftNucleotide(ADNPointer<ADNNucleotide> nt);
  ADNPointer<ADNNucleotide> GetRightNucleotide();
  void SetRightNucleotide(ADNPointer<ADNNucleotide> nt);
  void RemoveNucleotide(ADNPointer<ADNNucleotide> nt);
private:
  ADNPointer<ADNNucleotide> left_ = nullptr;
  ADNPointer<ADNNucleotide> right_ = nullptr;
};

class ADNSkipPair : public ADNCell {
public:
  ADNSkipPair() = default;
  ~ADNSkipPair() = default;

  CellType GetType() { return CellType::SkipPair; };

  void RemoveNucleotide(ADNPointer<ADNNucleotide> nt);
private:
  ADNPointer<ADNNucleotide> left_ = nullptr;
  ADNPointer<ADNNucleotide> right_ = nullptr;
};

class ADNLoop: Collection<ADNNucleotide> {
public:
  ADNLoop() : Collection<ADNNucleotide>() {};
  /**
  * Destructor for ANTLoop.
  * Deletes references in other ANTLoop, but not on the ANTSingleStrand.
  */
  ~ADNLoop() = default;

  void SetStart(ADNPointer<ADNNucleotide> nt);
  ADNPointer<ADNNucleotide> GetStart();
  void SetEnd(ADNPointer<ADNNucleotide> nt);
  ADNPointer<ADNNucleotide> GetEnd();
  void SetStrand(ADNPointer<ADNSingleStrand> ss);
  ADNPointer<ADNSingleStrand> GetStrand();

  CollectionMap<ADNNucleotide> GetNucleotides() const;

  void AddNucleotide(ADNPointer<ADNNucleotide> nt);
  void RemoveNucleotide(ADNPointer<ADNNucleotide> nt);
  //void PositionLoopNucleotides(ublas::vector<double> bsPositionPrev, ublas::vector<double> bsPositionNext);
  bool IsEmpty();

private:
  ADNPointer<ADNNucleotide> startNt_ = nullptr;
  ADNPointer<ADNNucleotide> endNt_ = nullptr;
  ADNWeakPointer<ADNSingleStrand> strand_;
};

class ADNLoopPair : public ADNCell {
public:
  ADNLoopPair() = default;
  ~ADNLoopPair() = default;

  CellType GetType() { return CellType::LoopPair; };

  ADNPointer<ADNLoop> GetLeftLoop();
  void SetLeftLoop(ADNPointer<ADNLoop> lp);
  ADNPointer<ADNLoop> GetRightLoop();
  void SetRightLoop(ADNPointer<ADNLoop> lp);

  void RemoveNucleotide(ADNPointer<ADNNucleotide> nt);

private:
  ADNPointer<ADNLoop> left_ = nullptr;
  ADNPointer<ADNLoop> right_ = nullptr;
};

class ADNDoubleStrand;

class ADNBaseSegment: public Identifiable, public PositionableSB, public SBStructuralNode, public Orientable {
public:
  ADNBaseSegment() : Identifiable(), PositionableSB(), SBStructuralNode(), Orientable() {};
  ADNBaseSegment(const ADNBaseSegment& other);
  ~ADNBaseSegment() = default;

  ADNBaseSegment& operator=(const ADNBaseSegment& other);

  void SetNumber(int n);
  int GetNumber();

  ADNPointer<ADNBaseSegment> GetPrev();
  ADNPointer<ADNBaseSegment> GetNext();

  ADNPointer<ADNDoubleStrand> GetDoubleStrand();

  void SetCell(ADNPointer<ADNCell> c);
  ADNPointer<ADNCell> GetCell() const;
  CellType GetCellType() const;
  void RemoveNucleotide(ADNPointer<ADNNucleotide> nt);

private:
  ADNPointer<ADNCell> cell_ = nullptr;
  int number_ = -1;  // number of the base in the double strand
};

class ADNDoubleStrand : public SBStructuralGroup, public Identifiable {
public:
  ADNDoubleStrand() : Identifiable(), SBStructuralGroup() {};
  //ADNDoubleStrand(int numBases);
  //ADNDoubleStrand(std::vector<ADNPointer<ADNBaseSegment>> bss);
  ~ADNDoubleStrand() = default;
  ADNDoubleStrand(const ADNDoubleStrand& other);

  ADNDoubleStrand& operator=(const ADNDoubleStrand& other);

  void SetInitialTwistAngle(double angle);
  double GetInitialTwistAngle() const;

  CollectionMap<ADNBaseSegment> GetBaseSegments() const;
  ADNPointer<ADNBaseSegment> GetNthBaseSegment(int n);  // return the base segment by position in the double strand

  ADNPointer<ADNBaseSegment> GetFirstBaseSegment();
  ADNPointer<ADNBaseSegment> GetLastBaseSegment();

  void AddBaseSegmentBeginning(ADNPointer<ADNBaseSegment> bs);
  void AddBaseSegmentEnd(ADNPointer<ADNBaseSegment> bs);

private:
  ADNPointer<ADNBaseSegment> start_ = nullptr;
  ADNPointer<ADNBaseSegment> end_ = nullptr;
  double initialTwistAngle_ = 0.0;
};