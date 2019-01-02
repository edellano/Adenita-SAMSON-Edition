#pragma once

/// \headerfile SBProxy.hpp "SBProxy.hpp"
#include "SBProxy.hpp"

/// \headerfile ADNPart.hpp "ADNPart.hpp"
#include "ADNPart.hpp"


// Class descriptor

// SAMSON Element generator pro tip: complete this descriptor to expose this class to SAMSON and other SAMSON Elements

SB_CLASS_BEGIN(ADNPart);

  SB_CLASS_TYPE(SBCClass::Custom);
  SB_CLASS_DESCRIPTION("ADNPart");

  SB_FACTORY_BEGIN;

    SB_CONSTRUCTOR_0(ADNPart);

  SB_FACTORY_END;

  SB_INTERFACE_BEGIN;

    SB_ATTRIBUTE_READ_ONLY(int, ADNPart, NumberOfAtoms, "Number of atoms", "Adenita");
    SB_ATTRIBUTE_READ_ONLY(int, ADNPart, NumberOfNucleotides, "Number of nucleotides", "Adenita");
    SB_ATTRIBUTE_READ_ONLY(int, ADNPart, NumberOfSingleStrands, "Number of single strands", "Adenita");
    SB_ATTRIBUTE_READ_ONLY(int, ADNPart, NumberOfBaseSegments, "Number of base segments", "Adenita");
    SB_ATTRIBUTE_READ_ONLY(int, ADNPart, NumberOfDoubleStrands, "Number of double strands", "Adenita");
    SB_ATTRIBUTE_READ_WRITE(std::string const &, ADNPart, Name, "Name", "Adenita");

    SB_ATTRIBUTE_READ_ONLY(SBNode*, ADNPart, ThisNode, "Itself", "Node");
    SB_ATTRIBUTE_READ_ONLY(SBNode*, ADNPart, Parent, "Parent", "Node");
    SB_ATTRIBUTE_READ_ONLY(bool, ADNPart, Selected, "Selected", "Node");
    SB_ATTRIBUTE_READ_WRITE(bool, ADNPart, SelectionFlag, "Selection Flag", "Node");
    SB_ATTRIBUTE_READ_WRITE(bool, ADNPart, VisibilityFlag, "Visibility Flag", "Node");
    SB_ATTRIBUTE_READ_ONLY(bool, ADNPart, Visible, "Visible", "Node");

  SB_INTERFACE_END;

SB_CLASS_END(ADNPart);
