/// \headerfile SBProxy.hpp "SBProxy.hpp"
#include "SBProxy.hpp"

/// \headerfile SELatticeCreatorEditor.hpp "SELatticeCreatorEditor.hpp"
#include "SELatticeCreatorEditor.hpp"


// Class descriptor

// SAMSON Element generator pro tip: complete this descriptor to expose this class to SAMSON and other SAMSON Elements

SB_CLASS_BEGIN(SELatticeCreatorEditor);

	SB_CLASS_TYPE(SBCClass::Editor);
        SB_CLASS_DESCRIPTION("SELatticeCreatorEditor : Create dsDNA in a Honeycomb or Square Lattice");

	SB_FACTORY_BEGIN;

		SB_CONSTRUCTOR_0(SELatticeCreatorEditor);

	SB_FACTORY_END;

	SB_INTERFACE_BEGIN;

	SB_INTERFACE_END;

SB_CLASS_END(SELatticeCreatorEditor);

