#pragma once

#include "ADNBasicOperations.hpp"
#include "DASCreator.hpp"


namespace DASOperations {
  struct MergePair {
    ADNPointer<ADNSingleStrand> first = nullptr;
    ADNPointer<ADNSingleStrand> second = nullptr;
    ADNPointer<ADNPart> firstPart = nullptr;
    ADNPointer<ADNPart> secondPart = nullptr;
  };
  struct Connections {
    MergePair stringPair;
    MergePair compStringPair;
  };

  Connections PrepareStrandsForConnection(ADNPointer<ADNPart> part1, ADNPointer<ADNPart> part2, ADNPointer<ADNNucleotide> nt1, ADNPointer<ADNNucleotide> nt2);

  void CreateCrossover(ADNPointer<ADNPart> part1, ADNPointer<ADNPart> part2, ADNPointer<ADNNucleotide> nt1, 
    ADNPointer<ADNNucleotide> nt2, bool two = false, std::string seq = "");

  void AddComplementaryStrands(ADNNanorobot* nanorobot, CollectionMap<ADNNucleotide> selectedNucleotides);
}