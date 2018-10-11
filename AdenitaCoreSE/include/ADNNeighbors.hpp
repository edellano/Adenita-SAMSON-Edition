#pragma once

#include "ADNPart.hpp"
#include "SBQuantity.hpp"


class ADNNeighborNt {
public:
  ADNNeighborNt(unsigned int idx, ADNPointer<ADNNucleotide> nt) : id_(idx), nt_(nt) {};

  unsigned int GetId() { return id_; };
  ADNPointer<ADNNucleotide> GetNucleotide() { return nt_; };

private:
  unsigned int id_;
  ADNPointer<ADNNucleotide> nt_;
};

class ADNNeighbors {
public:
  ADNNeighbors();
  ~ADNNeighbors() = default;

  ADNNeighborNt* GetPINucleotide(ADNPointer<ADNNucleotide> nt);
  std::vector<ADNNeighborNt*> GetNeighbors(ADNNeighborNt* nt);
  CollectionMap<ADNNucleotide> GetNeighbors(ADNPointer<ADNNucleotide> nt);

  void SetFromOwnSingleStrand(bool b);
  void SetIncludePairs(bool b);
  void SetMaxCutOff(SBQuantity::length cutOff);
  void SetMinCutOff(SBQuantity::length cutOff);

  void InitializeNeighbors(ADNPointer<ADNPart> part);

private:
  SBQuantity::length maxCutOff_;
  SBQuantity::length minCutOff_;

  std::map<unsigned int, ADNNeighborNt*> ntIndices_;
  std::vector<unsigned int> neighborList_;
  std::vector<unsigned int> headList_;
  std::vector<unsigned int> numNeighborsList_;

  bool fromOwnSingleStrand_ = false;
  bool includePairs_ = false;
};