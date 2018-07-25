/// \headerfile SBProxy.hpp "SBProxy.hpp"
#include "SBProxy.hpp"

/// \headerfile SEBreakEditor.hpp "SEBreakEditor.hpp"
#include "SEBreakEditor.hpp"


// Class descriptor

// SAMSON Element generator pro tip: complete this descriptor to expose this class to SAMSON and other SAMSON Elements

SB_CLASS_BEGIN(SEBreakEditor);

	SB_CLASS_TYPE(SBCClass::Editor);
        SB_CLASS_DESCRIPTION("Break tool");

	SB_FACTORY_BEGIN;

		SB_CONSTRUCTOR_0(SEBreakEditor);

	SB_FACTORY_END;

	SB_INTERFACE_BEGIN;

	SB_INTERFACE_END;

SB_CLASS_END(SEBreakEditor);

