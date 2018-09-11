#pragma once

#include "SBProxy.hpp"
#include "PIBindingRegion.hpp"


// Class descriptor

// SAMSON Element generator pro tip: complete this descriptor to expose this class to SAMSON and other SAMSON Elements

SB_CLASS_BEGIN(PIBindingRegion);

  SB_CLASS_TYPE(SBCClass::Custom);
  SB_CLASS_DESCRIPTION("PIBindingRegion");

  SB_FACTORY_BEGIN;

    SB_CONSTRUCTOR_0(PIBindingRegion);

  SB_FACTORY_END;

  SB_INTERFACE_BEGIN;

  SB_INTERFACE_END;

SB_CLASS_END(PIBindingRegion);