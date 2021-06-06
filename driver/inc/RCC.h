#ifndef RCC_H
#define RCC_H

#include "stm32l4r9xx.h"
#include "MemoryAccess.h"
#include "MemoryUtility.h"
#include <Peripheral.h>
#include <cstdint>

#ifdef RCC
#undef RCC
#endif // #ifdef RCC


class RCC
{
public:

  RCC(RCC_TypeDef *RCCPeripheralPtr);

  //! This enum class represents errors which can happen during method calls
  enum class ErrorCode : uint8_t
  {
    OK              = 0u,
    NOT_IMPLEMENTED = 1u,
    INTERNAL        = 2u, //!< Indicates problem with the implementation, should never happen (NON-TESTABLE)
  };

  ErrorCode enablePeripheralClock(Peripheral peripheral);

  ErrorCode disablePeripheralClock(Peripheral peripheral);

  ErrorCode isClockEnabled(Peripheral peripheral, bool &isPeripheralEnabled) const;

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
    uint32_t   enableBitPosition;
  };

  const EnablePeripheralClockRegisterMapping* findEnableClockRegisterMapping(Peripheral peripheral) const;

  volatile void* getPointerToRegister(Register register) const;
  
  bool setBitInRegister(Register register, uint32_t bitInRegister);

  bool resetBitInRegister(Register rccRegister, uint32_t bitInRegister);

  bool isBitSetInRegister(Register rccRegister, uint32_t bitInRegister, bool &isBitSet) const;

  static const EnablePeripheralClockRegisterMapping s_enablePeripheralClockMapping[];

  //! Pointer to RCC peripheral
  RCC_TypeDef *m_RCCPeripheralPtr;

};

#endif // #ifndef RCC_H