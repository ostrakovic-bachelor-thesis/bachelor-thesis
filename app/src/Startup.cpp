#include "Startup.h"
#include "ResetCOntrol.h"
#include "ClockControl.h"
#include "GPIO.h"
#include "USART.h"
#include "DMA2D.h"
#include "PowerControl.h"
#include "LTDC.h"
#include "DSIHost.h"
#include "FlashController.h"
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
#include "GUITouchEvent.h"
#include "FT3267TouchDevice.h"
#include "GUITouchController.h"
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
#include "RaydiumRM67160Config.h"
#include "EXTIConfig.h"
#include "AppFrameBuffer.h"

#include "GUIObjectDescription.h"

void initDriver(void);
void initBSP(void);

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

uint8_t g_brightness = 140u;

void panic(void)
{
  volatile bool loopWhileTrue = true;

  while (loopWhileTrue);
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


void startup(void)
{
  EXTI &exti = DriverManager::getInstance(DriverManager::EXTIInstance::GENERIC);
  SysTick &sysTick = DriverManager::getInstance(DriverManager::SysTickInstance::GENERIC);
  USART &usart2 = DriverManager::getInstance(DriverManager::USARTInstance::USART2);
  DMA2D &dma2d = DriverManager::getInstance(DriverManager::DMA2DInstance::GENERIC);

  initDriver();
  initBSP();

  ArrayList<GUI::Container::ObjectInfo,5u> guiContainerObjectInfoList;
  GUI::Container guiContainer = GUI::Container(guiContainerObjectInfoList, g_frameBuffer);

  GUI::Rectangle backgroundUpPartGuiRectangle(dma2d, sysTick, g_frameBuffer);
  GUI::Rectangle backgroundDownPartGuiRectangle(dma2d, sysTick, g_frameBuffer);
  GUI::Image     untzLogoImage(dma2d, sysTick, g_frameBuffer);
  GUI::Image     brightnessImage(dma2d, sysTick, g_frameBuffer);
  GUI::Image     playButtonImage(dma2d, sysTick, g_frameBuffer);

  bool isPlayStarted = true;

  backgroundUpPartGuiRectangle.init(g_backgroundUpPartGuiRectangleDescription);
  backgroundDownPartGuiRectangle.init(g_backgroundDownPartGuiRectangleDescription);
  untzLogoImage.init(g_untzLogoImageDescription);
  brightnessImage.init(g_brightnessImageDescription);
  playButtonImage.init(g_playButtonImageDescription);

  GUI::Image::TouchEventCallbackDescription touchEventCallbackDescription =
  {
    .functionPtr = [](void *argument, GUI::RectangleBase &guiRectangle, const GUI::TouchEvent &touchEvent)
    {
      static const GUI::Position firstStartPosition = guiRectangle.getPosition(GUI::Position::Tag::CENTER);
      static GUI::Position startPosition;
      static GUI::Point startTouchPoint;

      switch (touchEvent.getType())
      {
        case GUI::TouchEvent::Type::TOUCH_START:
        {
          startTouchPoint = *(touchEvent.getTouchPoints().getBeginIterator());
          startPosition   = guiRectangle.getPosition(GUI::Position::Tag::CENTER);
        }
        break;

        case GUI::TouchEvent::Type::TOUCH_MOVE:
        case GUI::TouchEvent::Type::TOUCH_STOP:
        {
          const GUI::Point touchPoint = *(touchEvent.getTouchPoints().getBeginIterator());
          int16_t brightnessChange = static_cast<int16_t>(touchPoint.x) - firstStartPosition.x;
          if (brightnessChange > 50)
          {
            brightnessChange = 50;
          }
          if (brightnessChange < -50)
          {
            brightnessChange = -50;
          }

          g_brightness = 140 + 2 * brightnessChange;
          g_displayRM67160.setDisplayBrightness(g_brightness);


          GUI::Position newPosition =
          {
            .x   = startPosition.x + static_cast<int16_t>(touchPoint.x) - static_cast<int16_t>(startTouchPoint.x),
            .y   = startPosition.y,
            .tag = startPosition.tag
          };

          if (newPosition.x > (firstStartPosition.x + 50))
          {
            newPosition.x = firstStartPosition.x + 50;
          }

          if (newPosition.x < (firstStartPosition.x - 50))
          {
            newPosition.x = firstStartPosition.x - 50;
          }

          guiRectangle.moveToPosition(newPosition);
        }
        break;

        default:
        {
          // do nothing
        }
        break;
      }
    },
    .argument = nullptr
  };

  GUI::Image::TouchEventCallbackDescription touchEventCallbackDescription2 =
  {
    .functionPtr = [](void *argument, GUI::RectangleBase &guiRectangle, const GUI::TouchEvent &touchEvent)
    {
      switch (touchEvent.getType())
      {
        case GUI::TouchEvent::Type::TOUCH_START:
        {
          *reinterpret_cast<bool*>(argument) = !(*reinterpret_cast<bool*>(argument));
        }
        break;

        case GUI::TouchEvent::Type::TOUCH_MOVE:
        case GUI::TouchEvent::Type::TOUCH_STOP:
        default:
        {
          // do nothing
        }
        break;
      }
    },
    .argument = &isPlayStarted
  };

  brightnessImage.registerTouchEventCallback(touchEventCallbackDescription);
  playButtonImage.registerTouchEventCallback(touchEventCallbackDescription2);

  guiContainer.addObject(&backgroundUpPartGuiRectangle, 0u);
  guiContainer.addObject(&backgroundDownPartGuiRectangle, 1u);
  guiContainer.addObject(&untzLogoImage, 10u);
  guiContainer.addObject(&playButtonImage, 15u);
  guiContainer.addObject(&brightnessImage, 20u);

  GUI::Container::CallbackDescription drawCompletedCallback =
  {
    .functionPtr = [](void *argument) { reinterpret_cast<DSIHost*>(argument)->startTransferFromLTDC(); },
    .argument = &(DriverManager::getInstance(DriverManager::DSIHostInstance::GENERIC))
  };

  guiContainer.registerDrawCompletedCallback(drawCompletedCallback);

  GUI::TouchController touchController;
  touchController.registerContainer(&guiContainer);

  {
    GUI::ErrorCode errorCode = g_ft3267TouchDevice.init();
    if (GUI::ErrorCode::OK != errorCode)
    {
      panic();
    }

    g_ft3267TouchDevice.registerTouchEventListener(&touchController);
  }

  USARTLogger usartLogger(usart2);


  bool ledState      = false;
  uint32_t timestamp = sysTick.getTicks();

  int16_t x = untzLogoImage.getPosition(GUI::Position::Tag::CENTER).x;
  int16_t direction = 1;

  StringBuilder<500u> stringBuilder;

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

      if (isPlayStarted)
      {
        if (x >= g_frameBuffer.getWidth())
        {
          direction = -1;
        }
        else if (0 >= x)
        {
          direction = 1;
        }

        x = x + direction * 3;

        GUI::Position newPosition = untzLogoImage.getPosition(GUI::Position::Tag::CENTER);
        newPosition.x = x;
        untzLogoImage.moveToPosition(newPosition);
      }

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

void initDriver(void)
{
  PowerControl &powerControl = DriverManager::getInstance(DriverManager::PowerControlInstance::GENERIC);
  SystemConfig &systemConfig = DriverManager::getInstance(DriverManager::SystemConfigInstance::GENERIC);
  ClockControl &clockControl = DriverManager::getInstance(DriverManager::ClockControlInstance::GENERIC);
  EXTI &exti = DriverManager::getInstance(DriverManager::EXTIInstance::GENERIC);
  SysTick &sysTick = DriverManager::getInstance(DriverManager::SysTickInstance::GENERIC);
  USART &usart2 = DriverManager::getInstance(DriverManager::USARTInstance::USART2);
  DMA2D &dma2d = DriverManager::getInstance(DriverManager::DMA2DInstance::GENERIC);
  I2C &i2c1 = DriverManager::getInstance(DriverManager::I2CInstance::I2C1);
  LTDC &ltdc = DriverManager::getInstance(DriverManager::LTDCInstance::GENERIC);
  DSIHost &dsiHost = DriverManager::getInstance(DriverManager::DSIHostInstance::GENERIC);
  FlashController &flashController = DriverManager::getInstance(DriverManager::FlashControllerInstance::GENERIC);
  InterruptController &interruptController = DriverManager::getInstance(DriverManager::InterruptControllerInstance::GENERIC);

  PowerControl::ErrorCode powerControlErrorCode = powerControl.init();
  if (PowerControl::ErrorCode::OK != powerControlErrorCode)
  {
    panic();
  }

  ClockControl::ErrorCode clockControlErrorCode = clockControl.enableClock(ClockControl::Clock::HSE);
  if (ClockControl::ErrorCode::OK != clockControlErrorCode)
  {
    panic();
  }

  flashController.setFlashAccessLatency(FlashController::Latency::WAIT_STATE_0);

  powerControlErrorCode =
    powerControl.setDynamicVoltageScalingRange(PowerControl::DynamicVoltageScalingRange::HIGH_PERFORMANCE_BOOST);
  if (PowerControl::ErrorCode::OK != powerControlErrorCode)
  {
    panic();
  }

  ClockControl::PLLConfiguration pllConfig =
  {
    .inputClockDivider    = 1u,
    .inputClockMultiplier = 15u,
    .outputClockPDivider  = 7u,
    .outputClockQDivider  = 4u,
    .outputClockRDivider  = 2u,
    .enableOutputClockP   = false,
    .enableOutputClockQ   = false,
    .enableOutputClockR   = true
  };

  clockControl.setClockSource(ClockControl::Clock::PLL, ClockControl::Clock::HSE);
  if (ClockControl::ErrorCode::OK != clockControlErrorCode)
  {
    panic();
  }

  clockControlErrorCode = clockControl.configurePLL(pllConfig);
  if (ClockControl::ErrorCode::OK != clockControlErrorCode)
  {
    panic();
  }

  flashController.setFlashAccessLatency(FlashController::Latency::WAIT_STATE_4);

  clockControlErrorCode = clockControl.setClockPrescaler(ClockControl::Clock::AHB, ClockControl::Prescaler::PRESC_2);
  if (ClockControl::ErrorCode::OK != clockControlErrorCode)
  {
    panic();
  }

  clockControlErrorCode = clockControl.setClockSource(ClockControl::Clock::SYSTEM_CLOCK, ClockControl::Clock::PLL);
  if (ClockControl::ErrorCode::OK != clockControlErrorCode)
  {
    panic();
  }

  clockControlErrorCode = clockControl.setClockPrescaler(ClockControl::Clock::AHB, ClockControl::Prescaler::PRESC_1);
  if (ClockControl::ErrorCode::OK != clockControlErrorCode)
  {
    panic();
  }

  clockControlErrorCode = clockControl.setClockPrescaler(ClockControl::Clock::APB1, ClockControl::Prescaler::PRESC_1);
  if (ClockControl::ErrorCode::OK != clockControlErrorCode)
  {
    panic();
  }

  clockControlErrorCode = clockControl.setClockPrescaler(ClockControl::Clock::APB2, ClockControl::Prescaler::PRESC_1);
  if (ClockControl::ErrorCode::OK != clockControlErrorCode)
  {
    panic();
  }

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

  clockControlErrorCode = clockControl.configurePLL(pllSai2Config);
  if (ClockControl::ErrorCode::OK != clockControlErrorCode)
  {
    panic();
  }

  SysTick::ErrorCode sysTickErrorCode = sysTick.init(g_sysTickConfig);
  if (SysTick::ErrorCode::OK != sysTickErrorCode)
  {
    panic();
  }

  powerControlErrorCode = powerControl.enablePowerSupplyVDDIO2();
  if (PowerControl::ErrorCode::OK != powerControlErrorCode)
  {
    panic();
  }

  USART::ErrorCode usartErrorCode = usart2.init(g_usart2Config);
  if (USART::ErrorCode::OK != usartErrorCode)
  {
    panic();
  }

  I2C::ErrorCode i2cErrorCode = i2c1.init(g_i2c1Config);
  if (I2C::ErrorCode::OK != i2cErrorCode)
  {
    panic();
  }

  DMA2D::ErrorCode dma2dErrorCode = dma2d.init();
  if (DMA2D::ErrorCode::OK != dma2dErrorCode)
  {
    panic();
  }

  GPIOManager::ErrorCode gpioManagerErrorCode = GPIOManager::configureGPIOPins(g_gpioPinsConfig);
  if (GPIOManager::ErrorCode::OK != gpioManagerErrorCode)
  {
    panic();
  }

  LTDC::ErrorCode ltdcErrorCode = ltdc.init(g_ltdcConfig, g_ltdcLayer1Config);
  if (LTDC::ErrorCode::OK != ltdcErrorCode)
  {
    panic();
  }

  DSIHost::ErrorCode dsiErrorCode = dsiHost.init(g_dsiHostConfig);
  if (DSIHost::ErrorCode::OK != dsiErrorCode)
  {
    panic();
  }

  SystemConfig::ErrorCode systemConfigErrorCode = systemConfig.init();
  if (SystemConfig::ErrorCode::OK != systemConfigErrorCode)
  {
    panic();
  }

  systemConfigErrorCode = systemConfig.mapGPIOToEXTILine(SystemConfig::EXTILine::EXTI1, SystemConfig::GPIOPort::GPIOI);
  if (SystemConfig::ErrorCode::OK != systemConfigErrorCode)
  {
    panic();
  }

  EXTI::ErrorCode extiErrorCode = exti.configureEXTILine(EXTI::EXTILine::LINE1, g_extiLine1Config);
  if (EXTI::ErrorCode::OK != extiErrorCode)
  {
    panic();
  }

  InterruptController::ErrorCode interruptControllerErrorCode = interruptController.enableInterrupt(USART2_IRQn);
  if (InterruptController::ErrorCode::OK != interruptControllerErrorCode)
  {
    panic();
  }

  interruptControllerErrorCode = interruptController.enableInterrupt(EXTI1_IRQn);
  if (InterruptController::ErrorCode::OK != interruptControllerErrorCode)
  {
    panic();
  }

  interruptControllerErrorCode = interruptController.enableInterrupt(I2C1_EV_IRQn);
  if (InterruptController::ErrorCode::OK != interruptControllerErrorCode)
  {
    panic();
  }

  interruptControllerErrorCode = interruptController.enableInterrupt(I2C1_ER_IRQn);
  if (InterruptController::ErrorCode::OK != interruptControllerErrorCode)
  {
    panic();
  }

  interruptControllerErrorCode = interruptController.enableInterrupt(DMA2D_IRQn);
  if (InterruptController::ErrorCode::OK != interruptControllerErrorCode)
  {
    panic();
  }
}

void initBSP(void)
{
  MFXSTM32L152::ErrorCode mfxErrorCode = g_mfx.init(g_mfxConfig);
  if (MFXSTM32L152::ErrorCode::OK != mfxErrorCode)
  {
    panic();
  }

  uint8_t mfxId = 0u;
  mfxErrorCode = g_mfx.getID(mfxId);
  if (MFXSTM32L152::ErrorCode::OK != mfxErrorCode)
  {
    panic();
  }

  mfxErrorCode = g_mfx.wakeUp();
  if (MFXSTM32L152::ErrorCode::OK != mfxErrorCode)
  {
    panic();
  }

  mfxErrorCode = g_mfx.enableGPIO();
  if (MFXSTM32L152::ErrorCode::OK != mfxErrorCode)
  {
    panic();
  }

  mfxErrorCode = g_mfx.enableInterrupt(MFXSTM32L152::Interrupt::GPIO);
  if (MFXSTM32L152::ErrorCode::OK != mfxErrorCode)
  {
    panic();
  }

  mfxErrorCode = g_mfx.configureIRQPin(g_mfxIrqPinConfiguration);
  if (MFXSTM32L152::ErrorCode::OK != mfxErrorCode)
  {
    panic();
  }

  mfxErrorCode = g_mfx.configureGPIOPin(MFXSTM32L152::GPIOPin::PIN0, g_mfxGPIOPin0Config);
  if (MFXSTM32L152::ErrorCode::OK != mfxErrorCode)
  {
    panic();
  }

  mfxErrorCode = g_mfx.configureGPIOPin(MFXSTM32L152::GPIOPin::PIN1, g_mfxGPIOPin1Config);
  if (MFXSTM32L152::ErrorCode::OK != mfxErrorCode)
  {
    panic();
  }

  mfxErrorCode = g_mfx.configureGPIOPin(MFXSTM32L152::GPIOPin::PIN2, g_mfxGPIOPin2Config);
  if (MFXSTM32L152::ErrorCode::OK != mfxErrorCode)
  {
    panic();
  }

  mfxErrorCode = g_mfx.configureGPIOPin(MFXSTM32L152::GPIOPin::PIN3, g_mfxGPIOPin3Config);
  if (MFXSTM32L152::ErrorCode::OK != mfxErrorCode)
  {
    panic();
  }

  mfxErrorCode = g_mfx.configureGPIOPin(MFXSTM32L152::GPIOPin::PIN4, g_mfxGPIOPin4Config);
  if (MFXSTM32L152::ErrorCode::OK != mfxErrorCode)
  {
    panic();
  }

  mfxErrorCode = g_mfx.configureGPIOPin(MFXSTM32L152::GPIOPin::PIN9, g_mfxGPIOPin9Config);
  if (MFXSTM32L152::ErrorCode::OK != mfxErrorCode)
  {
    panic();
  }

  mfxErrorCode = g_mfx.registerGPIOInterruptCallback(MFXSTM32L152::GPIOPin::PIN1, turnOnGreenLed, nullptr);
  if (MFXSTM32L152::ErrorCode::OK != mfxErrorCode)
  {
    panic();
  }

  mfxErrorCode = g_mfx.registerGPIOInterruptCallback(MFXSTM32L152::GPIOPin::PIN2, turnOffGreenLed, nullptr);
  if (MFXSTM32L152::ErrorCode::OK != mfxErrorCode)
  {
    panic();
  }

  mfxErrorCode = g_mfx.registerGPIOInterruptCallback(MFXSTM32L152::GPIOPin::PIN9, ft3267InterruptHandler, nullptr);
  if (MFXSTM32L152::ErrorCode::OK != mfxErrorCode)
  {
    panic();
  }

  RaydiumRM67160::ErrorCode raydiumRM67160ErrorCode = g_displayRM67160.init(g_rm67160Config);
  if (RaydiumRM67160::ErrorCode::OK != raydiumRM67160ErrorCode)
  {
    panic();
  }

  FT3267::ErrorCode ft3267ErrorCode = g_ft3267.init(g_ft3267Config);
  if (FT3267::ErrorCode::OK != ft3267ErrorCode)
  {
    panic();
  }

  uint8_t ft3267Id = 0u;
  ft3267ErrorCode = g_ft3267.getID(ft3267Id);
  if (FT3267::ErrorCode::OK != ft3267ErrorCode)
  {
    panic();
  }
}
