#ifndef FT3267_CONFIG_H
#define FT3267_CONFIG_H

#include "FT3267.h"


FT3267::FT3267Config g_ft3267Config =
{
  .peripheralAddress                = 0x38,
  .performPowerOn                   = false,
  .setFT3267ResetLineToLowCallback  = nullptr,
  .setFT3267ResetLineToHighCallback = nullptr
};

#endif // #ifndef FT3267_CONFIG_H