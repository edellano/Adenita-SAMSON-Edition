#include "ADNConformations.hpp"


ADNConformation::ADNConformation(const ADNConformation & n) : SBMStructuralModelConformation(n)
{
  *this = n;
}

ADNConformation & ADNConformation::operator=(const ADNConformation& other)
{
  SBMStructuralModelConformation::operator =(other);

  return *this;
}
