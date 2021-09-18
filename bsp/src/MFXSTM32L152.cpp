#include "MFXSTM32L152.h"
#include "MemoryUtility.h"
#include "RegisterUtility.h"


MFXSTM32L152::MFXSTM32L152(I2C *I2CPtr):
  m_I2CPtr(I2CPtr)
{}

MFXSTM32L152::ErrorCode MFXSTM32L152::init(MFXSTM32L152Config &mfxstm32l152Config)
{
  m_peripheralAddress = mfxstm32l152Config.peripheralAddress;

  return ErrorCode::OK;
}

MFXSTM32L152::ErrorCode MFXSTM32L152::getID(uint8_t &id)
{
  return readRegister(static_cast<uint8_t>(RegisterAddress::ID), &id, sizeof(uint8_t));
}

MFXSTM32L152::ErrorCode MFXSTM32L152::enableGPIO(void)
{
  return setBitsInRegister(RegisterAddress::SYSTEM_CONTROL,
    static_cast<uint8_t>(SystemControl::GPIO_ENABLE),
    static_cast<uint8_t>(SystemControl::ALTERNATE_GPIO_ENABLE));
}

MFXSTM32L152::ErrorCode MFXSTM32L152::enableGPIOInterrupt(void)
{
  return setBitsInRegister(RegisterAddress::IRQ_SRC_EN,
    static_cast<uint8_t>(Interrupt::GPIO));
}

MFXSTM32L152::ErrorCode MFXSTM32L152::configureGPIOPin(GPIOPin pin, const GPIOPinConfiguration &pinConfiguration)
{
  if (GPIOPinMode::INTERRUPT != pinConfiguration.mode)
  {
    disableGPIOPinInterrupt(pin);
  }

  setGPIOPinDirection(pin, pinConfiguration.mode);

  if (GPIOPinMode::OUTPUT == pinConfiguration.mode)
  {
    setGPIOPinType(pin, pinConfiguration.outputType);
  }
  else
  {
    setGPIOPinType(pin, pinConfiguration.pullConfig);
  }

  setGPIOPullConfig(pin, pinConfiguration.pullConfig);

  if (GPIOPinMode::INTERRUPT == pinConfiguration.mode)
  {
    setGPIOPinInterruptEventType(pin, pinConfiguration.interruptTrigger);
    setGPIOPinInterruptActivityType(pin, pinConfiguration.interruptTrigger);
    enableGPIOPinInterrupt(pin);
  }

  return ErrorCode::OK;
}

MFXSTM32L152::ErrorCode MFXSTM32L152::setGPIOPinState(GPIOPin pin, GPIOPinState state)
{
  ErrorCode errorCode;

  if (GPIOPinState::HIGH == state)
  {
    errorCode = setGPIOPinStateToHigh(pin);
  }
  else
  {
    errorCode = setGPIOPinStateToLow(pin);
  }

  return errorCode;
}

MFXSTM32L152::ErrorCode MFXSTM32L152::getGPIOPinState(GPIOPin pin, GPIOPinState &state)
{
  const uint8_t pinOffset = static_cast<uint8_t>(pin) % SIZE_OF_MFXSTM32L152_REGISTER_IN_BITS;
  const RegisterAddress registerAddress = mapToRegisterAddress(RegisterAddress::GPIO_STATE, pin);

  uint8_t registerValue;
  ErrorCode errorCode = readRegister(static_cast<uint8_t>(registerAddress), &registerValue, sizeof(uint8_t));

  state = MemoryUtility<uint8_t>::isBitSet(registerValue, pinOffset) ? GPIOPinState::HIGH : GPIOPinState::LOW;

  return errorCode;
}

MFXSTM32L152::ErrorCode MFXSTM32L152::setGPIOPinDirection(GPIOPin pin, GPIOPinMode mode)
{
  ErrorCode errorCode;

  if (GPIOPinMode::OUTPUT == mode)
  {
    errorCode = configureGPIOPinToOutputDirection(pin);
  }
  else
  {
    errorCode = configureGPIOPinToInputDirection(pin);
  }

  return errorCode;
}

MFXSTM32L152::ErrorCode MFXSTM32L152::setGPIOPinType(GPIOPin pin, GPIOOutputType outputType)
{
  ErrorCode errorCode;

  if (GPIOOutputType::OPEN_DRAIN == outputType)
  {
    errorCode = configureGPIOPinToOpenDrainOutputType(pin);
  }
  else
  {
    errorCode = configureGPIOPinToPushPullOutputType(pin);
  }

  return errorCode;
}

MFXSTM32L152::ErrorCode MFXSTM32L152::setGPIOPinType(GPIOPin pin, GPIOPullConfig pullConfig)
{
  ErrorCode errorCode;

  if (GPIOPullConfig::NO_PULL == pullConfig)
  {
    errorCode = configureGPIOPinToNoPullInputType(pin);
  }
  else
  {
    errorCode = configureGPIOPinToPullInputType(pin);
  }

  return errorCode;
}

MFXSTM32L152::ErrorCode MFXSTM32L152::setGPIOPullConfig(GPIOPin pin, GPIOPullConfig pullConfig)
{
  ErrorCode errorCode;

  if (GPIOPullConfig::PULL_UP == pullConfig)
  {
    errorCode = configureGPIOPullConfigToPullUp(pin);
  }
  else
  {
    errorCode = configureGPIOPullConfigToPullDown(pin);
  }

  return errorCode;
}

MFXSTM32L152::ErrorCode
MFXSTM32L152::setGPIOPinInterruptEventType(GPIOPin pin, GPIOInterruptTrigger interruptTrigger)
{
  ErrorCode errorCode;

  if ((GPIOInterruptTrigger::RISING_EDGE == interruptTrigger) ||
      (GPIOInterruptTrigger::FALLING_EDGE == interruptTrigger))
  {
    errorCode = setGPIOPinInterruptEventTypeToEdge(pin);
  }
  else
  {
    errorCode = setGPIOPinInterruptEventTypeToLevel(pin);
  }

  return errorCode;
}

MFXSTM32L152::ErrorCode
MFXSTM32L152::setGPIOPinInterruptActivityType(GPIOPin pin, GPIOInterruptTrigger interruptTrigger)
{
  ErrorCode errorCode;

  if ((GPIOInterruptTrigger::RISING_EDGE == interruptTrigger) ||
      (GPIOInterruptTrigger::HIGH_LEVEL == interruptTrigger))
  {
    errorCode = setGPIOPinInterruptActivitTypeToHighLevelOrRisingEdge(pin);
  }
  else
  {
    errorCode = setGPIOPinInterruptActivitTypeToLowLevelOrFallingEdge(pin);
  }

  return errorCode;
}

MFXSTM32L152::ErrorCode MFXSTM32L152::enableGPIOPinInterrupt(GPIOPin pin)
{
  const uint8_t pinOffset = static_cast<uint8_t>(pin) % SIZE_OF_MFXSTM32L152_REGISTER_IN_BITS;
  const RegisterAddress registerAddress = mapToRegisterAddress(RegisterAddress::GPIO_IRQ_EN, pin);

  return setBitsInRegister(registerAddress, pinOffset);
}

MFXSTM32L152::ErrorCode MFXSTM32L152::disableGPIOPinInterrupt(GPIOPin pin)
{
  const uint8_t pinOffset = static_cast<uint8_t>(pin) % SIZE_OF_MFXSTM32L152_REGISTER_IN_BITS;
  const RegisterAddress registerAddress = mapToRegisterAddress(RegisterAddress::GPIO_IRQ_EN, pin);

  return resetBitsInRegister(registerAddress, pinOffset);
}

MFXSTM32L152::ErrorCode MFXSTM32L152::configureGPIOPinToOutputDirection(GPIOPin pin)
{
  const uint8_t pinOffset = static_cast<uint8_t>(pin) % SIZE_OF_MFXSTM32L152_REGISTER_IN_BITS;
  const RegisterAddress registerAddress = mapToRegisterAddress(RegisterAddress::GPIO_DIRECTION, pin);

  return setBitsInRegister(registerAddress, pinOffset);
}

MFXSTM32L152::ErrorCode MFXSTM32L152::configureGPIOPinToInputDirection(GPIOPin pin)
{
  const uint8_t pinOffset = static_cast<uint8_t>(pin) % SIZE_OF_MFXSTM32L152_REGISTER_IN_BITS;
  const RegisterAddress registerAddress = mapToRegisterAddress(RegisterAddress::GPIO_DIRECTION, pin);

  return resetBitsInRegister(registerAddress, pinOffset);
}

MFXSTM32L152::ErrorCode MFXSTM32L152::configureGPIOPinToOpenDrainOutputType(GPIOPin pin)
{
  const uint8_t pinOffset = static_cast<uint8_t>(pin) % SIZE_OF_MFXSTM32L152_REGISTER_IN_BITS;
  const RegisterAddress registerAddress = mapToRegisterAddress(RegisterAddress::GPIO_TYPE, pin);

  return setBitsInRegister(registerAddress, pinOffset);
}

MFXSTM32L152::ErrorCode MFXSTM32L152::configureGPIOPinToPushPullOutputType(GPIOPin pin)
{
  const uint8_t pinOffset = static_cast<uint8_t>(pin) % SIZE_OF_MFXSTM32L152_REGISTER_IN_BITS;
  const RegisterAddress registerAddress = mapToRegisterAddress(RegisterAddress::GPIO_TYPE, pin);

  return resetBitsInRegister(registerAddress, pinOffset);
}

MFXSTM32L152::ErrorCode MFXSTM32L152::configureGPIOPinToNoPullInputType(GPIOPin pin)
{
  const uint8_t pinOffset = static_cast<uint8_t>(pin) % SIZE_OF_MFXSTM32L152_REGISTER_IN_BITS;
  const RegisterAddress registerAddress = mapToRegisterAddress(RegisterAddress::GPIO_TYPE, pin);

  return resetBitsInRegister(registerAddress, pinOffset);
}

MFXSTM32L152::ErrorCode MFXSTM32L152::configureGPIOPinToPullInputType(GPIOPin pin)
{
  const uint8_t pinOffset = static_cast<uint8_t>(pin) % SIZE_OF_MFXSTM32L152_REGISTER_IN_BITS;
  const RegisterAddress registerAddress = mapToRegisterAddress(RegisterAddress::GPIO_TYPE, pin);

  return setBitsInRegister(registerAddress, pinOffset);
}

MFXSTM32L152::ErrorCode MFXSTM32L152::configureGPIOPullConfigToPullUp(GPIOPin pin)
{
  const uint8_t pinOffset = static_cast<uint8_t>(pin) % SIZE_OF_MFXSTM32L152_REGISTER_IN_BITS;
  const RegisterAddress registerAddress = mapToRegisterAddress(RegisterAddress::GPIO_PUPD, pin);

  return setBitsInRegister(registerAddress, pinOffset);
}

MFXSTM32L152::ErrorCode MFXSTM32L152::configureGPIOPullConfigToPullDown(GPIOPin pin)
{
  const uint8_t pinOffset = static_cast<uint8_t>(pin) % SIZE_OF_MFXSTM32L152_REGISTER_IN_BITS;
  const RegisterAddress registerAddress = mapToRegisterAddress(RegisterAddress::GPIO_PUPD, pin);

  return resetBitsInRegister(registerAddress, pinOffset);
}

MFXSTM32L152::ErrorCode MFXSTM32L152::setGPIOPinInterruptEventTypeToEdge(GPIOPin pin)
{
  const uint8_t pinOffset = static_cast<uint8_t>(pin) % SIZE_OF_MFXSTM32L152_REGISTER_IN_BITS;
  const RegisterAddress registerAddress = mapToRegisterAddress(RegisterAddress::GPIO_IRQ_EVT, pin);

  return setBitsInRegister(registerAddress, pinOffset);
}

MFXSTM32L152::ErrorCode MFXSTM32L152::setGPIOPinInterruptEventTypeToLevel(GPIOPin pin)
{
  const uint8_t pinOffset = static_cast<uint8_t>(pin) % SIZE_OF_MFXSTM32L152_REGISTER_IN_BITS;
  const RegisterAddress registerAddress = mapToRegisterAddress(RegisterAddress::GPIO_IRQ_EVT, pin);

  return resetBitsInRegister(registerAddress, pinOffset);
}

MFXSTM32L152::ErrorCode
MFXSTM32L152::setGPIOPinInterruptActivitTypeToHighLevelOrRisingEdge(GPIOPin pin)
{
  const uint8_t pinOffset = static_cast<uint8_t>(pin) % SIZE_OF_MFXSTM32L152_REGISTER_IN_BITS;
  const RegisterAddress registerAddress = mapToRegisterAddress(RegisterAddress::GPIO_IRQ_ATYPE, pin);

  return setBitsInRegister(registerAddress, pinOffset);
}

MFXSTM32L152::ErrorCode
MFXSTM32L152::setGPIOPinInterruptActivitTypeToLowLevelOrFallingEdge(GPIOPin pin)
{
  const uint8_t pinOffset = static_cast<uint8_t>(pin) % SIZE_OF_MFXSTM32L152_REGISTER_IN_BITS;
  const RegisterAddress registerAddress = mapToRegisterAddress(RegisterAddress::GPIO_IRQ_ATYPE, pin);

  return resetBitsInRegister(registerAddress, pinOffset);
}

MFXSTM32L152::ErrorCode MFXSTM32L152::setGPIOPinStateToHigh(GPIOPin pin)
{
  const uint8_t pinOffset = static_cast<uint8_t>(pin) % SIZE_OF_MFXSTM32L152_REGISTER_IN_BITS;
  const RegisterAddress registerAddress = mapToRegisterAddress(RegisterAddress::GPIO_SET_HIGH, pin);

  return setBitsInRegister(registerAddress, pinOffset);
}

MFXSTM32L152::ErrorCode MFXSTM32L152::setGPIOPinStateToLow(GPIOPin pin)
{
  const uint8_t pinOffset = static_cast<uint8_t>(pin) % SIZE_OF_MFXSTM32L152_REGISTER_IN_BITS;
  const RegisterAddress registerAddress = mapToRegisterAddress(RegisterAddress::GPIO_SET_LOW, pin);

  return setBitsInRegister(registerAddress, pinOffset);
}

MFXSTM32L152::RegisterAddress
MFXSTM32L152::mapToRegisterAddress(RegisterAddress baseRegisterAddress, GPIOPin pin)
{
  constexpr uint8_t SIZE_OF_REGISTER_IN_BITS = 8u;
  const uint8_t registerAddressOffset = static_cast<uint8_t>(pin) / SIZE_OF_REGISTER_IN_BITS;
  const uint8_t registerAddress = static_cast<uint8_t>(baseRegisterAddress) + registerAddressOffset;

  return static_cast<RegisterAddress>(registerAddress);
}

inline void MFXSTM32L152::setBitsInRegister(uint8_t &registerValue, uint8_t bitPosition)
{
  registerValue = MemoryUtility<uint8_t>::setBit(registerValue, bitPosition);
}

template<typename... Args>
inline void MFXSTM32L152::setBitsInRegister(uint8_t &registerValue, uint8_t bitPosition, Args... args)
{
  setBitsInRegister(registerValue, bitPosition);
  setBitsInRegister(registerValue, args...);
}

template<typename... Args>
MFXSTM32L152::ErrorCode MFXSTM32L152::setBitsInRegister(RegisterAddress registerAddress, Args... args)
{
  uint8_t registerValue;

  ErrorCode errorCode = readRegister(static_cast<uint8_t>(registerAddress), &registerValue, sizeof(uint8_t));

  if (ErrorCode::OK == errorCode)
  {
    setBitsInRegister(registerValue, args...);
  }

  if (ErrorCode::OK == errorCode)
  {
    writeRegister(static_cast<uint8_t>(registerAddress), &registerValue, sizeof(uint8_t));
  }

  return errorCode;
}

inline void MFXSTM32L152::resetBitsInRegister(uint8_t &registerValue, uint8_t bitPosition)
{
  registerValue = MemoryUtility<uint8_t>::resetBit(registerValue, bitPosition);
}

template<typename... Args>
inline void MFXSTM32L152::resetBitsInRegister(uint8_t &registerValue, uint8_t bitPosition, Args... args)
{
  resetBitsInRegister(registerValue, bitPosition);
  resetBitsInRegister(registerValue, args...);
}

template<typename... Args>
MFXSTM32L152::ErrorCode MFXSTM32L152::resetBitsInRegister(RegisterAddress registerAddress, Args... args)
{
  uint8_t registerValue;

  ErrorCode errorCode = readRegister(static_cast<uint8_t>(registerAddress), &registerValue, sizeof(uint8_t));

  if (ErrorCode::OK == errorCode)
  {
    resetBitsInRegister(registerValue, args...);
  }

  if (ErrorCode::OK == errorCode)
  {
    writeRegister(static_cast<uint8_t>(registerAddress), &registerValue, sizeof(uint8_t));
  }

  return errorCode;
}

MFXSTM32L152::ErrorCode
MFXSTM32L152::readRegister(uint8_t registerAddress, void *messagePtr, uint32_t messageLen)
{
  I2C::ErrorCode errorCode = m_I2CPtr->readMemory(m_peripheralAddress, registerAddress, messagePtr, messageLen);

  return mapToErrorCode(errorCode);
}

MFXSTM32L152::ErrorCode
MFXSTM32L152::writeRegister(uint8_t registerAddress, const void *messagePtr, uint32_t messageLen)
{
  I2C::ErrorCode errorCode = m_I2CPtr->writeMemory(m_peripheralAddress, registerAddress, messagePtr, messageLen);

  return mapToErrorCode(errorCode);
}

MFXSTM32L152::ErrorCode MFXSTM32L152::mapToErrorCode(I2C::ErrorCode i2cErrorCode)
{
  switch (i2cErrorCode)
  {
    case I2C::ErrorCode::OK:
      return ErrorCode::OK;

    case I2C::ErrorCode::BUSY:
      return ErrorCode::BUSY;

    default:
      return ErrorCode::INTERNAL;
  }
}