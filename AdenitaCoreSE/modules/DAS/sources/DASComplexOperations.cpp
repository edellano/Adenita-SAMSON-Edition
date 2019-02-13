#include "DASComplexOperations.hpp"
#include "DASBackToTheAtom.hpp"

DASOperations::SixSingleStrands DASOperations::LinkSingleStrands(ADNPointer<ADNPart> part, ADNPointer<ADNSingleStrand> first_strand, ADNPointer<ADNSingleStrand> second_strand, std::string seq)
{
  int length = boost::numeric_cast<int>(seq.size());

  ADNPointer<ADNNucleotide> threePrime = first_strand->GetThreePrime();
  ADNPointer<ADNNucleotide> fivePrime = second_strand->GetFivePrime();

  SBVector3 direction = (fivePrime->GetPosition() - threePrime->GetPosition()).normalizedVersion();
  SBQuantity::length expectedLength = SBQuantity::nanometer(ADNConstants::BP_RISE) * (length + 1);  // we need to accomodate space for distance between the ends
  SBPosition3 startPos = (fivePrime->GetPosition() + threePrime->GetPosition())*0.5 - expectedLength*0.5*direction;

  ADNPointer<ADNSingleStrand> ss = DASCreator::CreateSingleStrand(part, length, startPos, direction);
  ss->create();
  DASBackToTheAtom* btta = new DASBackToTheAtom();
  btta->SetPositionsForNewNucleotides(part, ss->GetNucleotides());

  auto ssN = ADNBasicOperations::MergeSingleStrands(part, first_strand, ss);
  auto ssNN = ADNBasicOperations::MergeSingleStrands(part, ssN, second_strand);
  part->DeregisterSingleStrand(ss);
  part->DeregisterSingleStrand(ssN);

  DASOperations::SixSingleStrands res;
  res.first = ssNN;

  return res;
}

//ADNPointer<ADNDoubleStrand> DASOperations::CreateDoubleStrandsBetweenSingleStrands(ADNPointer<ADNDoubleStrand> first_strand, ADNPointer<ADNDoubleStrand> second_strand, std::string seq)
//{
//  int length = boost::numeric_cast<int>(seq.size());
//
//  ADNPointer<ADNBaseSegment> endBs = first_strand->GetLastBaseSegment();
//  ADNPointer<ADNBaseSegment> startBs = second_strand->GetFirstBaseSegment();
//
//  SBVector3 direction = (startBs->GetPosition() - endBs->GetPosition()).normalizedVersion();
//  SBQuantity::length expectedLength = SBQuantity::nanometer(ADNConstants::BP_RISE) * (length + 1);  // we need to accomodate space for distance between the ends
//  SBPosition3 startPos = (startBs->GetPosition() + endBs->GetPosition())*0.5 - expectedLength*0.5*direction;
//
//  ADNPointer<ADNDoubleStrand> ds = DASEditor::CreateDoubleStrand(length, startPos, direction);
//
//  return ds;
//}

DASOperations::SixSingleStrands DASOperations::CreateCrossover(ADNPointer<ADNPart> part, ADNPointer<ADNNucleotide> nt1, ADNPointer<ADNNucleotide> nt2, bool two)
{
  SixSingleStrands ssLeftOvers;

  ADNPointer<ADNSingleStrand> firstStrand = nt1->GetStrand();
  ADNPointer<ADNSingleStrand> secondStrand = nt2->GetStrand();

  bool sameStrand = firstStrand == secondStrand;

  if (nt1->IsEnd() && nt2->IsEnd()) {
    if (sameStrand) firstStrand->IsCircular(true);
    else {
      ADNPointer<ADNNucleotide> fPrime = nt1;
      ADNPointer<ADNNucleotide> tPrime = nt2;

      if (fPrime->GetEnd() == ThreePrime && tPrime->GetEnd() == FivePrime) {
        fPrime = nt2;
        tPrime = nt1;
      }
      auto fPrimeStrand = fPrime->GetStrand();
      auto tPrimeStrand = tPrime->GetStrand();
      auto resSs = ADNBasicOperations::MergeSingleStrands(part, tPrimeStrand, fPrimeStrand);
      ssLeftOvers.first = firstStrand;
      ssLeftOvers.second = secondStrand;
    }    
  }
  else {
      
    ADNPointer<ADNSingleStrand> firstStrandTwo = nullptr;
    ADNPointer<ADNSingleStrand> secondStrandTwo = nullptr;

    bool circResOne = false;
    bool circResTwo = false;

    ssLeftOvers.first = firstStrand;
    ssLeftOvers.second = secondStrand;

    bool caseOne = true;
    // order nucleotides
    if (sameStrand) {
      auto orderedNt = ADNBasicOperations::OrderNucleotides(nt1, nt2);
      if (orderedNt.first == nt2 && orderedNt.second == nt1) {
        caseOne = false;
        circResOne = true;
        circResTwo = firstStrand->IsCircular();
      }
      else {
        circResOne = firstStrand->IsCircular();
        circResTwo = true;
      }

      ssLeftOvers.second = nullptr;
    }

    if (caseOne) {
      // break first nucleotide in 3'
      if (nt1->GetEnd() != ThreePrime) {
        auto ntNext = nt1->GetNext();
        auto pair1 = ADNBasicOperations::BreakSingleStrand(part, ntNext);
        firstStrand = pair1.first;
        secondStrandTwo = pair1.second;
      }
      // break second nucleotide in 5'
      if (nt2->GetEnd() != FivePrime) {
        auto pair2 = ADNBasicOperations::BreakSingleStrand(part, nt2);
        secondStrand = pair2.second;
        firstStrandTwo = pair2.first;
      }
    }
    else {
      // break first nucleotide in 3'
      if (nt1->GetEnd() != FivePrime) {
        auto pair1 = ADNBasicOperations::BreakSingleStrand(part, nt1);
        firstStrand = pair1.second;
        secondStrandTwo = pair1.first;
      }
      // break second nucleotide in 5'
      if (nt2->GetEnd() != ThreePrime) {
        auto ntNext = nt2->GetNext();
        auto pair2 = ADNBasicOperations::BreakSingleStrand(part, ntNext);
        secondStrand = pair2.first;
        firstStrandTwo = pair2.second;
      }
    }
    // connect strands
    if (sameStrand) {
 
      if (firstStrand->getNumberOfNucleotides() == 0) {
        ssLeftOvers.third = firstStrand;
        secondStrand->IsCircular(true);
      }

      if (secondStrand->getNumberOfNucleotides() == 0) {
        ssLeftOvers.fourth = secondStrand;
        firstStrand->IsCircular(true);
      }

      if (firstStrandTwo->getNumberOfNucleotides() == 0) {
        ssLeftOvers.fifth = firstStrandTwo;
        firstStrand->IsCircular(true);
      }

      if (secondStrandTwo->getNumberOfNucleotides() == 0) {
        ssLeftOvers.sixth = secondStrandTwo;
        secondStrand->IsCircular(true);
      }

      if (two && firstStrandTwo != nullptr && secondStrandTwo != nullptr) {
        if (firstStrandTwo == secondStrandTwo) {
          firstStrandTwo->IsCircular(true);
        }
        else {
          auto ssConnectTwo = ADNBasicOperations::MergeSingleStrands(part, firstStrandTwo, secondStrandTwo);
          ssConnectTwo->IsCircular(circResTwo);
          ssLeftOvers.fifth = firstStrandTwo;
          ssLeftOvers.sixth = secondStrandTwo;
        }
      }
    }
    else {
      auto ssConnect = ADNBasicOperations::MergeSingleStrands(part, firstStrand, secondStrand);
      ssLeftOvers.third = firstStrand;
      ssLeftOvers.fourth = secondStrand;
      ssConnect->IsCircular(circResOne);
      if (two && firstStrandTwo != nullptr && secondStrandTwo != nullptr) {
        auto ssConnectTwo = ADNBasicOperations::MergeSingleStrands(part, firstStrandTwo, secondStrandTwo);
        ssConnectTwo->IsCircular(circResTwo);
        ssLeftOvers.fifth = firstStrandTwo;
        ssLeftOvers.sixth = secondStrandTwo;
      }
    }
  }

  return ssLeftOvers;
}
