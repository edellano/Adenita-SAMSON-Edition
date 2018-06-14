/// \headerfile SBProxy.hpp "SBProxy.hpp"
#include "SBProxy.hpp"

/// \headerfile SEVMTestVisualModel.hpp "SEVMTestVisualModel.hpp"
#include "SENucleotidesVisualModel.hpp"


// Class descriptor

// SAMSON Element generator pro tip: complete this descriptor to expose this class to SAMSON and other SAMSON Elements

SB_CLASS_BEGIN(SENucleotidesVisualModel);

SB_CLASS_TYPE(SBCClass::VisualModel);
SB_CLASS_DESCRIPTION("Visualization of DNA in multiple scales ");

SB_FACTORY_BEGIN;

SB_CONSTRUCTOR_0(SENucleotidesVisualModel);
SB_CONSTRUCTOR_1(SENucleotidesVisualModel, const SBNodeIndexer&);
SB_CONSTRUCTOR_1(SENucleotidesVisualModel, std::vector<SBDDataGraphNode*>&);

SB_FACTORY_END;

SB_INTERFACE_BEGIN;

SB_FUNCTION_0(void, SENucleotidesVisualModel, createThis);
SB_FUNCTION_0(void, SENucleotidesVisualModel, initialize);
SB_FUNCTION_1(void, SENucleotidesVisualModel, changeScale, double);
SB_FUNCTION_1(void, SENucleotidesVisualModel, setMSVModel, MSVModel*);
SB_FUNCTION_1(void, SENucleotidesVisualModel, bindingRegionPropertiesChanged, int);
SB_FUNCTION_3(void, SENucleotidesVisualModel, setBindingRegionParameters, int, int, int);
SB_FUNCTION_1(void, SENucleotidesVisualModel, changeDimension, double);
SB_FUNCTION_1(void, SENucleotidesVisualModel, setSorting, int);
SB_FUNCTION_1(void, SENucleotidesVisualModel, setOverlay, int);
SB_FUNCTION_1(void, SENucleotidesVisualModel, rotateX, double);
SB_FUNCTION_1(void, SENucleotidesVisualModel, rotateY, double);
SB_FUNCTION_1(void, SENucleotidesVisualModel, rotateZ, double);
SB_FUNCTION_0(void, SENucleotidesVisualModel, applyRotation);
SB_FUNCTION_3(void, SENucleotidesVisualModel, translate, double, double, double);
SB_FUNCTION_3(void, SENucleotidesVisualModel, applyTranslation, double, double, double);
SB_FUNCTION_0(void, SENucleotidesVisualModel, setPossibleCrossovers);
SB_FUNCTION_0(void, SENucleotidesVisualModel, setCrossovers);

SB_INTERFACE_END;

SB_CLASS_END(SENucleotidesVisualModel);

