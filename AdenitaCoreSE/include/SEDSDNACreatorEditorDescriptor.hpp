/// \headerfile SBProxy.hpp "SBProxy.hpp"
#include "SBProxy.hpp"

/// \headerfile SEDSDNACreatorEditor.hpp "SEDSDNACreatorEditor.hpp"
#include "SEDSDNACreatorEditor.hpp"


// Class descriptor

// SAMSON Element generator pro tip: complete this descriptor to expose this class to SAMSON and other SAMSON Elements

SB_CLASS_BEGIN(SEDSDNACreatorEditor);

	SB_CLASS_TYPE(SBCClass::Editor);
        SB_CLASS_DESCRIPTION("DNA Creator : Add ssDNA and dsDNA");

	SB_FACTORY_BEGIN;

		SB_CONSTRUCTOR_0(SEDSDNACreatorEditor);

	SB_FACTORY_END;

	SB_INTERFACE_BEGIN;

	SB_INTERFACE_END;

SB_CLASS_END(SEDSDNACreatorEditor);

