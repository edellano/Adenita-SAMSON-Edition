/// \headerfile SBProxy.hpp "SBProxy.hpp"
#include "SBProxy.hpp"

/// \headerfile SEWireframeEditor.hpp "SEWireframeEditor.hpp"
#include "SEWireframeEditor.hpp"


// Class descriptor

// SAMSON Element generator pro tip: complete this descriptor to expose this class to SAMSON and other SAMSON Elements

SB_CLASS_BEGIN(SEWireframeEditor);

	SB_CLASS_TYPE(SBCClass::Editor);
        SB_CLASS_DESCRIPTION("Create a wireframe nanostructure");

	SB_FACTORY_BEGIN;

		SB_CONSTRUCTOR_0(SEWireframeEditor);

	SB_FACTORY_END;

	SB_INTERFACE_BEGIN;

	SB_INTERFACE_END;

SB_CLASS_END(SEWireframeEditor);

