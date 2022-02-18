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

PowerControl::ErrorCode PowerControl::enablePowerSupplyVDDIO2(void)
{
  enableVDDIO2Monitoring();
  while (not isVDDIO2AboveThreshold());

  validateVDDIO2PowerSupply();

  return ErrorCode::OK;
}

PowerControl::DynamicVoltageScalingRange PowerControl::getDynamicVoltageScalingRange(void) const
{
  if (isLowPowerDynamicVoltageScalingRangeActive())
  {
    return DynamicVoltageScalingRange::LOW_POWER;
  }
  else if (isHighPerformanceBoostModeActive())
  {
    return DynamicVoltageScalingRange::HIGH_PERFORMANCE_BOOST;
  }
  else
  {
    return DynamicVoltageScalingRange::HIGH_PERFORMANCE_NORMAL;
  }
}

PowerControl::ErrorCode PowerControl::setDynamicVoltageScalingRange(DynamicVoltageScalingRange range)
{
  switch (range)
  {
    case DynamicVoltageScalingRange::LOW_POWER:
    {
      setLowPowerDynamicVoltageScalingRange();
    }
    break;

    case DynamicVoltageScalingRange::HIGH_PERFORMANCE_NORMAL:
    {
      if (DynamicVoltageScalingRange::HIGH_PERFORMANCE_NORMAL != getDynamicVoltageScalingRange())
      {
        setHighPowerNormalDynamicVoltageScalingRange();
      }

      if (DynamicVoltageScalingRange::LOW_POWER == getDynamicVoltageScalingRange())
      {
        setHighPowerDynamicVoltageScalingRange();
        while (not isSelectedDynamicVoltageScalingRangeReady());
      }
    }
    break;

    case DynamicVoltageScalingRange::HIGH_PERFORMANCE_BOOST:
    {
      if (DynamicVoltageScalingRange::HIGH_PERFORMANCE_BOOST != getDynamicVoltageScalingRange())
      {
        setHighPowerBoostDynamicVoltageScalingRange();
      }

      if (DynamicVoltageScalingRange::LOW_POWER == getDynamicVoltageScalingRange())
      {
        setHighPowerDynamicVoltageScalingRange();
        while (not isSelectedDynamicVoltageScalingRangeReady());
      }
    }
    break;

    default:
    {
      // do nothing
    }
    break;
  }

  return ErrorCode::OK;
}

inline PowerControl::ErrorCode PowerControl::enablePeripheralClock(void)
{
  ResetControl::ErrorCode errorCode = m_resetControlPtr->enablePeripheralClock(getPeripheralTag());

  return (ResetControl::ErrorCode::OK == errorCode) ? ErrorCode::OK : ErrorCode::CAN_NOT_TURN_ON_PERIPHERAL_CLOCK;
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

inline bool PowerControl::isVDDIO2AboveThreshold(void) const
{
  constexpr uint8_t PWR_SR2_PVMO2_POSITION = 13u;
  return not RegisterUtility<uint32_t>::isBitSetInRegister(&(m_PWRPeripheralPtr->SR2), PWR_SR2_PVMO2_POSITION);
}

inline bool PowerControl::isLowPowerDynamicVoltageScalingRangeActive(void) const
{
  constexpr uint8_t PWR_CR1_VOS_POSITION    = 9u;
  constexpr uint8_t PWR_CR1_VOS_NUM_OF_BITS = 2u;
  constexpr uint8_t PWR_CR1_VOS_RANGE2      = 0b10;

  const uint32_t vosBitsValue = RegisterUtility<uint32_t>::getBitsInRegister(
    &(m_PWRPeripheralPtr->CR1),
    PWR_CR1_VOS_POSITION,
    PWR_CR1_VOS_NUM_OF_BITS);

  return (PWR_CR1_VOS_RANGE2 == vosBitsValue);
}

inline bool PowerControl::isHighPerformanceBoostModeActive(void) const
{
  constexpr uint8_t PWR_CR5_R1MODE_POSITION = 8u;

  return not RegisterUtility<uint32_t>::isBitSetInRegister(&(m_PWRPeripheralPtr->CR5), PWR_CR5_R1MODE_POSITION);
}

inline void PowerControl::setLowPowerDynamicVoltageScalingRange(void)
{
  constexpr uint8_t PWR_CR1_VOS_POSITION    = 9u;
  constexpr uint8_t PWR_CR1_VOS_NUM_OF_BITS = 2u;
  constexpr uint8_t PWR_CR1_VOS_RANGE2      = 0b10;

  RegisterUtility<uint32_t>::setBitsInRegister(&(m_PWRPeripheralPtr->CR1),
    PWR_CR1_VOS_POSITION,
    PWR_CR1_VOS_NUM_OF_BITS,
    PWR_CR1_VOS_RANGE2);
}

inline void PowerControl::setHighPowerDynamicVoltageScalingRange(void)
{
  constexpr uint8_t PWR_CR1_VOS_POSITION    = 9u;
  constexpr uint8_t PWR_CR1_VOS_NUM_OF_BITS = 2u;
  constexpr uint8_t PWR_CR1_VOS_RANGE1      = 0b01;

  RegisterUtility<uint32_t>::setBitsInRegister(&(m_PWRPeripheralPtr->CR1),
    PWR_CR1_VOS_POSITION,
    PWR_CR1_VOS_NUM_OF_BITS,
    PWR_CR1_VOS_RANGE1);
}

inline void PowerControl::setHighPowerNormalDynamicVoltageScalingRange(void)
{
  constexpr uint32_t PWR_CR5_R1MODE_POSITION = 8u;

  RegisterUtility<uint32_t>::setBitInRegister(&(m_PWRPeripheralPtr->CR5), PWR_CR5_R1MODE_POSITION);
}

inline void PowerControl::setHighPowerBoostDynamicVoltageScalingRange(void)
{
  constexpr uint32_t PWR_CR5_R1MODE_POSITION = 8u;

  RegisterUtility<uint32_t>::resetBitInRegister(&(m_PWRPeripheralPtr->CR5), PWR_CR5_R1MODE_POSITION);
}

inline bool PowerControl::isSelectedDynamicVoltageScalingRangeReady(void) const
{
  constexpr uint8_t PWR_SR2_VOSF_POSITION = 10u;
  return not RegisterUtility<uint32_t>::isBitSetInRegister(&(m_PWRPeripheralPtr->SR2), PWR_SR2_VOSF_POSITION);
}