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
using DNABlocks = SBResidue::ResidueType;

enum End {
  ThreePrime = 3,
  FivePrime = 5,
  NotEnd = 0,
  FiveAndThreePrime = 8,
};

enum CellType {
  Undefined = -1,
  BasePair = 0,
  SkipPair = 1,
  LoopPair = 2,
  ALL = 99,
};

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
  std::string CellTypeToString(CellType t);
  bool IsAtomInBackboneByName(std::string name);
  SBElement::Type GetElementType(std::string atomName);
  std::map<std::string, std::vector<std::string>> GetNucleotideBonds(DNABlocks t);
}

/* Classes */

class ADNAtom;
class ADNBaseSegment;
class ADNNucleotideGroup;
class ADNNucleotide;
class ADNSingleStrand;

class ADNAtom: public SBAtom {
  SB_CLASS
public:
  ADNAtom() : SBAtom() {};
  ADNAtom(const ADNAtom& other);
  ~ADNAtom() = default;

  ADNAtom& operator=(const ADNAtom& other);

  std::string const & GetName() const;
  void SetName(const std::string &name);
  Position3D const & GetPosition() const;
  void SetPosition(Position3D const &newPosition);

  SBNode* getNt() const;
  SBNode* getNtGroup() const;

  bool IsInBackbone();
};

SB_REGISTER_TARGET_TYPE(ADNAtom, "ADNAtom", "292377CD-F926-56E9-52AB-D6B623C3A104");
SB_DECLARE_BASE_TYPE(ADNAtom, SBAtom);

// mixin needs ADNAtom completely defined, hence defined here
class PositionableSB {
public:
  PositionableSB();
  ~PositionableSB() = default;
  PositionableSB(const PositionableSB& other);

  PositionableSB& operator=(const PositionableSB& other);

  void SetPosition(Position3D pos);
  Position3D GetPosition() const;

  ADNPointer<ADNAtom> GetCenterAtom() const;
  void SetCenterAtom(ADNPointer<ADNAtom> centerAtom);
  void HideCenterAtom();

private:
  ADNPointer<ADNAtom> centerAtom_;
};

class ADNBackbone: public SBBackbone, public PositionableSB {
  SB_CLASS
public:
  ADNBackbone();
  ADNBackbone(const ADNBackbone& other);
  ~ADNBackbone() = default;

  ADNBackbone& operator=(const ADNBackbone& other);

  bool AddAtom(ADNPointer<ADNAtom> atom);
  bool DeleteAtom(ADNPointer<ADNAtom> atom);
  CollectionMap<ADNAtom> GetAtoms() const;
  int getNumberOfAtoms() const;

  NucleotideGroup GetGroupType();

  ADNPointer<ADNNucleotide> GetNucleotide() const;
};

SB_REGISTER_TARGET_TYPE(ADNBackbone, "ADNBackbone", "CFA95C6A-7686-3029-F93C-2F1FD1988C33");
SB_DECLARE_BASE_TYPE(ADNBackbone, SBBackbone);

class ADNSidechain: public PositionableSB, public SBSideChain {
  SB_CLASS
public:
  ADNSidechain();
  ADNSidechain(const ADNSidechain& other);
  ~ADNSidechain() = default;

  ADNSidechain& operator=(const ADNSidechain& other);

  bool AddAtom(ADNPointer<ADNAtom> atom);
  bool DeleteAtom(ADNPointer<ADNAtom> atom);
  CollectionMap<ADNAtom> GetAtoms() const;
  int getNumberOfAtoms() const;

  NucleotideGroup GetGroupType();

  ADNPointer<ADNNucleotide> GetNucleotide() const;
};

SB_REGISTER_TARGET_TYPE(ADNSidechain, "ADNSidechain", "CD6919A2-5B4C-7723-AAD7-804157EA51EA");
SB_DECLARE_BASE_TYPE(ADNSidechain, SBSideChain);

class ADNNucleotide: public PositionableSB, public SBResidue, public Orientable {
  SB_CLASS
public:
  ADNNucleotide();
  ADNNucleotide(const ADNNucleotide& other);
  ~ADNNucleotide() = default;

  ADNNucleotide& operator=(const ADNNucleotide& other);

  void SetType(DNABlocks t);
  DNABlocks GetType();
  DNABlocks getNucleotideType() const;

  void SetPair(ADNPointer<ADNNucleotide> nt);
  ADNPointer<ADNNucleotide> GetPair();

  ADNPointer<ADNNucleotide> GetPrev();
  ADNPointer<ADNNucleotide> GetNext();
  ADNPointer<ADNSingleStrand> GetStrand();
  SBNode* getSingleStrand() const;

  void SetBaseSegment(ADNPointer<ADNBaseSegment> bs);
  ADNPointer<ADNBaseSegment> GetBaseSegment();
  SBNode* getBaseSegment() const;
  std::string getBaseSegmentType() const;

  End GetEnd();
  void SetEnd(End e);
  bool IsEnd();

  ADNPointer<ADNBackbone> GetBackbone();
  void SetBackbone(ADNPointer<ADNBackbone> bb);
  ADNPointer<ADNSidechain> GetSidechain();
  void SetSidechain(ADNPointer<ADNSidechain> sc);

  void SetSidechainPosition(Position3D pos);
  Position3D GetSidechainPosition();
  void SetBackbonePosition(Position3D pos);
  Position3D GetBackbonePosition();

  std::string const & GetName() const;
  void SetName(const std::string &name);

  void AddAtom(NucleotideGroup g, ADNPointer<ADNAtom> a);
  void ADNNucleotide::DeleteAtom(NucleotideGroup g, ADNPointer<ADNAtom> a);
  CollectionMap<ADNAtom> GetAtoms();
  CollectionMap<ADNAtom> GetAtomsByName(std::string name);
  void HideCenterAtoms();

  // Local base is always the standard basis */
  ublas::matrix<double> GetGlobalBasisTransformation();
  bool GlobalBaseIsSet();

private:
  ADNWeakPointer<ADNNucleotide> pair_;
  ADNWeakPointer<ADNBaseSegment> bs_;  // base segment to which the nucleotide belongs to

  End end_;
};

SB_REGISTER_TARGET_TYPE(ADNNucleotide, "ADNNucleotide", "26603E7A-7792-0C83-B1D5-6C1D222B3379");
SB_DECLARE_BASE_TYPE(ADNNucleotide, SBResidue);

class ADNSingleStrand: public SBChain {
  SB_CLASS
public:
  ADNSingleStrand() : SBChain() {};
  //ADNSingleStrand(int numNts);
  //ADNSingleStrand(std::vector<ADNPointer<ADNNucleotide>> nts);
  ADNSingleStrand(const ADNSingleStrand& other);
  ~ADNSingleStrand() = default;

  ADNSingleStrand& operator=(const ADNSingleStrand& other);

  std::string const & GetName() const;
  void SetName(const std::string &name);

  ADNPointer<ADNNucleotide> GetFivePrime();
  SBNode* getFivePrime() const;
  ADNPointer<ADNNucleotide> GetThreePrime();
  SBNode* getThreePrime() const;

  // if using these functions, make sure nucleotides are properly added
  void SetFivePrime(ADNPointer<ADNNucleotide> nt);
  void SetThreePrime(ADNPointer<ADNNucleotide> nt);

  void IsScaffold(bool b);
  bool IsScaffold();
  int getNumberOfNucleotides() const;
  CollectionMap<ADNNucleotide> GetNucleotides() const;
  ADNPointer<ADNNucleotide> GetNucleotide(unsigned int id) const;
  void AddNucleotideThreePrime(ADNPointer<ADNNucleotide> nt);  // add nucleotide to the three prime end
  void AddNucleotideFivePrime(ADNPointer<ADNNucleotide> nt);  // add nucleotide to the five prime end
  void AddNucleotide(ADNPointer<ADNNucleotide> nt, ADNPointer<ADNNucleotide> nextNt);  // add nucleotide at any position

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
  std::string GetSequence() const;
  std::string getSequence() const;
  /**
  * Returns GC content as a % in this strand
  */
  double GetGCContent() const;
  double getGCContent() const;

  void SetSequence(std::string seq);
  void SetDefaultName();

private:
  bool isScaffold_ = false;
  ADNPointer<ADNNucleotide> fivePrime_;
  ADNPointer<ADNNucleotide> threePrime_;
};

SB_REGISTER_TARGET_TYPE(ADNSingleStrand, "ADNSingleStrand", "8EB118A4-A8BF-19F5-5171-C68582AC6262");
SB_DECLARE_BASE_TYPE(ADNSingleStrand, SBChain);

class ADNCell : public SBStructuralGroup {
  SB_CLASS
public:
  ADNCell() : SBStructuralGroup() {};
  virtual ~ADNCell() {};
  virtual CellType GetType() { return CellType::Undefined; };
  virtual void RemoveNucleotide(ADNPointer<ADNNucleotide> nt) {};
  virtual CollectionMap<ADNNucleotide> GetNucleotides() { return CollectionMap<ADNNucleotide>(); };
};

SB_REGISTER_TARGET_TYPE(ADNCell, "ADNCell", "E6BFD315-2734-B4A6-5808-E784AA4102EF");
SB_DECLARE_BASE_TYPE(ADNCell, SBStructuralGroup);

class ADNBasePair : public ADNCell {
  SB_CLASS
public:
  ADNBasePair() = default;
  ~ADNBasePair() = default;

  CellType GetType() { return CellType::BasePair; };

  ADNPointer<ADNNucleotide> GetLeftNucleotide();
  SBNode* getLeft() const;
  void SetLeftNucleotide(ADNPointer<ADNNucleotide> nt);
  ADNPointer<ADNNucleotide> GetRightNucleotide();
  SBNode* getRight() const;
  void SetRightNucleotide(ADNPointer<ADNNucleotide> nt);
  void AddPair(ADNPointer<ADNNucleotide> left, ADNPointer<ADNNucleotide> right);
  void PairNucleotides();
  void RemoveNucleotide(ADNPointer<ADNNucleotide> nt);
  CollectionMap<ADNNucleotide> GetNucleotides();
private:
  ADNPointer<ADNNucleotide> left_ = nullptr;
  ADNPointer<ADNNucleotide> right_ = nullptr;
};

SB_REGISTER_TARGET_TYPE(ADNBasePair, "ADNBasePair", "71C5049C-EC51-8DC5-15EF-1525E4DBAB42");
SB_DECLARE_BASE_TYPE(ADNBasePair, ADNCell);

class ADNSkipPair : public ADNCell {
  SB_CLASS
public:
  ADNSkipPair() = default;
  ~ADNSkipPair() = default;

  CellType GetType() { return CellType::SkipPair; };

  void RemoveNucleotide(ADNPointer<ADNNucleotide> nt);
private:
};

SB_REGISTER_TARGET_TYPE(ADNSkipPair, "ADNSkipPair", "65441545-3022-773B-49A5-FF39A89AE754");
SB_DECLARE_BASE_TYPE(ADNSkipPair, ADNCell);

class ADNLoop: public SBStructuralGroup {
  SB_CLASS
public:
  ADNLoop() : SBStructuralGroup() {};
  /**
  * Destructor for ANTLoop.
  * Deletes references in other ANTLoop, but not on the ANTSingleStrand.
  */
  ~ADNLoop() = default;

  void SetStart(ADNPointer<ADNNucleotide> nt);
  ADNPointer<ADNNucleotide> GetStart();
  SBNode* getStartNucleotide() const;
  void SetEnd(ADNPointer<ADNNucleotide> nt);
  ADNPointer<ADNNucleotide> GetEnd();
  SBNode* getEndNucleotide() const;
  void SetBaseSegment(ADNPointer<ADNBaseSegment> bs, bool setPositions = false);
  int getNumberOfNucleotides() const;
  std::string getLoopSequence() const;

  CollectionMap<ADNNucleotide> GetNucleotides() const;

  void AddNucleotide(ADNPointer<ADNNucleotide> nt);
  void RemoveNucleotide(ADNPointer<ADNNucleotide> nt);
  bool IsEmpty();

private:
  ADNPointer<ADNNucleotide> startNt_ = nullptr;
  ADNPointer<ADNNucleotide> endNt_ = nullptr;
  CollectionMap<ADNNucleotide> nucleotides_;
};

SB_REGISTER_TARGET_TYPE(ADNLoop, "ADNLoop", "8531205A-01B2-C438-1E26-A50699CA6678");
SB_DECLARE_BASE_TYPE(ADNLoop, SBStructuralGroup);

class ADNLoopPair : public ADNCell {
  SB_CLASS
public:
  ADNLoopPair() = default;
  ~ADNLoopPair() = default;

  CellType GetType() { return CellType::LoopPair; };

  ADNPointer<ADNLoop> GetLeftLoop();
  SBNode* getLeft() const;
  void SetLeftLoop(ADNPointer<ADNLoop> lp);
  ADNPointer<ADNLoop> GetRightLoop();
  SBNode* getRight() const;
  void SetRightLoop(ADNPointer<ADNLoop> lp);

  void RemoveNucleotide(ADNPointer<ADNNucleotide> nt);
  CollectionMap<ADNNucleotide> GetNucleotides();
private:
  ADNPointer<ADNLoop> left_ = nullptr;
  ADNPointer<ADNLoop> right_ = nullptr;
};

SB_REGISTER_TARGET_TYPE(ADNLoopPair, "ADNLoopPair", "F9CB2D19-D635-F494-D87D-EC619763E577");
SB_DECLARE_BASE_TYPE(ADNLoopPair, ADNCell);

class ADNDoubleStrand;

class ADNBaseSegment: public PositionableSB, public SBStructuralGroup, public Orientable {
  SB_CLASS
public:
  ADNBaseSegment() : PositionableSB(), SBStructuralGroup(), Orientable() {};
  ADNBaseSegment(CellType cellType);
  ADNBaseSegment(const ADNBaseSegment& other);
  ~ADNBaseSegment() = default;

  ADNBaseSegment& operator=(const ADNBaseSegment& other);

  void SetNumber(int n);
  int GetNumber() const;
  void setNumber(int n);
  int getNumber() const;

  ADNPointer<ADNBaseSegment> GetPrev() const;
  ADNPointer<ADNBaseSegment> GetNext() const;

  ADNPointer<ADNDoubleStrand> GetDoubleStrand();
  CollectionMap<ADNNucleotide> GetNucleotides();

  void SetCell(ADNCell* c);  // we use raw pointers so subclassing will work
  ADNPointer<ADNCell> GetCell() const;
  CellType GetCellType() const;
  std::string getCellType() const;
  void RemoveNucleotide(ADNPointer<ADNNucleotide> nt);

private:
  ADNPointer<ADNCell> cell_ = nullptr;
  int number_ = -1;  // number of the base in the double strand
};

SB_REGISTER_TARGET_TYPE(ADNBaseSegment, "ADNBaseSegment", "114D0E73-D768-0DF5-3C1A-11569CB91F25");
SB_DECLARE_BASE_TYPE(ADNBaseSegment, SBStructuralGroup);

class ADNDoubleStrand : public SBStructuralGroup {
  SB_CLASS
public:
  ADNDoubleStrand() : SBStructuralGroup() {};
  ~ADNDoubleStrand() = default;
  ADNDoubleStrand(const ADNDoubleStrand& other);

  ADNDoubleStrand& operator=(const ADNDoubleStrand& other);

  void SetInitialTwistAngle(double angle);
  double GetInitialTwistAngle() const;
  double getInitialTwistAngle() const;

  int GetLength() const;
  int getLength() const;

  CollectionMap<ADNBaseSegment> GetBaseSegments() const;
  ADNPointer<ADNBaseSegment> GetNthBaseSegment(int n);  // return the base segment by position in the double strand

  ADNPointer<ADNBaseSegment> GetFirstBaseSegment() const;
  SBNode* getFirstBaseSegment() const;
  ADNPointer<ADNBaseSegment> GetLastBaseSegment() const;
  SBNode* getLastBaseSegment() const;

  void AddBaseSegmentBeginning(ADNPointer<ADNBaseSegment> bs);
  void AddBaseSegmentEnd(ADNPointer<ADNBaseSegment> bs);

private:
  ADNPointer<ADNBaseSegment> start_ = nullptr;
  ADNPointer<ADNBaseSegment> end_ = nullptr;
  double initialTwistAngle_ = 0.0;
};

SB_REGISTER_TARGET_TYPE(ADNDoubleStrand, "ADNDoubleStrand", "D87CDA71-C36A-EA63-9B20-24DD78A4BC4C");
SB_DECLARE_BASE_TYPE(ADNDoubleStrand, SBStructuralGroup);