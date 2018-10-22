#pragma once

#include "SBMStructuralModelConformation.hpp"


using ADNConformation = SBMStructuralModelConformation;

////! ADNConformation: set different positions for atoms of a ADNPart (e.g. 2D or 1D)
//class ADNConformation : public SBMStructuralModelConformation  {
//  SB_CLASS
//public:
//  //! Default constructor
//  ADNConformation() : SBMStructuralModelConformation() {};
//  ADNConformation(const std::string &name, const SBNodeIndexer &nodeIndexer) : SBMStructuralModelConformation(name, nodeIndexer) {};
//  //! Copy constructor
//  ADNConformation(const ADNConformation &n);
//  //! Default destructor
//  ~ADNConformation() = default;
//
//  //! Operator =
//  ADNConformation& operator=(const ADNConformation& other);
//
//private:
//
//};
//
//SB_REGISTER_TARGET_TYPE(ADNConformation, "ADNConformation", "AF507697-7D76-AA64-B8DE-24808FCFFAA4");
//SB_DECLARE_BASE_TYPE(ADNConformation, SBMStructuralModelConformation);