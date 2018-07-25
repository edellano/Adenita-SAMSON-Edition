/// \headerfile SEAdenitaCoreSEAppDescriptor.hpp "SEAdenitaCoreSEAppDescriptor.hpp"
#include "SEAdenitaCoreSEAppDescriptor.hpp"
#include "SEAdenitaVisualModelDescriptor.hpp"
#include "SEAdenitaVisualModelPropertiesDescriptor.hpp"
#include "ADNModelDescriptor.hpp"
#include "ADNPartDescriptor.hpp"
#include "SEWireframeEditorDescriptor.hpp"
#include "SEBreakEditorDescriptor.hpp"
#include "SEDeleteEditorDescriptor.hpp"



// Describe the SAMSON Element

// SAMSON Element generator pro tip: modify the information below if necessary
// (for example when a new class is added, when the version number changes, to describe categories more precisely, etc.)

SB_ELEMENT_DESCRIPTION("Modeling and Visualization of DNA Nanostructures");
SB_ELEMENT_DOCUMENTATION("Resource/Documentation/doc.html");
SB_ELEMENT_VERSION_NUMBER("0.7.0");

SB_ELEMENT_CLASSES_BEGIN;

	SB_ELEMENT_CLASS(SEAdenitaCoreSEApp);
  SB_ELEMENT_CLASS(ADNAtom);
  SB_ELEMENT_CLASS(ADNBackbone);
  SB_ELEMENT_CLASS(ADNSidechain);
  SB_ELEMENT_CLASS(ADNNucleotide);
  SB_ELEMENT_CLASS(ADNSingleStrand);
  SB_ELEMENT_CLASS(ADNCell);
  SB_ELEMENT_CLASS(ADNBasePair);
  SB_ELEMENT_CLASS(ADNLoopPair);
  SB_ELEMENT_CLASS(ADNSkipPair);
  SB_ELEMENT_CLASS(ADNLoop);
  SB_ELEMENT_CLASS(ADNBaseSegment);
  SB_ELEMENT_CLASS(ADNDoubleStrand);
  SB_ELEMENT_CLASS(ADNPart);
  SB_ELEMENT_CLASS(SEAdenitaVisualModel);
  SB_ELEMENT_CLASS(SEAdenitaVisualModelProperties);
  SB_ELEMENT_CLASS(SEWireframeEditor);
  SB_ELEMENT_CLASS(SEBreakEditor);
  SB_ELEMENT_CLASS(SEDeleteEditor);

SB_ELEMENT_CLASSES_END;

SB_ELEMENT_CATEGORIES_BEGIN;

	SB_ELEMENT_CATEGORY(SBClass::Category::General);

SB_ELEMENT_CATEGORIES_END;
