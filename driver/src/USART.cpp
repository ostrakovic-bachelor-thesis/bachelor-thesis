#include "USART.h"
#include "MemoryAccess.h"
#include "MemoryUtility.h"
#include "RegisterUtility.h"


USART::USART(USART_TypeDef *USARTPeripheralPtr):
  m_USARTPeripheralPtr(USARTPeripheralPtr)
{}

USART::ErrorCode USART::init(const USARTConfig &usartConfig)
{
  disableUSART();

  uint32_t registerValueCR1 = MemoryAccess::getRegisterValue(&(m_USARTPeripheralPtr->CR1));
  uint32_t registerValueCR2 = MemoryAccess::getRegisterValue(&(m_USARTPeripheralPtr->CR2));
 
  setFrameFormat(registerValueCR1, usartConfig.frameFormat);
  setOversampling(registerValueCR1, usartConfig.oversampling);
  setParity(registerValueCR1, usartConfig.parity);

  setStopBits(registerValueCR2, usartConfig.stopBits);

  MemoryAccess::setRegisterValue(&(m_USARTPeripheralPtr->CR1), registerValueCR1);
  MemoryAccess::setRegisterValue(&(m_USARTPeripheralPtr->CR2), registerValueCR2);

  enableUSART();

  return ErrorCode::OK;
}

void USART::setFrameFormat(uint32_t &registerValueCR1, FrameFormat frameFormat)
{
  constexpr uint32_t USART_CR1_PCE_POSITION = 10u;
  constexpr uint32_t USART_CR1_M0_POSITION  = 12u;
  constexpr uint32_t USART_CR1_M1_POSITION  = 28u;

  if (MemoryUtility<uint8_t>::isBitSet(static_cast<uint8_t>(frameFormat), 2u))
  {
    registerValueCR1 = MemoryUtility<uint32_t>::setBit(registerValueCR1, USART_CR1_PCE_POSITION);
  }
  else
  {
    registerValueCR1 = MemoryUtility<uint32_t>::resetBit(registerValueCR1, USART_CR1_PCE_POSITION);
  }
  
  if (MemoryUtility<uint8_t>::isBitSet(static_cast<uint8_t>(frameFormat), 0u))
  {
    registerValueCR1 = MemoryUtility<uint32_t>::setBit(registerValueCR1, USART_CR1_M0_POSITION);
  }
  else
  {
    registerValueCR1 = MemoryUtility<uint32_t>::resetBit(registerValueCR1, USART_CR1_M0_POSITION);
  }

  if (MemoryUtility<uint8_t>::isBitSet(static_cast<uint8_t>(frameFormat), 1u))
  {
    registerValueCR1 = MemoryUtility<uint32_t>::setBit(registerValueCR1, USART_CR1_M1_POSITION);
  }
  else
  {
    registerValueCR1 = MemoryUtility<uint32_t>::resetBit(registerValueCR1, USART_CR1_M1_POSITION);
  }
}

void USART::setOversampling(uint32_t &registerValueCR1, Oversampling oversampling)
{
  constexpr uint8_t OVERSAMPLING_NUM_OF_BITS = 1u;
  constexpr uint8_t USART_CR1_OVER8_POSITION = 15u;

  registerValueCR1 = MemoryUtility<uint32_t>::setBits(
    registerValueCR1, 
    USART_CR1_OVER8_POSITION,
    OVERSAMPLING_NUM_OF_BITS,
    static_cast<uint32_t>(oversampling));
}

void USART::setParity(uint32_t &registerValueCR1, Parity parity)
{
  constexpr uint8_t PARITY_NUM_OF_BITS = 1u;
  constexpr uint8_t USART_CR1_PS_POSITION = 9u;

  registerValueCR1 = MemoryUtility<uint32_t>::setBits(
    registerValueCR1, 
    USART_CR1_PS_POSITION,
    PARITY_NUM_OF_BITS,
    static_cast<uint32_t>(parity));
}

void USART::setStopBits(uint32_t &registerValueCR2, StopBits stopBits)
{
  constexpr uint8_t STOP_BITS_NUM_OF_BITS = 2u;
  constexpr uint8_t USART_CR2_STOP_POSITION = 12u;

  registerValueCR2 = MemoryUtility<uint32_t>::setBits(
    registerValueCR2, 
    USART_CR2_STOP_POSITION,
    STOP_BITS_NUM_OF_BITS,
    static_cast<uint32_t>(stopBits));
}

void USART::disableUSART(void)
{
  constexpr uint8_t USART_CR1_UE_POSITION = 0u;
  RegisterUtility<uint32_t>::resetBitInRegister(&(m_USARTPeripheralPtr->CR1), USART_CR1_UE_POSITION);
}

void USART::enableUSART(void)
{
  constexpr uint8_t USART_CR1_UE_POSITION = 0u;
  RegisterUtility<uint32_t>::setBitInRegister(&(m_USARTPeripheralPtr->CR1), USART_CR1_UE_POSITION);
}