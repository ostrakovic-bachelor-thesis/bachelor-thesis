#include "InterruptController.h"
#include "MemoryAccess.h"
#include "MemoryUtility.h"


InterruptController::InterruptController(NVIC_Type *NVICPtr, SCB_Type *SCBPtr):
  m_NVICPtr(NVICPtr),
  m_SCBPtr(SCBPtr)
{}

InterruptController::ErrorCode 
InterruptController::setInterruptPriority(int32_t irqNumber, uint32_t interruptPriority)
{
  constexpr uint8_t NUMBER_OF_BITS_IN_UINT8_T = 8u;

  const uint32_t regValue = interruptPriority << (NUMBER_OF_BITS_IN_UINT8_T - __NVIC_PRIO_BITS);

  if (0 <= irqNumber)
  {
    MemoryAccess::setRegisterValue(&(m_NVICPtr->IP[irqNumber]), regValue);
  }
  else
  {
    MemoryAccess::setRegisterValue(&(m_SCBPtr->SHP[12 - irqNumber]), regValue);
  }

  return ErrorCode::OK;
}