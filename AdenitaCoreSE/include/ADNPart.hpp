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

  ADNPointer<ADNDoubleStrand> GetDoubleStrand(int id);
  void RegisterDoubleStrand(ADNPointer<ADNDoubleStrand> ds);
  ADNPointer<ADNSingleStrand> GetSingleStrand(int c_id);
  void RegisterSingleStrand(ADNPointer<ADNSingleStrand> ss);

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
};

SB_REGISTER_TARGET_TYPE(ADNPart, "ADNPart", "D3809709-A2EA-DDC1-9753-A40B2B9DE57E");
SB_DECLARE_BASE_TYPE(ADNPart, SBStructuralModel);