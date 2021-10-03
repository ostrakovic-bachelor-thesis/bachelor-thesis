#ifndef GPIO_MANAGER_H
#define GPIO_MANAGER_H

#include "GPIO.h"
#include <cstdint>


class GPIOManager
{
public:

  //! This enum class represents errors which can happen during method calls
  enum class ErrorCode : uint8_t
  {
    OK = 0u,
  };

  struct GPIOPinConfiguration
  {
    GPIO &gpio;
    GPIO::Pin pin;
    GPIO::PinConfiguration pinConfiguration;
  };

  template<uint32_t size>
  static ErrorCode configureGPIOPins(GPIOPinConfiguration (&gpioPinsConfig)[size]);
};

template<uint32_t size>
GPIOManager::ErrorCode GPIOManager::configureGPIOPins(GPIOPinConfiguration (&gpioPinsConfig)[size])
{
  for (uint32_t i = 0u; i < size; ++i)
  {
    gpioPinsConfig[i].gpio.configurePin(gpioPinsConfig[i].pin, gpioPinsConfig[i].pinConfiguration);
  }

  return ErrorCode::OK;
}

#endif // #ifndef GPIO_MANAGER_H