#include "SysTickConfig.h"


SysTick::SysTickConfig g_sysTickConfig =
{
  .ticksPerSecond  = 10000u,
  .enableInterrupt = true,
  .enableOnInit    = true
};