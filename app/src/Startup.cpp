#include "Startup.h"
#include "ResetCOntrol.h"
#include "ClockControl.h"
#include "GPIO.h"
#include "USART.h"
#include "DMA2D.h"
#include "PowerControl.h"
#include "LTDC.h"
#include "DSIHost.h"
#include "DriverManager.h"
#include "GPIOManager.h"
#include "MFXSTM32L152.h"
#include "RaydiumRM67160.h"
#include "FT3267.h"
#include "FrameBuffer.h"
#include <cstdint>
#include <cstdio>
#include <cinttypes>

#include "GPIOConfig.h"
#include "USARTConfig.h"
#include "I2CConfig.h"
#include "SysTickConfig.h"
#include "LTDCConfig.h"
#include "DSIHostConfig.h"
#include "MFXSTM32L152Config.h"
#include "FT3267Config.h"


const uint32_t g_fgBitmap[20][20] =
{
  {0xFF00FF0F, 0xFF00FF0F, 0xFF00FF0F, 0xFF00FF0F, 0xFF00FF0F, 0xFF00FF0F, 0xFF00FF0F, 0xFF00FF0F, 0xFF00FF0F, 0xFF00FF0F,
    0xFF00FF0F, 0xFF00FF0F, 0xFF00FF0F, 0xFF00FF0F, 0xFF00FF0F, 0xFF00FF0F, 0xFF00FF0F, 0xFF00FF0F, 0xFF00FF0F, 0xFF00FF0F},
  {0xFF00FF0F, 0xFF00FF0F, 0xFF00FF0F, 0xFF00FF0F, 0xFF00FF0F, 0xFF00FF0F, 0xFF00FF0F, 0xFF00FF0F, 0xFF00FF0F, 0xFF00FF0F,
    0xFF00FF0F, 0xFF00FF0F, 0xFF00FF0F, 0xFF00FF0F, 0xFF00FF0F, 0xFF00FF0F, 0xFF00FF0F, 0xFF00FF0F, 0xFF00FF0F, 0xFF00FF0F},
  {0xFF00FF0F, 0xFF00FF0F, 0xFF00FF0F, 0xFF00FF0F, 0xFF00FF0F, 0xFF00FF0F, 0xFF00FF0F, 0xFF00FF0F, 0xFF00FF0F, 0xFF00FF0F,
    0xFF00FF0F, 0xFF00FF0F, 0xFF00FF0F, 0xFF00FF0F, 0xFF00FF0F, 0xFF00FF0F, 0xFF00FF0F, 0xFF00FF0F, 0xFF00FF0F, 0xFF00FF0F},
  {0xFF00FF0F, 0xFF00FF0F, 0xFF00FF0F, 0xFF00FF0F, 0xFF00FF0F, 0xFF00FF0F, 0xFF00FF0F, 0xFF00FF0F, 0xFF00FF0F, 0xFF00FF0F,
    0xFF00FF0F, 0xFF00FF0F, 0xFF00FF0F, 0xFF00FF0F, 0xFF00FF0F, 0xFF00FF0F, 0xFF00FF0F, 0xFF00FF0F, 0xFF00FF0F, 0xFF00FF0F},
  {0xFF00FF0F, 0xFF00FF0F, 0xFF00FF0F, 0xFF00FF0F, 0xFF00FF0F, 0xFF00FF0F, 0xFF00FF0F, 0xFF00FF0F, 0xFF00FF0F, 0xFF00FF0F,
    0xFF00FF0F, 0xFF00FF0F, 0xFF00FF0F, 0xFF00FF0F, 0xFF00FF0F, 0xFF00FF0F, 0xFF00FF0F, 0xFF00FF0F, 0xFF00FF0F, 0xFF00FF0F},
  {0xFF00FF0F, 0xFF00FF0F, 0xFF00FF0F, 0xFF00FF0F, 0xFF00FF0F, 0xFF00FF0F, 0xFF00FF0F, 0xFF00FF0F, 0xFF00FF0F, 0xFF00FF0F,
    0xFF00FF0F, 0xFF00FF0F, 0xFF00FF0F, 0xFF00FF0F, 0xFF00FF0F, 0xFF00FF0F, 0xFF00FF0F, 0xFF00FF0F, 0xFF00FF0F, 0xFF00FF0F},
  {0xFF00FF0F, 0xFF00FF0F, 0xFF00FF0F, 0xFF00FF0F, 0xFF00FF0F, 0xFF00FF0F, 0xFF00FF0F, 0xFF00FF0F, 0xFF00FF0F, 0xFF00FF0F,
    0xFF00FF0F, 0xFF00FF0F, 0xFF00FF0F, 0xFF00FF0F, 0xFF00FF0F, 0xFF00FF0F, 0xFF00FF0F, 0xFF00FF0F, 0xFF00FF0F, 0xFF00FF0F},
  {0xFF00FF0F, 0xFF00FF0F, 0xFF00FF0F, 0xFF00FF0F, 0xFF00FF0F, 0xFF00FF0F, 0xFF00FF0F, 0xFF00FF0F, 0xFF00FF0F, 0xFF00FF0F,
    0xFF00FF0F, 0xFF00FF0F, 0xFF00FF0F, 0xFF00FF0F, 0xFF00FF0F, 0xFF00FF0F, 0xFF00FF0F, 0xFF00FF0F, 0xFF00FF0F, 0xFF00FF0F},
  {0x0FFF00FF, 0x0FFF00FF, 0x0FFF00FF, 0x0FFF00FF, 0x0FFF00FF, 0x0FFF00FF, 0x0FFF00FF, 0x0FFF00FF, 0x0FFF00FF, 0x0FFF00FF,
    0x0FFF00FF, 0x0FFF00FF, 0x0FFF00FF, 0x0FFF00FF, 0x0FFF00FF, 0x0FFF00FF, 0x0FFF00FF, 0x0FFF00FF, 0x0FFF00FF, 0x0FFF00FF},
  {0xFF00FF0F, 0xFF00FF0F, 0xFF00FF0F, 0xFF00FF0F, 0xFF00FF0F, 0xFF00FF0F, 0xFF00FF0F, 0xFF00FF0F, 0xFF00FF0F, 0xFF00FF0F,
    0xFF00FF0F, 0xFF00FF0F, 0xFF00FF0F, 0xFF00FF0F, 0xFF00FF0F, 0xFF00FF0F, 0xFF00FF0F, 0xFF00FF0F, 0xFF00FF0F, 0xFF00FF0F},
  {0xFF00FF0F, 0xFF00FF0F, 0xFF00FF0F, 0xFF00FF0F, 0xFF00FF0F, 0xFF00FF0F, 0xFF00FF0F, 0xFF00FF0F, 0xFF00FF0F, 0xFF00FF0F,
    0xFF00FF0F, 0xFF00FF0F, 0xFF00FF0F, 0xFF00FF0F, 0xFF00FF0F, 0xFF00FF0F, 0xFF00FF0F, 0xFF00FF0F, 0xFF00FF0F, 0xFF00FF0F},
  {0xFF00FF0F, 0xFF00FF0F, 0xFF00FF0F, 0xFF00FF0F, 0xFF00FF0F, 0xFF00FF0F, 0xFF00FF0F, 0xFF00FF0F, 0xFF00FF0F, 0xFF00FF0F,
    0xFF00FF0F, 0xFF00FF0F, 0xFF00FF0F, 0xFF00FF0F, 0xFF00FF0F, 0xFF00FF0F, 0xFF00FF0F, 0xFF00FF0F, 0xFF00FF0F, 0xFF00FF0F},
  {0xFF00FF0F, 0xFF00FF0F, 0xFF00FF0F, 0xFF00FF0F, 0xFF00FF0F, 0xFF00FF0F, 0xFF00FF0F, 0xFF00FF0F, 0xFF00FF0F, 0xFF00FF0F,
    0xFF00FF0F, 0xFF00FF0F, 0xFF00FF0F, 0xFF00FF0F, 0xFF00FF0F, 0xFF00FF0F, 0xFF00FF0F, 0xFF00FF0F, 0xFF00FF0F, 0xFF00FF0F},
  {0xFF00FF0F, 0xFF00FF0F, 0xFF00FF0F, 0xFF00FF0F, 0xFF00FF0F, 0xFF00FF0F, 0xFF00FF0F, 0xFF00FF0F, 0xFF00FF0F, 0xFF00FF0F,
    0xFF00FF0F, 0xFF00FF0F, 0xFF00FF0F, 0xFF00FF0F, 0xFF00FF0F, 0xFF00FF0F, 0xFF00FF0F, 0xFF00FF0F, 0xFF00FF0F, 0xFF00FF0F},
  {0xFF00FF0F, 0xFF00FF0F, 0xFF00FF0F, 0xFF00FF0F, 0xFF00FF0F, 0xFF00FF0F, 0xFF00FF0F, 0xFF00FF0F, 0xFF00FF0F, 0xFF00FF0F,
    0xFF00FF0F, 0xFF00FF0F, 0xFF00FF0F, 0xFF00FF0F, 0xFF00FF0F, 0xFF00FF0F, 0xFF00FF0F, 0xFF00FF0F, 0xFF00FF0F, 0xFF00FF0F},
  {0xFF00FF0F, 0xFF00FF0F, 0xFF00FF0F, 0xFF00FF0F, 0xFF00FF0F, 0xFF00FF0F, 0xFF00FF0F, 0xFF00FF0F, 0xFF00FF0F, 0xFF00FF0F,
    0xFF00FF0F, 0xFF00FF0F, 0xFF00FF0F, 0xFF00FF0F, 0xFF00FF0F, 0xFF00FF0F, 0xFF00FF0F, 0xFF00FF0F, 0xFF00FF0F, 0xFF00FF0F},
  {0xFF00FF0F, 0xFF00FF0F, 0xFF00FF0F, 0xFF00FF0F, 0xFF00FF0F, 0xFF00FF0F, 0xFF00FF0F, 0xFF00FF0F, 0xFF00FF0F, 0xFF00FF0F,
    0xFF00FF0F, 0xFF00FF0F, 0xFF00FF0F, 0xFF00FF0F, 0xFF00FF0F, 0xFF00FF0F, 0xFF00FF0F, 0xFF00FF0F, 0xFF00FF0F, 0xFF00FF0F},
  {0xFF00FF0F, 0xFF00FF0F, 0xFF00FF0F, 0xFF00FF0F, 0xFF00FF0F, 0xFF00FF0F, 0xFF00FF0F, 0xFF00FF0F, 0xFF00FF0F, 0xFF00FF0F,
    0xFF00FF0F, 0xFF00FF0F, 0xFF00FF0F, 0xFF00FF0F, 0xFF00FF0F, 0xFF00FF0F, 0xFF00FF0F, 0xFF00FF0F, 0xFF00FF0F, 0xFF00FF0F},
  {0x0FFF00FF, 0x0FFF00FF, 0x0FFF00FF, 0x0FFF00FF, 0x0FFF00FF, 0x0FFF00FF, 0x0FFF00FF, 0x0FFF00FF, 0x0FFF00FF, 0x0FFF00FF,
    0x0FFF00FF, 0x0FFF00FF, 0x0FFF00FF, 0x0FFF00FF, 0x0FFF00FF, 0x0FFF00FF, 0x0FFF00FF, 0x0FFF00FF, 0x0FFF00FF, 0x0FFF00FF},
  {0xFF00FF0F, 0xFF00FF0F, 0xFF00FF0F, 0xFF00FF0F, 0xFF00FF0F, 0xFF00FF0F, 0xFF00FF0F, 0xFF00FF0F, 0xFF00FF0F, 0xFF00FF0F,
    0xFF00FF0F, 0xFF00FF0F, 0xFF00FF0F, 0xFF00FF0F, 0xFF00FF0F, 0xFF00FF0F, 0xFF00FF0F, 0xFF00FF0F, 0xFF00FF0F, 0xFF00FF0F},
};

const uint32_t g_bgBitmap[9][9] =
{
  {0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF},
  {0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF},
  {0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF},
  {0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF},
  {0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF},
  {0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF},
  {0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF},
  {0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF},
  {0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF}
};

MFXSTM32L152 g_mfx = MFXSTM32L152(
  &DriverManager::getInstance(DriverManager::I2CInstance::I2C1),
  &DriverManager::getInstance(DriverManager::SysTickInstance::GENERIC));

RaydiumRM67160 g_displayRM67160 = RaydiumRM67160(
  &DriverManager::getInstance(DriverManager::DSIHostInstance::GENERIC),
  &DriverManager::getInstance(DriverManager::SysTickInstance::GENERIC));

FT3267 g_ft3267 = FT3267(
  &DriverManager::getInstance(DriverManager::I2CInstance::I2C1),
  &DriverManager::getInstance(DriverManager::SysTickInstance::GENERIC));

extern "C" void initSYSCLOCK(void);

void panic(void)
{
  volatile bool loopWhileTrue = true;

  while (loopWhileTrue);
}

void mfxIrqCallback(EXTI::EXTILine extiLine)
{
  MFXSTM32L152::ErrorCode errorCode = g_mfx.runtimeTask();
  if (MFXSTM32L152::ErrorCode::OK != errorCode)
  {
    panic();
  }
}

void turnOnGreenLed(void *argumentPtr)
{
  GPIO &gpioH = DriverManager::getInstance(DriverManager::GPIOInstance::GPIOH);

  gpioH.setPinState(GPIO::Pin::PIN4, GPIO::PinState::HIGH);
}

void turnOffGreenLed(void *argumentPtr)
{
  GPIO &gpioH = DriverManager::getInstance(DriverManager::GPIOInstance::GPIOH);

  gpioH.setPinState(GPIO::Pin::PIN4, GPIO::PinState::LOW);
}

void enableDSI3V3Callback(void)
{
  MFXSTM32L152::ErrorCode errorCode = g_mfx.configureGPIOPin(MFXSTM32L152::GPIOPin::PIN8, g_mfxGPIOPin8Config);
  if (MFXSTM32L152::ErrorCode::OK != errorCode)
  {
    panic();
  }

  errorCode = g_mfx.setGPIOPinState(MFXSTM32L152::GPIOPin::PIN8, MFXSTM32L152::GPIOPinState::LOW);
  if (MFXSTM32L152::ErrorCode::OK != errorCode)
  {
    panic();
  }
}

void enableDSI1V8Callback(void)
{
  MFXSTM32L152::ErrorCode errorCode = g_mfx.configureGPIOPin(MFXSTM32L152::GPIOPin::PIN18, g_mfxGPIOPin18Config);
  if (MFXSTM32L152::ErrorCode::OK != errorCode)
  {
    panic();
  }

  errorCode = g_mfx.setGPIOPinState(MFXSTM32L152::GPIOPin::PIN18, MFXSTM32L152::GPIOPinState::LOW);
  if (MFXSTM32L152::ErrorCode::OK != errorCode)
  {
    panic();
  }
}

void setDSIResetLineToLowCallback(void)
{
  //g_mfx.setGPIOPinState(MFXSTM32L152::GPIOPin::PIN10, MFXSTM32L152::GPIOPinState::LOW);
}

void setDSIResetLineToHighCallback(void)
{
  MFXSTM32L152::ErrorCode errorCode = g_mfx.configureGPIOPin(MFXSTM32L152::GPIOPin::PIN10, g_mfxGPIOPin10Config);
  if (MFXSTM32L152::ErrorCode::OK != errorCode)
  {
    panic();
  }

  errorCode = g_mfx.setGPIOPinState(MFXSTM32L152::GPIOPin::PIN10, MFXSTM32L152::GPIOPinState::HIGH);
  if (MFXSTM32L152::ErrorCode::OK != errorCode)
  {
    panic();
  }
}

void startup(void)
{
  PowerControl &powerControl = DriverManager::getInstance(DriverManager::PowerControlInstance::GENERIC);
  SystemConfig &systemConfig = DriverManager::getInstance(DriverManager::SystemConfigInstance::GENERIC);
  ClockControl &clockControl = DriverManager::getInstance(DriverManager::ClockControlInstance::GENERIC);
  EXTI &exti = DriverManager::getInstance(DriverManager::EXTIInstance::GENERIC);
  SysTick &sysTick = DriverManager::getInstance(DriverManager::SysTickInstance::GENERIC);
  USART &usart2 = DriverManager::getInstance(DriverManager::USARTInstance::USART2);
  InterruptController &interruptController = DriverManager::getInstance(DriverManager::InterruptControllerInstance::GENERIC);
  DMA2D &dma2D = DriverManager::getInstance(DriverManager::DMA2DInstance::GENERIC);
  I2C &i2c1 = DriverManager::getInstance(DriverManager::I2CInstance::I2C1);
  LTDC &ltdc = DriverManager::getInstance(DriverManager::LTDCInstance::GENERIC);
  DSIHost &dsiHost = DriverManager::getInstance(DriverManager::DSIHostInstance::GENERIC);

  initSYSCLOCK();

  for (uint32_t i = 0u; i < FRAME_BUFFER_SIZE; ++i)
  {
    g_frameBuffer[i] = 0xFF;
  }

  {
    ClockControl::PLLSAI2Configuration pllSai2Config =
    {
      .inputClockDivider    = 2u,
      .inputClockMultiplier = 30u,
      .outputClockPDivider  = 1u,
      .outputClockQDivider  = 1u,
      .outputClockRDivider  = 4u,
      .ltdcClockDivider     = 4u,
      .enableOutputClockP   = false,
      .enableOutputClockQ   = false,
      .enableOutputClockR   = true
    };

    ClockControl::ErrorCode errorCode = clockControl.configurePLL(pllSai2Config);
    if (ClockControl::ErrorCode::OK != errorCode)
    {
      panic();
    }
  }

  {
    SysTick::ErrorCode error = sysTick.init(g_sysTickConfig);
    if (SysTick::ErrorCode::OK != error)
    {
      panic();
    }
  }

  {
    PowerControl::ErrorCode errorCode = powerControl.init();
    if (PowerControl::ErrorCode::OK != errorCode)
    {
      panic();
    }

    errorCode = powerControl.enablePowerSupplyVDDIO2();
    if (PowerControl::ErrorCode::OK != errorCode)
    {
      panic();
    }
  }

  {
    USART::ErrorCode error = usart2.init(g_usart2Config);
    if (USART::ErrorCode::OK != error)
    {
      panic();
    }
  }

  {
    I2C::ErrorCode error = i2c1.init(g_i2c1Config);
    if (I2C::ErrorCode::OK != error)
    {
      panic();
    }
  }

  {
    DMA2D::ErrorCode error = dma2D.init();
    if (DMA2D::ErrorCode::OK != error)
    {
      panic();
    }
  }

  {
    GPIOManager::ErrorCode errorCode = GPIOManager::configureGPIOPins(g_gpioPinsConfig);
    if (GPIOManager::ErrorCode::OK != errorCode)
    {
      panic();
    }
  }

  {
    LTDC::ErrorCode errorCode = ltdc.init(g_ltdcConfig, g_ltdcLayer1Config);
    if (LTDC::ErrorCode::OK != errorCode)
    {
      panic();
    }
  }

  {
    DSIHost::ErrorCode errorCode = dsiHost.init(g_dsiHostConfig);
    if (DSIHost::ErrorCode::OK != errorCode)
    {
      panic();
    }
  }

/*
  DMA2D::BlendBitmapConfig blendBitmapConfig =
  {
    .blendRectangleDimension =
    {
      .width  = 10,+
      .height = 10
    },

    .foregroundBufferConfig =
    {
      .colorFormat = DMA2D::InputColorFormat::ABGR8888,

      .position =
      {
        .x = 0,
        .y = 0
      },

      .bufferDimension =
      {
        .width  = 20,
        .height = 20
      },

      .bufferPtr = g_fgBitmap
    },

    .backgroundBufferConfig =
    {
      .colorFormat = DMA2D::InputColorFormat::RGB888,

      .position =
      {
        .x = 0,
        .y = 0
      },

      .bufferDimension =
      {
        .width  = 12,
        .height = 12
      },

      .bufferPtr = g_bgBitmap
    },

    .outputBufferConfig =
    {
      .colorFormat = DMA2D::OutputColorFormat::RGB565,

      .position =
      {
        .x = 40,
        .y = 40
      },

      .bufferDimension =
      {
        .width  = 100,
        .height = 100
      },

      .bufferPtr = g_frameBuffer
    }
  };
*/

/*
  DMA2D::ErrorCode errorCode = dma2D.blendBitmap(blendBitmapConfig);
  if (DMA2D::ErrorCode::OK != errorCode)
  {
    panic();
  }
*/

  {
    SystemConfig::ErrorCode errorCode = systemConfig.init();
    if (SystemConfig::ErrorCode::OK != errorCode)
    {
      panic();
    }

    errorCode = systemConfig.mapGPIOToEXTILine(SystemConfig::EXTILine::EXTI1, SystemConfig::GPIOPort::GPIOI);
    if (SystemConfig::ErrorCode::OK != errorCode)
    {
      panic();
    }
  }

  {
    EXTI::EXTIConfig extiLine1Config =
    {
      .isInterruptMasked = false,
      .interruptTrigger  = EXTI::InterruptTrigger::RISING_EDGE,
      .interruptCallback = mfxIrqCallback
    };

    EXTI::ErrorCode errorCode = exti.configureEXTILine(EXTI::EXTILine::LINE1, extiLine1Config);
    if (EXTI::ErrorCode::OK != errorCode)
    {
      panic();
    }
  }

  {
    InterruptController::ErrorCode error = interruptController.enableInterrupt(USART2_IRQn);
    if (InterruptController::ErrorCode::OK != error)
    {
      panic();
    }

    error = interruptController.enableInterrupt(EXTI1_IRQn);
    if (InterruptController::ErrorCode::OK != error)
    {
      panic();
    }

    error = interruptController.enableInterrupt(I2C1_EV_IRQn);
    if (InterruptController::ErrorCode::OK != error)
    {
      panic();
    }

    error = interruptController.enableInterrupt(I2C1_ER_IRQn);
    if (InterruptController::ErrorCode::OK != error)
    {
      panic();
    }
  }

  {
    MFXSTM32L152::ErrorCode error = g_mfx.init(g_mfxConfig);
    if (MFXSTM32L152::ErrorCode::OK != error)
    {
      panic();
    }

    error = g_mfx.wakeUp();
    if (MFXSTM32L152::ErrorCode::OK != error)
    {
      panic();
    }

    error = g_mfx.enableGPIO();
    if (MFXSTM32L152::ErrorCode::OK != error)
    {
      panic();
    }

    error = g_mfx.enableInterrupt(MFXSTM32L152::Interrupt::GPIO);
    if (MFXSTM32L152::ErrorCode::OK != error)
    {
      panic();
    }

    error = g_mfx.configureIRQPin(g_mfxIrqPinConfiguration);
    if (MFXSTM32L152::ErrorCode::OK != error)
    {
      panic();
    }

    error = g_mfx.configureGPIOPin(MFXSTM32L152::GPIOPin::PIN0, g_mfxGPIOPin0Config);
    if (MFXSTM32L152::ErrorCode::OK != error)
    {
      panic();
    }

    error = g_mfx.configureGPIOPin(MFXSTM32L152::GPIOPin::PIN1, g_mfxGPIOPin1Config);
    if (MFXSTM32L152::ErrorCode::OK != error)
    {
      panic();
    }

    error = g_mfx.configureGPIOPin(MFXSTM32L152::GPIOPin::PIN2, g_mfxGPIOPin2Config);
    if (MFXSTM32L152::ErrorCode::OK != error)
    {
      panic();
    }

    error = g_mfx.configureGPIOPin(MFXSTM32L152::GPIOPin::PIN3, g_mfxGPIOPin3Config);
    if (MFXSTM32L152::ErrorCode::OK != error)
    {
      panic();
    }

    error = g_mfx.configureGPIOPin(MFXSTM32L152::GPIOPin::PIN4, g_mfxGPIOPin4Config);
    if (MFXSTM32L152::ErrorCode::OK != error)
    {
      panic();
    }

    error = g_mfx.registerGPIOInterruptCallback(MFXSTM32L152::GPIOPin::PIN1, turnOnGreenLed, nullptr);
    if (MFXSTM32L152::ErrorCode::OK != error)
    {
      panic();
    }

    error = g_mfx.registerGPIOInterruptCallback(MFXSTM32L152::GPIOPin::PIN2, turnOffGreenLed, nullptr);
    if (MFXSTM32L152::ErrorCode::OK != error)
    {
      panic();
    }
  }

  {
    RaydiumRM67160::RaydiumRM67160Config rm67160Config =
    {
      .enableDSI3V3Callback          = enableDSI3V3Callback,
      .enableDSI1V8Callback          = enableDSI1V8Callback,
      .setDSIResetLineToLowCallback  = setDSIResetLineToLowCallback,
      .setDSIResetLineToHighCallback = setDSIResetLineToHighCallback,
      .startColumnAddress            = 4u,
      .endColumnAddress              = 393u,
      .startRowAddress               = 0u,
      .endRowAddress                 = 389u,
      .defaultBrightness             = 120u,
    };

    RaydiumRM67160::ErrorCode errorCode = g_displayRM67160.init(rm67160Config);
    if (RaydiumRM67160::ErrorCode::OK != errorCode)
    {
      panic();
    }
  }

  {
    FT3267::ErrorCode errorCode = g_ft3267.init(g_ft3267Config);
    if (FT3267::ErrorCode::OK != errorCode)
    {
      panic();
    }
  }

  uint8_t mfxId    = 0u;
  uint8_t ft3267Id = 0u;

  {
    MFXSTM32L152::ErrorCode error = g_mfx.getID(mfxId);
    if (MFXSTM32L152::ErrorCode::OK != error)
    {
      panic();
    }
  }

  {
    FT3267::ErrorCode errorCode = g_ft3267.getID(ft3267Id);
    if (FT3267::ErrorCode::OK == errorCode)
    {
      panic();
    }
  }

  dsiHost.startTransferFromLTDC();

  uint8_t message[2000];
  uint32_t messageLen;

  uint8_t brightness = 120u;
  bool ledState      = false;
  uint32_t timestamp = sysTick.getTicks();

  while (true)
  {
    const uint64_t ticks = sysTick.getTicks();

    {
      EXTI::ErrorCode errorCode = exti.runtimeTask();
      if (EXTI::ErrorCode::OK != errorCode)
      {
        panic();
      }
    }

    if (not usart2.isWriteTransactionOngoing())
    {
      /*
      if (dma2D.isTransferOngoing())
      {
        messageLen = sprintf(reinterpret_cast<char*>(&message[0]),
          "systick: %" PRIu32 "\r\n", static_cast<uint32_t>(ticks));
      }
      else
      {
        messageLen = sprintf(reinterpret_cast<char*>(&message[0]),
          "DMA2D completed, systick: %" PRIu32 "\r\n", static_cast<uint32_t>(ticks));
      }
      */

      messageLen = sprintf(reinterpret_cast<char*>(&message[0]),
        "systick: %" PRIu32 " %x\r\n", static_cast<uint32_t>(ticks), ft3267Id);

      usart2.write(message, messageLen);
    }

    MFXSTM32L152::ErrorCode error = g_mfx.runtimeTask();
    if (MFXSTM32L152::ErrorCode::OK != error)
    {
      panic();
    }

    if (sysTick.getElapsedTimeInMs(timestamp) > 5000u)
    {
      timestamp = sysTick.getTicks();

      if (ledState)
      {
        g_mfx.setGPIOPinState(MFXSTM32L152::GPIOPin::PIN0, MFXSTM32L152::GPIOPinState::LOW);
      }
      else
      {
        g_mfx.setGPIOPinState(MFXSTM32L152::GPIOPin::PIN0, MFXSTM32L152::GPIOPinState::HIGH);
      }

      ledState = !ledState;

      brightness += 10u;
      g_displayRM67160.setDisplayBrightness(brightness);
    }
  }
}