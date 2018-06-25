#pragma once

#include "ADNPart.hpp"

namespace ADNBasicOperations {
  // Concatenate
  ADNPointer<ADNSingleStrand> MergeSingleStrands(ADNPointer<ADNSingleStrand> first_strand, ADNPointer<ADNSingleStrand> second_strand);
  ADNPointer<ADNSingleStrand> MergeSingleStrands(ADNPointer<ADNPart> part, ADNPointer<ADNSingleStrand> first_strand, ADNPointer<ADNSingleStrand> second_strand);
  ADNPointer<ADNDoubleStrand> MergeDoubleStrand(ADNPointer<ADNDoubleStrand> first_strand, ADNPointer<ADNDoubleStrand> second_strand);
  ADNPointer<ADNDoubleStrand> MergeDoubleStrand(ADNPointer<ADNPart> part, ADNPointer<ADNDoubleStrand> first_strand, ADNPointer<ADNDoubleStrand> second_strand);
  ADNPointer<ADNPart> MergeParts(ADNPointer<ADNPart> part1, ADNPointer<ADNPart> part2);

  // Break
  std::pair<ADNPointer<ADNSingleStrand>, ADNPointer<ADNSingleStrand>> BreakSingleStrand(ADNPointer<ADNNucleotide> nt);
  std::pair<ADNPointer<ADNDoubleStrand>, ADNPointer<ADNDoubleStrand>> BreakDoubleStrand(ADNPointer<ADNBaseSegment> bs);

  // Delete
  std::pair<ADNPointer<ADNSingleStrand>, ADNPointer<ADNSingleStrand>> DeleteNucleotide(ADNPointer<ADNNucleotide> nt);
  std::pair<ADNPointer<ADNDoubleStrand>, ADNPointer<ADNDoubleStrand>> DeleteBaseSegment(ADNPointer<ADNBaseSegment> bs);
  void DeleteSingleStrand(ADNPointer<ADNSingleStrand> ss);
  void DeleteDoubleStrand(ADNPointer<ADNDoubleStrand> ds);

  // Mutations
  void MutateNucleotide(ADNPointer<ADNNucleotide> nt, DNABlocks newType, bool changePair = true);
  void SetSingleStrandSequence(ADNPointer<ADNSingleStrand> ss, std::string seq, bool changePair = true);
  void MutateBasePairIntoLoopPair(ADNPointer<ADNBaseSegment> bs);

  // Geometric operations
  void TwistDoubleHelix(ADNPointer<ADNDoubleStrand> ds, double deg);

  // Ordering
  std::pair<ADNPointer<ADNNucleotide>, ADNPointer<ADNNucleotide>> OrderNucleotides(ADNPointer<ADNNucleotide> nt1, ADNPointer<ADNNucleotide> nt2);
}