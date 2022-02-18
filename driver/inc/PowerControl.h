#ifndef POWER_CONTROL_H
#define POWER_CONTROL_H

#include "stm32l4r9xx.h"
#include "ResetControl.h"
#include "Peripheral.h"


class PowerControl
{
public:

  PowerControl(PWR_TypeDef *PWRPeripheralPtr, ResetControl *resetControlPtr);

#ifdef UNIT_TEST
  virtual
#endif // #ifdef UNIT_TEST
  ~PowerControl() = default;

  //! This enum class represents errors which can happen during method calls
  enum class ErrorCode : uint8_t
  {
    OK                               = 0u,
    VOLTAGE_IS_BELOW_THRESHOLD       = 1u,
    CAN_NOT_TURN_ON_PERIPHERAL_CLOCK = 2u
  };

  enum class DynamicVoltageScalingRange : uint8_t
  {
    LOW_POWER               = 0u,
    HIGH_PERFORMANCE_NORMAL = 1u,
    HIGH_PERFORMANCE_BOOST  = 2u
  };

#ifdef UNIT_TEST
  virtual
#endif // #ifdef UNIT_TEST
  ErrorCode init(void);

#ifdef UNIT_TEST
  virtual
#endif // #ifdef UNIT_TEST
  ErrorCode enablePowerSupplyVDDIO2(void);

#ifdef UNIT_TEST
  virtual
#endif // #ifdef UNIT_TEST
  DynamicVoltageScalingRange getDynamicVoltageScalingRange(void) const;

#ifdef UNIT_TEST
  virtual
#endif // #ifdef UNIT_TEST
  ErrorCode setDynamicVoltageScalingRange(DynamicVoltageScalingRange range);

  /**
   * @brief   Method gets peripheral tag of the PowerControl instance.
   * @details PowerControl peripheral tag is pointer to underlaying PWR peripheral
   *          instance casted to Peripheral type.
   *
   * @return  Peripheral tag of the PowerControl instance.
   */
  inline Peripheral getPeripheralTag(void) const
  {
    return static_cast<Peripheral>(reinterpret_cast<uintptr_t>(const_cast<PWR_TypeDef*>(m_PWRPeripheralPtr)));
  }

private:

  ErrorCode enablePeripheralClock(void);

  void enableVDDIO2Monitoring(void);
  bool isVDDIO2AboveThreshold(void) const;
  void validateVDDIO2PowerSupply(void);

  bool isLowPowerDynamicVoltageScalingRangeActive(void) const;
  bool isHighPerformanceBoostModeActive(void) const;
  void setLowPowerDynamicVoltageScalingRange(void);
  void setHighPowerDynamicVoltageScalingRange(void);
  void setHighPowerNormalDynamicVoltageScalingRange(void);
  void setHighPowerBoostDynamicVoltageScalingRange(void);
  bool isSelectedDynamicVoltageScalingRangeReady(void) const;

  //! Pointer to PWR peripheral
  PWR_TypeDef *m_PWRPeripheralPtr;

  //! Pointer to Reset Control module
  ResetControl *m_resetControlPtr;
};

#endif // #ifndef POWER_CONTROL_H