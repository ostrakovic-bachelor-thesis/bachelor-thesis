#ifndef DRIVER_MANAGER_H
#define DRIVER_MANAGER_H

#include "Peripheral.h"
#include "DMA2D.h"
#include "GPIO.h"
#include "USART.h"
#include "I2C.h"
#include "ClockControl.h"
#include "ResetControl.h"
#include "PowerControl.h"
#include "SysTick.h"
#include "InterruptController.h"
#include "EXTI.h"
#include "SystemConfig.h"
#include "LTDC.h"
#include "DSIHost.h"
#include "FlashController.h"
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

  enum class I2CInstance : uint8_t
  {
    I2C1 = 0u,
    I2C2 = 1u,
    I2C3 = 2u
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

  enum class DMA2DInstance : uint8_t
  {
    GENERIC = 0u
  };

  enum class PowerControlInstance : uint8_t
  {
    GENERIC = 0u
  };

  enum class EXTIInstance : uint8_t
  {
    GENERIC = 0u
  };

  enum class SystemConfigInstance : uint8_t
  {
    GENERIC = 0u
  };

  enum class LTDCInstance : uint8_t
  {
    GENERIC = 0u
  };

  enum class DSIHostInstance : uint8_t
  {
    GENERIC = 0u
  };

  enum class FlashControllerInstance : uint8_t
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

  inline static I2C& getInstance(I2CInstance i2cInstance)
  {
    return s_i2cDriverInstance[static_cast<uint8_t>(i2cInstance)];
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

  inline static DMA2D& getInstance(DMA2DInstance dma2DInstance)
  {
    return s_dma2dDriverInstance;
  }

  inline static PowerControl& getInstance(PowerControlInstance powerControlInstance)
  {
    return s_powerControlDriverInstance;
  }

  inline static EXTI& getInstance(EXTIInstance extiInstance)
  {
    return s_extiDriverInstance;
  }

  inline static SystemConfig& getInstance(SystemConfigInstance systemConfigInstance)
  {
    return s_systemConfigDriverInstance;
  }

  inline static LTDC& getInstance(LTDCInstance ltdcInstance)
  {
    return s_ltdcDriverInstance;
  }

  inline static DSIHost& getInstance(DSIHostInstance dsiHostInstance)
  {
    return s_dsiHostDriverInstance;
  }

  inline static FlashController& getInstance(FlashControllerInstance flashControllerInstance)
  {
    return s_flashControllerDriverInstance;
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
  static DMA2D s_dma2dDriverInstance;

  //! TODO
  static GPIO s_gpioDriverInstance[];

  //! TODO
  static USART s_usartDriverInstance[];

  //! TODO
  static I2C s_i2cDriverInstance[];

  //! TODO
  static PowerControl s_powerControlDriverInstance;

  //! TODO
  static EXTI s_extiDriverInstance;

  //! TODO
  static SystemConfig s_systemConfigDriverInstance;

  //! TODO
  static LTDC s_ltdcDriverInstance;

  //! TODO
  static DSIHost s_dsiHostDriverInstance;

  //! TODO
  static FlashController s_flashControllerDriverInstance;
};

#endif // #ifndef DRIVER_MANAGER_H