#pragma once

#include "ADNConstants.hpp"
#include "ADNModel.hpp"
#include "SBMStructuralModel.hpp"
#include "SBMStructuralModelNodeRoot.hpp"


class ADNPart : public SBStructuralModel {
  SB_CLASS
public:

  ADNPart() : SBStructuralModel() { SetName("Adenita Component"); };
  ADNPart(const ADNPart &n);
  ~ADNPart() = default;

  ADNPart& operator=(const ADNPart& other);

  void serialize(SBCSerializer* serializer, const SBNodeIndexer& nodeIndexer, const SBVersionNumber& sdkVersionNumber = SB_SDK_VERSION_NUMBER, const SBVersionNumber& classVersionNumber = SBVersionNumber(1, 0, 0)) const;														///< Serializes the node
  void unserialize(SBCSerializer* serializer, const SBNodeIndexer& nodeIndexer, const SBVersionNumber& sdkVersionNumber = SB_SDK_VERSION_NUMBER, const SBVersionNumber& classVersionNumber = SBVersionNumber(1, 0, 0));											///< Unserializes the node

  std::string const & GetName() const;
  void SetName(const std::string &name);

  void RegisterDoubleStrand(ADNPointer<ADNDoubleStrand> ds);
  void RegisterBaseSegmentEnd(ADNPointer<ADNDoubleStrand> ds, ADNPointer<ADNBaseSegment> bs, bool addToDs = true);
  void RegisterSingleStrand(ADNPointer<ADNSingleStrand> ss);
  void RegisterNucleotideThreePrime(ADNPointer<ADNSingleStrand> ss, ADNPointer<ADNNucleotide> nt, bool addToSs = true);
  void RegisterNucleotideFivePrime(ADNPointer<ADNSingleStrand> ss, ADNPointer<ADNNucleotide> nt, bool addToSs = true);
  void RegisterNucleotide(ADNPointer<ADNSingleStrand> ss, ADNPointer<ADNNucleotide> nt, 
    ADNPointer<ADNNucleotide> ntNext, bool addToSs = true);
  void RegisterAtom(ADNPointer<ADNNucleotide> nt, NucleotideGroup g, ADNPointer<ADNAtom> at, bool create = false);

  unsigned int GetBaseSegmentIndex(ADNPointer<ADNBaseSegment> bs);

  CollectionMap<ADNSingleStrand> GetSingleStrands() const;
  CollectionMap<ADNDoubleStrand> GetDoubleStrands() const;
  CollectionMap<ADNBaseSegment> GetBaseSegments(CellType celltype = ALL) const;
  CollectionMap<ADNSingleStrand> GetScaffolds() const;
  CollectionMap<ADNNucleotide> GetNucleotides(CellType celltype = ALL) const;
  CollectionMap<ADNAtom> GetAtoms() const;

  int GetNumberOfDoubleStrands() const;
  int getNumberOfDoubleStrands() const;
  int GetNumberOfSingleStrands() const;
  int getNumberOfSingleStrands() const;
  int GetNumberOfNucleotides() const;
  int getNumberOfNucleotides() const;
  int GetNumberOfAtoms() const;
  int getNumberOfAtoms() const;
  int GetNumberOfBaseSegments() const;
  int getNumberOfBaseSegments() const;

  void DeregisterSingleStrand(ADNPointer<ADNSingleStrand> ss, bool removeFromParent = true, bool removeFromIndex = true);
  void DeregisterNucleotide(ADNPointer<ADNNucleotide> nt, bool removeFromSs = true, bool removeFromBs = true, bool removeFromIndex = true);
  void DeregisterDoubleStrand(ADNPointer<ADNDoubleStrand> ds, bool removeFromParent = true, bool removeFromIndex = true);
  void DeregisterBaseSegment(ADNPointer<ADNBaseSegment> bs, bool removeFromDs = true, bool removeFromIndex = true);
  void DeregisterAtom(ADNPointer<ADNAtom> atom, bool removeFromAtom = true);

  bool loadedViaSAMSON();
  void loadedViaSAMSON(bool l);

protected:
private:
  // inside these pointers ids are unique
  CollectionMap<ADNAtom> atomsIndex_;
  CollectionMap<ADNNucleotide> nucleotidesIndex_;
  CollectionMap<ADNSingleStrand> singleStrandsIndex_;
  CollectionMap<ADNBaseSegment> baseSegmentsIndex_;
  CollectionMap<ADNDoubleStrand> doubleStrandsIndex_;

  bool loadedViaSAMSON_ = false;
};

SB_REGISTER_TARGET_TYPE(ADNPart, "ADNPart", "D3809709-A2EA-DDC1-9753-A40B2B9DE57E");
SB_DECLARE_BASE_TYPE(ADNPart, SBStructuralModel);