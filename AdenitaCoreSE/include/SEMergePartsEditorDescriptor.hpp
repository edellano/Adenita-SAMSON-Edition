/// \headerfile SBProxy.hpp "SBProxy.hpp"
#include "SBProxy.hpp"

/// \headerfile SEMergePartsEditor.hpp "SEMergePartsEditor.hpp"
#include "SEMergePartsEditor.hpp"


// Class descriptor

// SAMSON Element generator pro tip: complete this descriptor to expose this class to SAMSON and other SAMSON Elements

SB_CLASS_BEGIN(SEMergePartsEditor);

	SB_CLASS_TYPE(SBCClass::Editor);
        SB_CLASS_DESCRIPTION("Merge components into a new one");

	SB_FACTORY_BEGIN;

		SB_CONSTRUCTOR_0(SEMergePartsEditor);

	SB_FACTORY_END;

	SB_INTERFACE_BEGIN;

	SB_INTERFACE_END;

SB_CLASS_END(SEMergePartsEditor);

