/// \headerfile SBProxy.hpp "SBProxy.hpp"
#include "SBProxy.hpp"

/// \headerfile SEAdenitaCoreSEApp.hpp "SEAdenitaCoreSEApp.hpp"
#include "SEAdenitaCoreSEApp.hpp"


// Class descriptor

// SAMSON Element generator pro tip: complete this descriptor to expose this class to SAMSON and other SAMSON Elements

SB_CLASS_BEGIN(SEAdenitaCoreSEApp);

	SB_CLASS_TYPE(SBCClass::App);
        SB_CLASS_DESCRIPTION("Adenita: DNA Nanostructures Modeling and Visualization Toolkit");

	SB_FACTORY_BEGIN;

		SB_CONSTRUCTOR_0(SEAdenitaCoreSEApp);

	SB_FACTORY_END;

	SB_INTERFACE_BEGIN;

    SB_FUNCTION_3(void, SEAdenitaCoreSEApp, AddPartToActiveLayer, ADNPointer<ADNPart>, bool, bool);

	SB_INTERFACE_END;

SB_CLASS_END(SEAdenitaCoreSEApp);

