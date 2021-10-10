#include "PowerControl.h"
#include "RegisterUtility.h"


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


PowerControl::ErrorCode PowerControl::enablePowerSupplyVDDIO2(void)
{
  enableVDDIO2Monitoring();
  while (not isVDDIO2AboveThreshold());

  validateVDDIO2PowerSupply();

  return ErrorCode::OK;
}

inline void PowerControl::validateVDDIO2PowerSupply(void)
{
  constexpr uint8_t PWR_CR2_IOSV_POSITION = 9u;
  RegisterUtility<uint32_t>::setBitInRegister(&(m_PWRPeripheralPtr->CR2), PWR_CR2_IOSV_POSITION);
}

inline void PowerControl::enableVDDIO2Monitoring(void)
{
  constexpr uint8_t PWR_CR2_PVME2_POSITION = 5u;
  RegisterUtility<uint32_t>::setBitInRegister(&(m_PWRPeripheralPtr->CR2), PWR_CR2_PVME2_POSITION);
}

inline bool PowerControl::isVDDIO2AboveThreshold(void)
{
  constexpr uint8_t PWR_SR2_PVMO2_POSITION = 13u;
  return not RegisterUtility<uint32_t>::isBitSetInRegister(&(m_PWRPeripheralPtr->SR2), PWR_SR2_PVMO2_POSITION);
}