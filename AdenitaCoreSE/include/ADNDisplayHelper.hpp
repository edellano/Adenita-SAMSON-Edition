#pragma once

#include <SAMSON.hpp>
#include "ADNAuxiliary.hpp"
#include <iostream>
#include <iomanip>
#include "ADNVectorMath.hpp"
//#include "ANTPolyhedron.hpp"
#include "ADNPart.hpp"
#include "ADNArray.hpp"
#include "SEConfig.hpp"

namespace ADNDisplayHelper {

  void displayLine(SBPosition3 start, SBPosition3 end, std::string text = "");
  void displayCylinder(SBPosition3 start, SBPosition3 end, std::string text = "");
  void displayLine(ublas::vector<double> center, ublas::vector<double> dir, int length);
  void displayVector(SBVector3 vec, SBPosition3 shift);
  void displayVector(SBVector3 vec, SBPosition3 shift, float * color, int length);
  void displayArrow(SBVector3 vec, SBPosition3 shift);
  void displayArrow(SBPosition3 start, SBPosition3 end, unsigned int nodeIndex, float * color, bool selectable = false);
  void displayLengthText(SBPosition3 start, SBPosition3 end, std::string text = "");
  void displayDirectedCylinder(SBPosition3 start, SBPosition3 end);
  void displayDirectedCylinder(SBPosition3 start, SBPosition3 end, float * color, int radius);
  void displayPlane(SBVector3 vec, SBPosition3 shift);
  void displayOrthoPlane(SBVector3 vec, SBPosition3 shift);
  void displaySphere(SBPosition3 pos, float radius);
  //! Display only the top scales of a part
  /*!
    \param ADNPointer to the ADNPart
  */
  void displayPart(ADNPointer<ADNPart> part);
};

