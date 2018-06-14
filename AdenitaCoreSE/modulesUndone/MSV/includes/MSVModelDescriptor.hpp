/// \headerfile SBProxy.hpp "SBProxy.hpp"
#include "SBProxy.hpp"

/// \headerfile SENucleotidesVisualModel.hpp "SENucleotidesVisualModel.hpp"
#include "MSVModel.hpp"

// Class descriptor

// SAMSON Element generator pro tip: complete this descriptor to expose this class to SAMSON and other SAMSON Elements

SB_CLASS_BEGIN(MSVModel);

	SB_CLASS_TYPE(SBCClass::Custom);
	SB_CLASS_DESCRIPTION("Model for Visualization of DNA on multiple levels");

	SB_FACTORY_BEGIN;

		SB_CONSTRUCTOR_0(MSVModel);

	SB_FACTORY_END;

	SB_INTERFACE_BEGIN;
	SB_INTERFACE_END;

SB_CLASS_END(MSVModel);