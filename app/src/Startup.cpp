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
#include "GUIContainer.h"
#include "StringBuilder.h"
#include "USARTLogger.h"
#include "GUIScene.h"
#include "TouchEvent.h"
#include "FT3267TouchDevice.h"
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

// TODO remove later
class TouchEventLogger : public ITouchEventListener
{
public:

  TouchEventLogger(USARTLogger &usartLogger):
    m_usartLogger(usartLogger)
  {}

  void notify(const TouchEvent &touchEvent) override
  {
    const uint64_t ticks = DriverManager::getInstance(DriverManager::SysTickInstance::GENERIC).getTicks();
    const IArrayList<TouchEvent::TouchPoint> &touchPoints = touchEvent.getTouchPoints();

    m_stringBuilder.reset();
    m_stringBuilder.append("systick: ");
    m_stringBuilder.append(static_cast<uint32_t>(ticks));

    switch (touchEvent.getType())
    {
      case TouchEvent::Type::TOUCH_START:
      {
        m_stringBuilder.append("\tTOUCH START\t");
      }
      break;

      case TouchEvent::Type::TOUCH_MOVE:
      {
        m_stringBuilder.append("\tTOUCH MOVE\t");
      }
      break;

      case TouchEvent::Type::TOUCH_STOP:
      {
        m_stringBuilder.append("\tTOUCH END\t");
      }
      break;
    }

    m_stringBuilder.append(" touch count: ");
    m_stringBuilder.append(touchPoints.getSize());

    if (0u < touchPoints.getSize())
    {
      TouchEvent::TouchPoint touchPoint;
      touchPoints.getElement(0u, &touchPoint);
      m_stringBuilder.append(" point1 x: ");
      m_stringBuilder.append(touchPoint.x);
      m_stringBuilder.append(" y: ");
      m_stringBuilder.append(touchPoint.y);
    }

    if (1u < touchPoints.getSize())
    {
      TouchEvent::TouchPoint touchPoint;
      touchPoints.getElement(1u, &touchPoint);
      m_stringBuilder.append(" point2 x: ");
      m_stringBuilder.append(touchPoint.x);
      m_stringBuilder.append(" y: ");
      m_stringBuilder.append(touchPoint.y);
    }

    m_stringBuilder.append("\r\n");
  }

  void sendLog(void)
  {
    m_usartLogger.write(m_stringBuilder);
  }

private:

  USARTLogger &m_usartLogger;
  StringBuilder<500u> m_stringBuilder;
};

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

FT3267TouchDevice g_ft3267TouchDevice(g_ft3267);

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

  const GUI::Rectangle::RectangleDescription guiRectangleDescription1 =
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
        .tag = GUI::Position::Tag::TOP_LEFT_CORNER
      }
    },
    .color  =
    {
      .red   = 255u,
      .green = 0u,
      .blue  = 0u
    }
  };

  const GUI::Rectangle::RectangleDescription guiRectangleDescription2 =
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
        .tag = GUI::Position::Tag::TOP_LEFT_CORNER
      }
    },
    .color  =
    {
      .red   = 0u,
      .green = 0u,
      .blue  = 255u
    }
  };

  const GUI::Image::ImageDescription imageDescription =
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
        .tag = GUI::Position::Tag::TOP_LEFT_CORNER
      }
    },
    .bitmapDescription =
    {
      .colorFormat = GUI::ColorFormat::RGB888,
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

  const GUI::Image::ImageDescription image2Description =
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
        .tag = GUI::Position::Tag::TOP_LEFT_CORNER
      }
    },
    .bitmapDescription =
    {
      .colorFormat = GUI::ColorFormat::ARGB8888,
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


  ArrayList<GUI::Container::ObjectInfo,5u> guiContainerObjectInfoList;
  GUI::Container guiContainer = GUI::Container(guiContainerObjectInfoList, g_frameBuffer);

  GUI::Rectangle guiRectangle1(dma2d, sysTick, g_frameBuffer);
  GUI::Rectangle guiRectangle2(dma2d, sysTick, g_frameBuffer);
  GUI::Image     guiImage(dma2d, sysTick, g_frameBuffer);
  GUI::Image     guiImage2(dma2d, sysTick, g_frameBuffer);

  guiRectangle1.init(guiRectangleDescription1);
  guiRectangle2.init(guiRectangleDescription2);
  guiImage.init(imageDescription);
  guiImage2.init(image2Description);

  guiContainer.addObject(&guiRectangle2, 5u);
  guiContainer.addObject(&guiRectangle1, 0u);
  guiContainer.addObject(&guiImage2, 10u);
  guiContainer.addObject(&guiImage, 7u);

  GUI::Container::CallbackDescription drawCompletedCallback =
  {
    .functionPtr = [](void *argument) { reinterpret_cast<DSIHost*>(argument)->startTransferFromLTDC(); },
    .argument = &dsiHost
  };

  guiContainer.registerDrawCompletedCallback(drawCompletedCallback);

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

  USARTLogger usartLogger(usart2);
  TouchEventLogger touchEventLogger(usartLogger);

  {
    FT3267::ErrorCode errorCode = g_ft3267.init(g_ft3267Config);
    if (FT3267::ErrorCode::OK != errorCode)
    {
      panic();
    }
  }

  {
    FT3267TouchDevice::ErrorCode errorCode = g_ft3267TouchDevice.init();
    if (FT3267TouchDevice::ErrorCode::OK != errorCode)
    {
      panic();
    }

    g_ft3267TouchDevice.registerTouchEventListener(&touchEventLogger);
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

  uint8_t brightness = 120u;
  bool ledState      = false;
  uint32_t timestamp = sysTick.getTicks();

  int16_t x = -100;
  int16_t direction = 1;

  StringBuilder<500u> stringBuilder;

  guiRectangle1.init(guiRectangleDescription1);
  guiRectangle2.init(guiRectangleDescription2);
  guiImage.init(imageDescription);
  guiImage2.init(image2Description);

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

    touchEventLogger.sendLog();

    if (sysTick.getElapsedTimeInMs(timestamp) >= 25u)
    {
      timestamp = sysTick.getTicks();

      // if (ledState)
      // {
      //   g_mfx.setGPIOPinState(MFXSTM32L152::GPIOPin::PIN0, MFXSTM32L152::GPIOPinState::LOW);
      // }
      // else
      // {
      //   g_mfx.setGPIOPinState(MFXSTM32L152::GPIOPin::PIN0, MFXSTM32L152::GPIOPinState::HIGH);
      // }

      // ledState = !ledState;

      if (x >= g_frameBuffer.getWidth())
      {
        direction = -1;
      }
      else if (-100 >= x)
      {
        direction = 1;
      }

      x = x + direction * 5;

      GUI::Position newPosition = guiRectangle2.getPosition(GUI::Position::Tag::TOP_LEFT_CORNER);
      newPosition.x = x;
      guiRectangle2.moveToPosition(newPosition);

      newPosition = guiImage2.getPosition(GUI::Position::Tag::TOP_LEFT_CORNER);
      newPosition.x = x;
      guiImage2.moveToPosition(newPosition);

      guiContainer.draw(GUI::DrawHardware::DMA2D);

      //while (not guiContainer.isDrawCompleted());

      // stringBuilder.reset();
      // stringBuilder.append("systick: ");
      // stringBuilder.append(static_cast<uint32_t>(ticks));
      // stringBuilder.append("\r\nCPU performance:\r\n");

      // uint32_t i = 0u;
      // for (auto it = guiContainer.getBeginIterator(); it != guiContainer.getEndIterator(); it++)
      // {
      //   uint64_t drawingTimeCPUInUs   = -1;
      //   uint64_t drawingTimeDMA2DInUs = -1;
      //   (*it)->getDrawingTime(GUI::DrawHardware::CPU, drawingTimeCPUInUs);
      //   (*it)->getDrawingTime(GUI::DrawHardware::DMA2D, drawingTimeDMA2DInUs);
      //   stringBuilder.append(i);
      //   stringBuilder.append(". drawing time DMA2D -> ");
      //   stringBuilder.append(static_cast<uint32_t>(drawingTimeDMA2DInUs));
      //   stringBuilder.append("\tCPU -> ");
      //   stringBuilder.append(static_cast<uint32_t>(drawingTimeCPUInUs));
      //   stringBuilder.append("\r\n");
      // }

      // usartLogger.write(stringBuilder);

      //brightness += 10u;
      //g_displayRM67160.setDisplayBrightness(brightness);
    }
  }
}