#pragma once

#include "ADNBasicOperations.hpp"
#include "DASCreator.hpp"


namespace DASOperations {
  // return structures
  struct FourSingleStrands {
    ADNPointer<ADNSingleStrand> first;
    ADNPointer<ADNSingleStrand> second;
    ADNPointer<ADNSingleStrand> third;
    ADNPointer<ADNSingleStrand> fourth;
  };

  // concatenate
  ADNPointer<ADNSingleStrand> LinkSingleStrands(ADNPointer<ADNSingleStrand> first_strand, ADNPointer<ADNSingleStrand> second_strand, std::string seq = "");
  ADNPointer<ADNDoubleStrand> CreateDoubleStrandsBetweenSingleStrands(ADNPointer<ADNDoubleStrand> first_strand, ADNPointer<ADNDoubleStrand> second_strand, std::string seq);
  std::pair<ADNPointer<ADNDoubleStrand>, ADNPointer<ADNDoubleStrand>> LinkDoubleStrandsWithXO(ADNPointer<ADNDoubleStrand> first_strand, ADNPointer<ADNDoubleStrand> second_strand);

  // break and connect
  FourSingleStrands CreateCrossover(ADNPointer<ADNPart> part, ADNPointer<ADNNucleotide> nt1, ADNPointer<ADNNucleotide> nt2);
}