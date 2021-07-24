#ifndef DRIVER_MANAGER_H
#define DRIVER_MANAGER_H

#include "Peripheral.h"
#include "GPIO.h"
#include "USART.h"
#include "ClockControl.h"
#include "ResetControl.h"
#include "SysTick.h"
#include "InterruptController.h"
#include <cstdint>


class DriverManager
{
public:

  enum class GPIOInstance : uint8_t
  {
    GPIOA = 0u,
    GPIOB = 1u,
    GPIOC = 2u,
    GPIOD = 3u,
    GPIOE = 4u,
    GPIOF = 5u,
    GPIOG = 6u,
    GPIOH = 7u,
    GPIOI = 8u
  };

  enum class USARTInstance : uint8_t
  {
    USART1  = 0u,
    USART2  = 1u,
    USART3  = 2u,
    UART4   = 3u,
    UART5   = 4u,
    LPUART1 = 5u
  };

  enum class ClockControlInstance : uint8_t
  {
    GENERIC = 0u
  };

  enum class ResetControlInstance : uint8_t
  {
    GENERIC = 0u
  };

  enum class SysTickInstance : uint8_t
  {
    GENERIC = 0u
  };

  enum class InterruptControllerInstance : uint8_t
  {
    GENERIC = 0u
  };

  inline static GPIO& getInstance(GPIOInstance gpioInstance)
  {
    return s_gpioDriverInstance[static_cast<uint8_t>(gpioInstance)];
  }

  inline static USART& getInstance(USARTInstance usartInstance)
  {
    return s_usartDriverInstance[static_cast<uint8_t>(usartInstance)];
  }

  inline static ClockControl& getInstance(ClockControlInstance clockControlInstance)
  {
    return s_clockControlDriverInstance;
  }

  inline static ResetControl& getInstance(ResetControlInstance resetControlInstance)
  {
    return s_resetControlDriverInstance;
  }

  inline static SysTick& getInstance(SysTickInstance sysTickInstance)
  {
    return s_sysTickDriverInstance;
  }

  inline static InterruptController& getInstance(InterruptControllerInstance interruptControllerInstance)
  {
    return s_interruptControllerDriverInstance;
  }

private:

  //! TODO
  static ClockControl s_clockControlDriverInstance;

  //! TODO
  static ResetControl s_resetControlDriverInstance;

  //! TODO
  static InterruptController s_interruptControllerDriverInstance;

  //! TODO
  static SysTick s_sysTickDriverInstance;

  //! TODO
  static GPIO s_gpioDriverInstance[];

  //! TODO
  static USART s_usartDriverInstance[];
};

#endif // #ifndef DRIVER_MANAGER_H