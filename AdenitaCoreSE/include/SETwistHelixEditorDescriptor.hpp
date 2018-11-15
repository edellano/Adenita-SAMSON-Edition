/// \headerfile SBProxy.hpp "SBProxy.hpp"
#include "SBProxy.hpp"

/// \headerfile SETwistHelixEditor.hpp "SETwistHelixEditor.hpp"
#include "SETwistHelixEditor.hpp"


// Class descriptor

// SAMSON Element generator pro tip: complete this descriptor to expose this class to SAMSON and other SAMSON Elements

SB_CLASS_BEGIN(SETwistHelixEditor);

	SB_CLASS_TYPE(SBCClass::Editor);
        SB_CLASS_DESCRIPTION("Twist double helices along its axis");

	SB_FACTORY_BEGIN;

		SB_CONSTRUCTOR_0(SETwistHelixEditor);

	SB_FACTORY_END;

	SB_INTERFACE_BEGIN;

	SB_INTERFACE_END;

SB_CLASS_END(SETwistHelixEditor);

