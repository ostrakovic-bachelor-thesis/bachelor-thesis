#include "USART.h"


USART::USART(USART_TypeDef *USARTPeripheralPtr):
  m_USARTPeripheralPtr(USARTPeripheralPtr)
{}

USART::ErrorCode USART::init(const USARTConfig &usartConfig)
{
  return ErrorCode::OK;
}