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

ADNPointer<ADNDoubleStrand> DASOperations::CreateDoubleStrandsBetweenSingleStrands(ADNPointer<ADNDoubleStrand> first_strand, ADNPointer<ADNDoubleStrand> second_strand, std::string seq)
{
  int length = boost::numeric_cast<int>(seq.size());

  ADNPointer<ADNBaseSegment> endBs = first_strand->GetLastBaseSegment();
  ADNPointer<ADNBaseSegment> startBs = second_strand->GetFirstBaseSegment();

  SBVector3 direction = (startBs->GetPosition() - endBs->GetPosition()).normalizedVersion();
  SBQuantity::length expectedLength = SBQuantity::nanometer(ADNConstants::BP_RISE) * (length + 1);  // we need to accomodate space for distance between the ends
  SBPosition3 startPos = (startBs->GetPosition() + endBs->GetPosition())*0.5 - expectedLength*0.5*direction;

  ADNPointer<ADNDoubleStrand> ds = DASEditor::CreateDoubleStrand(length, startPos, direction);

  return ds;
}
