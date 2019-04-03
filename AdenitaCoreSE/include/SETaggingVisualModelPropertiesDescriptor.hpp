/// \headerfile SBProxy.hpp "SBProxy.hpp"
#include "SBProxy.hpp"

/// \headerfile SETaggingVisualModelProperties.hpp "SETaggingVisualModelProperties.hpp"
#include "SETaggingVisualModelProperties.hpp"


// Class descriptor

// SAMSON Element generator pro tip: complete this descriptor to expose this class to SAMSON and other SAMSON Elements

SB_CLASS_BEGIN(SETaggingVisualModelProperties);

	SB_CLASS_TYPE(SBCClass::Properties);
	SB_CLASS_DESCRIPTION("Tagging Nucleotides");

	SB_FACTORY_BEGIN;

		SB_CONSTRUCTOR_0(SETaggingVisualModelProperties);

	SB_FACTORY_END;

	SB_INTERFACE_BEGIN;

	SB_INTERFACE_END;

SB_CLASS_END(SETaggingVisualModelProperties);

