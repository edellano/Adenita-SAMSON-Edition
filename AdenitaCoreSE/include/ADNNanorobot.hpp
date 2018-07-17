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
  //! Return the single strands of an ADNPart
  /*!
    \param a ADNPointer to a ADNPart
    \return a CollectionMap of ADNSingleStrand
  */
  CollectionMap<ADNSingleStrand> GetSingleStrands(ADNPointer<ADNPart> p);
  //! Return the double strands of an ADNPart
  /*!
    \param a ADNPointer to a ADNPart
    \return a CollectionMap of ADNSingleStrand
  */
  CollectionMap<ADNDoubleStrand> GetDoubleStrands(ADNPointer<ADNPart> p);

  //! Return the nucleotides of a single strand
  /*!
    \param a ADNPointer to a ADNSingleStrand
    \return a CollectionMap of ADNNucleotides
  */
  CollectionMap<ADNNucleotide> GetSingleStrandNucleotides(ADNPointer<ADNSingleStrand> ss);

  //! Return the single strand five prime nucleotide
  /*!
    \param a ADNPointer to a ADNSingleStrand
    \return a ADNPointer to the five prime ADNNucleotide
  */
  ADNPointer<ADNNucleotide> GetSingleStrandFivePrime(ADNPointer<ADNSingleStrand> ss);

  //! Return whether a single strand is a scaffold
  /*!
    \param a ADNPointer to a ADNSingleStrand
    \return a bool value true if it is a scaffold
  */
  bool IsScaffold(ADNPointer<ADNSingleStrand> ss);

  //! Return if the nucleotide is 5', 3', neither or both
  /*!
    \param a ADNPointer to a ADNNucleotide
    \return the End position on the strand
  */
  End GetNucleotideEnd(ADNPointer<ADNNucleotide> nt);

  //! Return the nucleotide next on the single strand
  /*!
    \param a ADNPointer to a ADNNucleotide
    \return a ADNPointer to the ADNNucleotide next on the single strand
  */
  ADNPointer<ADNNucleotide> GetNucleotideNext(ADNPointer<ADNNucleotide> nt);

  //! Return the position of a nucleotide
  /*!
    \param a ADNPointer to a ADNNucleotide
    \return the positon of the nucleotide as SBPosition3
  */
  SBPosition3 GetNucleotidePosition(ADNPointer<ADNNucleotide> nt);

  //! Return the position of the backbone of a nucleotide
  /*!
    \param a ADNPointer to a ADNNucleotide
    \return the positon of the backbone of the nucleotide as SBPosition3
  */
  SBPosition3 GetNucleotideBackbonePosition(ADNPointer<ADNNucleotide> nt);

  //! Return the position of the sidechain of a nucleotide
  /*!
    \param a ADNPointer to a ADNNucleotide
    \return the positon of the sidechain of the nucleotide as SBPosition3
  */
  SBPosition3 GetNucleotideSidechainPosition(ADNPointer<ADNNucleotide> nt);

  //! Hides center "mock" atom
  /*!
    \param a ADNPointer to a ADNNucleotide
  */
  void HideCenterAtoms(ADNPointer<ADNNucleotide> nt);

private:
  CollectionMap<ADNPart> partsIndex_;
};