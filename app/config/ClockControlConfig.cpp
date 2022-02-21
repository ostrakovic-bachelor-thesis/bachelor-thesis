#include "ClockControlConfig.h"


ClockControl::PLLConfiguration g_pllConfig =
{
  .inputClockDivider    = 1u,
  .inputClockMultiplier = 15u,
  .outputClockPDivider  = 7u,
  .outputClockQDivider  = 4u,
  .outputClockRDivider  = 2u,
  .enableOutputClockP   = false,
  .enableOutputClockQ   = false,
  .enableOutputClockR   = true
};

ClockControl::PLLSAI2Configuration g_pllSai2Config =
{
  .inputClockDivider    = 2u,
  .inputClockMultiplier = 30u,
  .outputClockPDivider  = 1u,
  .outputClockQDivider  = 1u,
  .outputClockRDivider  = 4u,
  .ltdcClockDivider     = 4u,
  .enableOutputClockP   = false,
  .enableOutputClockQ   = false,
  .enableOutputClockR   = true
};