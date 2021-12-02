#include "FT3267.h"


FT3267::FT3267(I2C *I2CPtr, SysTick *sysTickPtr):
  m_I2CPtr(I2CPtr),
  m_sysTickPtr(sysTickPtr)
{}

FT3267::ErrorCode FT3267::init(FT3267Config &ft3267Config)
{
  uint8_t firmwareId;
  m_configuration = ft3267Config;

  if (m_configuration.performPowerOn)
  {
    powerOn(m_configuration);
  }

  ErrorCode errorCode = disableGestureMode();

  if (ErrorCode::OK == errorCode)
  {
    errorCode = setOperationMode(OperationMode::INTERRUPT);
  }

  if (ErrorCode::OK == errorCode)
  {
    errorCode = getFirmwareID(firmwareId);
  }

  return errorCode;
}

FT3267::ErrorCode FT3267::getID(uint8_t &id)
{
  return readRegister(RegisterAddress::CHIP_ID, &id, sizeof(uint8_t));
}

FT3267::ErrorCode FT3267::getFirmwareID(uint8_t &firmwareId)
{
  return readRegister(RegisterAddress::FIRMWARE_ID, &firmwareId, sizeof(uint8_t));
}

FT3267::ErrorCode FT3267::setOperationMode(OperationMode operationMode)
{
  return writeRegister(
    RegisterAddress::OPERATION_MODE,
    reinterpret_cast<uint8_t*>(&operationMode),
    sizeof(uint8_t));
}

FT3267::ErrorCode FT3267::disableGestureMode(void)
{
  constexpr uint8_t DISABLE_GESTURE_MODE = 0x0u;
  uint32_t registerValue = DISABLE_GESTURE_MODE;

  return writeRegister(RegisterAddress::GESTURE_MODE_ENABLE, &registerValue, sizeof(uint8_t));
}

void FT3267::powerOn(const FT3267Config &ft3267Config)
{
  constexpr uint64_t WAIT_BEFORE_SET_FT3267_RESET_LINE_TO_HIGH_PERIOD_MS = 10u;

  ft3267Config.setFT3267ResetLineToLowCallback();

  waitMs(WAIT_BEFORE_SET_FT3267_RESET_LINE_TO_HIGH_PERIOD_MS);

  ft3267Config.setFT3267ResetLineToHighCallback();
}

FT3267::ErrorCode FT3267::readRegister(RegisterAddress registerAddress, void *messagePtr, uint32_t messageLen)
{
  I2C::ErrorCode errorCode = m_I2CPtr->readMemory(
    m_configuration.peripheralAddress,
    static_cast<uint8_t>(registerAddress),
    messagePtr,
    messageLen);

  if (I2C::ErrorCode::OK == errorCode)
  {
    while (m_I2CPtr->isTransactionOngoing());
  }

  return mapToErrorCode(errorCode);
}

FT3267::ErrorCode FT3267::writeRegister(RegisterAddress registerAddress, const void *messagePtr, uint32_t messageLen)
{
  I2C::ErrorCode errorCode = m_I2CPtr->writeMemory(
    m_configuration.peripheralAddress,
    static_cast<uint8_t>(registerAddress),
    messagePtr,
    messageLen);

  if (I2C::ErrorCode::OK == errorCode)
  {
    while (m_I2CPtr->isTransactionOngoing());
  }

  return mapToErrorCode(errorCode);
}

FT3267::ErrorCode FT3267::mapToErrorCode(I2C::ErrorCode i2cErrorCode)
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

void FT3267::waitMs(uint64_t periodToWaitInMs)
{
  const uint32_t timestamp = m_sysTickPtr->getTicks();

  while (m_sysTickPtr->getElapsedTimeInMs(timestamp) < periodToWaitInMs);
}