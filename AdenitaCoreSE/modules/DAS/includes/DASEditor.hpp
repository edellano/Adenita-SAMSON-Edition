#pragma once

#include "ADNNanorobot.hpp"
#include "ADNConstants.hpp"
//#include "DASBackToTheAtom.hpp"
//#include "DASCadnano.hpp"

// type of editor enumeration
enum EditorType {
  DoubleStrand = 0,
  Cylinder = 1,
  TwoDoubleHelices = 2,
  
  WireframeTetrahedron = 10,
  WireframeCube = 11,
  WireframeDodecahedron = 12,
  WireframeOctahedron = 13,
  WireframeIcosahedron = 14,

  WireframeCuboid = 20
};

namespace DASEditor {

  ADNPointer<ADNDoubleStrand> CreateDoubleStrand(int length, SBPosition3 start, SBVector3 direction);
  ADNPointer<ADNSingleStrand> CreateSingleStrand(int length, SBPosition3 start, SBVector3 direction);

  ADNPointer<ADNLoop> CreateLoop(ADNPointer<ADNSingleStrand> ss, ADNPointer<ADNNucleotide> nextNt, std::string seq);

  ///**
  // * Creates a straight double helix between the given spatial points
  // */
  //ADNPart* CreateTube(size_t length, SBPosition3 start, SBVector3 direction);
  //ADNPart* CreateRoundOrigami(SBQuantity::length diameter, SBPosition3 center, SBVector3 direction, int min_length);
  //ADNPart* CreateTwoTubes(size_t length, SBPosition3 start, SBVector3 direction, SBVector3 sepDir);
  //void AddDoubleStrandToANTPart(ADNPart* part, size_t length, SBPosition3 start, SBVector3 direction);
  ///**
  // * Generates a Round Structure with only the double strands defined for preview purposes
  // */
  //ADNPart* CreateCrippledRoundOrigami(SBQuantity::length diameter, SBPosition3 center, SBVector3 direction, int length);
  ///**
  //* Generates a crippled cuboid for viewing purposes
  //*/
  //ADNPart* CreateCrippledWireframeCuboid(SBPosition3 topLeft, int xSize, int ySize, int zSize);
  //void AddCrippledDoubleStrandToANTPart(SBPosition3 dsPosition, SBVector3 direction, int length, ADNPart& nanorobot);

  ///** Generates a small system for debuging crossovers purposes
  // */
  //ADNPart* DebugCrossoversPart();
};