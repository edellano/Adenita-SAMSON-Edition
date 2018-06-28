#pragma once

#include "SBProxy.hpp"
#include "ADNModel.hpp"


// Class descriptor

// SAMSON Element generator pro tip: complete this descriptor to expose this class to SAMSON and other SAMSON Elements

SB_CLASS_BEGIN(ADNAtom);

  SB_CLASS_TYPE(SBCClass::Custom);
  SB_CLASS_DESCRIPTION("ADNAtom");

  SB_FACTORY_BEGIN;

    SB_CONSTRUCTOR_0(ADNAtom);

  SB_FACTORY_END;

  SB_INTERFACE_BEGIN;

    SB_ATTRIBUTE_READ_WRITE(SBElement::Type, ADNAtom, ElementType, "Atom", "Type of atom");
    SB_ATTRIBUTE_READ_ONLY(SBNode*, ADNAtom, Nt, "Nucleotide", "Nucleotide to which the atom belongs");
    SB_ATTRIBUTE_READ_ONLY(SBNode*, ADNAtom, NtGroup, "Nucleotide group", "Nucleotide group to which the atom belongs");

  SB_INTERFACE_END;

SB_CLASS_END(ADNAtom);


SB_CLASS_BEGIN(ADNBackbone);

  SB_CLASS_TYPE(SBCClass::Custom);
  SB_CLASS_DESCRIPTION("ADNBackbone");

  SB_FACTORY_BEGIN;

    SB_CONSTRUCTOR_0(ADNBackbone);

  SB_FACTORY_END;

  SB_INTERFACE_BEGIN;

  SB_INTERFACE_END;

SB_CLASS_END(ADNBackbone);


SB_CLASS_BEGIN(ADNSidechain);

  SB_CLASS_TYPE(SBCClass::Custom);
  SB_CLASS_DESCRIPTION("ADNSidechain");

  SB_FACTORY_BEGIN;

    SB_CONSTRUCTOR_0(ADNSidechain);

  SB_FACTORY_END;

  SB_INTERFACE_BEGIN;

  SB_INTERFACE_END;

SB_CLASS_END(ADNSidechain);


SB_CLASS_BEGIN(ADNNucleotide);

  SB_CLASS_TYPE(SBCClass::Custom);
  SB_CLASS_DESCRIPTION("ADNNucleotide");

  SB_FACTORY_BEGIN;

    SB_CONSTRUCTOR_0(ADNNucleotide);

  SB_FACTORY_END;

  SB_INTERFACE_BEGIN;

    SB_ATTRIBUTE_READ_ONLY(DNABlocks, ADNNucleotide, NucleotideType, "Type", "Nucleotide type");
    SB_ATTRIBUTE_READ_ONLY(SBNode*, ADNNucleotide, SingleStrand, "Single Strand", "Single Strand");
    SB_ATTRIBUTE_READ_ONLY(SBNode*, ADNNucleotide, BaseSegment, "Base segment", "Base segment");

  SB_INTERFACE_END;

SB_CLASS_END(ADNNucleotide);


SB_CLASS_BEGIN(ADNSingleStrand);

  SB_CLASS_TYPE(SBCClass::Custom);
  SB_CLASS_DESCRIPTION("ADNSingleStrand");

  SB_FACTORY_BEGIN;

    SB_CONSTRUCTOR_0(ADNSingleStrand);

  SB_FACTORY_END;

  SB_INTERFACE_BEGIN;

  SB_INTERFACE_END;

SB_CLASS_END(ADNSingleStrand);


SB_CLASS_BEGIN(ADNCell);

  SB_CLASS_TYPE(SBCClass::Custom);
  SB_CLASS_DESCRIPTION("ADNCell");

  SB_FACTORY_BEGIN;

    SB_CONSTRUCTOR_0(ADNCell);

  SB_FACTORY_END;

  SB_INTERFACE_BEGIN;

  SB_INTERFACE_END;

SB_CLASS_END(ADNCell);


SB_CLASS_BEGIN(ADNBasePair);

  SB_CLASS_TYPE(SBCClass::Custom);
  SB_CLASS_DESCRIPTION("ADNBasePair");

  SB_FACTORY_BEGIN;

    SB_CONSTRUCTOR_0(ADNBasePair);

  SB_FACTORY_END;

  SB_INTERFACE_BEGIN;

    SB_ATTRIBUTE_READ_ONLY(SBNode*, ADNBasePair, Left, "Left", "Left nucleotide");
    SB_ATTRIBUTE_READ_ONLY(SBNode*, ADNBasePair, Right, "Right", "Right nucleotide");

  SB_INTERFACE_END;

SB_CLASS_END(ADNBasePair);


SB_CLASS_BEGIN(ADNSkipPair);

  SB_CLASS_TYPE(SBCClass::Custom);
  SB_CLASS_DESCRIPTION("ADNSkipPair");

  SB_FACTORY_BEGIN;

    SB_CONSTRUCTOR_0(ADNSkipPair);

  SB_FACTORY_END;

  SB_INTERFACE_BEGIN;

  SB_INTERFACE_END;

SB_CLASS_END(ADNSkipPair);


SB_CLASS_BEGIN(ADNLoopPair);

  SB_CLASS_TYPE(SBCClass::Custom);
  SB_CLASS_DESCRIPTION("ADNLoopPair");

  SB_FACTORY_BEGIN;

    SB_CONSTRUCTOR_0(ADNLoopPair);

  SB_FACTORY_END;

  SB_INTERFACE_BEGIN;

    SB_ATTRIBUTE_READ_ONLY(SBNode*, ADNLoopPair, Left, "Left", "Left loop");
    SB_ATTRIBUTE_READ_ONLY(SBNode*, ADNLoopPair, Right, "Right", "Right loop");

  SB_INTERFACE_END;

SB_CLASS_END(ADNLoopPair);


SB_CLASS_BEGIN(ADNLoop);

  SB_CLASS_TYPE(SBCClass::Custom);
  SB_CLASS_DESCRIPTION("ADNLoop");

  SB_FACTORY_BEGIN;

    SB_CONSTRUCTOR_0(ADNLoop);

  SB_FACTORY_END;

  SB_INTERFACE_BEGIN;

    SB_ATTRIBUTE_READ_ONLY(SBNode*, ADNLoop, StartNucleotide, "Start nucleotide", "Beggining of loop");
    SB_ATTRIBUTE_READ_ONLY(SBNode*, ADNLoop, EndNucleotide, "End nucleotide", "End of loop");

  SB_INTERFACE_END;

SB_CLASS_END(ADNLoop);


SB_CLASS_BEGIN(ADNBaseSegment);

  SB_CLASS_TYPE(SBCClass::Custom);
  SB_CLASS_DESCRIPTION("ADNBaseSegment");

  SB_FACTORY_BEGIN;

    SB_CONSTRUCTOR_0(ADNBaseSegment);

  SB_FACTORY_END;

  SB_INTERFACE_BEGIN;

    // show custom members in inspector
    SB_ATTRIBUTE_READ_WRITE(int, ADNBaseSegment, Number, "Number", "Helix turns at this position");

  SB_INTERFACE_END;

SB_CLASS_END(ADNBaseSegment);


SB_CLASS_BEGIN(ADNDoubleStrand);

  SB_CLASS_TYPE(SBCClass::Custom);
  SB_CLASS_DESCRIPTION("ADNDoubleStrand");

  SB_FACTORY_BEGIN;

    SB_CONSTRUCTOR_0(ADNDoubleStrand);

  SB_FACTORY_END;

  SB_INTERFACE_BEGIN;

    SB_ATTRIBUTE_READ_ONLY(int, ADNDoubleStrand, Length, "Length", "Number of base segments");

  SB_INTERFACE_END;

SB_CLASS_END(ADNDoubleStrand);