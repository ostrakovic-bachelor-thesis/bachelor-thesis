#ifndef RESET_CONTROL_H
#define RESET_CONTROL_H

#include "stm32l4r9xx.h"
#include "MemoryAccess.h"
#include "MemoryUtility.h"
#include "Peripheral.h"
#include <cstdint>


class ResetControl
{
public:

  /**
   * @brief Constructor of ResetControl class instance.
   * 
   * @param[in] - Pointer to a RCC peripheral instance.
   */
  ResetControl(RCC_TypeDef *RCCPeripheralPtr);

  //! This enum class represents errors which can happen during method calls
  enum class ErrorCode : uint8_t
  {
    OK              = 0u,
    NOT_IMPLEMENTED = 1u,
    INTERNAL        = 2u, //!< Indicates problem with the implementation, should never happen (NON-TESTABLE)
  };

  /**
   * @brief Method enables peripheral clock.
   *
   * @param[in] peripheral - Peripheral which clock will be enabled.
   * @return ::ErrorCode::OK is returned in the case of success. In the case of failure, 
   *         it returns corresponding error code from ::ErrorCode.
   */
  ErrorCode enablePeripheralClock(Peripheral peripheral);

  /**
   * @brief Method disables peripheral clock.
   *
   * @param[in] peripheral - Peripheral which clock will be disabled.
   * @return ::ErrorCode::OK is returned in the case of success. In the case of failure, 
   *         it returns corresponding error code from ::ErrorCode.
   */
  ErrorCode disablePeripheralClock(Peripheral peripheral);

  /**
   * @brief Method checks is peripheral clock enabled.
   *
   * @param[in]  peripheral     - Peripheral which clock will checked.
   * @param[out] isClockEnabled - Reference to variable in which state of enabling will be written.
   * @return ::ErrorCode::OK is returned in the case of success. In the case of failure, 
   *         it returns corresponding error code from ::ErrorCode.
   */
  ErrorCode isPeripheralClockEnabled(Peripheral peripheral, bool &isClockEnabled) const;

private:

  enum class Register : uint8_t
  {
    AHB1ENR,
    AHB2ENR,
    AHB3ENR,
    APB1ENR1,
    APB1ENR2,
    APB2ENR,
  };

  struct EnablePeripheralClockRegisterMapping
  {
    Peripheral peripheral;
    Register   enableRegister;
    uint8_t    enableBitPosition;
  };

  const EnablePeripheralClockRegisterMapping* findEnableClockRegisterMapping(Peripheral peripheral) const;

  volatile uint32_t* getPointerToRegister(Register register) const;
  
  bool setBitInRegister(Register register, uint8_t bitInRegister);

  bool resetBitInRegister(Register rccRegister, uint8_t bitInRegister);

  bool isBitSetInRegister(Register rccRegister, uint8_t bitInRegister, bool &isBitSet) const;

  static const EnablePeripheralClockRegisterMapping s_enablePeripheralClockMapping[];

  //! Pointer to RCC peripheral
  RCC_TypeDef *m_RCCPeripheralPtr;

};

#endif // #ifndef RESET_CONTROL_H