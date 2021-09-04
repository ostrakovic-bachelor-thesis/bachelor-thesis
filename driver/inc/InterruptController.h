#ifndef INTERRUPT_CONTROLLER_H
#define INTERRUPT_CONTROLLER_H

#include "stm32l4r9xx.h"
#include "CoreHardware.h"
#include <cstdint>


class InterruptController
{
public:

  InterruptController(NVIC_Type *NVICPtr, SCB_Type *SCBPtr);

#ifdef UNIT_TEST
  virtual
#endif // #ifdef UNIT_TEST
  ~InterruptController() = default;

  //! This enum class represents errors which can happen during method calls
  enum class ErrorCode : uint8_t
  {
    OK                              = 0u,
    IRQ_NUMBER_OUT_OF_RANGE         = 1u,
    INTERRUPT_PRIORITY_OUT_OF_RANGE = 2u
  };

#ifdef UNIT_TEST
  virtual
#endif // #ifdef UNIT_TEST
  ErrorCode setInterruptPriority(int32_t irqNumber, uint32_t interruptPriority);

#ifdef UNIT_TEST
  virtual
#endif // #ifdef UNIT_TEST
  ErrorCode enableInterrupt(int32_t irqNumber);

#ifdef UNIT_TEST
  /**
  * @brief Method gets raw pointer to underlaying NVIC core hardware instance.
   *
   * @return Pointer to underlaying NVIC core hardware instance.
   */
  inline void* getRawPointerNVIC(void) const
  {
    return reinterpret_cast<void*>(m_NVICPtr);
  }

  /**
   * @brief Method gets raw pointer to underlaying SCB core hardware instance.
   *
   * @return Pointer to underlaying SCB core hardware instance.
   */
  inline void* getRawPointerSCB(void) const
  {
    return reinterpret_cast<void*>(m_SCBPtr);
  }
#endif // #ifdef UNIT_TEST

private:

  static constexpr uint8_t  NUMBER_OF_BITS_IN_UINT8_T  = 8u;
  static constexpr uint32_t NUMBER_OF_BITS_IN_UINT32_T = 32u;

  static bool isIRQProcessorException(int32_t irqNumber);
  static bool isIRQNumberInValidRangeOfValues(int32_t irqNumber);
  static bool isInterruptPriorityInValidRangeOfValues(uint32_t interruptPriority);

  //! TODO
  NVIC_Type *m_NVICPtr;

  //! TODO
  SCB_Type *m_SCBPtr;

};

#endif // #ifndef INTERRUPT_CONTROLLER_H