#pragma once

#include "ADNPart.hpp"

// return types for operations
struct RTDoubleStrand {
  ADNPointer<ADNDoubleStrand> ds = nullptr;
  ADNPointer<ADNSingleStrand> ss1 = nullptr;
  ADNPointer<ADNSingleStrand> ss2 = nullptr;
};

namespace ADNBasicOperations {
  // Concatenate
  ADNPointer<ADNSingleStrand> MergeSingleStrands(ADNPointer<ADNPart> part, ADNPointer<ADNSingleStrand> first_strand, ADNPointer<ADNSingleStrand> second_strand);
  ADNPointer<ADNDoubleStrand> MergeDoubleStrand(ADNPointer<ADNPart> part, ADNPointer<ADNDoubleStrand> first_strand, ADNPointer<ADNDoubleStrand> second_strand);
  ADNPointer<ADNPart> MergeParts(ADNPointer<ADNPart> part1, ADNPointer<ADNPart> part2);

  // Extend
  CollectionMap<ADNNucleotide> AddNucleotidesThreePrime(ADNPointer<ADNPart> part, ADNPointer<ADNSingleStrand> ss, int number, SBVector3 dir);

  // Break
  std::pair<ADNPointer<ADNSingleStrand>, ADNPointer<ADNSingleStrand>> BreakSingleStrand(ADNPointer<ADNPart> part, ADNPointer<ADNNucleotide> nt);
  std::pair<ADNPointer<ADNDoubleStrand>, ADNPointer<ADNDoubleStrand>> BreakDoubleStrand(ADNPointer<ADNPart> part, ADNPointer<ADNBaseSegment> bs);

  // Delete
  std::pair<ADNPointer<ADNSingleStrand>, ADNPointer<ADNSingleStrand>> DeleteNucleotide(ADNPointer<ADNPart> part, ADNPointer<ADNNucleotide> nt);
  void DeleteNucleotideWithoutBreak(ADNPointer<ADNPart> part, ADNPointer<ADNNucleotide> nt);
  std::pair<ADNPointer<ADNDoubleStrand>, ADNPointer<ADNDoubleStrand>> DeleteBaseSegment(ADNPointer<ADNPart> part, ADNPointer<ADNBaseSegment> bs);
  void DeleteBaseSegmentWithoutBreak(ADNPointer<ADNBaseSegment> bs);
  void DeleteSingleStrand(ADNPointer<ADNSingleStrand> ss);
  void DeleteDoubleStrand(ADNPointer<ADNDoubleStrand> ds);

  // Mutations
  void MutateNucleotide(ADNPointer<ADNNucleotide> nt, DNABlocks newType, bool changePair = true);
  void SetSingleStrandSequence(ADNPointer<ADNSingleStrand> ss, std::string seq, bool changePair = true);
  void MutateBasePairIntoLoopPair(ADNPointer<ADNBaseSegment> bs);
  //! Shift start of the strand to the selected nucleotide and sequence.
  /*!
    \param a ADNPointer to the ADNNucleotide which should be the new 5' of its strand
    \param whether to keep the sequence as it was (reset it from new 5' on)
  */
  void SetStart(ADNPointer<ADNNucleotide> nt, bool resetSequence = false);
  //! Moves an ADNDoubleStrand to another ADNPart
  /*!
    \param ADNPointer to the part that contains the double strand
    \param ADNPointer to the new part
    \param ADNPointer to the ADNDoubleStrand we want to move
  */
  void MoveStrand(ADNPointer<ADNPart> oldPart, ADNPointer<ADNPart> part, ADNPointer<ADNDoubleStrand> ds);
  //! Moves an ADNDoubleStrand to another ADNPart
  /*!
    \param ADNPointer to the part that contains the double strand
    \param ADNPointer to the new part
    \param ADNPointer to the ADNDoubleStrand we want to move
  */
  void MoveStrand(ADNPointer<ADNPart> oldPart, ADNPointer<ADNPart> part, ADNPointer<ADNSingleStrand> ss);

  // Geometric operations
  void TwistDoubleHelix(ADNPointer<ADNDoubleStrand> ds, double deg);
  void CenterPart(ADNPointer<ADNPart> part);
  SBPosition3 CalculateCenterOfMass(ADNPointer<ADNPart> part);

  // Ordering
  std::pair<ADNPointer<ADNNucleotide>, ADNPointer<ADNNucleotide>> OrderNucleotides(ADNPointer<ADNNucleotide> nt1, ADNPointer<ADNNucleotide> nt2);
  std::pair<End, ADNPointer<ADNBaseSegment>> GetNextBaseSegment(ADNPointer<ADNNucleotide> nt);
}