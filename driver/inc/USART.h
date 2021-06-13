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

  enum class FrameFormat : uint8_t
  {
    BITS_6_WITH_PARITY = 0b110,
    BITS_7_WITH_PARITY = 0b100,
    BITS_8_WITH_PARITY = 0b101,
    BITS_7_WITHOUT_PARITY = 0b010,
    BITS_8_WITHOUT_PARITY = 0b000,
    BITS_9_WITHOUT_PARITY = 0b001
  };

  enum class Oversampling : uint8_t
  {
    OVERSAMPLING_8  = 0b1,
    OVERSAMPLING_16 = 0b0
  };

  enum class Parity : uint8_t
  {
    EVEN = 0b0,
    ODD  = 0b1
  };

  enum class StopBits : uint8_t
  {
    BIT_0_5 = 0b01,
    BIT_1_0 = 0b00,
    BIT_1_5 = 0b11,
    BIT_2_0 = 0b10,
  };

  struct USARTConfig
  {
    FrameFormat frameFormat;
    Oversampling oversampling;
    StopBits stopBits;
    Parity parity;
  };

  ErrorCode init(const USARTConfig &usartConfig);

private:

  void setFrameFormat(uint32_t &registerValueCR1, FrameFormat frameFormat);
  void setOversampling(uint32_t &registerValueCR1, Oversampling oversampling);
  void setParity(uint32_t &registerValueCR1, Parity parity);
  void setStopBits(uint32_t &registerValueCR2, StopBits stopBits);

  void disableUSART(void);
  void enableUSART(void);

  //! Pointer to USART peripheral
  USART_TypeDef *m_USARTPeripheralPtr;

};

#endif // #ifndef USART_H