#include "EXTI.h"


EXTI::EXTI(EXTI_TypeDef *EXTIPeripheralPtr):
  m_EXTIPeripheralPtr(EXTIPeripheralPtr)
{}

EXTI::ErrorCode EXTI::configureEXITLine(EXTILine line, EXTIConfig config)
{
  return ErrorCode::OK;
}
