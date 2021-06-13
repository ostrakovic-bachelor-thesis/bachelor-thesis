#ifndef USART_H
#define USART_H

#include "stm32l4r9xx.h"
#include <cstdint>


class USART
{
public:

  USART(USART_TypeDef *USARTPeripheralPtr);

  //! This enum class represents errors which can happen during method calls
  enum class ErrorCode : uint8_t
  {
    OK = 0u,
  };

  enum class WordLength : uint8_t
  {
    BITS_7 = 0b10,
    BITS_8 = 0b00,
    BITS_9 = 0b01
  };

  struct USARTConfig
  {
    WordLength wordLength;
  };

  ErrorCode init(const USARTConfig &usartConfig);

private:

  //! Pointer to USART peripheral
  USART_TypeDef *m_USARTPeripheralPtr;

};

#endif // #ifndef USART_H