#pragma once

#include "ADNBasicOperations.hpp"
#include "DASEditor.hpp"


namespace DASOperations {
  // concatenate
  ADNPointer<ADNSingleStrand> LinkSingleStrands(ADNPointer<ADNSingleStrand> first_strand, ADNPointer<ADNSingleStrand> second_strand, std::string seq = "");
  ADNPointer<ADNDoubleStrand> CreateDoubleStrandsBetweenSingleStrands(ADNPointer<ADNDoubleStrand> first_strand, ADNPointer<ADNDoubleStrand> second_strand, std::string seq);
  std::pair<ADNPointer<ADNDoubleStrand>, ADNPointer<ADNDoubleStrand>> LinkDoubleStrandsWithXO(ADNPointer<ADNDoubleStrand> first_strand, ADNPointer<ADNDoubleStrand> second_strand);
}