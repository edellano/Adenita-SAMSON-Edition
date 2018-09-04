#pragma once

#include "ADNNanorobot.hpp"
#include "ADNConstants.hpp"
#include "ADNBasicOperations.hpp"

// type of editor enumeration

namespace DASCreator {

  enum EditorType {
    DoubleStrand = 0,
    Nanotube = 1,
    TwoDoubleHelices = 2,

    WireframeTetrahedron = 10,
    WireframeCube = 11,
    WireframeDodecahedron = 12,
    WireframeOctahedron = 13,
    WireframeIcosahedron = 14,

    WireframeCuboid = 20
  };

  //! Creates a double strand
  /*!
    \param the length of the double strand in base pairs
    \param starting point of the double strand
    \param direction vector
  */
  ADNPointer<ADNDoubleStrand> CreateDoubleStrand(ADNPointer<ADNPart> part, int length, SBPosition3 start, SBVector3 direction);
  //! Creates a single strand
  /*!
    \param the length of the double strand in base pairs
    \param starting point of the double strand
    \param direction vector
  */
  ADNPointer<ADNSingleStrand> CreateSingleStrand(ADNPointer<ADNPart> part, int length, SBPosition3 start, SBVector3 direction);

  ADNPointer<ADNLoop> CreateLoop(ADNPointer<ADNSingleStrand> ss, ADNPointer<ADNNucleotide> nextNt, std::string seq, ADNPointer<ADNPart> part = nullptr);

  //! Creates a ADNPart containing a nanotube
  /*!
    \param the radius of the nanotube
    \param position of the center of the bottom circumpherence
    \param direction vector towards which to grow the nanotube
    \param length of the nanotube in base pairs
    \param whether to create a mock part containing only the high-level details
  */
  ADNPointer<ADNPart> CreateNanotube(SBQuantity::length radius, SBPosition3 center, SBVector3 direction, int length, bool mock = false);
  //! Creates a ADNPart containing only the high-level model (double strands) of a nanotube for displaying purposes
  /*!
    \param the radius of the nanotube
    \param position of the center of the bottom circumpherence
    \param direction vector towards which to grow the nanotube
    \param length of the nanotube in base pairs
  */
  ADNPointer<ADNPart> CreateMockNanotube(SBQuantity::length radius, SBPosition3 center, SBVector3 direction, int length);

  //! Creates a ADNPart containing a double stranded DNA Ring
  /*!
    \param the radius of the ring
    \param position of the center of the ring
    \param normal to the ring
    \param whether to create a mock part containing only the high-level details
  */
  ADNPointer<ADNPart> CreateDSRing(SBQuantity::length radius, SBPosition3 center, SBVector3 normal, bool mock = false);

  //ADNPart* CreateTwoTubes(size_t length, SBPosition3 start, SBVector3 direction, SBVector3 sepDir);
  
  //! Helper function that creates a double strand in a ADNPart
  /*!
    \param the ADNPart to which the double strand will be added
    \param length of the double strand in base pairs
    \param position of the 5' in space
    \param direction of the double strand
  */
  ADNPointer<ADNDoubleStrand> AddDoubleStrandToADNPart(ADNPointer<ADNPart> part, size_t length, SBPosition3 start, SBVector3 direction);

  //! Helper function that adds a double strand containing only the high-level model (base segments) to a mock ADNPart
  /*!
  \param the ADNPart to which the double strand will be added
  \param length of the double strand in base pairs
  \param position of the 5' in space
  \param direction of the double strand
  */
  ADNPointer<ADNDoubleStrand> AddMockDoubleStrandToADNPart(ADNPointer<ADNPart> part, size_t length, SBPosition3 start, SBVector3 direction);

  //! Helper function that creates a single strand B-DNA like in a ADNPart
  /*!
    \param the ADNPart to which the double strand will be added
    \param length of the single strand in bases
    \param position of the 5' in space
    \param direction of the double strand
  */
  ADNPointer<ADNSingleStrand> AddSingleStrandToADNPart(ADNPointer<ADNPart> part, size_t length, SBPosition3 start, SBVector3 direction);

  //* Generates a crippled cuboid for viewing purposes
  //*/
  //ADNPart* CreateCrippledWireframeCuboid(SBPosition3 topLeft, int xSize, int ySize, int zSize);

  ///** Generates a small system for debuging crossovers purposes
  // */
  //ADNPart* DebugCrossoversPart();
};