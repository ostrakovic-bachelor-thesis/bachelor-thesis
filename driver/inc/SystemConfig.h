#ifndef SYSTEM_CONFIG_H
#define SYSTEM_CONFIG_H

#include "stm32l4r9xx.h"
#include "Peripheral.h"


class SystemConfig
{
public:

  SystemConfig(SYSCFG_TypeDef *sysCfgPeripheralPtr);

  //! This enum class represents errors which can happen during method calls
  enum class ErrorCode : uint8_t
  {
    OK = 0u
  };

  enum class EXTILine : uint8_t
  {
    EXTI0  = 0u,
    EXTI1  = 1u,
    EXTI2  = 2u,
    EXTI3  = 3u,
    EXTI4  = 4u,
    EXTI5  = 5u,
    EXTI6  = 6u,
    EXTI7  = 7u,
    EXTI8  = 8u,
    EXTI9  = 9u,
    EXTI10 = 10u,
    EXTI11 = 11u,
    EXTI12 = 12u,
    EXTI13 = 13u,
    EXTI14 = 14u,
    EXTI15 = 15u
  };

  enum class GPIOPort : uint8_t
  {
    GPIOA = 0b0000,
    GPIOB = 0b0001,
    GPIOC = 0b0010,
    GPIOD = 0b0011,
    GPIOE = 0b0100,
    GPIOF = 0b0101,
    GPIOG = 0b0110,
    GPIOH = 0b0111,
    GPIOI = 0b1000
  };

  ErrorCode mapGPIOToEXTILine(EXTILine extiLine, GPIOPort gpioPort);

private:

  SYSCFG_TypeDef *m_sysCfgPeripheralPtr;
};

#endif // #ifndef SYSTEM_CONFIG_H