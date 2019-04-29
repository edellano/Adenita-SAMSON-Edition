/// \headerfile SBProxy.hpp "SBProxy.hpp"
#include "SBProxy.hpp"

/// \headerfile SEWireframeEditor.hpp "SEWireframeEditor.hpp"
#include "SEBottomUpEditor.hpp"


// Class descriptor

// SAMSON Element generator pro tip: complete this descriptor to expose this class to SAMSON and other SAMSON Elements

SB_CLASS_BEGIN(SEBottomUpEditor);

	SB_CLASS_TYPE(SBCClass::Editor);
        SB_CLASS_DESCRIPTION("Build an Adenita design from bottom-up");

	SB_FACTORY_BEGIN;

		SB_CONSTRUCTOR_0(SEBottomUpEditor);

	SB_FACTORY_END;

	SB_INTERFACE_BEGIN;

	SB_INTERFACE_END;

SB_CLASS_END(SEBottomUpEditor);

