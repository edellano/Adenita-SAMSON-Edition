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

  SixSingleStrands LinkSingleStrands(ADNPointer<ADNPart> part, ADNPointer<ADNSingleStrand> first_strand, ADNPointer<ADNSingleStrand> second_strand, std::string seq = "");
  ADNPointer<ADNDoubleStrand> CreateDoubleStrandsBetweenSingleStrands(ADNPointer<ADNDoubleStrand> first_strand, ADNPointer<ADNDoubleStrand> second_strand, std::string seq);
  std::pair<ADNPointer<ADNDoubleStrand>, ADNPointer<ADNDoubleStrand>> LinkDoubleStrandsWithXO(ADNPointer<ADNDoubleStrand> first_strand, ADNPointer<ADNDoubleStrand> second_strand);

  SixSingleStrands CreateCrossover(ADNPointer<ADNPart> part, ADNPointer<ADNNucleotide> nt1, ADNPointer<ADNNucleotide> nt2, bool two = false);
}