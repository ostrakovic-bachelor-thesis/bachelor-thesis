#ifndef DRIVER_MANAGER_H
#define DRIVER_MANAGER_H

#include "Peripheral.h"
#include "GPIO.h"
#include "USART.h"


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

  inline static GPIO& getInstance(GPIOInstance gpioInstance)
  {
    return s_gpioDriverInstance[static_cast<uint8_t>(gpioInstance)];
  }

  inline static USART& getInstance(USARTInstance usartInstance)
  {
    return s_usartDriverInstance[static_cast<uint8_t>(usartInstance)];
  }

private:

  static GPIO s_gpioDriverInstance[];
  static USART s_usartDriverInstance[];
};

#endif // #ifndef DRIVER_MANAGER_H