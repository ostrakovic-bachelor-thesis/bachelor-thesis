#include "Startup.h"
#include "ResetCOntrol.h"
#include "ClockControl.h"
#include "GPIO.h"
#include "USART.h"
#include "DriverManager.h"
#include <cstdint>


void startup(void)
{
  ResetControl &resetControl = DriverManager::getInstance(DriverManager::ResetControlInstance::GENERIC);
  GPIO &gpioH = DriverManager::getInstance(DriverManager::GPIOInstance::GPIOH);

  {
    ResetControl::ErrorCode error = resetControl.enablePeripheralClock(Peripheral::GPIOH);
    if (ResetControl::ErrorCode::OK != error)
    {
      // Report the error
    }
  }

  {
    GPIO::PinConfiguration pinConfig;
  
    pinConfig.mode        = GPIO::PinMode::OUTPUT;
    pinConfig.pullConfig  = GPIO::PullConfig::PULL_DOWN;
    pinConfig.outputSpeed = GPIO::OutputSpeed::HIGH;
    pinConfig.outputType  = GPIO::OutputType::PUSH_PULL;

    GPIO::ErrorCode error = gpioH.configurePin(GPIO::Pin::PIN4, pinConfig);
    if (GPIO::ErrorCode::OK != error)
    {
      // Report the error
    }
  }

  while (true)
  {
    for (volatile uint32_t i = 0u; i < 100000u; ++i);
    gpioH.setPinState(GPIO::Pin::PIN4, GPIO::PinState::HIGH);
    for (volatile uint32_t i = 0u; i < 100000u; ++i);
    gpioH.setPinState(GPIO::Pin::PIN4, GPIO::PinState::LOW);
  }
}