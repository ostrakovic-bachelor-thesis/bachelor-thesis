#include "Startup.h"
#include "ResetCOntrol.h"
#include "ClockControl.h"
#include "GPIO.h"
#include "USART.h"
#include "DMA2D.h"
#include "DriverManager.h"
#include "GPIOManager.h"
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

extern "C" void initSYSCLOCK(void);

void panic(void)
{
  volatile bool loopWhileTrue = true;

  while (loopWhileTrue);
}

void startup(void)
{
  GPIO &gpioH = DriverManager::getInstance(DriverManager::GPIOInstance::GPIOH);
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

  {
    InterruptController::ErrorCode error = interruptController.enableInterrupt(USART2_IRQn);
    if (InterruptController::ErrorCode::OK != error)
    {
      panic();
    }

    error = interruptController.enableInterrupt(DMA2D_IRQn);
    if (InterruptController::ErrorCode::OK != error)
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

  uint8_t message[2000];
  uint32_t messageLen;

  while (true)
  {
    const uint64_t ticks = sysTick.getTicks();

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

    if ((ticks % 10000u) < 5000u)
    {
      gpioH.setPinState(GPIO::Pin::PIN4, GPIO::PinState::HIGH);
    }
    else
    {
      gpioH.setPinState(GPIO::Pin::PIN4, GPIO::PinState::LOW);
    }
  }
}