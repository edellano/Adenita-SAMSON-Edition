#include "PICrossovers.hpp"

std::vector<XOPair> PICrossovers::GetCrossovers(ADNPointer<ADNPart> part)
{
  auto nucleotides = part->GetNucleotides();

  std::vector<XOPair> xos;

  SB_FOR(ADNPointer<ADNNucleotide> nt, nucleotides) {
    ADNPointer<ADNBaseSegment> bs = nt->GetBaseSegment();
    ADNPointer<ADNDoubleStrand> ds1 = bs->GetDoubleStrand();

    ADNPointer<ADNNucleotide> next = nt->GetNext();
    if (next != nullptr) {
      ADNPointer<ADNDoubleStrand> ds2 = next->GetBaseSegment()->GetDoubleStrand();
      if (ds1 != ds2) {
        XOPair xo = std::make_pair(nt, next);
        xos.push_back(xo);

        nt->setHighlightingFlag(true);
        next->setHighlightingFlag(true);
      }
    }
  }

  return xos;
}

std::vector<XOPair> PICrossovers::GetPossibleCrossovers(ADNPointer<ADNPart> part, ADNPointer<ADNNucleotide> nt, ADNNeighbors* neigh)
{
  // highlight neighbors of selected nucleotide
  auto ntNeighbors = neigh->GetNeighbors(nt);
  SB_FOR(ADNPointer<ADNNucleotide> ntN, ntNeighbors) {
    ntN->setSelectionFlag(true);
  }

  return std::vector<XOPair>();
}

std::vector<XOPair> PICrossovers::GetPossibleCrossovers(ADNPointer<ADNPart> part, ADNNeighbors* neigh)
{
  if (neigh == nullptr) {
    // create neighbors
    neigh = new ADNNeighbors();
    SEConfig& c = SEConfig::GetInstance();
    neigh->SetMaxCutOff(SBQuantity::nanometer(c.debugOptions.maxCutOff));
    neigh->SetMinCutOff(SBQuantity::nanometer(c.debugOptions.minCutOff));
    neigh->SetIncludePairs(false);
    neigh->SetFromOwnSingleStrand(true);
    neigh->InitializeNeighbors(part);
  }

  std::vector<XOPair> xos;

  // highlight neighbors of selected nucleotide
  auto nts = part->GetNucleotides();
  SB_FOR(ADNPointer<ADNNucleotide> nt, nts) {
    auto ntXO = GetPossibleCrossovers(part, nt, neigh);
    xos.insert(xos.end(), ntXO.begin(), ntXO.end());
  }

  return std::vector<XOPair>();
}