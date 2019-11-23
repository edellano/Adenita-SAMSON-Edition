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

    SB_ATTRIBUTE_READ_ONLY(SBNode*, ADNAtom, Nt, "Nucleotide", "Adenita");
    SB_ATTRIBUTE_READ_ONLY(SBNode*, ADNAtom, NtGroup, "Nucleotide group", "Adenita");
    SB_ATTRIBUTE_READ_ONLY(SBPosition3 const&, ADNAtom, Position, "Position", "Adenita");

    SB_ATTRIBUTE_READ_WRITE(bool const&, ADNAtom, Aromaticity, "Aromaticity", "Chemistry");
    SB_ATTRIBUTE_READ_ONLY(SBQuantity::dimensionless, ADNAtom, Electronegativity, "Electronegativity", "Chemistry");
    SB_ATTRIBUTE_READ_WRITE(bool const&, ADNAtom, Resonance, "Resonance", "Chemistry");

    SB_ATTRIBUTE_READ_ONLY(SBQuantity::mass, ADNAtom, AtomicWeight, "Atomic weight", "Element");
    SB_ATTRIBUTE_READ_ONLY(std::string, ADNAtom, Block, "Block", "Element");
    SB_ATTRIBUTE_READ_WRITE(SBElement::Type, ADNAtom, ElementType, "Element", "Element");
    SB_ATTRIBUTE_READ_ONLY(std::string, ADNAtom, ElementName, "Element name", "Element");
    SB_ATTRIBUTE_READ_ONLY(std::string, ADNAtom, ElementSymbol, "Element symbol", "Element");
    SB_ATTRIBUTE_READ_ONLY(unsigned int, ADNAtom, Group, "Group", "Element");
    SB_ATTRIBUTE_READ_WRITE(short const&, ADNAtom, Hybridization, "Hybridization", "Element");
    SB_ATTRIBUTE_READ_WRITE(int const&, ADNAtom, OxydationState, "Oxydation state", "Element");
    SB_ATTRIBUTE_READ_ONLY(std::string, ADNAtom, Period, "Period", "Element");
    SB_ATTRIBUTE_READ_WRITE(bool const&, ADNAtom, WaterFlag, "Water flag", "Element");

    SB_ATTRIBUTE_READ_WRITE_CLEAR_ARRAY(char* const&, ADNAtom, MoleculeName, "Molecule name", "Identity");
    SB_ATTRIBUTE_READ_WRITE(std::string const&, ADNAtom, Name, "Name", "Identity");
    SB_ATTRIBUTE_READ_WRITE_CLEAR_ARRAY(char* const&, ADNAtom, SubstructureName, "Substructure name", "Identity");

    SB_ATTRIBUTE_READ_ONLY(SBNode*, ADNAtom, ThisNode, "Itself", "Node");
    SB_ATTRIBUTE_READ_ONLY(SBNode*, ADNAtom, Parent, "Parent", "Node");
    SB_ATTRIBUTE_READ_ONLY(bool, ADNAtom, Selected, "Selected", "Node");
    SB_ATTRIBUTE_READ_WRITE(bool, ADNAtom, SelectionFlag, "Selection Flag", "Node");
    SB_ATTRIBUTE_READ_WRITE(bool, ADNAtom, VisibilityFlag, "Visibility Flag", "Node");
    SB_ATTRIBUTE_READ_ONLY(bool, ADNAtom, Visible, "Visible", "Node");

    SB_ATTRIBUTE_READ_WRITE_CLEAR_ARRAY(char* const&, ADNAtom, Comment, "Comment", "Other");
    SB_ATTRIBUTE_READ_WRITE_CLEAR_ARRAY(char* const&, ADNAtom, StatusBit, "Status bit", "Other");

    SB_ATTRIBUTE_READ_WRITE_CLEAR(char const&, ADNAtom, AltLocation, "Alt. location", "Protein");
    SB_ATTRIBUTE_READ_WRITE_CLEAR_ARRAY(char* const&, ADNAtom, Chain, "Chain", "Protein");
    SB_ATTRIBUTE_READ_WRITE_CLEAR(int const&, ADNAtom, ChainID, "Chain ID", "Protein");
    SB_ATTRIBUTE_READ_WRITE_CLEAR(int const&, ADNAtom, FormalCharge, "Formal charge", "Protein");
    SB_ATTRIBUTE_READ_WRITE_CLEAR(char const&, ADNAtom, InsertionCode, "Insertion code", "Protein");
    SB_ATTRIBUTE_READ_WRITE_CLEAR(SBQuantity::dimensionless const&, ADNAtom, Occupancy, "Occupancy", "Protein");
    SB_ATTRIBUTE_READ_WRITE_CLEAR(float const&, ADNAtom, PartialCharge, "Partial charge", "Protein");
    SB_ATTRIBUTE_READ_WRITE_CLEAR_ARRAY(char* const&, ADNAtom, RecordType, "Record type", "Protein");
    SB_ATTRIBUTE_READ_WRITE_CLEAR_ARRAY(char* const&, ADNAtom, ResidueName, "Residue name", "Protein");
    SB_ATTRIBUTE_READ_WRITE_CLEAR(int const&, ADNAtom, ResidueSequenceNumber, "Residue sequence number", "Protein");
    SB_ATTRIBUTE_READ_WRITE_CLEAR_ARRAY(char* const&, ADNAtom, Segment, "Segment", "Protein");
    SB_ATTRIBUTE_READ_WRITE_CLEAR(int const&, ADNAtom, SerialNumber, "Serial number", "Protein");
    SB_ATTRIBUTE_READ_WRITE_CLEAR(int const&, ADNAtom, SubstructureSequenceNumber, "Substructure sequence number", "Protein");
    SB_ATTRIBUTE_READ_WRITE_CLEAR(double const&, ADNAtom, TemperatureFactor, "Temperature factor", "Protein");

    SB_ATTRIBUTE_READ_ONLY(SBQuantity::length, ADNAtom, CovalentRadius, "Covalent radius", "Structure");
    SB_ATTRIBUTE_READ_WRITE(bool, ADNAtom, MobilityFlag, "Mobile", "Structure");
    SB_ATTRIBUTE_READ_WRITE(SBPosition3 const&, ADNAtom, Position, "Position", "Structure");
    SB_ATTRIBUTE_READ_ONLY(SBQuantity::length, ADNAtom, VanDerWaalsRadius, "Van der Waals radius", "Structure");

    SB_ATTRIBUTE_READ_WRITE_CLEAR(int const&, ADNAtom, CustomType, "Custom type", "Typization");
    SB_ATTRIBUTE_READ_WRITE_CLEAR_ARRAY(char* const&, ADNAtom, SYBYLType, "SYBYL type", "Typization");

  SB_INTERFACE_END;

SB_CLASS_END(ADNAtom);


SB_CLASS_BEGIN(ADNBackbone);

  SB_CLASS_TYPE(SBCClass::Custom);
  SB_CLASS_DESCRIPTION("ADNBackbone");

  SB_FACTORY_BEGIN;

    SB_CONSTRUCTOR_0(ADNBackbone);

  SB_FACTORY_END;

  SB_INTERFACE_BEGIN;

    SB_ATTRIBUTE_READ_WRITE(std::string const &, ADNBackbone, Name, "Name", "Adenita");
    SB_ATTRIBUTE_READ_ONLY(int, ADNBackbone, NumberOfAtoms, "Number of atoms", "Adenita");

    SB_ATTRIBUTE_READ_ONLY(SBNode*, ADNBackbone, ThisNode, "Itself", "Node");
    SB_ATTRIBUTE_READ_ONLY(SBNode*, ADNBackbone, Parent, "Parent", "Node");
    SB_ATTRIBUTE_READ_ONLY(bool, ADNBackbone, Selected, "Selected", "Node");
    SB_ATTRIBUTE_READ_WRITE(bool, ADNBackbone, SelectionFlag, "Selection Flag", "Node");
    SB_ATTRIBUTE_READ_WRITE(bool, ADNBackbone, VisibilityFlag, "Visibility Flag", "Node");
    SB_ATTRIBUTE_READ_ONLY(bool, ADNBackbone, Visible, "Visible", "Node");

  SB_INTERFACE_END;

SB_CLASS_END(ADNBackbone);


SB_CLASS_BEGIN(ADNSidechain);

  SB_CLASS_TYPE(SBCClass::Custom);
  SB_CLASS_DESCRIPTION("ADNSidechain");

  SB_FACTORY_BEGIN;

    SB_CONSTRUCTOR_0(ADNSidechain);

  SB_FACTORY_END;

  SB_INTERFACE_BEGIN;

    SB_ATTRIBUTE_READ_WRITE(std::string const &, ADNSidechain, Name, "Name", "Adenita");
    SB_ATTRIBUTE_READ_ONLY(int, ADNSidechain, NumberOfAtoms, "Number of atoms", "Adenita");

    SB_ATTRIBUTE_READ_ONLY(SBNode*, ADNSidechain, ThisNode, "Itself", "Node");
    SB_ATTRIBUTE_READ_ONLY(SBNode*, ADNSidechain, Parent, "Parent", "Node");
    SB_ATTRIBUTE_READ_ONLY(bool, ADNSidechain, Selected, "Selected", "Node");
    SB_ATTRIBUTE_READ_WRITE(bool, ADNSidechain, SelectionFlag, "Selection Flag", "Node");
    SB_ATTRIBUTE_READ_WRITE(bool, ADNSidechain, VisibilityFlag, "Visibility Flag", "Node");
    SB_ATTRIBUTE_READ_ONLY(bool, ADNSidechain, Visible, "Visible", "Node");

  SB_INTERFACE_END;

SB_CLASS_END(ADNSidechain);


SB_CLASS_BEGIN(ADNNucleotide);

  SB_CLASS_TYPE(SBCClass::Custom);
  SB_CLASS_DESCRIPTION("ADNNucleotide");

  SB_FACTORY_BEGIN;

    SB_CONSTRUCTOR_0(ADNNucleotide);

  SB_FACTORY_END;

  SB_INTERFACE_BEGIN;

    SB_ATTRIBUTE_READ_WRITE(std::string const &, ADNNucleotide, Name, "Name", "Adenita");
    SB_ATTRIBUTE_READ_ONLY(DNABlocks, ADNNucleotide, NucleotideType, "Type", "Adenita");
    SB_ATTRIBUTE_READ_ONLY(SBNode*, ADNNucleotide, SingleStrand, "Single Strand", "Adenita");
    SB_ATTRIBUTE_READ_ONLY(SBNode*, ADNNucleotide, BaseSegment, "Base segment", "Adenita");
    SB_ATTRIBUTE_READ_ONLY(SBNode*, ADNNucleotide, Next, "Next nucleotide", "Adenita");
    SB_ATTRIBUTE_READ_ONLY(SBNode*, ADNNucleotide, Prev, "Previous nucleotide", "Adenita");
    SB_ATTRIBUTE_READ_ONLY(SBNode*, ADNNucleotide, Pair, "Pair nucleotide", "Adenita");
    SB_ATTRIBUTE_READ_ONLY(std::string, ADNNucleotide, BaseSegmentType, "Base segment type", "Adenita");
    SB_ATTRIBUTE_READ_ONLY(std::string, ADNNucleotide, EndType, "End type", "Adenita");
    SB_ATTRIBUTE_READ_WRITE(std::string, ADNNucleotide, Tag, "Tag", "Adenita");

    SB_ATTRIBUTE_READ_ONLY(SBNode*, ADNNucleotide, ThisNode, "Itself", "Node");
    SB_ATTRIBUTE_READ_ONLY(SBNode*, ADNNucleotide, Parent, "Parent", "Node");
    SB_ATTRIBUTE_READ_ONLY(bool, ADNNucleotide, Selected, "Selected", "Node");
    SB_ATTRIBUTE_READ_WRITE(bool, ADNNucleotide, SelectionFlag, "Selection Flag", "Node");
    SB_ATTRIBUTE_READ_WRITE(bool, ADNNucleotide, VisibilityFlag, "Visibility Flag", "Node");
    SB_ATTRIBUTE_READ_ONLY(bool, ADNNucleotide, Visible, "Visible", "Node");

  SB_INTERFACE_END;

SB_CLASS_END(ADNNucleotide);


SB_CLASS_BEGIN(ADNSingleStrand);

  SB_CLASS_TYPE(SBCClass::Custom);
  SB_CLASS_DESCRIPTION("ADNSingleStrand");

  SB_FACTORY_BEGIN;

    SB_CONSTRUCTOR_0(ADNSingleStrand);

  SB_FACTORY_END;

  SB_INTERFACE_BEGIN;

    SB_ATTRIBUTE_READ_WRITE(std::string const &, ADNSingleStrand, Name, "Name", "Adenita");
    SB_ATTRIBUTE_READ_WRITE(bool, ADNSingleStrand, IsScaffold, "Is Scaffold", "Adenita");
    SB_ATTRIBUTE_READ_WRITE(bool, ADNSingleStrand, IsCircular, "Is Circular", "Adenita");
    SB_ATTRIBUTE_READ_ONLY(int, ADNSingleStrand, NumberOfNucleotides, "Nucleotides", "Adenita");
    SB_ATTRIBUTE_READ_WRITE(std::string, ADNSingleStrand, Sequence, "Sequence", "Adenita");
    SB_ATTRIBUTE_READ_ONLY(double, ADNSingleStrand, GCContent, "GC %", "Adenita");
    SB_ATTRIBUTE_READ_ONLY(SBNode*, ADNSingleStrand, FivePrime, "5'", "Adenita");
    SB_ATTRIBUTE_READ_ONLY(SBNode*, ADNSingleStrand, ThreePrime, "3'", "Adenita");

    SB_ATTRIBUTE_READ_ONLY(SBNode*, ADNSingleStrand, ThisNode, "Itself", "Node");
    SB_ATTRIBUTE_READ_ONLY(SBNode*, ADNSingleStrand, Parent, "Parent", "Node");
    SB_ATTRIBUTE_READ_ONLY(bool, ADNSingleStrand, Selected, "Selected", "Node");
    SB_ATTRIBUTE_READ_WRITE(bool, ADNSingleStrand, SelectionFlag, "Selection Flag", "Node");
    SB_ATTRIBUTE_READ_WRITE(bool, ADNSingleStrand, VisibilityFlag, "Visibility Flag", "Node");
    SB_ATTRIBUTE_READ_ONLY(bool, ADNSingleStrand, Visible, "Visible", "Node");

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

    SB_ATTRIBUTE_READ_WRITE(std::string const &, ADNBasePair, Name, "Name", "Adenita");
    SB_ATTRIBUTE_READ_ONLY(SBNode*, ADNBasePair, Left, "Left", "Adenita");
    SB_ATTRIBUTE_READ_ONLY(SBNode*, ADNBasePair, Right, "Right", "Adenita");

    SB_ATTRIBUTE_READ_ONLY(SBNode*, ADNBasePair, ThisNode, "Itself", "Node");
    SB_ATTRIBUTE_READ_ONLY(SBNode*, ADNBasePair, Parent, "Parent", "Node");
    SB_ATTRIBUTE_READ_ONLY(bool, ADNBasePair, Selected, "Selected", "Node");
    SB_ATTRIBUTE_READ_WRITE(bool, ADNBasePair, SelectionFlag, "Selection Flag", "Node");
    SB_ATTRIBUTE_READ_WRITE(bool, ADNBasePair, VisibilityFlag, "Visibility Flag", "Node");
    SB_ATTRIBUTE_READ_ONLY(bool, ADNBasePair, Visible, "Visible", "Node");

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

    SB_ATTRIBUTE_READ_WRITE(std::string const &, ADNLoopPair, Name, "Name", "Adenita");
    SB_ATTRIBUTE_READ_ONLY(SBNode*, ADNLoopPair, Left, "Left", "Adenita");
    SB_ATTRIBUTE_READ_ONLY(SBNode*, ADNLoopPair, Right, "Right", "Adenita");

    SB_ATTRIBUTE_READ_ONLY(SBNode*, ADNLoopPair, ThisNode, "Itself", "Node");
    SB_ATTRIBUTE_READ_ONLY(SBNode*, ADNLoopPair, Parent, "Parent", "Node");
    SB_ATTRIBUTE_READ_ONLY(bool, ADNLoopPair, Selected, "Selected", "Node");
    SB_ATTRIBUTE_READ_WRITE(bool, ADNLoopPair, SelectionFlag, "Selection Flag", "Node");
    SB_ATTRIBUTE_READ_WRITE(bool, ADNLoopPair, VisibilityFlag, "Visibility Flag", "Node");
    SB_ATTRIBUTE_READ_ONLY(bool, ADNLoopPair, Visible, "Visible", "Node");

  SB_INTERFACE_END;

SB_CLASS_END(ADNLoopPair);


SB_CLASS_BEGIN(ADNLoop);

  SB_CLASS_TYPE(SBCClass::Custom);
  SB_CLASS_DESCRIPTION("ADNLoop");

  SB_FACTORY_BEGIN;

    SB_CONSTRUCTOR_0(ADNLoop);

  SB_FACTORY_END;

  SB_INTERFACE_BEGIN;

    SB_ATTRIBUTE_READ_WRITE(std::string const &, ADNLoop, Name, "Name", "Adenita");
    SB_ATTRIBUTE_READ_ONLY(int, ADNLoop, NumberOfNucleotides, "Number of nucleotides", "Adenita");
    SB_ATTRIBUTE_READ_ONLY(std::string, ADNLoop, LoopSequence, "Sequence", "Adenita");
    SB_ATTRIBUTE_READ_ONLY(SBNode*, ADNLoop, StartNucleotide, "Start nucleotide", "Adenita");
    SB_ATTRIBUTE_READ_ONLY(SBNode*, ADNLoop, EndNucleotide, "End nucleotide", "Adenita");

    SB_ATTRIBUTE_READ_ONLY(SBNode*, ADNLoop, ThisNode, "Itself", "Node");
    SB_ATTRIBUTE_READ_ONLY(SBNode*, ADNLoop, Parent, "Parent", "Node");
    SB_ATTRIBUTE_READ_ONLY(bool, ADNLoop, Selected, "Selected", "Node");
    SB_ATTRIBUTE_READ_WRITE(bool, ADNLoop, SelectionFlag, "Selection Flag", "Node");
    SB_ATTRIBUTE_READ_WRITE(bool, ADNLoop, VisibilityFlag, "Visibility Flag", "Node");
    SB_ATTRIBUTE_READ_ONLY(bool, ADNLoop, Visible, "Visible", "Node");

  SB_INTERFACE_END;

SB_CLASS_END(ADNLoop);


SB_CLASS_BEGIN(ADNBaseSegment);

  SB_CLASS_TYPE(SBCClass::Custom);
  SB_CLASS_DESCRIPTION("ADNBaseSegment");

  SB_FACTORY_BEGIN;

    SB_CONSTRUCTOR_0(ADNBaseSegment);

  SB_FACTORY_END;

  SB_INTERFACE_BEGIN;

    SB_ATTRIBUTE_READ_ONLY(SBNode*, ADNBaseSegment, DoubleStrand, "Double Strand", "Adenita");
    SB_ATTRIBUTE_READ_WRITE(std::string const &, ADNBaseSegment, Name, "Name", "Adenita");
    SB_ATTRIBUTE_READ_ONLY(int, ADNBaseSegment, Number, "Number", "Adenita");
    SB_ATTRIBUTE_READ_ONLY(std::string, ADNBaseSegment, CellType, "Contains", "Adenita");

    SB_ATTRIBUTE_READ_ONLY(SBNode*, ADNBaseSegment, ThisNode, "Itself", "Node");
    SB_ATTRIBUTE_READ_ONLY(SBNode*, ADNBaseSegment, Parent, "Parent", "Node");
    SB_ATTRIBUTE_READ_ONLY(bool, ADNBaseSegment, Selected, "Selected", "Node");
    SB_ATTRIBUTE_READ_WRITE(bool, ADNBaseSegment, SelectionFlag, "Selection Flag", "Node");
    SB_ATTRIBUTE_READ_WRITE(bool, ADNBaseSegment, VisibilityFlag, "Visibility Flag", "Node");
    SB_ATTRIBUTE_READ_ONLY(bool, ADNBaseSegment, Visible, "Visible", "Node");

  SB_INTERFACE_END;

SB_CLASS_END(ADNBaseSegment);


SB_CLASS_BEGIN(ADNDoubleStrand);

  SB_CLASS_TYPE(SBCClass::Custom);
  SB_CLASS_DESCRIPTION("ADNDoubleStrand");

  SB_FACTORY_BEGIN;

    SB_CONSTRUCTOR_0(ADNDoubleStrand);

  SB_FACTORY_END;

  SB_INTERFACE_BEGIN;

    SB_ATTRIBUTE_READ_WRITE(std::string const &, ADNDoubleStrand, Name, "Name", "Adenita");
    SB_ATTRIBUTE_READ_ONLY(int, ADNDoubleStrand, Length, "Length", "Adenita");
    SB_ATTRIBUTE_READ_ONLY(double, ADNDoubleStrand, InitialTwistAngle, "Helical twist offset", "Adenita");
    SB_ATTRIBUTE_READ_WRITE(bool, ADNDoubleStrand, IsCircular, "Is Circular", "Adenita");
    SB_ATTRIBUTE_READ_ONLY(SBNode*, ADNDoubleStrand, FirstBaseSegment, "First base segment", "Adenita");
    SB_ATTRIBUTE_READ_ONLY(SBNode*, ADNDoubleStrand, LastBaseSegment, "Last base segment", "Adenita");

    SB_ATTRIBUTE_READ_ONLY(SBNode*, ADNDoubleStrand, ThisNode, "Itself", "Node");
    SB_ATTRIBUTE_READ_ONLY(SBNode*, ADNDoubleStrand, Parent, "Parent", "Node");
    SB_ATTRIBUTE_READ_ONLY(bool, ADNDoubleStrand, Selected, "Selected", "Node");
    SB_ATTRIBUTE_READ_WRITE(bool, ADNDoubleStrand, SelectionFlag, "Selection Flag", "Node");
    SB_ATTRIBUTE_READ_WRITE(bool, ADNDoubleStrand, VisibilityFlag, "Visibility Flag", "Node");
    SB_ATTRIBUTE_READ_ONLY(bool, ADNDoubleStrand, Visible, "Visible", "Node");

  SB_INTERFACE_END;

SB_CLASS_END(ADNDoubleStrand);