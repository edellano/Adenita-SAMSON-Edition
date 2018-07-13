#pragma once

#include "ADNConstants.hpp"
#include "ADNPart.hpp"


//! ADNNanorobot: main interface to Adenita logical model.
class ADNNanorobot : public Nameable, public Positionable, public Orientable {
public:
  //! Default constructor
  ADNNanorobot() : Nameable(), Positionable(), Orientable() {};
  //! Copy constructor
  ADNNanorobot(const ADNNanorobot &n);
  //! Default destructor
  ~ADNNanorobot() = default;

  //! Operator =
  ADNNanorobot& operator=(const ADNNanorobot& other);

  //! Returns the number of double strands
  int GetNumberOfDoubleStrands();
  //! Returns the number of base segments
  int GetNumberOfBaseSegments();
  //! Returns the number of single strands
  int GetNumberOfSingleStrands();
  //! Returns the number of nucleotides
  int GetNumberOfNucleotides();

  //! Returns all the registered ADNPart
  CollectionMap<ADNPart> GetParts() const;
  //! Returns all the registered ADNSingleStrand
  CollectionMap<ADNSingleStrand> GetSingleStrands() const;

  //! Adds an ADNPart to the internal part index
  /*!
    \param a pointer to the ADNPart you want to add. The ADNPart is added to the current layer.
  */
  void RegisterPart(ADNPointer<ADNPart> part);
  //! Deletes an ADNPart from the internal part index
  /*!
    \param a pointer to the ADNPart you want to delete from the index. The ADNPart is removed from the parent.
  */
  void DeregisterPart(ADNPointer<ADNPart> part);

  //! Return the position of a nucleotide
  /*!
    \param a ADNPointer to a ADNNucleotide
    \return the positon of the nucleotide as SBPosition3
  */
  SBPosition3 GetNucleotidePosition(ADNPointer<ADNNucleotide> nt);

  //! Hides center "mock" atom
  /*!
    \param a ADNPointer to a ADNNucleotide
  */
  void HideCenterAtoms(ADNPointer<ADNNucleotide> nt);

private:
  CollectionMap<ADNPart> partsIndex_;
};