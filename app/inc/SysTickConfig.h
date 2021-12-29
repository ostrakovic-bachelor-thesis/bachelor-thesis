#ifndef SYS_TICK_CONFIG_H
#define SYS_TICK_CONFIG_H

#include "SysTick.h"


SysTick::SysTickConfig g_sysTickConfig =
{
  .ticksPerSecond  = 10000u,
  .enableInterrupt = true,
  .enableOnInit    = true
};

#endif // #ifndef SYS_TICK_CONFIG_H