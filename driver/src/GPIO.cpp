#include "GPIO.h"
#include "MemoryAccess.h"
#include "RegisterUtility.h"


GPIO::GPIO(GPIO_TypeDef *gpioPortPtr, ResetControl *resetControlPtr):
  m_gpioPortPtr(gpioPortPtr),
  m_resetControlPtr(resetControlPtr),
  m_isPinInUsageBitField(0u)
{}

bool GPIO::isPinInUsage(Pin pin) const
{
  return MemoryUtility<uint16_t>::isBitSet(m_isPinInUsageBitField, static_cast<uint8_t>(pin));
}

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

  ErrorCode errorCode = enablePeripheralClock();
  if (ErrorCode::OK != errorCode)
  {
    return errorCode;
  }

  setPinMode(pin, pinConfiguration.mode);
  setPullConfig(pin, (PinMode::ANALOG == pinConfiguration.mode) ? PullConfig::NO_PULL : pinConfiguration.pullConfig);

  if ((PinMode::INPUT != pinConfiguration.mode) & (PinMode::ANALOG != pinConfiguration.mode))
  {
    setOutputSpeed(pin, pinConfiguration.outputSpeed);
    setOutputType(pin, pinConfiguration.outputType);
  }

  if (PinMode::AF == pinConfiguration.mode)
  {
    setAlternateFunction(pin, pinConfiguration.alternateFunction);
  }

  putPinInUsage(pin);

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
  constexpr uint8_t PIN_MODE_NUM_OF_BITS = 2u;
  RegisterUtility<uint32_t>::setBitsInRegister(
    &(m_gpioPortPtr->MODER),
    PIN_MODE_NUM_OF_BITS * static_cast<uint8_t>(pin),
    PIN_MODE_NUM_OF_BITS,
    static_cast<uint32_t>(mode));
}

inline GPIO::PinMode GPIO::readPinMode(Pin pin) const
{
  constexpr uint8_t PIN_MODE_NUM_OF_BITS = 2u;
  const uint32_t pinMode = RegisterUtility<uint32_t>::getBitsInRegister(
    &(m_gpioPortPtr->MODER),
    PIN_MODE_NUM_OF_BITS * static_cast<uint8_t>(pin),
    PIN_MODE_NUM_OF_BITS);

  return static_cast<PinMode>(pinMode);
}

inline void GPIO::setPullConfig(Pin pin, PullConfig pullConfig)
{
  constexpr uint8_t PULL_CONFIG_NUM_OF_BITS = 2u;
  RegisterUtility<uint32_t>::setBitsInRegister(
    &(m_gpioPortPtr->PUPDR),
    PULL_CONFIG_NUM_OF_BITS * static_cast<uint8_t>(pin),
    PULL_CONFIG_NUM_OF_BITS,
    static_cast<uint32_t>(pullConfig));
}

inline void GPIO::setOutputSpeed(Pin pin, OutputSpeed outputSpeed)
{
  constexpr uint8_t OUTPUT_SPEED_NUM_OF_BITS = 2u;
  RegisterUtility<uint32_t>::setBitsInRegister(
    &(m_gpioPortPtr->OSPEEDR),
    OUTPUT_SPEED_NUM_OF_BITS * static_cast<uint8_t>(pin),
    OUTPUT_SPEED_NUM_OF_BITS,
    static_cast<uint32_t>(outputSpeed));
}

inline void GPIO::setOutputType(Pin pin, OutputType outputType)
{
  constexpr uint8_t OUTPUT_TYPE_NUM_OF_BITS = 1u;
  RegisterUtility<uint32_t>::setBitsInRegister(
    &(m_gpioPortPtr->OTYPER),
    OUTPUT_TYPE_NUM_OF_BITS * static_cast<uint8_t>(pin),
    OUTPUT_TYPE_NUM_OF_BITS,
    static_cast<uint32_t>(outputType));
}

inline void GPIO::setAlternateFunction(Pin pin, AlternateFunction alternateFunction)
{
  constexpr uint8_t ALTERNATE_FUNCTION_NUM_OF_BITS = 4u;
  const uint8_t registerIndex = static_cast<uint8_t>(pin) / 8u;
  const uint8_t position = static_cast<uint8_t>(pin) % 8u;

  RegisterUtility<uint32_t>::setBitsInRegister(
    &(m_gpioPortPtr->AFR[registerIndex]),
    ALTERNATE_FUNCTION_NUM_OF_BITS * position,
    ALTERNATE_FUNCTION_NUM_OF_BITS,
    static_cast<uint32_t>(alternateFunction));
}

inline void GPIO::setOutputPinState(Pin pin, PinState state)
{
  constexpr uint8_t PIN_STATE_NUM_OF_BITS = 1u;
  RegisterUtility<uint32_t>::setBitsInRegister(
    &(m_gpioPortPtr->ODR),
    PIN_STATE_NUM_OF_BITS * static_cast<uint8_t>(pin),
    PIN_STATE_NUM_OF_BITS,
    static_cast<uint32_t>(state));
}

inline GPIO::PinState GPIO::readOutputPinState(Pin pin) const
{
  constexpr uint8_t PIN_STATE_NUM_OF_BITS = 1u;
  const uint32_t pinState = RegisterUtility<uint32_t>::getBitsInRegister(
    &(m_gpioPortPtr->ODR),
    PIN_STATE_NUM_OF_BITS * static_cast<uint8_t>(pin),
    PIN_STATE_NUM_OF_BITS);

  return static_cast<PinState>(pinState);
}

inline GPIO::PinState GPIO::readInputPinState(Pin pin) const
{
  constexpr uint8_t PIN_STATE_NUM_OF_BITS = 1u;
  const uint32_t pinState = RegisterUtility<uint32_t>::getBitsInRegister(
    &(m_gpioPortPtr->IDR),
    PIN_STATE_NUM_OF_BITS * static_cast<uint8_t>(pin),
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

inline void GPIO::putPinInUsage(Pin pin)
{
  m_isPinInUsageBitField = MemoryUtility<uint16_t>::setBit(m_isPinInUsageBitField, static_cast<uint8_t>(pin));
}

GPIO::ErrorCode GPIO::enablePeripheralClock(void)
{
  bool isPeripheralClockEnabled;
  ResetControl::ErrorCode errorCode =
    m_resetControlPtr->isPeripheralClockEnabled(getPeripheralTag(), isPeripheralClockEnabled);

  if ((ResetControl::ErrorCode::OK == errorCode) && (not isPeripheralClockEnabled))
  {
    errorCode = m_resetControlPtr->enablePeripheralClock(getPeripheralTag());
  }

  return (ResetControl::ErrorCode::OK == errorCode) ? ErrorCode::OK : ErrorCode::CAN_NOT_TURN_ON_PERIPHERAL_CLOCK;
}