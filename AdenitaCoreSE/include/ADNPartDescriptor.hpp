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

    SB_ATTRIBUTE_READ_ONLY(int, ADNPart, NumberOfAtoms, "Atoms", "Number of atoms");
    SB_ATTRIBUTE_READ_ONLY(int, ADNPart, NumberOfNucleotides, "Nucleotides", "Number of nucleotides");
    SB_ATTRIBUTE_READ_ONLY(int, ADNPart, NumberOfSingleStrands, "Single strands", "Number of single strands");
    SB_ATTRIBUTE_READ_ONLY(int, ADNPart, NumberOfBaseSegments, "Base segments", "Number of base segments");
    SB_ATTRIBUTE_READ_ONLY(int, ADNPart, NumberOfDoubleStrands, "Double strands", "Number of double strands");
    SB_ATTRIBUTE_READ_WRITE(std::string const &, ADNPart, Name, "Name", "Name of the Part");

  SB_INTERFACE_END;

SB_CLASS_END(ADNPart);
