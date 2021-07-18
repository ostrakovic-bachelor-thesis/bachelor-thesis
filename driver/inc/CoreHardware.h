#ifndef CORE_HARDWARE_H
#define CORE_HARDWARE_H

#include "stm32l4r9xx.h"
#include <cstdint>


#ifdef SysTick 
#undef SysTick 
#endif // #ifdef SysTick 

#ifdef NVIC
#undef NVIC
#endif // #ifdef NVIC

#ifdef SCB
#undef SCB
#endif // #ifdef SCB


enum class CoreHardware : uintptr_t
{
  SYSTICK = SysTick_BASE,
  NVIC    = NVIC_BASE,
  SCB     = SCB_BASE,

#ifdef UNIT_TEST_DRIVER
  INVALID_CORE_HARDWARE = 0u,
#endif // #ifdef UNIT_TEST_DRIVER
};

#endif // #ifndef CORE_HARDWARE_H