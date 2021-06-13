#ifndef PERIPHERAL_H
#define PERIPHERAL_H

#include "stm32l4r9xx.h"
#include <cstdint>

#ifdef GPIOA
#undef GPIOA
#endif // #ifdef GPIOA

#ifdef GPIOB
#undef GPIOB
#endif // #ifdef GPIOB

#ifdef GPIOC
#undef GPIOC
#endif // #ifdef GPIOC

#ifdef GPIOD
#undef GPIOD
#endif // #ifdef GPIOD

#ifdef GPIOE
#undef GPIOE
#endif // #ifdef GPIOE

#ifdef GPIOF
#undef GPIOF
#endif // #ifdef GPIOF

#ifdef GPIOG
#undef GPIOG
#endif // #ifdef GPIOG

#ifdef GPIOH
#undef GPIOH
#endif // #ifdef GPIOH

#ifdef GPIOI
#undef GPIOI
#endif // #ifdef GPIOI


enum class Peripheral : uint32_t
{
  GPIOA = GPIOA_BASE,
  GPIOB = GPIOB_BASE,
  GPIOC = GPIOC_BASE,
  GPIOD = GPIOD_BASE,
  GPIOE = GPIOE_BASE,
  GPIOF = GPIOF_BASE,
  GPIOG = GPIOG_BASE,
  GPIOH = GPIOH_BASE,
  GPIOI = GPIOI_BASE,
#ifdef UNIT_TEST_DRIVER
  INVALID_PERIPHERAL = 0u,
#endif // #ifdef UNIT_TEST_DRIVER
};

#endif // #ifndef PERIPHERAL_H