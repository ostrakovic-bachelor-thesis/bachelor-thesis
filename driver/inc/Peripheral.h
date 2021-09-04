#ifndef PERIPHERAL_H
#define PERIPHERAL_H

#include "stm32l4r9xx.h"
#include <cstdint>


#ifdef RCC
#undef RCC
#endif // #ifdef RCC

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

#ifdef USART1
#undef USART1
#endif // #ifdef USART1

#ifdef USART2
#undef USART2
#endif // #ifdef USART2

#ifdef USART3
#undef USART3
#endif // #ifdef USART3

#ifdef UART4
#undef UART4
#endif // #ifdef UART4

#ifdef UART5
#undef UART5
#endif // #ifdef UART5

#ifdef LPUART1
#undef LPUART1
#endif // #ifdef LPUART1

#ifdef DMA2D
#undef DMA2D
#endif // #ifdef DMA2D

#ifdef LTDC
#undef LTDC
#endif // #ifdef LTDC

#ifdef I2C1
#undef I2C1
#endif // #ifdef I2C1

#ifdef I2C2
#undef I2C2
#endif // #ifdef I2C2

#ifdef I2C3
#undef I2C3
#endif // #ifdef I2C3


enum class Peripheral : uintptr_t
{
  RCC     = RCC_BASE,
  GPIOA   = GPIOA_BASE,
  GPIOB   = GPIOB_BASE,
  GPIOC   = GPIOC_BASE,
  GPIOD   = GPIOD_BASE,
  GPIOE   = GPIOE_BASE,
  GPIOF   = GPIOF_BASE,
  GPIOG   = GPIOG_BASE,
  GPIOH   = GPIOH_BASE,
  GPIOI   = GPIOI_BASE,
  USART1  = USART1_BASE,
  USART2  = USART2_BASE,
  USART3  = USART3_BASE,
  UART4   = UART4_BASE,
  UART5   = UART5_BASE,
  LPUART1 = LPUART1_BASE,
  DMA2D   = DMA2D_BASE,
  I2C1    = I2C1_BASE,
  I2C2    = I2C2_BASE,
  I2C3    = I2C3_BASE,

#ifdef UNIT_TEST
  INVALID_PERIPHERAL = 0u,
#endif // #ifdef UNIT_TEST
};

#endif // #ifndef PERIPHERAL_H