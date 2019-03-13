#include "DASComplexOperations.hpp"
#include "DASBackToTheAtom.hpp"

DASOperations::SixSingleStrands DASOperations::CreateCrossover(ADNPointer<ADNPart> part, ADNPointer<ADNNucleotide> nt1, ADNPointer<ADNNucleotide> nt2, bool two, std::string seq)
{
  SixSingleStrands ssLeftOvers;

  ADNPointer<ADNSingleStrand> firstStrand = nt1->GetStrand();
  ADNPointer<ADNSingleStrand> secondStrand = nt2->GetStrand();

  ADNPointer<ADNSingleStrand> joinStrand1 = nullptr;
  ADNPointer<ADNSingleStrand> joinStrand2 = nullptr;

  if (!seq.empty()) {
    int length = boost::numeric_cast<int>(seq.size());

    SBVector3 direction = (nt1->GetPosition() - nt2->GetPosition()).normalizedVersion();
    SBQuantity::length expectedLength = SBQuantity::nanometer(ADNConstants::BP_RISE) * (length + 1);  // we need to accomodate space for distance between the ends
    SBPosition3 startPos = (nt2->GetPosition() + nt1->GetPosition())*0.5 - expectedLength * 0.5 * direction;

    if (two) {
      auto res = DASCreator::AddDoubleStrandToADNPart(part, length, startPos, direction);
      joinStrand1 = res.ss1;
      joinStrand2 = res.ss2;

      joinStrand1->SetSequence(seq);
      DASBackToTheAtom* btta = new DASBackToTheAtom();
      btta->SetPositionsForNewNucleotides(part, joinStrand1->GetNucleotides());

      res.ds->create();
      part->DeregisterDoubleStrand(res.ds);
      part->RegisterDoubleStrand(res.ds);

      joinStrand1->create();
      part->DeregisterSingleStrand(joinStrand1);
      part->RegisterSingleStrand(joinStrand1);
      joinStrand2->create();
      part->DeregisterSingleStrand(joinStrand2);
      part->RegisterSingleStrand(joinStrand2);
    }
    else {
      auto res = DASCreator::AddSingleStrandToADNPart(part, length, startPos, direction);
      joinStrand1 = res.ss1;
      joinStrand1->SetSequence(seq);

      DASBackToTheAtom* btta = new DASBackToTheAtom();
      btta->SetPositionsForNewNucleotides(part, joinStrand1->GetNucleotides());
      
      res.ds->create();
      part->DeregisterDoubleStrand(res.ds);
      part->RegisterDoubleStrand(res.ds);
      joinStrand1->create();
      part->DeregisterSingleStrand(joinStrand1);
      part->RegisterSingleStrand(joinStrand1);
    }
  }

  bool sameStrand = firstStrand == secondStrand;

  if (nt1->IsEnd() && nt2->IsEnd()) {
    if (sameStrand) {
      // todo: join
      firstStrand->IsCircular(true);
    }
    else {
      ADNPointer<ADNNucleotide> fPrime = nt1;
      ADNPointer<ADNNucleotide> tPrime = nt2;

      if (fPrime->GetEnd() == ThreePrime && tPrime->GetEnd() == FivePrime) {
        fPrime = nt2;
        tPrime = nt1;
      }
      auto fPrimeStrand = fPrime->GetStrand();
      auto tPrimeStrand = tPrime->GetStrand();

      if (joinStrand1 != nullptr) {
        auto ssN = ADNBasicOperations::MergeSingleStrands(part, joinStrand1, fPrimeStrand);
        auto ssNN = ADNBasicOperations::MergeSingleStrands(part, tPrimeStrand, ssN);
        part->DeregisterSingleStrand(ssN);
        part->DeregisterSingleStrand(joinStrand1);
      }
      else {
        auto resSs = ADNBasicOperations::MergeSingleStrands(part, tPrimeStrand, fPrimeStrand);
      }
      ssLeftOvers.first = tPrimeStrand;
      ssLeftOvers.second = fPrimeStrand;
    }    
  }
  else {
      
    ADNPointer<ADNNucleotide> firstPrev = nullptr;
    ADNPointer<ADNNucleotide> secondNext = nullptr;
    ADNPointer<ADNSingleStrand> firstStrandTwo = nullptr;
    ADNPointer<ADNSingleStrand> secondStrandTwo = nullptr;

    ssLeftOvers.first = firstStrand;

    // order nucleotides
    if (sameStrand) {
      bool circ = firstStrand->IsCircular();
      // break first nucleotide in 5'
      if (nt1->GetEnd() != FivePrime) {
        firstPrev = nt1->GetPrev(true);
        auto pair1 = ADNBasicOperations::BreakSingleStrand(part, nt1);
        firstStrand = pair1.second;
        secondStrandTwo = pair1.first;
      }
      // break second nucleotide in 3'
      if (nt2->GetEnd() != ThreePrime) {
        secondNext = nt2->GetNext(true);
        auto pair2 = ADNBasicOperations::BreakSingleStrand(part, secondNext);
        secondStrand = pair2.first;
        firstStrandTwo = pair2.second;
      }

      // connect strands
      if (nt2->GetStrand() != nt1->GetStrand()) {

        if (joinStrand1 != nullptr) {
          auto ssN = ADNBasicOperations::MergeSingleStrands(part, nt2->GetStrand(), joinStrand1);
          auto ssNN = ADNBasicOperations::MergeSingleStrands(part, ssN, nt1->GetStrand());
          part->DeregisterSingleStrand(ssN);
          part->DeregisterSingleStrand(joinStrand1);
        }
        else {
          auto ssConnect = ADNBasicOperations::MergeSingleStrands(part, nt1->GetStrand(), nt2->GetStrand());
        }

        if (two) {
          auto ssN = ADNBasicOperations::MergeSingleStrands(part, joinStrand2, secondNext->GetStrand());
          part->DeregisterSingleStrand(joinStrand2);
          ssN->IsCircular(true);
          if (firstPrev->GetStrand() != secondNext->GetStrand()) {
            auto ssNN = ADNBasicOperations::MergeSingleStrands(part, firstPrev->GetStrand(), ssN);
            part->DeregisterSingleStrand(ssN);
            ssNN->IsCircular(true);
          }
        }
      }
      else
      {
        if (joinStrand1 != nullptr) {
          auto ssN = ADNBasicOperations::MergeSingleStrands(part, joinStrand1, nt2->GetStrand());
          part->DeregisterSingleStrand(joinStrand1);
        }
        nt2->GetStrand()->IsCircular(true);
        if (circ) {
          // left over strands need to be attached if they are not
          firstStrandTwo = firstPrev->GetStrand();
          secondStrandTwo = secondNext->GetStrand();
          auto ssConnectTwo = ADNBasicOperations::MergeSingleStrands(part, firstStrandTwo, secondStrandTwo);
          if (two) {
            if (joinStrand2 != nullptr) {
              auto ssN = ADNBasicOperations::MergeSingleStrands(part, joinStrand2, secondStrandTwo);
              auto ssNN = ADNBasicOperations::MergeSingleStrands(part, firstStrandTwo, ssN);
              part->DeregisterSingleStrand(ssN);
              part->DeregisterSingleStrand(joinStrand2);
            }
            ssConnectTwo->IsCircular(true);
          }
        }
        else {
          if (two) {
            firstStrandTwo = firstPrev->GetStrand();
            secondStrandTwo = secondNext->GetStrand();
            auto ssN = ADNBasicOperations::MergeSingleStrands(part, joinStrand2, secondStrandTwo);
            auto ssNN = ADNBasicOperations::MergeSingleStrands(part, firstStrandTwo, ssN);
            part->DeregisterSingleStrand(ssN);
            part->DeregisterSingleStrand(joinStrand2);
          }
        }
      }
    }
    else {
      ssLeftOvers.first = firstStrand;
      ssLeftOvers.second = secondStrand;

      // break first nucleotide in 3'
      if (nt1->GetEnd() != ThreePrime) {
        secondNext = nt1->GetNext(true);
        auto pair1 = ADNBasicOperations::BreakSingleStrand(part, secondNext);
        secondStrand = pair1.first;
        firstStrandTwo = pair1.second;
      }
      // break second nucleotide in 5'
      if (nt2->GetEnd() != FivePrime) {
        firstPrev = nt2->GetPrev(true);
        auto pair2 = ADNBasicOperations::BreakSingleStrand(part, firstPrev);
        firstStrand = pair2.second;
        secondStrandTwo = pair2.first;
      }

      /*if (joinStrand1 != nullptr) {
        auto ssN = ADNBasicOperations::MergeSingleStrands(part, joinStrand1, firstStrand);
        auto ssNN = ADNBasicOperations::MergeSingleStrands(part, secondStrand, ssN);
        part->DeregisterSingleStrand(ssN);
        part->DeregisterSingleStrand(joinStrand1);
      }
      else {
        auto ssConnect = ADNBasicOperations::MergeSingleStrands(part, secondStrand, firstStrand);
      }

      if (two && firstStrandTwo != nullptr && secondStrandTwo != nullptr) {
        auto ssN = ADNBasicOperations::MergeSingleStrands(part, joinStrand2, firstStrandTwo);
        auto ssNN = ADNBasicOperations::MergeSingleStrands(part, secondStrandTwo, ssN);
        part->DeregisterSingleStrand(ssN);
        part->DeregisterSingleStrand(joinStrand2);
      }*/
    }

    if (firstStrand->getNumberOfNucleotides() == 0) ssLeftOvers.third = firstStrand;
    if (secondStrand->getNumberOfNucleotides() == 0) ssLeftOvers.fourth = secondStrand;
    if (firstStrandTwo != nullptr && firstStrandTwo->getNumberOfNucleotides() == 0) ssLeftOvers.fifth = firstStrandTwo;
    if (secondStrandTwo != nullptr && secondStrandTwo->getNumberOfNucleotides() == 0) ssLeftOvers.sixth = secondStrandTwo;
  }

  return ssLeftOvers;
}

void DASOperations::CreateDoubleCrossover(ADNPointer<ADNPart> part, ADNPointer<ADNNucleotide> nt11, ADNPointer<ADNNucleotide> nt12, 
  ADNPointer<ADNNucleotide> nt21, ADNPointer<ADNNucleotide> nt22, std::string seq)
{
  auto ssLeftOvers = CreateCrossover(part, nt11, nt12, true, seq);
  if (ssLeftOvers.first != nullptr) part->DeregisterSingleStrand(ssLeftOvers.first);
  if (ssLeftOvers.second != nullptr) part->DeregisterSingleStrand(ssLeftOvers.second);
  if (ssLeftOvers.third != nullptr) part->DeregisterSingleStrand(ssLeftOvers.third);
  if (ssLeftOvers.fourth != nullptr) part->DeregisterSingleStrand(ssLeftOvers.fourth);
  if (ssLeftOvers.fifth != nullptr) part->DeregisterSingleStrand(ssLeftOvers.fifth);
  if (ssLeftOvers.sixth != nullptr) part->DeregisterSingleStrand(ssLeftOvers.sixth);

  ssLeftOvers = CreateCrossover(part, nt21, nt22, true, seq);
  if (ssLeftOvers.first != nullptr) part->DeregisterSingleStrand(ssLeftOvers.first);
  if (ssLeftOvers.second != nullptr) part->DeregisterSingleStrand(ssLeftOvers.second);
  if (ssLeftOvers.third != nullptr) part->DeregisterSingleStrand(ssLeftOvers.third);
  if (ssLeftOvers.fourth != nullptr) part->DeregisterSingleStrand(ssLeftOvers.fourth);
  if (ssLeftOvers.fifth != nullptr) part->DeregisterSingleStrand(ssLeftOvers.fifth);
  if (ssLeftOvers.sixth != nullptr) part->DeregisterSingleStrand(ssLeftOvers.sixth);

  // create crossover
  int dist = 6;
  ADNPointer<ADNNucleotide> ntXO1 = nt11;
  ADNPointer<ADNNucleotide> ntXO2 = nt21;

  for (int i = 0; i < dist; ++i) {
    ntXO1 = ntXO1->GetNext();
    ntXO2 = ntXO2->GetPrev();
  }
  ntXO2 = ntXO2->GetPair();

  if (ntXO2 != nullptr) {
    /*ssLeftOvers = CreateCrossover(part, ntXO1, ntXO2, true);
    if (ssLeftOvers.first != nullptr) part->DeregisterSingleStrand(ssLeftOvers.first);
    if (ssLeftOvers.second != nullptr) part->DeregisterSingleStrand(ssLeftOvers.second);
    if (ssLeftOvers.third != nullptr) part->DeregisterSingleStrand(ssLeftOvers.third);
    if (ssLeftOvers.fourth != nullptr) part->DeregisterSingleStrand(ssLeftOvers.fourth);
    if (ssLeftOvers.fifth != nullptr) part->DeregisterSingleStrand(ssLeftOvers.fifth);
    if (ssLeftOvers.sixth != nullptr) part->DeregisterSingleStrand(ssLeftOvers.sixth);*/
  }
}
