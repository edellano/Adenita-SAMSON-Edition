#pragma once

#include "ADNPart.hpp"
#include "SBQuantity.hpp"


class PINeighborNt {
public:
  PINeighborNt(unsigned int idx, ADNPointer<ADNNucleotide> nt) : id_(idx), nt_(nt) {};

  unsigned int GetId() { return id_; };
  ADNPointer<ADNNucleotide> GetNucleotide() { return nt_; };

private:
  unsigned int id_;
  ADNPointer<ADNNucleotide> nt_;
};

class PINeighbors {
public:
  PINeighbors(ADNPointer<ADNPart> part, SBQuantity::length cutOff, bool fromOwnSingleStrand = false);
  ~PINeighbors() = default;

  PINeighborNt* GetPINucleotide(ADNPointer<ADNNucleotide> nt);
  std::vector<PINeighborNt*> GetNeighbors(PINeighborNt* nt);
  CollectionMap<ADNNucleotide> GetNeighbors(ADNPointer<ADNNucleotide> nt);

private:
  SBQuantity::length cutOff_;
  std::map<unsigned int, PINeighborNt*> ntIndices_;
  std::vector<unsigned int> neighborList_;
  std::vector<unsigned int> headList_;
  std::vector<unsigned int> numNeighborsList_;

  void InitializeNeighbors(ADNPointer<ADNPart> part, bool fromOwnSingleStrand);
};