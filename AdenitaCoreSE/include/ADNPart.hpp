#pragma once

#include "ADNConstants.hpp"
#include "ADNModel.hpp"
#include "SBMStructuralModel.hpp"
#include "SBMStructuralModelNodeRoot.hpp"


using BaseSegments = std::map<int, ADNBaseSegment*>;
using NtSegments = std::map<ADNNucleotide*, ADNBaseSegment*>;
using NTMap = boost::bimap<unsigned int, ADNNucleotide*>;

class ADNPart : public Orientable, public PositionableSB, public SBStructuralModel {
  SB_CLASS
public:

  ADNPart() : Orientable(), PositionableSB(), SBStructuralModel() {};
  ADNPart(const ADNPart &n);
  ~ADNPart() = default;

  ADNPart& operator=(const ADNPart& other);

  std::string const & GetName() const;
  void SetName(const std::string &name);

  void RegisterDoubleStrand(ADNPointer<ADNDoubleStrand> ds);
  void RegisterBaseSegmentEnd(ADNPointer<ADNDoubleStrand> ds, ADNPointer<ADNBaseSegment> bs);
  void RegisterSingleStrand(ADNPointer<ADNSingleStrand> ss);
  void RegisterNucleotideThreePrime(ADNPointer<ADNSingleStrand> ss, ADNPointer<ADNNucleotide> nt);
  void RegisterNucleotideFivePrime(ADNPointer<ADNSingleStrand> ss, ADNPointer<ADNNucleotide> nt);
  void RegisterAtom(ADNPointer<ADNNucleotide> nt, NucleotideGroup g, ADNPointer<ADNAtom> at);

  unsigned int GetBaseSegmentIndex(ADNPointer<ADNBaseSegment> bs);

  CollectionMap<ADNSingleStrand> GetSingleStrands() const;
  CollectionMap<ADNDoubleStrand> GetDoubleStrands() const;
  CollectionMap<ADNBaseSegment> GetBaseSegments(CellType celltype = ALL) const;
  CollectionMap<ADNSingleStrand> GetScaffolds() const;
  CollectionMap<ADNNucleotide> GetNucleotides(CellType celltype = ALL) const;
  CollectionMap<ADNAtom> GetAtoms() const;

  int GetNumberOfDoubleStrands();
  int GetNumberOfSingleStrands();
  int GetNumberOfNucleotides();
  int GetNumberOfAtoms();
  int GetNumberOfBaseSegments();

  void DeregisterSingleStrand(ADNPointer<ADNSingleStrand> ss);
  void DeregisterDoubleStrand(ADNPointer<ADNDoubleStrand> ds);

protected:
private:
  // inside these pointers ids are unique
  CollectionMap<ADNAtom> atomsIndex_;
  CollectionMap<ADNNucleotide> nucleotidesIndex_;
  CollectionMap<ADNSingleStrand> singleStrandsIndex_;
  CollectionMap<ADNBaseSegment> baseSegmentsIndex_;
  CollectionMap<ADNDoubleStrand> doubleStrandsIndex_;
};

SB_REGISTER_TARGET_TYPE(ADNPart, "ADNPart", "D3809709-A2EA-DDC1-9753-A40B2B9DE57E");
SB_DECLARE_BASE_TYPE(ADNPart, SBStructuralModel);