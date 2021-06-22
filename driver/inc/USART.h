#ifndef USART_H
#define USART_H

#include "stm32l4r9xx.h"
#include "ClockControl.h"
#include <cstdint>


class USART
{
public:

  USART(USART_TypeDef *USARTPeripheralPtr, ClockControl *clockControlPtr);

  //! This enum class represents errors which can happen during method calls
  enum class ErrorCode : uint8_t
  {
    OK                               = 0u,
    USART_CONFIG_PARAM_INVALID_VALUE = 1u,
    BAUDRATE_SETUP_PROBLEM           = 2u,
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
    OVERSAMPLING_16 = 0b0,
    OVERSAMPLING_8  = 0b1
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

  enum class Baudrate : uint32_t
  {
    BAUDRATE_57600  = 57600u,
    BAUDRATE_115200 = 115200u,
    BAUDRATE_230400 = 230400u,
    BAUDRATE_460800 = 460800u,
    BAUDRATE_921600 = 921600u
  };

  struct USARTConfig
  {
    FrameFormat frameFormat;
    Oversampling oversampling;
    StopBits stopBits;
    Parity parity;
    Baudrate baudrate;
  };

  ErrorCode init(const USARTConfig &usartConfig);

private:
  
  static constexpr uint32_t BRR_INVALID_VALUE = 0u;

  void setFrameFormat(uint32_t &registerValueCR1, FrameFormat frameFormat);
  void setOversampling(uint32_t &registerValueCR1, Oversampling oversampling);
  void setParity(uint32_t &registerValueCR1, Parity parity);
  void setStopBits(uint32_t &registerValueCR2, StopBits stopBits);
  ErrorCode setBaudrate(Baudrate baudrateValue, Oversampling oversampling);
  
  static bool isUSARTConfigurationValid(const USARTConfig &usartConfig);
  static bool isFrameFormatInValidRangeOfValues(FrameFormat frameFormat);
  static bool isOversamplingInValidRangeOfValues(Oversampling oversampling);
  static bool isParityInValidRangeOfValues(Parity parity);
  static bool isStopBitsInValidRangeOfValues(StopBits stopBits);
  static bool isBaudrateInValidRangeOfValues(Baudrate baudrate);

  bool findBRRAndPRESCValue(
    uint32_t baudrate, 
    uint32_t inputClockFrequency, 
    Oversampling oversampling, 
    uint32_t &BRRValue, 
    uint32_t &PRESCValue);

  uint32_t findBRR(
    uint32_t baudrate, 
    uint32_t inputClockFrequency, 
    uint32_t prescaler, 
    Oversampling oversampling);

  static bool isValidUsartDiv(uint32_t value);
  static bool isBaudrateDeviationWithinTolerance(float baudrateDeviation);
  static uint32_t usartDivToBRR(uint32_t usartDiv, Oversampling oversamlping);

  void setBRRRegister(uint32_t BRRValue);
  void setPRESCRegister(uint32_t PRESCValue);

  void disableUSART(void);
  void enableUSART(void);

  static const uint32_t s_prescaler[];

  //! Pointer to USART peripheral
  USART_TypeDef *m_USARTPeripheralPtr;

  //! Pointer to Clock Control module
  ClockControl *m_clockControlPtr;

};

#endif // #ifndef USART_H