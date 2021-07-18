#ifndef INTERRUPT_CONTROLLER_H
#define INTERRUPT_CONTROLLER_H

#include "stm32l4r9xx.h"
#include "CoreHardware.h"
#include <cstdint>


class InterruptController
{
public:

  InterruptController(NVIC_Type *NVICPtr, SCB_Type *SCBPtr);

  //! This enum class represents errors which can happen during method calls
  enum class ErrorCode : uint8_t
  {
    OK = 0u
  };

  ErrorCode setInterruptPriority(int32_t irqNumber, uint32_t interruptPriority);

#ifdef UNIT_TEST_DRIVER
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
#endif // #ifdef UNIT_TEST_DRIVER

private:

  //! TODO
  NVIC_Type *m_NVICPtr;

  //! TODO
  SCB_Type *m_SCBPtr;

};

#endif // #ifndef INTERRUPT_CONTROLLER_H