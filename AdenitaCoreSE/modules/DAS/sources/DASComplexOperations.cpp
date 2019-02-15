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

      if (joinStrand1 != nullptr) {
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
      }
    }

    if (firstStrand->getNumberOfNucleotides() == 0) ssLeftOvers.third = firstStrand;
    if (secondStrand->getNumberOfNucleotides() == 0) ssLeftOvers.fourth = secondStrand;
    if (firstStrandTwo != nullptr && firstStrandTwo->getNumberOfNucleotides() == 0) ssLeftOvers.fifth = firstStrandTwo;
    if (secondStrandTwo != nullptr && secondStrandTwo->getNumberOfNucleotides() == 0) ssLeftOvers.sixth = secondStrandTwo;
  }

  return ssLeftOvers;
}
