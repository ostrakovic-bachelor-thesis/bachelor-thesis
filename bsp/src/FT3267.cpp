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

FT3267::ErrorCode FT3267::registerTouchEventCallback(TouchEventCallbackFunc callback, void *callbackArgument)
{
  m_callback = callback;
  m_callbackArgument = callbackArgument;

  return ErrorCode::OK;
}

FT3267::ErrorCode FT3267::runtimeTask(void)
{
  TouchEventInfo touchEventInfo;
  ErrorCode errorCode = getTouchCount(touchEventInfo.touchCount);

  if ((ErrorCode::OK == errorCode) && (0u != touchEventInfo.touchCount))
  {
    errorCode = getTouchPoint1(touchEventInfo.touchPoints[0]);
    mapToDisplayCoordinates(touchEventInfo.touchPoints[0].position);
  }

  if ((ErrorCode::OK == errorCode) && (1u < touchEventInfo.touchCount))
  {
    errorCode = getTouchPoint2(touchEventInfo.touchPoints[1]);
    mapToDisplayCoordinates(touchEventInfo.touchPoints[1].position);
  }

  if (nullptr != m_callback)
  {
    m_callback(m_callbackArgument, touchEventInfo);
  }

  return errorCode;
}

void FT3267::mapToDisplayCoordinates(TouchPosition &touchPosition)
{
  if (nullptr != m_configuration.mapFromTouchScreenToDisplayCoordinatesFunc)
  {
    touchPosition = m_configuration.mapFromTouchScreenToDisplayCoordinatesFunc(touchPosition);
  }
}

FT3267::ErrorCode FT3267::getTouchCount(uint8_t &touchCount)
{
  uint8_t touchDataStatusRegisterVal;
  ErrorCode errorCode = readRegister(RegisterAddress::TOUCH_DATA_STATUS, &touchDataStatusRegisterVal, sizeof(uint8_t));

  touchCount = touchDataStatusRegisterVal;

  return errorCode;
}

FT3267::ErrorCode FT3267::getTouchPoint1(TouchPoint &touchPoint)
{
  uint8_t touchPoint1InfoRegisterVal[4];
  ErrorCode errorCode = readRegister(
      RegisterAddress::TOUCH_POINT_1_INFO,
      &touchPoint1InfoRegisterVal,
      sizeof(touchPoint1InfoRegisterVal));

  touchPoint.position.x = getTouchPositionXCoordinate(touchPoint1InfoRegisterVal);
  touchPoint.position.y = getTouchPositionYCoordinate(touchPoint1InfoRegisterVal);
  touchPoint.event      = getTouchEvent(touchPoint1InfoRegisterVal);

  return errorCode;
}

FT3267::ErrorCode FT3267::getTouchPoint2(TouchPoint &touchPoint)
{
  uint8_t touchPoint2InfoRegisterVal[4];
  ErrorCode errorCode = readRegister(
      RegisterAddress::TOUCH_POINT_2_INFO,
      &touchPoint2InfoRegisterVal,
      sizeof(touchPoint2InfoRegisterVal));

  touchPoint.position.x = getTouchPositionXCoordinate(touchPoint2InfoRegisterVal);
  touchPoint.position.y = getTouchPositionYCoordinate(touchPoint2InfoRegisterVal);
  touchPoint.event      = getTouchEvent(touchPoint2InfoRegisterVal);

  return errorCode;
}

uint16_t FT3267::getTouchPositionXCoordinate(uint8_t touchPointXInfoRegisterVal[4])
{
  constexpr uint8_t FT3267_TOUCH_POINT_X_INFO_X_POS_SIZE = 12u;

  uint16_t xPos = MemoryUtility<uint16_t>::setBits(
      0u,
      BITS_IN_BYTE,
      FT3267_TOUCH_POINT_X_INFO_X_POS_SIZE - BITS_IN_BYTE,
      touchPointXInfoRegisterVal[0]);

  xPos = MemoryUtility<uint16_t>::setBits(xPos, 0u, BITS_IN_BYTE, touchPointXInfoRegisterVal[1]);

  return xPos;
}

uint16_t FT3267::getTouchPositionYCoordinate(uint8_t touchPointXInfoRegisterVal[4])
{
  constexpr uint8_t FT3267_TOUCH_POINT_X_INFO_Y_POS_SIZE = 12u;

  uint16_t yPos = MemoryUtility<uint16_t>::setBits(
      0u,
      BITS_IN_BYTE,
      FT3267_TOUCH_POINT_X_INFO_Y_POS_SIZE - BITS_IN_BYTE,
      touchPointXInfoRegisterVal[2]);

  yPos = MemoryUtility<uint16_t>::setBits(yPos, 0u, BITS_IN_BYTE, touchPointXInfoRegisterVal[3]);

  return yPos;
}

FT3267::TouchEvent FT3267::getTouchEvent(uint8_t touchPointXInfoRegisterVal[4])
{
  constexpr uint8_t FT3267_TOUCH_POINT_X_INFO_TOUCH_EVENT_POSITION = 6u;
  constexpr uint8_t FT3267_TOUCH_POINT_X_INFO_TOUCH_EVENT_SIZE     = 2u;

  return static_cast<TouchEvent>(MemoryUtility<uint16_t>::getBits(
    touchPointXInfoRegisterVal[0],
    FT3267_TOUCH_POINT_X_INFO_TOUCH_EVENT_POSITION,
    FT3267_TOUCH_POINT_X_INFO_TOUCH_EVENT_SIZE));
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