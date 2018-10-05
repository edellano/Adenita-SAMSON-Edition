#include "ADNNeighbors.hpp"


ADNNeighbors::ADNNeighbors()
{
  minCutOff_ = SBQuantity::nanometer(0.0);
}

ADNNeighborNt * ADNNeighbors::GetPINucleotide(ADNPointer<ADNNucleotide> nt)
{
  ADNNeighborNt* piNt = nullptr;

  for (auto &p: ntIndices_) {
    piNt = p.second;
    if (piNt->GetNucleotide() == nt) break;
  }

  return piNt;
}

std::vector<ADNNeighborNt*> ADNNeighbors::GetNeighbors(ADNNeighborNt* nt)
{
  std::vector<ADNNeighborNt*> neighbors;

  unsigned int idx = nt->GetId();
  unsigned int pos = headList_[idx];
  unsigned int sz = numNeighborsList_[idx];

  for (unsigned int i = 0; i < sz; ++i) {
    unsigned int neighborIdx = neighborList_[pos + i];
    ADNNeighborNt* nt = ntIndices_.at(neighborIdx);
    neighbors.push_back(nt);
  }

  return neighbors;
}

CollectionMap<ADNNucleotide> ADNNeighbors::GetNeighbors(ADNPointer<ADNNucleotide> nt)
{
  ADNNeighborNt* piNt = GetPINucleotide(nt);
  // repeat code to avoid an extra loop
  CollectionMap<ADNNucleotide> neighbors;

  unsigned int idx = piNt->GetId();
  unsigned int pos = headList_[idx];
  unsigned int sz = numNeighborsList_[idx];

  for (unsigned int i = 0; i < sz; ++i) {
    unsigned int neighborIdx = neighborList_[pos + i];
    ADNNeighborNt* nt = ntIndices_.at(neighborIdx);
    neighbors.addReferenceTarget(nt->GetNucleotide()());
  }

  return neighbors;
}

void ADNNeighbors::SetFromOwnSingleStrand(bool b)
{
  fromOwnSingleStrand_ = b;
}

void ADNNeighbors::SetIncludePairs(bool b)
{
  includePairs_ = b;
}

void ADNNeighbors::SetMaxCutOff(SBQuantity::length cutOff)
{
  maxCutOff_ = cutOff;
}

void ADNNeighbors::SetMinCutOff(SBQuantity::length cutOff)
{
  minCutOff_ = cutOff;
}

void ADNNeighbors::InitializeNeighbors(ADNPointer<ADNPart> part)
{
  ADNLogger& logger = ADNLogger::GetLogger();

  auto nts = part->GetNucleotides();
  headList_ = std::vector<unsigned int>(nts.size());
  numNeighborsList_ = std::vector<unsigned int>(nts.size());

  int create_index = true;
  unsigned int neighborIdx = 0;

  // we iterate like this to ensure looping always in the same order
  unsigned int sz = nts.size();
  for (unsigned int i = 0; i < sz; ++i) {
    auto nt1 = nts[i];

    SBPosition3 pos1 = nt1->GetPosition();
    // store just first nt1
    if (i == 0) {
      ADNNeighborNt* piNt1 = new ADNNeighborNt(i, nt1);
      ntIndices_.insert(std::make_pair(i, piNt1));
    }
    // update head list with position
    headList_[i] = neighborIdx;

    unsigned int numNeighbors = 0;

    for (unsigned int j = 0; j < sz; ++j) {
      auto nt2 = nts[j];

      if (nt2 == nt1) {
        if (i != j) {
          std::string msg = "Creating neighbor list: looping over neighbors in different order";
          logger.LogDebug(msg);
        }
        continue;
      }

      // first time of the loop we create the other indices
      if (create_index) {
        ADNNeighborNt* piNt2 = new ADNNeighborNt(j, nt2);
        ntIndices_.insert(std::make_pair(j, piNt2));
      }

      if (!fromOwnSingleStrand_ && nt2->GetStrand() == nt1->GetStrand()) {
        continue;
      }

      if (!includePairs_ && nt2->GetPair() == nt1) {
        continue;
      }

      SBPosition3 pos2 = nt2->GetPosition();
      auto dist = (pos2 - pos1).norm();
      if (dist < maxCutOff_ && dist > minCutOff_) {
        // neighbors
        neighborList_.push_back(j);
        ++neighborIdx;
        ++numNeighbors;
      }

    }

    numNeighborsList_[i] = numNeighbors;
    create_index = false;
  }
}
