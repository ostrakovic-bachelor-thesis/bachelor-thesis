#include "USART.h"
#include "MemoryAccess.h"
#include "MemoryUtility.h"


USART::USART(USART_TypeDef *USARTPeripheralPtr):
  m_USARTPeripheralPtr(USARTPeripheralPtr)
{}

USART::ErrorCode USART::init(const USARTConfig &usartConfig)
{
  constexpr uint32_t USART_CR1_M0_POSITION = 12u;
  constexpr uint32_t USART_CR1_M1_POSITION = 28u;

  uint32_t registerValueCR1 = MemoryAccess::getRegisterValue(&(m_USARTPeripheralPtr->CR1));
 
  if (0b01 == (static_cast<uint32_t>(usartConfig.wordLength) & 0b01)) 
  {
    registerValueCR1 = MemoryUtility<uint32_t>::setBit(registerValueCR1, USART_CR1_M0_POSITION);
  }
  else
  {
    registerValueCR1 = MemoryUtility<uint32_t>::resetBit(registerValueCR1, USART_CR1_M0_POSITION);
  }

  if(0b10 == (static_cast<uint32_t>(usartConfig.wordLength) & 0b10))
  {
     registerValueCR1 = MemoryUtility<uint32_t>::setBit(registerValueCR1, USART_CR1_M1_POSITION);
  }
  else
  {
    registerValueCR1 = MemoryUtility<uint32_t>::resetBit(registerValueCR1, USART_CR1_M1_POSITION);
  }

  MemoryAccess::setRegisterValue(&(m_USARTPeripheralPtr->CR1), registerValueCR1);

  return ErrorCode::OK;
}


  