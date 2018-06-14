#pragma once

#include "ANTNanorobot.hpp"
#include "ANTConstants.hpp"
#include "DASBackToTheAtom.hpp"
#include "DASCadnano.hpp"

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

class DASEditor {
public:
  DASEditor() { configuration_ = new SEConfig(); }
  ~DASEditor() = default;
  /**
   * Creates a straight double helix between the given spatial points
   */
  ANTPart* CreateTube(size_t length, SBPosition3 start, SBVector3 direction);
  ANTPart* CreateRoundOrigami(SBQuantity::length diameter, SBPosition3 center, SBVector3 direction, int min_length);
  ANTPart* CreateTwoTubes(size_t length, SBPosition3 start, SBVector3 direction, SBVector3 sepDir);
  void AddDoubleStrandToANTPart(ANTPart* part, size_t length, SBPosition3 start, SBVector3 direction);
  /**
   * Generates a Round Structure with only the double strands defined for preview purposes
   */
  ANTPart* CreateCrippledRoundOrigami(SBQuantity::length diameter, SBPosition3 center, SBVector3 direction, int length);
  /**
  * Generates a crippled cuboid for viewing purposes
  */
  ANTPart* CreateCrippledWireframeCuboid(SBPosition3 topLeft, int xSize, int ySize, int zSize);
  void AddCrippledDoubleStrandToANTPart(SBPosition3 dsPosition, SBVector3 direction, int length, ANTPart& nanorobot);

  /** Generates a small system for debuging crossovers purposes
   */
  ANTPart* DebugCrossoversPart();

  SEConfig * configuration_;
};