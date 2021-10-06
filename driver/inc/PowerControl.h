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
    CAN_NOT_TURN_ON_PERIPHERAL_CLOCK = 1u
  };

#ifdef UNIT_TEST
  virtual
#endif // #ifdef UNIT_TEST
  ErrorCode init(void);

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

  //! Pointer to PWR peripheral
  PWR_TypeDef *m_PWRPeripheralPtr;

  //! Pointer to Reset Control module
  ResetControl *m_resetControlPtr;
};

#endif // #ifndef POWER_CONTROL_H