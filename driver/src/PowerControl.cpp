#include "PowerControl.h"


PowerControl::PowerControl(PWR_TypeDef *PWRPeripheralPtr, ResetControl *resetControlPtr):
  m_PWRPeripheralPtr(PWRPeripheralPtr),
  m_resetControlPtr(resetControlPtr)
{}

PowerControl::ErrorCode PowerControl::init(void)
{
  ErrorCode errorCode = enablePeripheralClock();

  return errorCode;
}

inline PowerControl::ErrorCode PowerControl::enablePeripheralClock(void)
{
  ResetControl::ErrorCode errorCode = m_resetControlPtr->enablePeripheralClock(getPeripheralTag());

  return (ResetControl::ErrorCode::OK == errorCode) ? ErrorCode::OK : ErrorCode::CAN_NOT_TURN_ON_PERIPHERAL_CLOCK;
}