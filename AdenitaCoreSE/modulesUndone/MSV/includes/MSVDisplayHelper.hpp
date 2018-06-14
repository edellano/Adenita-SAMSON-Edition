#pragma once

#include <SAMSON.hpp>
#include "ANTAuxiliary.hpp"
#include <iostream>
#include <iomanip>
#include "ANTVectorMath.hpp"
#include "ANTPolyhedron.hpp"
#include "ANTPart.hpp"

namespace MSVDisplayHelper {


  //display aux functions
  void displayLine(SBPosition3 start, SBPosition3 end, std::string text = "");
  void displayLine(ublas::vector<double> center, ublas::vector<double> dir, int length);
  void displayVector(SBVector3 vec, SBPosition3 shift);
  void displayVector(SBVector3 vec, SBPosition3 shift, float * color, int length);
  void displayArrow(SBVector3 vec, SBPosition3 shift);
  void displayArrow(SBPosition3 start, SBPosition3 end, unsigned int nodeIndex, float * color, bool selectable = false);
  void displayLengthText(SBPosition3 start, SBPosition3 end, std::string text = "");
  void displayDirectedCylinder(SBPosition3 start, SBPosition3 end);
  void displayDirectedCylinder(SBPosition3 start, SBPosition3 end, float * color, int radius);
  void displayPlane(SBVector3 vec, SBPosition3 shift);
  void displayTriangleMesh(ANTPolyhedron * polyhedron);
  void displayPolyhedron(ANTPart * part);
  void displayOrthoPlane(SBVector3 vec, SBPosition3 shift);
  void displayInfoPanel(ANTPart* part);
  void displaySphere(SBPosition3 pos, float radius);
  //void displayLoop(ANTNucleotide * start, ANTNucleotide * end, ANTNucleotideList loopNts);
};

