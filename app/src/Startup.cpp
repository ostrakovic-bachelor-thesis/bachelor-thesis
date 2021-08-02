#include "Startup.h"
#include "ResetCOntrol.h"
#include "ClockControl.h"
#include "GPIO.h"
#include "USART.h"
#include "DMA2D.h"
#include "DriverManager.h"
#include <cstdint>
#include <cstdio>
#include <cinttypes>


uint16_t g_frameBuffer[100][150] = { 0u };

//extern "C" void initSYSCLOCK(void);

void panic(void)
{
  volatile bool loopWhileTrue = true;

  while (loopWhileTrue);
}

void startup(void)
{
  ResetControl &resetControl = DriverManager::getInstance(DriverManager::ResetControlInstance::GENERIC);
  GPIO &gpioA = DriverManager::getInstance(DriverManager::GPIOInstance::GPIOA);
  GPIO &gpioH = DriverManager::getInstance(DriverManager::GPIOInstance::GPIOH);
  SysTick &sysTick = DriverManager::getInstance(DriverManager::SysTickInstance::GENERIC);
  USART &usart2 = DriverManager::getInstance(DriverManager::USARTInstance::USART2);
  InterruptController &interruptController = DriverManager::getInstance(DriverManager::InterruptControllerInstance::GENERIC);
  DMA2D &dma2D = DriverManager::getInstance(DriverManager::DMA2DInstance::GENERIC);

  //initSYSCLOCK();

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
    ResetControl::ErrorCode error = resetControl.enablePeripheralClock(Peripheral::GPIOH);
    if (ResetControl::ErrorCode::OK != error)
    {
      panic();
    }

    error = resetControl.enablePeripheralClock(Peripheral::GPIOA);
    if (ResetControl::ErrorCode::OK != error)
    {
      panic();
    }

    error = resetControl.enablePeripheralClock(Peripheral::USART2);
    if (ResetControl::ErrorCode::OK != error)
    {
      panic();
    }

    error = resetControl.enablePeripheralClock(Peripheral::DMA2D);
    if (ResetControl::ErrorCode::OK != error)
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

  {
    GPIO::PinConfiguration pinConfig =
    {
      .mode        = GPIO::PinMode::OUTPUT,
      .pullConfig  = GPIO::PullConfig::PULL_DOWN,
      .outputSpeed = GPIO::OutputSpeed::HIGH,
      .outputType  = GPIO::OutputType::PUSH_PULL
    };

    GPIO::ErrorCode error = gpioH.configurePin(GPIO::Pin::PIN4, pinConfig);
    if (GPIO::ErrorCode::OK != error)
    {
      panic();
    }
  }

  {
    GPIO::PinConfiguration pinConfig =
    {
      .mode              = GPIO::PinMode::AF,
      .pullConfig        = GPIO::PullConfig::NO_PULL,
      .outputSpeed       = GPIO::OutputSpeed::HIGH,
      .outputType        = GPIO::OutputType::PUSH_PULL,
      .alternateFunction = GPIO::AlternateFunction::AF7
    };

    GPIO::ErrorCode error = gpioA.configurePin(GPIO::Pin::PIN2, pinConfig);
    if (GPIO::ErrorCode::OK != error)
    {
      panic();
    }

    error = gpioA.configurePin(GPIO::Pin::PIN3, pinConfig);
    if (GPIO::ErrorCode::OK != error)
    {
      panic();
    }
  }

  {
    DMA2D::FillRectangleConfig fillRectangleConfig =
    {
      .outputColorFormat = DMA2D::OutputColorFormat::BGR565,
      .color =
      {
        .alpha = 0,
        .red   = 10,
        .green = 20,
        .blue  = 10
      },
      .position =
      {
        .x = 50,
        .y = 50
      },
      .rectangleDimension =
      {
        .width  = 50,
        .height = 50
      },
      .frameBufferDimension =
      {
        .width  = 150,
        .height = 100
      },
      .frameBufferPtr = g_frameBuffer
    };

    DMA2D::ErrorCode error = dma2D.fillRectangle(fillRectangleConfig);
    if (DMA2D::ErrorCode::OK != error)
    {
      panic();
    }
  }

  uint8_t message[2000];
  uint32_t messageLen;

  while (true)
  {
    const uint64_t ticks = sysTick.getTicks();

    if (not usart2.isWriteTransactionOngoing())
    {
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