/// \headerfile SBProxy.hpp "SBProxy.hpp"
#include "SBProxy.hpp"

/// \headerfile SEGoldnanoParticlesVisualModel.hpp "SEGoldnanoParticlesVisualModel.hpp"
#include "SEGoldnanoParticlesVisualModel.hpp"


// Class descriptor

// SAMSON Element generator pro tip: complete this descriptor to expose this class to SAMSON and other SAMSON Elements

SB_CLASS_BEGIN(SEGoldnanoParticlesVisualModel);

	SB_CLASS_TYPE(SBCClass::VisualModel);
	SB_CLASS_DESCRIPTION("SEGoldnanoParticlesVisualModel : SAMSON Element generator pro tip: modify me");

	SB_FACTORY_BEGIN;

		SB_CONSTRUCTOR_0(SEGoldnanoParticlesVisualModel);
		SB_CONSTRUCTOR_1(SEGoldnanoParticlesVisualModel, const SBNodeIndexer&);

	SB_FACTORY_END;

	SB_INTERFACE_BEGIN;

	SB_INTERFACE_END;

SB_CLASS_END(SEGoldnanoParticlesVisualModel);

