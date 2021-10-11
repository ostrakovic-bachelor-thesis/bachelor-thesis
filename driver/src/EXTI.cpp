#include "EXTI.h"
#include "RegisterUtility.h"


EXTI::EXTI(EXTI_TypeDef *EXTIPeripheralPtr):
  m_EXTIPeripheralPtr(EXTIPeripheralPtr)
{}

EXTI::ErrorCode EXTI::configureEXITLine(EXTILine line, EXTIConfig config)
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

  return ErrorCode::OK;
}

void EXTI::IRQHandler(IRQTag irqTag)
{

}

void EXTI::unmaskInterruptLine(EXTILine line)
{
  volatile uint32_t * const registerPtr = (EXTILine::LINE32 > line) ?
    &(m_EXTIPeripheralPtr->IMR1) :
    &(m_EXTIPeripheralPtr->IMR2);
  const uint8_t bitPosition = static_cast<uint32_t>(line) % NUMBER_OF_BITS_IN_UINT32_T;

  RegisterUtility<uint32_t>::setBitInRegister(registerPtr, bitPosition);
}

void EXTI::maskInterruptLine(EXTILine line)
{
  volatile uint32_t * const registerPtr = (EXTILine::LINE32 > line) ?
    &(m_EXTIPeripheralPtr->IMR1) :
    &(m_EXTIPeripheralPtr->IMR2);
  const uint8_t bitPosition = static_cast<uint32_t>(line) % NUMBER_OF_BITS_IN_UINT32_T;

  RegisterUtility<uint32_t>::resetBitInRegister(registerPtr, bitPosition);
}

void EXTI::enableInterruptTriggeringOnRisingEdge(EXTILine line)
{
  volatile uint32_t * const registerPtr = (EXTILine::LINE32 > line) ?
    &(m_EXTIPeripheralPtr->RTSR1) :
    &(m_EXTIPeripheralPtr->RTSR2);
  const uint8_t bitPosition = static_cast<uint32_t>(line) % NUMBER_OF_BITS_IN_UINT32_T;

  RegisterUtility<uint32_t>::setBitInRegister(registerPtr, bitPosition);
}

void EXTI::disableInterruptTriggeringOnRisingEdge(EXTILine line)
{
  volatile uint32_t * const registerPtr = (EXTILine::LINE32 > line) ?
    &(m_EXTIPeripheralPtr->RTSR1) :
    &(m_EXTIPeripheralPtr->RTSR2);
  const uint8_t bitPosition = static_cast<uint32_t>(line) % NUMBER_OF_BITS_IN_UINT32_T;

  RegisterUtility<uint32_t>::resetBitInRegister(registerPtr, bitPosition);
}

void EXTI::enableInterruptTriggeringOnFallingEdge(EXTILine line)
{
  volatile uint32_t * const registerPtr = (EXTILine::LINE32 > line) ?
    &(m_EXTIPeripheralPtr->FTSR1) :
    &(m_EXTIPeripheralPtr->FTSR2);
  const uint8_t bitPosition = static_cast<uint32_t>(line) % NUMBER_OF_BITS_IN_UINT32_T;

  RegisterUtility<uint32_t>::setBitInRegister(registerPtr, bitPosition);
}

void EXTI::disableInterruptTriggeringOnFallingEdge(EXTILine line)
{
  volatile uint32_t * const registerPtr = (EXTILine::LINE32 > line) ?
    &(m_EXTIPeripheralPtr->FTSR1) :
    &(m_EXTIPeripheralPtr->FTSR2);
  const uint8_t bitPosition = static_cast<uint32_t>(line) % NUMBER_OF_BITS_IN_UINT32_T;

  RegisterUtility<uint32_t>::resetBitInRegister(registerPtr, bitPosition);
}