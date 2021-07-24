#include "InterruptController.h"
#include "MemoryAccess.h"
#include "MemoryUtility.h"
#include "RegisterUtility.h"


InterruptController::InterruptController(NVIC_Type *NVICPtr, SCB_Type *SCBPtr):
  m_NVICPtr(NVICPtr),
  m_SCBPtr(SCBPtr)
{}

InterruptController::ErrorCode
InterruptController::setInterruptPriority(int32_t irqNumber, uint32_t interruptPriority)
{
  if (not isIRQNumberInValidRangeOfValues(irqNumber))
  {
    return ErrorCode::IRQ_NUMBER_OUT_OF_RANGE;
  }

  if (not isInterruptPriorityInValidRangeOfValues(interruptPriority))
  {
    return ErrorCode::INTERRUPT_PRIORITY_OUT_OF_RANGE;
  }

  const uint32_t regValue =
    MemoryUtility<uint32_t>::setBits(0u, NUMBER_OF_BITS_IN_UINT8_T - __NVIC_PRIO_BITS, __NVIC_PRIO_BITS, interruptPriority);

  if (isIRQProcessorException(irqNumber))
  {
    MemoryAccess::setRegisterValue(&(m_SCBPtr->SHP[12 - irqNumber]), regValue);
  }
  else
  {
    MemoryAccess::setRegisterValue(&(m_NVICPtr->IP[irqNumber]), regValue);
  }

  return ErrorCode::OK;
}

InterruptController::ErrorCode InterruptController::enableInterrupt(int32_t irqNumber)
{
  if (not isIRQNumberInValidRangeOfValues(irqNumber))
  {
    return ErrorCode::IRQ_NUMBER_OUT_OF_RANGE;
  }

  if (not isIRQProcessorException(irqNumber))
  {
    const uint32_t index       = irqNumber / NUMBER_OF_BITS_IN_UINT32_T;
    const uint32_t bitPosition = irqNumber % NUMBER_OF_BITS_IN_UINT32_T;

    RegisterUtility<uint32_t>::setBitInRegister(&(m_NVICPtr->ISER[index]), bitPosition);
  }

  return ErrorCode::OK;
}

inline bool InterruptController::isIRQNumberInValidRangeOfValues(int32_t irqNumber)
{
  constexpr int32_t IRQ_NUMBER_MIN_VALUE = MemoryManagement_IRQn;
  constexpr int32_t IRQ_NUMBER_MAX_VALUE = DMAMUX1_OVR_IRQn;

  return (IRQ_NUMBER_MIN_VALUE <= irqNumber) && (IRQ_NUMBER_MAX_VALUE >= irqNumber);
}

inline bool InterruptController::isInterruptPriorityInValidRangeOfValues(uint32_t interruptPriority)
{
  constexpr uint32_t INTERRUPT_PRIORITY_MAX_VALUE = 1u << (NUMBER_OF_BITS_IN_UINT8_T - __NVIC_PRIO_BITS);

  return INTERRUPT_PRIORITY_MAX_VALUE > interruptPriority;
}

inline bool InterruptController::isIRQProcessorException(int32_t irqNumber)
{
  constexpr int32_t PROCESSOR_EXCEPTION_MIN_IRQ_NUMBER = -16u;
  constexpr int32_t PROCESSOR_EXCEPTION_MAX_IRQ_NUMBER = -1u;

  return (PROCESSOR_EXCEPTION_MIN_IRQ_NUMBER <= irqNumber) && (PROCESSOR_EXCEPTION_MAX_IRQ_NUMBER >= irqNumber);
}