#include "DASComplexOperations.hpp"

//ADNPointer<ADNSingleStrand> DASOperations::LinkSingleStrands(ADNPointer<ADNSingleStrand> first_strand, ADNPointer<ADNSingleStrand> second_strand, std::string seq)
//{
//  int length = boost::numeric_cast<int>(seq.size());
//
//  ADNPointer<ADNNucleotide> threePrime = first_strand->GetThreePrime();
//  ADNPointer<ADNNucleotide> fivePrime = second_strand->GetFivePrime();
//
//  SBVector3 direction = (fivePrime->GetPosition() - threePrime->GetPosition()).normalizedVersion();
//  SBQuantity::length expectedLength = SBQuantity::nanometer(ADNConstants::BP_RISE) * (length + 1);  // we need to accomodate space for distance between the ends
//  SBPosition3 startPos = (fivePrime->GetPosition() + threePrime->GetPosition())*0.5 - expectedLength*0.5*direction;
//
//  ADNPointer<ADNSingleStrand> ss = DASEditor::CreateSingleStrand(length, startPos, direction);
//  auto ssN = ADNBasicOperations::MergeSingleStrands(first_strand, ss);
//  auto ssNN = ADNBasicOperations::MergeSingleStrands(ssN, second_strand);
//
//  return ssNN;
//}

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

std::pair<ADNPointer<ADNSingleStrand>, ADNPointer<ADNSingleStrand>> DASOperations::CreateCrossover(ADNPointer<ADNPart> part, ADNPointer<ADNNucleotide> nt1, ADNPointer<ADNNucleotide> nt2)
{
  std::pair<ADNPointer<ADNSingleStrand>, ADNPointer<ADNSingleStrand>> ssPair;

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
      ssPair.first = fPrimeStrand;
      ssPair.second = tPrimeStrand;
    }
    else {
      // break first nucleotide in 3'
      ADNPointer<ADNSingleStrand> firstStrand = nt1->GetStrand();
      if (nt1->GetEnd() != ThreePrime) {
        auto ntNext = nt1->GetNext();
        auto pair1 = ADNBasicOperations::BreakSingleStrand(part, ntNext);
        ssPair.first = firstStrand;
        firstStrand = pair1.first;
      }
      // break second nucleotide in 5'
      ADNPointer<ADNSingleStrand> secondStrand = nt2->GetStrand();
      if (nt2->GetEnd() != FivePrime) {
        auto pair2 = ADNBasicOperations::BreakSingleStrand(part, nt2);
        ssPair.second = secondStrand;
        secondStrand = pair2.second;
      }
      // connect trands
      //auto ssConnect = ADNBasicOperations::MergeSingleStrands(part, firstStrand, secondStrand);
    }
  }

  return ssPair;
}
