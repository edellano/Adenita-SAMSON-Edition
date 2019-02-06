/// \headerfile SBProxy.hpp "SBProxy.hpp"
#include "SBProxy.hpp"

/// \headerfile SEConnectSSDNAEditor.hpp "SEConnectSSDNAEditor.hpp"
#include "SEConnectSSDNAEditor.hpp"


// Class descriptor

// SAMSON Element generator pro tip: complete this descriptor to expose this class to SAMSON and other SAMSON Elements

SB_CLASS_BEGIN(SEConnectSSDNAEditor);

	SB_CLASS_TYPE(SBCClass::Editor);
        SB_CLASS_DESCRIPTION("ssDNA Connection : Connect and create crossovers between ssDNA");

	SB_FACTORY_BEGIN;

		SB_CONSTRUCTOR_0(SEConnectSSDNAEditor);

	SB_FACTORY_END;

	SB_INTERFACE_BEGIN;

	SB_INTERFACE_END;

SB_CLASS_END(SEConnectSSDNAEditor);

