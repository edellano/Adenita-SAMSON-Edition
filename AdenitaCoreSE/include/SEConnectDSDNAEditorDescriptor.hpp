/// \headerfile SBProxy.hpp "SBProxy.hpp"
#include "SBProxy.hpp"

/// \headerfile SEConnectSSDNAEditor.hpp "SEConnectSSDNAEditor.hpp"
#include "SEConnectDSDNAEditor.hpp"


// Class descriptor

// SAMSON Element generator pro tip: complete this descriptor to expose this class to SAMSON and other SAMSON Elements

SB_CLASS_BEGIN(SEConnectDSDNAEditor);

	SB_CLASS_TYPE(SBCClass::Editor);
        SB_CLASS_DESCRIPTION("dsDNA Connection Editor: Join two dsDNA strands");

	SB_FACTORY_BEGIN;

		SB_CONSTRUCTOR_0(SEConnectDSDNAEditor);

	SB_FACTORY_END;

	SB_INTERFACE_BEGIN;

	SB_INTERFACE_END;

SB_CLASS_END(SEConnectDSDNAEditor);

