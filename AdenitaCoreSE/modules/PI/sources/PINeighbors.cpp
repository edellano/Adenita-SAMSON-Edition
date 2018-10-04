#include "PINeighbors.hpp"

PINeighbors::PINeighbors(ADNPointer<ADNPart> part, SBQuantity::length cutOff, bool fromOwnSingleStrand)
{
  cutOff_ = cutOff;
  headList_ = std::vector<unsigned int>(part->GetNumberOfNucleotides());
  numNeighborsList_ = std::vector<unsigned int>(part->GetNumberOfNucleotides());

  InitializeNeighbors(part, fromOwnSingleStrand);
}

PINeighborNt * PINeighbors::GetPINucleotide(ADNPointer<ADNNucleotide> nt)
{
  PINeighborNt* piNt = nullptr;

  for (auto &p: ntIndices_) {
    piNt = p.second;
    if (piNt->GetNucleotide() == nt) break;
  }

  return piNt;
}

std::vector<PINeighborNt*> PINeighbors::GetNeighbors(PINeighborNt* nt)
{
  std::vector<PINeighborNt*> neighbors;

  unsigned int idx = nt->GetId();
  unsigned int pos = headList_[idx];
  unsigned int sz = numNeighborsList_[idx];

  for (unsigned int i = 0; i < sz; ++i) {
    unsigned int neighborIdx = neighborList_[pos + i];
    PINeighborNt* nt = ntIndices_.at(neighborIdx);
    neighbors.push_back(nt);
  }

  return neighbors;
}

CollectionMap<ADNNucleotide> PINeighbors::GetNeighbors(ADNPointer<ADNNucleotide> nt)
{
  PINeighborNt* piNt = GetPINucleotide(nt);
  // repeat code to avoid an extra loop
  CollectionMap<ADNNucleotide> neighbors;

  unsigned int idx = piNt->GetId();
  unsigned int pos = headList_[idx];
  unsigned int sz = numNeighborsList_[idx];

  for (unsigned int i = 0; i < sz; ++i) {
    unsigned int neighborIdx = neighborList_[pos + i];
    PINeighborNt* nt = ntIndices_.at(neighborIdx);
    neighbors.addReferenceTarget(nt->GetNucleotide()());
  }

  return neighbors;
}

void PINeighbors::InitializeNeighbors(ADNPointer<ADNPart> part, bool fromOwnSingleStrand)
{
  ADNLogger& logger = ADNLogger::GetLogger();

  auto nts = part->GetNucleotides();
  int create_index = true;
  unsigned int neighborIdx = 0;

  // we iterate like this to ensure looping always in the same order
  unsigned int sz = nts.size();
  for (unsigned int i = 0; i < sz; ++i) {
    auto nt1 = nts[i];

    SBPosition3 pos1 = nt1->GetPosition();
    // store just first nt1
    if (i == 0) {
      PINeighborNt* piNt1 = new PINeighborNt(i, nt1);
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
        PINeighborNt* piNt2 = new PINeighborNt(j, nt2);
        ntIndices_.insert(std::make_pair(j, piNt2));
      }

      if (!fromOwnSingleStrand && nt2->GetStrand() == nt1->GetStrand()) {
        continue;
      }

      SBPosition3 pos2 = nt2->GetPosition();
      if ((pos2-pos1).norm() < cutOff_) {
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
