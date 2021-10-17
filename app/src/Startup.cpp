#include "Startup.h"
#include "ResetCOntrol.h"
#include "ClockControl.h"
#include "GPIO.h"
#include "USART.h"
#include "DMA2D.h"
#include "PowerControl.h"
#include "DriverManager.h"
#include "GPIOManager.h"
#include "MFXSTM32L152.h"
#include "GPIOConfiguration.h"
#include <cstdint>
#include <cstdio>
#include <cinttypes>

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


uint16_t g_frameBuffer[100][100] = { 0u };

MFXSTM32L152 g_mfx = MFXSTM32L152(
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

void startup(void)
{
  PowerControl &powerControl = DriverManager::getInstance(DriverManager::PowerControlInstance::GENERIC);
  SystemConfig &systemConfig = DriverManager::getInstance(DriverManager::SystemConfigInstance::GENERIC);
  EXTI &exti = DriverManager::getInstance(DriverManager::EXTIInstance::GENERIC);
  SysTick &sysTick = DriverManager::getInstance(DriverManager::SysTickInstance::GENERIC);
  USART &usart2 = DriverManager::getInstance(DriverManager::USARTInstance::USART2);
  InterruptController &interruptController = DriverManager::getInstance(DriverManager::InterruptControllerInstance::GENERIC);
  DMA2D &dma2D = DriverManager::getInstance(DriverManager::DMA2DInstance::GENERIC);
  I2C &i2c1 = DriverManager::getInstance(DriverManager::I2CInstance::I2C1);

  initSYSCLOCK();

  {
    SysTick::SysTickConfig sysTickConfig =
    {
      .ticksPerSecond  = 1000u,
      .enableInterrupt = true,
      .enableOnInit    = true
    };

    SysTick::ErrorCode error = sysTick.init(sysTickConfig);
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
    USART::USARTConfig usartConfig =
    {
      .frameFormat  = USART::FrameFormat::BITS_8_WITHOUT_PARITY,
      .oversampling = USART::Oversampling::OVERSAMPLING_16,
      .stopBits     = USART::StopBits::BIT_1_0,
      .baudrate     = USART::Baudrate::BAUDRATE_115200
    };

    USART::ErrorCode error = usart2.init(usartConfig);
    if (USART::ErrorCode::OK != error)
    {
      panic();
    }
  }

  {
    I2C::I2CConfig i2cConfig =
    {
      .addressingMode    = I2C::AddressingMode::ADDRESS_7_BITS,
      .clockFrequencySCL = 100000u // 100 kHzs
    };

    I2C::ErrorCode error = i2c1.init(i2cConfig);
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

  GPIOManager::ErrorCode errorCode = GPIOManager::configureGPIOPins(gpioPinsConfiguration);
  if (GPIOManager::ErrorCode::OK != errorCode)
  {
    panic();
  }

/*
  DMA2D::BlendBitmapConfig blendBitmapConfig =
  {
    .blendRectangleDimension =
    {
      .width  = 10,
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
    EXTI::EXTIConfig extiLineConfig =
    {
      .isInterruptMasked = false,
      .interruptTrigger  = EXTI::InterruptTrigger::RISING_EDGE,
      .interruptCallback = mfxIrqCallback
    };

    EXTI::ErrorCode errorCode = exti.configureEXTILine(EXTI::EXTILine::LINE1, extiLineConfig);
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

    error = interruptController.enableInterrupt(I2C1_EV_IRQn);
    if (InterruptController::ErrorCode::OK != error)
    {
      panic();
    }
  }

  {
    MFXSTM32L152::MFXSTM32L152Config mfxConfig =
    {
      .peripheralAddress    = 0x42,
      .wakeUpPinGPIOPortPtr = &DriverManager::getInstance(DriverManager::GPIOInstance::GPIOB),
      .wakeUpPin            = GPIO::Pin::PIN2,
    };

    MFXSTM32L152::ErrorCode error = g_mfx.init(mfxConfig);
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

    MFXSTM32L152::IRQPinConfiguration irqPinConfiguration =
    {
      .outputType = MFXSTM32L152::GPIOOutputType::PUSH_PULL,
      .polarity   = MFXSTM32L152::IRQPinPolarity::HIGH
    };

    error = g_mfx.configureIRQPin(irqPinConfiguration);
    if (MFXSTM32L152::ErrorCode::OK != error)
    {
      panic();
    }

    MFXSTM32L152::GPIOPinConfiguration mfxGPIOPinConfig =
    {
      .mode       = MFXSTM32L152::GPIOPinMode::OUTPUT,
      .outputType = MFXSTM32L152::GPIOOutputType::PUSH_PULL,
      .pullConfig = MFXSTM32L152::GPIOPullConfig::NO_PULL
    };

    error = g_mfx.configureGPIOPin(MFXSTM32L152::GPIOPin::PIN0, mfxGPIOPinConfig);
    if (MFXSTM32L152::ErrorCode::OK != error)
    {
      panic();
    }

    mfxGPIOPinConfig.mode             = MFXSTM32L152::GPIOPinMode::INTERRUPT;
    mfxGPIOPinConfig.outputType       = MFXSTM32L152::GPIOOutputType::PUSH_PULL;
    mfxGPIOPinConfig.pullConfig       = MFXSTM32L152::GPIOPullConfig::PULL_DOWN;
    mfxGPIOPinConfig.interruptTrigger = MFXSTM32L152::GPIOInterruptTrigger::HIGH_LEVEL;

    error = g_mfx.configureGPIOPin(MFXSTM32L152::GPIOPin::PIN1, mfxGPIOPinConfig);
    if (MFXSTM32L152::ErrorCode::OK != error)
    {
      panic();
    }

    error = g_mfx.configureGPIOPin(MFXSTM32L152::GPIOPin::PIN2, mfxGPIOPinConfig);
    if (MFXSTM32L152::ErrorCode::OK != error)
    {
      panic();
    }

    error = g_mfx.configureGPIOPin(MFXSTM32L152::GPIOPin::PIN3, mfxGPIOPinConfig);
    if (MFXSTM32L152::ErrorCode::OK != error)
    {
      panic();
    }

    error = g_mfx.configureGPIOPin(MFXSTM32L152::GPIOPin::PIN4, mfxGPIOPinConfig);
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

  uint8_t mfxId = 0u;
  {
    MFXSTM32L152::ErrorCode error = g_mfx.getID(mfxId);
    if (MFXSTM32L152::ErrorCode::OK != error)
    {
      panic();
    }
  }

  uint8_t message[2000];
  uint32_t messageLen;

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
        "systick: %" PRIu32 "\r\n", static_cast<uint32_t>(ticks));

      usart2.write(message, messageLen);
    }

    MFXSTM32L152::ErrorCode error = g_mfx.runtimeTask();
    if (MFXSTM32L152::ErrorCode::OK != error)
    {
      panic();
    }

    if ((ticks % 10000u) < 5000u)
    {
      g_mfx.setGPIOPinState(MFXSTM32L152::GPIOPin::PIN0, MFXSTM32L152::GPIOPinState::LOW);
    }
    else
    {
      g_mfx.setGPIOPinState(MFXSTM32L152::GPIOPin::PIN0, MFXSTM32L152::GPIOPinState::HIGH);
    }
  }
}