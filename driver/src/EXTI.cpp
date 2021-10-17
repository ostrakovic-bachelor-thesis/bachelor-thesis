#include "EXTI.h"
#include "RegisterUtility.h"


EXTI::EXTI(EXTI_TypeDef *EXTIPeripheralPtr):
  m_EXTIPeripheralPtr(EXTIPeripheralPtr)
{}

EXTI::ErrorCode EXTI::configureEXTILine(EXTILine line, EXTIConfig config)
{
  if (config.isInterruptMasked)
  {
    maskInterruptLine(line);
  }
  else
  {
    unmaskInterruptLine(line);

    if (InterruptTrigger::BOTH_EDGE == config.interruptTrigger)
    {
      enableInterruptTriggeringOnRisingEdge(line);
      enableInterruptTriggeringOnFallingEdge(line);
    }
    else if (InterruptTrigger::RISING_EDGE == config.interruptTrigger)
    {
      enableInterruptTriggeringOnRisingEdge(line);
      disableInterruptTriggeringOnFallingEdge(line);
    }
    else
    {
      enableInterruptTriggeringOnFallingEdge(line);
      disableInterruptTriggeringOnRisingEdge(line);
    }
  }

  registerInterruptCallback(line, config.interruptCallback);

  return ErrorCode::OK;
}

void EXTI::IRQHandler(EXTILine extiLineRangeStart, EXTILine extiLineRangeEnd)
{
  for (uint8_t extiLine = static_cast<uint8_t>(extiLineRangeStart);
    extiLine <= static_cast<uint8_t>(extiLineRangeEnd);
    ++extiLine)
  {
    if (isInterruptPending(static_cast<EXTILine>(extiLine)))
    {
      clearPendingInterrupt(static_cast<EXTILine>(extiLine));
      requestInterruptCallbackCalling(static_cast<EXTILine>(extiLine));
    }
  }
}

EXTI::ErrorCode EXTI::runtimeTask(void)
{
  for (uint32_t extiLine = 0u; extiLine <= NUMBER_OF_EXTI_LINES; extiLine++)
  {
    if (isCallingOfInterruptCallbackRequested(static_cast<EXTILine>(extiLine)))
    {
      clearInterruptCallbackCallingRequest(static_cast<EXTILine>(extiLine));
      callInterruptCallback(static_cast<EXTILine>(extiLine));
    }
  }

  return ErrorCode::OK;
}

inline void EXTI::unmaskInterruptLine(EXTILine line)
{
  volatile uint32_t * const registerPtr = (EXTILine::LINE32 > line) ?
    &(m_EXTIPeripheralPtr->IMR1) :
    &(m_EXTIPeripheralPtr->IMR2);
  const uint8_t bitPosition = static_cast<uint32_t>(line) % NUMBER_OF_BITS_IN_UINT32_T;

  RegisterUtility<uint32_t>::setBitInRegister(registerPtr, bitPosition);
}

inline void EXTI::maskInterruptLine(EXTILine line)
{
  volatile uint32_t * const registerPtr = (EXTILine::LINE32 > line) ?
    &(m_EXTIPeripheralPtr->IMR1) :
    &(m_EXTIPeripheralPtr->IMR2);
  const uint8_t bitPosition = static_cast<uint32_t>(line) % NUMBER_OF_BITS_IN_UINT32_T;

  RegisterUtility<uint32_t>::resetBitInRegister(registerPtr, bitPosition);
}

inline void EXTI::enableInterruptTriggeringOnRisingEdge(EXTILine line)
{
  volatile uint32_t * const registerPtr = (EXTILine::LINE32 > line) ?
    &(m_EXTIPeripheralPtr->RTSR1) :
    &(m_EXTIPeripheralPtr->RTSR2);
  const uint8_t bitPosition = static_cast<uint32_t>(line) % NUMBER_OF_BITS_IN_UINT32_T;

  RegisterUtility<uint32_t>::setBitInRegister(registerPtr, bitPosition);
}

inline void EXTI::disableInterruptTriggeringOnRisingEdge(EXTILine line)
{
  volatile uint32_t * const registerPtr = (EXTILine::LINE32 > line) ?
    &(m_EXTIPeripheralPtr->RTSR1) :
    &(m_EXTIPeripheralPtr->RTSR2);
  const uint8_t bitPosition = static_cast<uint32_t>(line) % NUMBER_OF_BITS_IN_UINT32_T;

  RegisterUtility<uint32_t>::resetBitInRegister(registerPtr, bitPosition);
}

inline void EXTI::enableInterruptTriggeringOnFallingEdge(EXTILine line)
{
  volatile uint32_t * const registerPtr = (EXTILine::LINE32 > line) ?
    &(m_EXTIPeripheralPtr->FTSR1) :
    &(m_EXTIPeripheralPtr->FTSR2);
  const uint8_t bitPosition = static_cast<uint32_t>(line) % NUMBER_OF_BITS_IN_UINT32_T;

  RegisterUtility<uint32_t>::setBitInRegister(registerPtr, bitPosition);
}

inline void EXTI::disableInterruptTriggeringOnFallingEdge(EXTILine line)
{
  volatile uint32_t * const registerPtr = (EXTILine::LINE32 > line) ?
    &(m_EXTIPeripheralPtr->FTSR1) :
    &(m_EXTIPeripheralPtr->FTSR2);
  const uint8_t bitPosition = static_cast<uint32_t>(line) % NUMBER_OF_BITS_IN_UINT32_T;

  RegisterUtility<uint32_t>::resetBitInRegister(registerPtr, bitPosition);
}

inline bool EXTI::isInterruptPending(EXTILine line) const
{
  volatile uint32_t * const registerPtr = (EXTILine::LINE32 > line) ?
    &(m_EXTIPeripheralPtr->PR1) :
    &(m_EXTIPeripheralPtr->PR2);
  const uint8_t bitPosition = static_cast<uint32_t>(line) % NUMBER_OF_BITS_IN_UINT32_T;

  return RegisterUtility<uint32_t>::isBitSetInRegister(registerPtr, bitPosition);
}

inline void EXTI::clearPendingInterrupt(EXTILine line)
{
  volatile uint32_t * const registerPtr = (EXTILine::LINE32 > line) ?
    &(m_EXTIPeripheralPtr->PR1) :
    &(m_EXTIPeripheralPtr->PR2);
  const uint8_t bitPosition = static_cast<uint32_t>(line) % NUMBER_OF_BITS_IN_UINT32_T;

  RegisterUtility<uint32_t>::setBitInRegister(registerPtr, bitPosition);
}

inline void EXTI::registerInterruptCallback(EXTILine line, InterruptCallback_t interruptCallback)
{
  m_interruptCallbacks[static_cast<uint8_t>(line)] = interruptCallback;
}

inline void EXTI::callInterruptCallback(EXTILine line)
{
  if (nullptr != m_interruptCallbacks[static_cast<uint8_t>(line)])
  {
    m_interruptCallbacks[static_cast<uint8_t>(line)](line);
  }
}

inline void EXTI::requestInterruptCallbackCalling(EXTILine line)
{
  m_callInterruptCallback[static_cast<uint8_t>(line)] = true;
}

inline bool EXTI::isCallingOfInterruptCallbackRequested(EXTILine line) const
{
  return m_callInterruptCallback[static_cast<uint8_t>(line)];
}

inline void EXTI::clearInterruptCallbackCallingRequest(EXTILine line)
{
  m_callInterruptCallback[static_cast<uint8_t>(line)] = false;
}


