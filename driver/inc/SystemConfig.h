#ifndef SYSTEM_CONFIG_H
#define SYSTEM_CONFIG_H

#include "stm32l4r9xx.h"
#include "ResetControl.h"
#include "Peripheral.h"


class SystemConfig
{
public:

  SystemConfig(SYSCFG_TypeDef *sysCfgPeripheralPtr, ResetControl *resetControlPtr);

  //! This enum class represents errors which can happen during method calls
  enum class ErrorCode : uint8_t
  {
    OK                               = 0u,
    INVALID_GPIO_PORT                = 1u,
    CAN_NOT_TURN_ON_PERIPHERAL_CLOCK = 2u
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

  ErrorCode init(void);

  ErrorCode mapGPIOToEXTILine(EXTILine extiLine, GPIOPort gpioPort);

  inline Peripheral getPeripheralTag(void) const
  {
    return static_cast<Peripheral>(reinterpret_cast<uintptr_t>(const_cast<SYSCFG_TypeDef*>(m_sysCfgPeripheralPtr)));
  }

private:

  ErrorCode enablePeripheralClock(void);
  void setGPIOToEXTILineMapping(EXTILine extiLine, GPIOPort gpioPort);

  static bool isGPIOPortValid(GPIOPort gpioPort, EXTILine extiLine);

  //! Pointer to SYSCFG peripheral
  SYSCFG_TypeDef *m_sysCfgPeripheralPtr;

  //! Pointer to Reset Control module
  ResetControl *m_resetControlPtr;
};

#endif // #ifndef SYSTEM_CONFIG_H