#pragma once

#include "ADNConstants.hpp"
#include "ADNPart.hpp"


class ADNNanorobot : public Nameable, public Positionable, public Orientable {
public:
  ADNNanorobot() : Nameable(), Positionable(), Orientable() {};
  ADNNanorobot(const ADNNanorobot &n);
  ~ADNNanorobot() = default;

  /* Operators */
  ADNNanorobot& operator=(const ADNNanorobot& other);

  int GetNumberOfDoubleStrands();
  int GetNumberOfBaseSegments();
  int GetNumberOfSingleStrands();
  int GetNumberOfNucleotides();

  CollectionMap<ADNPart> GetParts() const;

private:
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