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
#include "GUIRectangle.h"
#include "GUIImage.h"
#include "GUIDrawPerformanceMeasurement.h"
#include "StringBuilder.h"
#include "USARTLogger.h"
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
#include "AppFrameBuffer.h"


//extern const uint8_t rimacImageBitmap[390 * 390 * 3 + 1];
extern const uint8_t iconBitmap[100 * 100 * 3 + 1];
extern const uint8_t rimacLogoBitmap[300 * 71 * 4];


bool g_isTouchEventInfoRefreshed = false;
FT3267::TouchEventInfo g_touchEventInfo;

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

void ft3267InterruptHandler(void *argumentPtr)
{
  g_ft3267.runtimeTask();
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
  MFXSTM32L152::ErrorCode errorCode = g_mfx.configureGPIOPin(MFXSTM32L152::GPIOPin::PIN10, g_mfxGPIOPin10Config);
  if (MFXSTM32L152::ErrorCode::OK != errorCode)
  {
    panic();
  }

  errorCode = g_mfx.setGPIOPinState(MFXSTM32L152::GPIOPin::PIN10, MFXSTM32L152::GPIOPinState::LOW);
  if (MFXSTM32L152::ErrorCode::OK != errorCode)
  {
    panic();
  }
}

void setDSIResetLineToHighCallback(void)
{
  //MFXSTM32L152::ErrorCode errorCode = g_mfx.configureGPIOPin(MFXSTM32L152::GPIOPin::PIN10, g_mfxGPIOPin10Config);
  //if (MFXSTM32L152::ErrorCode::OK != errorCode)
  //{
  //  panic();
  //}

  MFXSTM32L152::ErrorCode errorCode =
    g_mfx.setGPIOPinState(MFXSTM32L152::GPIOPin::PIN10, MFXSTM32L152::GPIOPinState::HIGH);
  if (MFXSTM32L152::ErrorCode::OK != errorCode)
  {
    panic();
  }
}

void touchEventHandler(void *argumentPtr, FT3267::TouchEventInfo touchEventInfo)
{
  g_isTouchEventInfoRefreshed = true;
  g_touchEventInfo = touchEventInfo;
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
  DMA2D &dma2d = DriverManager::getInstance(DriverManager::DMA2DInstance::GENERIC);
  I2C &i2c1 = DriverManager::getInstance(DriverManager::I2CInstance::I2C1);
  LTDC &ltdc = DriverManager::getInstance(DriverManager::LTDCInstance::GENERIC);
  DSIHost &dsiHost = DriverManager::getInstance(DriverManager::DSIHostInstance::GENERIC);

  initSYSCLOCK();


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
    DMA2D::ErrorCode error = dma2d.init();
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

  const GUIRectangle::RectangleDescription guiRectangleDescription1 =
  {
    .baseDescription =
    {
      .dimension =
      {
        .width  = 410u,
        .height = 420u
      },
      .position =
      {
        .x   = -10,
        .y   = -20,
        .tag = GUIRectangle::Position::Tag::TOP_LEFT_CORNER
      }
    },
    .color  =
    {
      .red   = 255u,
      .green = 0u,
      .blue  = 0u
    }
  };

  const GUIRectangle::RectangleDescription guiRectangleDescription2 =
  {
    .baseDescription =
    {
      .dimension =
      {
        .width  = 200u,
        .height = 150u
      },
      .position =
      {
        .x   = 120,
        .y   = 120,
        .tag = GUIRectangle::Position::Tag::TOP_LEFT_CORNER
      }
    },
    .color  =
    {
      .red   = 0u,
      .green = 0u,
      .blue  = 255u
    }
  };

  const GUIImage::ImageDescription imageDescription =
  {
    .baseDescription =
    {
      .dimension =
      {
        .width  = 80,
        .height = 80u
      },
      .position =
      {
        .x   = 150,
        .y   = 270,
        .tag = GUIRectangle::Position::Tag::TOP_LEFT_CORNER
      }
    },
    .bitmapDescription =
    {
      .colorFormat = GUIImage::ColorFormat::RGB888,
      .dimension =
      {
        .width  = 100u,
        .height = 100u
      },
      .copyPosition =
      {
        .x = 10,
        .y = 10
      },
      .bitmapPtr = iconBitmap
    }
  };

  const GUIImage::ImageDescription image2Description =
  {
    .baseDescription =
    {
      .dimension =
      {
        .width  = 220u,
        .height = 71u
      },
      .position =
      {
        .x   = 95,
        .y   = 160,
        .tag = GUIRectangle::Position::Tag::TOP_LEFT_CORNER
      }
    },
    .bitmapDescription =
    {
      .colorFormat = GUIImage::ColorFormat::ARGB8888,
      .dimension =
      {
        .width  = 300u,
        .height = 71u
      },
      .copyPosition =
      {
        .x = 70,
        .y = 0
      },
      .bitmapPtr = rimacLogoBitmap
    }
  };

  GUIRectangle guiRectangle1(dma2d, g_frameBuffer);
  GUIRectangle guiRectangle2(dma2d, g_frameBuffer);
  GUIImage     guiImage(dma2d, g_frameBuffer);
  GUIImage     guiImage2(dma2d, g_frameBuffer);

  guiRectangle1.init(guiRectangleDescription1);
  guiRectangle2.init(guiRectangleDescription2);
  guiImage.init(imageDescription);
  guiImage2.init(image2Description);

  GUIDrawPerformanceMeasurement::GUIDrawPerformanceMeasurementConfig guiDrawImagePerformanceMeasurementConfig
  {
    .guiObjectPtr = &guiImage,
    .drawHardware = IGUIObject::DrawHardware::DMA2D
  };

  GUIDrawPerformanceMeasurement::GUIDrawPerformanceMeasurementConfig guiDrawImage2PerformanceMeasurementConfig
  {
    .guiObjectPtr = &guiImage2,
    .drawHardware = IGUIObject::DrawHardware::CPU
  };

  GUIDrawPerformanceMeasurement guiDrawImagePerformanceMeasurement(sysTick);
  GUIDrawPerformanceMeasurement guiDrawImage2PerformanceMeasurement(sysTick);

  guiDrawImagePerformanceMeasurement.init(guiDrawImagePerformanceMeasurementConfig);
  guiDrawImage2PerformanceMeasurement.init(guiDrawImage2PerformanceMeasurementConfig);

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

    error = interruptController.enableInterrupt(DMA2D_IRQn);
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

    error = g_mfx.configureGPIOPin(MFXSTM32L152::GPIOPin::PIN9, g_mfxGPIOPin9Config);
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

    error = g_mfx.registerGPIOInterruptCallback(MFXSTM32L152::GPIOPin::PIN9, ft3267InterruptHandler, nullptr);
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

    g_ft3267.registerTouchEventCallback(touchEventHandler, nullptr);
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
    if (FT3267::ErrorCode::OK != errorCode)
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

  int16_t x = -100;
  int16_t direction = 1;

  StringBuilder<200u> stringBuilder;
  USARTLogger usartLogger(usart2);

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

    stringBuilder.reset();
    stringBuilder.append("systick: ");
    stringBuilder.append(static_cast<uint32_t>(ticks));

    if(g_isTouchEventInfoRefreshed)
    {
      g_isTouchEventInfoRefreshed = false;

      stringBuilder.append(" touch count: ");
      stringBuilder.append(g_touchEventInfo.touchCount);

      if (g_touchEventInfo.touchCount >= 1u)
      {
        stringBuilder.append(" point1 x: ");
        stringBuilder.append(g_touchEventInfo.touchPoints[0].position.x);
        stringBuilder.append(" y: ");
        stringBuilder.append(g_touchEventInfo.touchPoints[0].position.y);
      }

      if (g_touchEventInfo.touchCount >= 2u)
      {
        stringBuilder.append(" point2 x: ");
        stringBuilder.append(g_touchEventInfo.touchPoints[1].position.x);
        stringBuilder.append(" y: ");
        stringBuilder.append(g_touchEventInfo.touchPoints[1].position.y);
      }
    }
    else
    {
      stringBuilder.append(" no touch(es)");
    }

    stringBuilder.append("\r\n");
    usartLogger.write(stringBuilder);


    if (sysTick.getElapsedTimeInMs(timestamp) > 50u)
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

      if (x >= g_frameBuffer.getWidth())
      {
        direction = -1;
      }
      else if (-100 >= x)
      {
        direction = 1;
      }

      x = x + direction * 5;

      GUIRectangle::Position newPosition = guiRectangle2.getPosition(GUIRectangle::Position::Tag::TOP_LEFT_CORNER);
      newPosition.x = x;
      guiRectangle2.moveToPosition(newPosition);

      newPosition = guiImage2.getPosition(GUIRectangle::Position::Tag::TOP_LEFT_CORNER);
      newPosition.x = x;

      guiImage2.moveToPosition(newPosition);

      while (dma2d.isTransferOngoing());
      guiRectangle1.draw(IGUIObject::DrawHardware::DMA2D);
      while (dma2d.isTransferOngoing());
      guiRectangle2.draw(IGUIObject::DrawHardware::CPU);
      guiDrawImagePerformanceMeasurement.execute();
      guiDrawImage2PerformanceMeasurement.execute();

      dsiHost.startTransferFromLTDC();

      //brightness += 10u;
      //g_displayRM67160.setDisplayBrightness(brightness);
    }
  }
}