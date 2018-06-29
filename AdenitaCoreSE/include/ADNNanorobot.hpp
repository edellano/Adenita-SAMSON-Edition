#pragma once

#include "ADNConstants.hpp"
#include "ADNPart.hpp"


class ADNNanorobot : public Nameable, public Positionable, public Orientable {
public:
  ADNNanorobot() : Nameable(), Positionable(), Orientable() {};
  ADNNanorobot(const ADNNanorobot &n);
  ~ADNNanorobot() = default;

  /* Operators */
  ADNNanorobot& operator=(const ADNNanorobot& other);

  int GetNumberOfDoubleStrands();
  int GetNumberOfBaseSegments();
  int GetNumberOfSingleStrands();
  int GetNumberOfNucleotides();

  CollectionMap<ADNPart> GetParts() const;
  CollectionMap<ADNSingleStrand> GetSingleStrands() const;

private:
};