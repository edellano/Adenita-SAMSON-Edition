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

    SB_ATTRIBUTE_READ_WRITE(SBElement::Type, ADNAtom, ElementType, "Atom", "Properties");
    SB_ATTRIBUTE_READ_ONLY(SBNode*, ADNAtom, Nt, "Nucleotide", "Properties");
    SB_ATTRIBUTE_READ_ONLY(SBNode*, ADNAtom, NtGroup, "Nucleotide group", "Properties");

  SB_INTERFACE_END;

SB_CLASS_END(ADNAtom);


SB_CLASS_BEGIN(ADNBackbone);

  SB_CLASS_TYPE(SBCClass::Custom);
  SB_CLASS_DESCRIPTION("ADNBackbone");

  SB_FACTORY_BEGIN;

    SB_CONSTRUCTOR_0(ADNBackbone);

  SB_FACTORY_END;

  SB_INTERFACE_BEGIN;

    SB_ATTRIBUTE_READ_WRITE(std::string const &, ADNBackbone, Name, "Name", "Properties");
    SB_ATTRIBUTE_READ_ONLY(int, ADNBackbone, NumberOfAtoms, "Number of atoms", "Properties");

  SB_INTERFACE_END;

SB_CLASS_END(ADNBackbone);


SB_CLASS_BEGIN(ADNSidechain);

  SB_CLASS_TYPE(SBCClass::Custom);
  SB_CLASS_DESCRIPTION("ADNSidechain");

  SB_FACTORY_BEGIN;

    SB_CONSTRUCTOR_0(ADNSidechain);

  SB_FACTORY_END;

  SB_INTERFACE_BEGIN;

    SB_ATTRIBUTE_READ_WRITE(std::string const &, ADNSidechain, Name, "Name", "Properties");
    SB_ATTRIBUTE_READ_ONLY(int, ADNSidechain, NumberOfAtoms, "Number of atoms", "Properties");

  SB_INTERFACE_END;

SB_CLASS_END(ADNSidechain);


SB_CLASS_BEGIN(ADNNucleotide);

  SB_CLASS_TYPE(SBCClass::Custom);
  SB_CLASS_DESCRIPTION("ADNNucleotide");

  SB_FACTORY_BEGIN;

    SB_CONSTRUCTOR_0(ADNNucleotide);

  SB_FACTORY_END;

  SB_INTERFACE_BEGIN;

    SB_ATTRIBUTE_READ_WRITE(std::string const &, ADNNucleotide, Name, "Name", "Properties");
    SB_ATTRIBUTE_READ_ONLY(DNABlocks, ADNNucleotide, NucleotideType, "Type", "Properties");
    SB_ATTRIBUTE_READ_ONLY(SBNode*, ADNNucleotide, SingleStrand, "Single Strand", "Properties");
    SB_ATTRIBUTE_READ_ONLY(SBNode*, ADNNucleotide, BaseSegment, "Base segment", "Properties");

  SB_INTERFACE_END;

SB_CLASS_END(ADNNucleotide);


SB_CLASS_BEGIN(ADNSingleStrand);

  SB_CLASS_TYPE(SBCClass::Custom);
  SB_CLASS_DESCRIPTION("ADNSingleStrand");

  SB_FACTORY_BEGIN;

    SB_CONSTRUCTOR_0(ADNSingleStrand);

  SB_FACTORY_END;

  SB_INTERFACE_BEGIN;

    SB_ATTRIBUTE_READ_WRITE(std::string const &, ADNSingleStrand, Name, "Name", "Properties");
    SB_ATTRIBUTE_READ_ONLY(int, ADNSingleStrand, NumberOfNucleotides, "Nucleotides", "Properties");
    // SB_ATTRIBUTE_READ_ONLY(std::string, ADNSingleStrand, Sequence, "Sequence", "Properties");
    SB_ATTRIBUTE_READ_ONLY(double, ADNSingleStrand, GCContent, "GC %", "Properties");
    SB_ATTRIBUTE_READ_ONLY(SBNode*, ADNSingleStrand, FivePrime, "5'", "Properties");
    SB_ATTRIBUTE_READ_ONLY(SBNode*, ADNSingleStrand, ThreePrime, "3'", "Properties");

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

    SB_ATTRIBUTE_READ_WRITE(std::string const &, ADNBasePair, Name, "Name", "Properties");
    SB_ATTRIBUTE_READ_ONLY(SBNode*, ADNBasePair, Left, "Left", "Properties");
    SB_ATTRIBUTE_READ_ONLY(SBNode*, ADNBasePair, Right, "Right", "Properties");

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

    SB_ATTRIBUTE_READ_WRITE(std::string const &, ADNLoopPair, Name, "Name", "Properties");
    SB_ATTRIBUTE_READ_ONLY(SBNode*, ADNLoopPair, Left, "Left", "Properties");
    SB_ATTRIBUTE_READ_ONLY(SBNode*, ADNLoopPair, Right, "Right", "Properties");

  SB_INTERFACE_END;

SB_CLASS_END(ADNLoopPair);


SB_CLASS_BEGIN(ADNLoop);

  SB_CLASS_TYPE(SBCClass::Custom);
  SB_CLASS_DESCRIPTION("ADNLoop");

  SB_FACTORY_BEGIN;

    SB_CONSTRUCTOR_0(ADNLoop);

  SB_FACTORY_END;

  SB_INTERFACE_BEGIN;

    SB_ATTRIBUTE_READ_WRITE(std::string const &, ADNLoop, Name, "Name", "Properties");
    SB_ATTRIBUTE_READ_ONLY(int, ADNLoop, NumberOfNucleotides, "Number of nucleotides", "Properties");
    SB_ATTRIBUTE_READ_ONLY(std::string, ADNLoop, LoopSequence, "Sequence", "Properties");
    SB_ATTRIBUTE_READ_ONLY(SBNode*, ADNLoop, StartNucleotide, "Start nucleotide", "Nucleotides");
    SB_ATTRIBUTE_READ_ONLY(SBNode*, ADNLoop, EndNucleotide, "End nucleotide", "Nucleotides");

  SB_INTERFACE_END;

SB_CLASS_END(ADNLoop);


SB_CLASS_BEGIN(ADNBaseSegment);

  SB_CLASS_TYPE(SBCClass::Custom);
  SB_CLASS_DESCRIPTION("ADNBaseSegment");

  SB_FACTORY_BEGIN;

    SB_CONSTRUCTOR_0(ADNBaseSegment);

  SB_FACTORY_END;

  SB_INTERFACE_BEGIN;

    SB_ATTRIBUTE_READ_WRITE(std::string const &, ADNBaseSegment, Name, "Name", "Properties");
    SB_ATTRIBUTE_READ_ONLY(int, ADNBaseSegment, Number, "Number", "Properties");
    SB_ATTRIBUTE_READ_ONLY(std::string, ADNBaseSegment, CellType, "Contains", "Properties");

  SB_INTERFACE_END;

SB_CLASS_END(ADNBaseSegment);


SB_CLASS_BEGIN(ADNDoubleStrand);

  SB_CLASS_TYPE(SBCClass::Custom);
  SB_CLASS_DESCRIPTION("ADNDoubleStrand");

  SB_FACTORY_BEGIN;

    SB_CONSTRUCTOR_0(ADNDoubleStrand);

  SB_FACTORY_END;

  SB_INTERFACE_BEGIN;

    SB_ATTRIBUTE_READ_WRITE(std::string const &, ADNDoubleStrand, Name, "Name", "Properties");
    SB_ATTRIBUTE_READ_ONLY(int, ADNDoubleStrand, Length, "Length", "Properties");
    SB_ATTRIBUTE_READ_ONLY(double, ADNDoubleStrand, InitialTwistAngle, "Helical twist offset", "Properties");
    SB_ATTRIBUTE_READ_ONLY(SBNode*, ADNDoubleStrand, FirstBaseSegment, "First base segment", "Double strand extremes");
    SB_ATTRIBUTE_READ_ONLY(SBNode*, ADNDoubleStrand, LastBaseSegment, "Last base segment", "Double strand extremes");

  SB_INTERFACE_END;

SB_CLASS_END(ADNDoubleStrand);