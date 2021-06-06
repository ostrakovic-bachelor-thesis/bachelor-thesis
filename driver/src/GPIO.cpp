#include "GPIO.h"


GPIO::GPIO(GPIO_TypeDef *GPIOPortPtr):
  m_GPIOPortPtr(GPIOPortPtr)
{}

GPIO::ErrorCode GPIO::configurePin(Pin pin, const PinConfiguration &pinConfiguration)
{
  if (not isPinValueInValidRangeOfValues(pin))
  {
    return ErrorCode::INVALID_PIN_VALUE;
  }

  if (not isPinConfigurationValid(pinConfiguration))
  {
    return ErrorCode::PIN_CONFIG_PARAM_INVALID_VALUE;
  }

  setPinMode(pin, pinConfiguration.mode);
  setPullConfig(pin, (PinMode::ANALOG == pinConfiguration.mode) ? PullConfig::NO_PULL :pinConfiguration.pullConfig);
  
  if ((PinMode::INPUT != pinConfiguration.mode) & (PinMode::ANALOG != pinConfiguration.mode))
  {
    setOutputSpeed(pin, pinConfiguration.outputSpeed);
    setOutputType(pin, pinConfiguration.outputType);
  }

  if (PinMode::AF == pinConfiguration.mode)
  {
    setAlternateFunction(pin, pinConfiguration.alternateFunction);
  }

  return ErrorCode::OK;
}

GPIO::ErrorCode GPIO::setPinState(Pin pin, PinState state)
{
  if (not isPinValueInValidRangeOfValues(pin))
  {
    return ErrorCode::INVALID_PIN_VALUE;
  }

  if (not isPinStateInValidRangeOfValues(state))
  {
    return ErrorCode::INVALID_PIN_STATE_VALUE;
  }

  if (readPinMode(pin) != PinMode::OUTPUT)
  {
    return ErrorCode::PIN_MODE_IS_NOT_APPROPRIATE;
  }

  setOutputPinState(pin, state);

  return ErrorCode::OK;
}

GPIO::ErrorCode GPIO::getPinState(Pin pin, PinState &state) const
{
  if (not isPinValueInValidRangeOfValues(pin))
  {
    return ErrorCode::INVALID_PIN_VALUE;
  }

  switch (readPinMode(pin))
  {
    case GPIO::PinMode::OUTPUT:
    {
      state = readOutputPinState(pin);
      return ErrorCode::OK;
    }

    case GPIO::PinMode::INPUT:
    {
      state = readInputPinState(pin);
      return ErrorCode::OK;
    }
  
    default:
    {
      return ErrorCode::PIN_MODE_IS_NOT_APPROPRIATE;
    }
  }
}

GPIO::ErrorCode GPIO::getPinMode(Pin pin, PinMode &pinMode) const
{
  ErrorCode errorCode = ErrorCode::INVALID_PIN_VALUE;

  if (isPinValueInValidRangeOfValues(pin))
  {
    pinMode = readPinMode(pin);
    errorCode = ErrorCode::OK;
  }

  return errorCode;
}

inline void GPIO::setPinMode(Pin pin, PinMode mode)
{
  constexpr uint32_t PIN_MODE_NUM_OF_BITS = 2u;
  MemoryUtility::setBitsInRegister(&(m_GPIOPortPtr->MODER), 
                                   PIN_MODE_NUM_OF_BITS * static_cast<uint32_t>(pin), 
                                   PIN_MODE_NUM_OF_BITS, 
                                   static_cast<uint32_t>(mode));
}

inline GPIO::PinMode GPIO::readPinMode(Pin pin) const
{
  constexpr uint32_t PIN_MODE_NUM_OF_BITS = 2u;
  const uint32_t pinMode = 
    MemoryUtility::getBitsInRegister(&(m_GPIOPortPtr->MODER), 
                                     PIN_MODE_NUM_OF_BITS * static_cast<uint32_t>(pin), 
                                     PIN_MODE_NUM_OF_BITS);
  return static_cast<PinMode>(pinMode);                                   
}

inline void GPIO::setPullConfig(Pin pin, PullConfig pullConfig)
{
  constexpr uint32_t PULL_CONFIG_NUM_OF_BITS = 2u;
  MemoryUtility::setBitsInRegister(&(m_GPIOPortPtr->PUPDR),
                                   PULL_CONFIG_NUM_OF_BITS * static_cast<uint32_t>(pin), 
                                   PULL_CONFIG_NUM_OF_BITS, 
                                   static_cast<uint32_t>(pullConfig));
}

inline void GPIO::setOutputSpeed(Pin pin, OutputSpeed outputSpeed)
{
  constexpr uint32_t OUTPUT_SPEED_NUM_OF_BITS = 2u;
  MemoryUtility::setBitsInRegister(&(m_GPIOPortPtr->OSPEEDR),
                                   OUTPUT_SPEED_NUM_OF_BITS * static_cast<uint32_t>(pin), 
                                   OUTPUT_SPEED_NUM_OF_BITS, 
                                   static_cast<uint32_t>(outputSpeed));
}

inline void GPIO::setOutputType(Pin pin, OutputType outputType)
{
  constexpr uint32_t OUTPUT_TYPE_NUM_OF_BITS = 1u;
  MemoryUtility::setBitsInRegister(&(m_GPIOPortPtr->OTYPER),
                                   OUTPUT_TYPE_NUM_OF_BITS * static_cast<uint32_t>(pin), 
                                   OUTPUT_TYPE_NUM_OF_BITS, 
                                   static_cast<uint32_t>(outputType));
}

inline void GPIO::setAlternateFunction(Pin pin, AlternateFunction alternateFunction)
{
  constexpr uint32_t ALTERNATE_FUNCTION_NUM_OF_BITS = 4u;
  const uint32_t registerIndex = static_cast<uint32_t>(pin) / 8u;
  const uint32_t position = static_cast<uint32_t>(pin) % 8u;

  MemoryUtility::setBitsInRegister(&(m_GPIOPortPtr->AFR[registerIndex]),
                                   ALTERNATE_FUNCTION_NUM_OF_BITS * position, 
                                   ALTERNATE_FUNCTION_NUM_OF_BITS, 
                                   static_cast<uint32_t>(alternateFunction));
}

inline void GPIO::setOutputPinState(Pin pin, PinState state)
{
  constexpr uint32_t PIN_STATE_NUM_OF_BITS = 1u;
  MemoryUtility::setBitsInRegister(&(m_GPIOPortPtr->ODR),
                                   PIN_STATE_NUM_OF_BITS * static_cast<uint32_t>(pin), 
                                   PIN_STATE_NUM_OF_BITS, 
                                   static_cast<uint32_t>(state));
}

inline GPIO::PinState GPIO::readOutputPinState(Pin pin) const
{
  constexpr uint32_t PIN_STATE_NUM_OF_BITS = 1u;
  const uint32_t pinState =
    MemoryUtility::getBitsInRegister(&(m_GPIOPortPtr->ODR), 
                                     PIN_STATE_NUM_OF_BITS * static_cast<uint32_t>(pin), 
                                     PIN_STATE_NUM_OF_BITS);
  return static_cast<PinState>(pinState);    
}

inline GPIO::PinState GPIO::readInputPinState(Pin pin) const
{
  constexpr uint32_t PIN_STATE_NUM_OF_BITS = 1u;
  const uint32_t pinState =
    MemoryUtility::getBitsInRegister(&(m_GPIOPortPtr->IDR), 
                                     PIN_STATE_NUM_OF_BITS * static_cast<uint32_t>(pin), 
                                     PIN_STATE_NUM_OF_BITS);
  return static_cast<PinState>(pinState);    
}

bool GPIO::isPinConfigurationValid(const PinConfiguration &pinConfiguration)
{
  bool isPinConfigValid = true;
  
  isPinConfigValid &= isModeInValidRangeOfValues(pinConfiguration.mode);
  isPinConfigValid &= isPullConfigInValidRangeOfValues(pinConfiguration.pullConfig);

  if ((PinMode::OUTPUT == pinConfiguration.mode) || (PinMode::AF == pinConfiguration.mode))
  {
    isPinConfigValid &= isOutputSpeedInValidRangeOfValues(pinConfiguration.outputSpeed);
    isPinConfigValid &= isOutputTypeInValidRangeOfValues(pinConfiguration.outputType);
  }

  if (PinMode::AF == pinConfiguration.mode)
  {
    isPinConfigValid &= isAlternateFunctionInValidRangeOfValues(pinConfiguration.alternateFunction);
  }

  return isPinConfigValid;
}

inline bool GPIO::isPinValueInValidRangeOfValues(Pin pin)
{
  return static_cast<uint32_t>(Pin::PIN15) >= static_cast<uint32_t>(pin);
}

inline bool GPIO::isModeInValidRangeOfValues(PinMode mode)
{
  return static_cast<uint32_t>(PinMode::ANALOG) >= static_cast<uint32_t>(mode);
}

inline bool GPIO::isPullConfigInValidRangeOfValues(PullConfig pullConfig)
{
  return static_cast<uint32_t>(PullConfig::PULL_DOWN) >= static_cast<uint32_t>(pullConfig);
}

inline bool GPIO::isOutputSpeedInValidRangeOfValues(OutputSpeed outputSpeed)
{
  return static_cast<uint32_t>(OutputSpeed::VERY_HIGH) >= static_cast<uint32_t>(outputSpeed);
}

inline bool GPIO::isOutputTypeInValidRangeOfValues(OutputType outputType)
{
  return static_cast<uint32_t>(OutputType::OPEN_DRAIN) >= static_cast<uint32_t>(outputType);
}

inline bool GPIO::isAlternateFunctionInValidRangeOfValues(AlternateFunction alternateFunction)
{
  return static_cast<uint32_t>(AlternateFunction::AF15) >= static_cast<uint32_t>(alternateFunction);
}


inline bool GPIO::isPinStateInValidRangeOfValues(PinState pinState)
{
  return static_cast<uint32_t>(PinState::HIGH) >= static_cast<uint32_t>(pinState);
}
