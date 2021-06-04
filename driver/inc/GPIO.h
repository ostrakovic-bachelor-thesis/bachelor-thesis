#ifndef GPIO_H
#define GPIO_H

#include "stm32l4r9xx.h"
#include "MemoryAccess.h"
#include "MemoryUtility.h"
#include <cstdint>


class GPIO
{
public:

  GPIO(GPIO_TypeDef *GPIOPortPtr);

  enum class ErrorCode : uint32_t
  {
    OK                             = 0u,
    INVALID_PIN_VALUE              = 1u,
    PIN_CONFIG_PARAM_INVALID_VALUE = 2u,
    PIN_MODE_IS_NOT_APPROPRIATE    = 3u
  };

  enum class Pin : uint32_t
  {
    PIN0  = 0u,
    PIN1  = 1u,
    PIN2  = 2u,
    PIN3  = 3u,
    PIN4  = 4u,
    PIN5  = 5u,
    PIN6  = 6u,
    PIN7  = 7u,
    PIN8  = 8u,
    PIN9  = 9u,
    PIN10 = 10u,
    PIN11 = 11u,
    PIN12 = 12u,
    PIN13 = 13u,
    PIN14 = 14u,
    PIN15 = 15u
  };

  enum class PinState : uint32_t
  {
    LOW  = 0b0, //< GPIO pin connected to GND
    HIGH = 0b1  //< GPIO pin connected to VDD
  };

  enum class PinMode : uint32_t
  {
    INPUT  = 0b00,
    OUTPUT = 0b01,
    AF     = 0b10,
    ANALOG = 0b11
  };

  enum class PullConfig : uint32_t
  {
    NO_PULL   = 0b00,
    PULL_UP   = 0b01,
    PULL_DOWN = 0b10
  };

  enum class OutputSpeed : uint8_t
  {
    LOW       = 0b00,
    MEDIUM    = 0b01,
    HIGH      = 0b10,
    VERY_HIGH = 0b11
  };

  enum class OutputType : uint8_t
  {
    PUSH_PULL  = 0b0,
    OPEN_DRAIN = 0b1
  };

  enum class AlternateFunction : uint8_t
  {
    AF0  = 0,
    AF1  = 1,
    AF2  = 2,
    AF3  = 3,
    AF4  = 4,
    AF5  = 5,
    AF6  = 6,
    AF7  = 7,
    AF8  = 8,
    AF9  = 9,
    AF10 = 10,
    AF11 = 11,
    AF12 = 12,
    AF13 = 13,
    AF14 = 14,
    AF15 = 15
  };

  struct PinConfiguration
  {
    PinMode mode;
    PullConfig pullConfig;
    OutputSpeed outputSpeed;
    OutputType outputType;
    AlternateFunction alternateFunction;
  };

  ErrorCode configurePin(Pin pin, const PinConfiguration &pinConfiguration);

  ErrorCode setPinState(Pin pin, PinState state);

private:

  void setPinMode(Pin pin, PinMode mode);

  void setPullConfig(Pin pin, PullConfig pullConfig);

  void setOutputSpeed(Pin pin, OutputSpeed outputSpeed);

  void setOutputType(Pin pin, OutputType outputType);

  void setAlternateFunction(Pin pin, AlternateFunction alternateFunction);

  static bool isPinConfigurationValid(const PinConfiguration &pinConfiguration);

  static bool isPinValueInValidRangeOfValues(Pin pin);

  static bool isModeInValidRangeOfValues(PinMode mode);

  static bool isPullConfigInValidRangeOfValues(PullConfig pullConfig);

  static bool isOutputSpeedInValidRangeOfValues(OutputSpeed outputSpeed);

  static bool isOutputTypeInValidRangeOfValues(OutputType outputType);

  static bool isAlternateFunctionInValidRangeOfValues(AlternateFunction alternateFunction);

  //! Pointer to GPIO port
  GPIO_TypeDef *m_GPIOPortPtr;
};

#endif // #ifndef GPIO_H