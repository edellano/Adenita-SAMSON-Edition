#include "DASComplexOperations.hpp"
#include "DASBackToTheAtom.hpp"

DASOperations::FourSingleStrands DASOperations::LinkSingleStrands(ADNPointer<ADNPart> part, ADNPointer<ADNSingleStrand> first_strand, ADNPointer<ADNSingleStrand> second_strand, std::string seq)
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

  DASOperations::FourSingleStrands res;
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

DASOperations::FourSingleStrands DASOperations::CreateCrossover(ADNPointer<ADNPart> part, ADNPointer<ADNNucleotide> nt1, ADNPointer<ADNNucleotide> nt2)
{
  FourSingleStrands ssLeftOvers;

  ADNPointer<ADNNucleotide> fPrime = nt1;
  ADNPointer<ADNNucleotide> tPrime = nt2;
  if (fPrime->GetStrand() != tPrime->GetStrand()) {
    if (fPrime->IsEnd() && tPrime->IsEnd()) {
      if (fPrime->GetEnd() == ThreePrime && tPrime->GetEnd() == FivePrime) {
        fPrime = nt2;
        tPrime = nt1;
      }
      auto fPrimeStrand = fPrime->GetStrand();
      auto tPrimeStrand = tPrime->GetStrand();
      ADNBasicOperations::MergeSingleStrands(part, tPrimeStrand, fPrimeStrand);
      ssLeftOvers.first = fPrimeStrand;
      ssLeftOvers.second = tPrimeStrand;
    }
    else {
      // break first nucleotide in 3'
      ADNPointer<ADNSingleStrand> firstStrand = nt1->GetStrand();
      if (nt1->GetEnd() != ThreePrime) {
        auto ntNext = nt1->GetNext();
        auto pair1 = ADNBasicOperations::BreakSingleStrand(part, ntNext);
        ssLeftOvers.first = firstStrand;
        firstStrand = pair1.first;
      }
      // break second nucleotide in 5'
      ADNPointer<ADNSingleStrand> secondStrand = nt2->GetStrand();
      if (nt2->GetEnd() != FivePrime) {
        auto pair2 = ADNBasicOperations::BreakSingleStrand(part, nt2);
        ssLeftOvers.second = secondStrand;
        secondStrand = pair2.second;
      }
      // connect strands
      auto ssConnect = ADNBasicOperations::MergeSingleStrands(part, firstStrand, secondStrand);
      ssLeftOvers.third = firstStrand;
      ssLeftOvers.fourth = secondStrand;
    }
  }
  else {
    // make strand circular
  }

  return ssLeftOvers;
}

DASOperations::FourDoubleStrands DASOperations::CreateDsCrossover(ADNPointer<ADNPart> part, ADNPointer<ADNBaseSegment> bs1, ADNPointer<ADNBaseSegment> bs2)
{
  FourDoubleStrands dsLeftOvers;

  ADNPointer<ADNBaseSegment> firstBs = bs1;
  ADNPointer<ADNBaseSegment> lastBs = bs2;
  if (firstBs->GetDoubleStrand() != lastBs->GetDoubleStrand()) {
    if (firstBs->IsEnd() && lastBs->IsEnd()) {
      if (firstBs->IsLast() && lastBs->IsFirst()) {
        firstBs = bs2;
        lastBs = bs1;
      }
      auto firstStrand = firstBs->GetDoubleStrand();
      auto lastStrand = lastBs->GetDoubleStrand();
      ADNBasicOperations::MergeDoubleStrand(part, lastStrand, firstStrand);
      dsLeftOvers.first = firstStrand;
      dsLeftOvers.second = lastStrand;
    }
    else {
      // break first base segment so it is the last
      ADNPointer<ADNDoubleStrand> firstStrand = bs1->GetDoubleStrand();
      if (!bs1->IsLast()) {
        auto bsNext = bs1->GetNext();
        auto pair1 = ADNBasicOperations::BreakDoubleStrand(part, bsNext);
        dsLeftOvers.first = firstStrand;
        firstStrand = pair1.first;
      }
      // break second nucleotide in 5'
      ADNPointer<ADNDoubleStrand> secondStrand = bs2->GetDoubleStrand();
      if (!bs2->IsFirst()) {
        auto pair2 = ADNBasicOperations::BreakDoubleStrand(part, bs2);
        dsLeftOvers.second = secondStrand;
        secondStrand = pair2.second;
      }
      // connect trands
      auto ssConnect = ADNBasicOperations::MergeDoubleStrand(part, firstStrand, secondStrand);
      dsLeftOvers.third = firstStrand;
      dsLeftOvers.fourth = secondStrand;
    }
  }
  else {
    // make strand circular
  }

  return dsLeftOvers;
}
