#ifndef SYS_TICK_CONFIG_H
#define SYS_TICK_CONFIG_H

#include "SysTick.h"


SysTick::SysTickConfig g_sysTickConfig =
{
  .ticksPerSecond  = 1000u,
  .enableInterrupt = true,
  .enableOnInit    = true
};

#endif // #ifndef SYS_TICK_CONFIG_H