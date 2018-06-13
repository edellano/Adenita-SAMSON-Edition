#pragma once

#include "ADNConstants.hpp"
#include "ADNPart.hpp"


class ADNNanorobot : public Nameable, public Positionable, public Orientable, public Collection<ADNPart> {
public:
  ADNNanorobot() : Nameable(), Positionable(), Orientable(), Collection<ADNPart>() {};
  ADNNanorobot(const ADNNanorobot &n);
  ~ADNNanorobot() = default;

  /* Operators */
  ADNNanorobot& operator=(const ADNNanorobot& other);

  int GetLastDoubleStrandKey() const;
  int GetLastBaseSegmentKey() const;
  int GetLastSingleStrandKey() const;
  int GetLastNucleotideKey() const;
  int GetLastAtomKey() const;

  int GetNumberOfDoubleStrands();
  int GetNumberOfBaseSegments();
  int GetNumberOfSingleStrands();
  int GetNumberOfNucleotides();

  CollectionMap<ADNPart> GetParts() const;

  // everytime a new object is created it needs to be registered
  void RegisterPart(ADNPointer<ADNPart> part);
  void RegisterDoubleStrand(ADNPointer<ADNPart> part, ADNPointer<ADNDoubleStrand> ds);
  void RegisterBaseSegmentBeginning(ADNPointer<ADNDoubleStrand> ds, ADNPointer<ADNBaseSegment> bs);
  void RegisterBaseSegmentEnd(ADNPointer<ADNDoubleStrand> ds, ADNPointer<ADNBaseSegment> bs);
  void RegisterSingleStrand(ADNPointer<ADNPart> part, ADNPointer<ADNSingleStrand> ss);
  void RegisterNucleotideFivePrime(ADNPointer<ADNSingleStrand> ss, ADNPointer<ADNNucleotide> nt);
  void RegisterNucleotideThreePrime(ADNPointer<ADNSingleStrand> ss, ADNPointer<ADNNucleotide> nt);
  void RegisterAtom(ADNPointer<ADNNucleotide> nt, ADNPointer<ADNNucleotideGroup> group, ADNPointer<ADNAtom> atom);

private:
  int lastDoubleStrandKey_ = -1;
  int lastBaseSegmentKey_ = -1;
  int lastSingleStrandKey_ = -1;
  int lastNucleotideKey_ = -1;
  int lastAtomKey_ = -1;

//  /* Getters */
//  std::map<int, ANTSingleStrand*> GetSingleStrands() const;
//  std::map<ANTDoubleStrand*, ANTPart*> GetDoubleStrands() const;
//  std::map<int, ANTPart*> GetParts() const;
//  ANTPart* GetPart(ANTSingleStrand* ss);
//  ANTPart* GetPart(ANTBaseSegment* bs);
//  std::map<ANTSingleStrand*, std::vector<ANTNucleotide*>> GetSelectedNucleotides();
//  std::vector<ANTPart*> GetSelectedParts();
//  std::vector<ANTSingleStrand*> GetSelectedSingleStrands();
//  std::vector<ANTDoubleStrand*> GetSelectedDoubleStrands();
//  ANTBaseSegment* GetBaseSegment(ANTNucleotide* nt);
//  std::vector<ANTBaseSegment*> GetSelectedBaseSegments();
//  std::vector<ANTVertex*> GetVertices();
//  Edges GetEdges();
//  Faces GetFaces();
//  std::vector<ANTBaseSegment*> ANTNanorobot::GetBaseSegments(CellType celltype = ALL);
//  Joints ANTNanorobot::GetSegmentJoints(CellType celltype = ALL);
//
//  /* Setters */
//  void RegisterPart(ANTPart* part);
//  void UpdatePart(ANTPart* part);
//
//  /* Methods */
//  std::vector<ANTSingleStrand*> GetScaffolds();
//  std::vector<ANTNucleotide*> GetNucleotides();
//  int GetNumberOfNucleotides();
//  int GetNumberOfAtoms();
//  int GetNumberOfBaseSegments(CellType celltype = ALL);
//
//  /**
//   * Generates a ANTNanorobot object from DataGraph information
//   */
//  static ANTNanorobot& GenerateANTNanorobot();
//  /**
//   * Concatenates two strands, doesn't merge SBChain
//   * \param the first strand
//   * \param the second strand
//   * \param a fulfill sequence
//   * \return the resulting strand
//   */

//  std::vector<ANTSingleStrand*> CreateSingleStrandPair();
//
//  /** Detects crossovers
//  */
//  std::vector<std::pair<ANTNucleotide*, ANTNucleotide*>> DetectCrossovers();
//  /** Detects possible crossovers
//  */
//  std::vector<std::pair<ANTNucleotide*, ANTNucleotide*>> DetectPossibleCrossovers();

//
//  /* Operations using SAMSON */
//  static SBVector3 GetPolygonNorm(ANTPolygon* face);
//
};