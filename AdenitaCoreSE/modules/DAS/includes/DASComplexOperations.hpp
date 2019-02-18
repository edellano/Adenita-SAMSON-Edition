#pragma once

#include "ADNBasicOperations.hpp"
#include "DASCreator.hpp"


namespace DASOperations {
  // return structures
  struct SixSingleStrands {
    ADNPointer<ADNSingleStrand> first = nullptr;
    ADNPointer<ADNSingleStrand> second = nullptr;
    ADNPointer<ADNSingleStrand> third = nullptr;
    ADNPointer<ADNSingleStrand> fourth = nullptr;
    ADNPointer<ADNSingleStrand> fifth = nullptr;
    ADNPointer<ADNSingleStrand> sixth = nullptr;
  };
  struct FourDoubleStrands {
    ADNPointer<ADNDoubleStrand> first = nullptr;
    ADNPointer<ADNDoubleStrand> second = nullptr;
    ADNPointer<ADNDoubleStrand> third = nullptr;
    ADNPointer<ADNDoubleStrand> fourth = nullptr;
  };

  SixSingleStrands CreateCrossover(ADNPointer<ADNPart> part, ADNPointer<ADNNucleotide> nt1, ADNPointer<ADNNucleotide> nt2, bool two = false, std::string seq = "");
  void CreateDoubleCrossover(ADNPointer<ADNPart> part, ADNPointer<ADNNucleotide> nt11, ADNPointer<ADNNucleotide> nt12, 
    ADNPointer<ADNNucleotide> nt21, ADNPointer<ADNNucleotide> nt22, std::string seq = "");
}