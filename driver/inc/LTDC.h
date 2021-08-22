#ifndef LTDC_H
#define LTDC_H

#include "stm32l4r9xx.h"
#include "Peripheral.h"
#include <cstdint>


class LTDC
{
public:

  LTDC(LTDC_TypeDef *LTDCPeripheralPtr,
    LTDC_Layer_TypeDef *LTDCPeripheralLayer1Ptr,
    LTDC_Layer_TypeDef *LTDCPeripheralLayer2Ptr);

  //! This enum class represents errors which can happen during method calls
  enum class ErrorCode : uint8_t
  {
    OK = 0u
  };

  struct LTDCConfig
  {
    uint16_t hsyncWidth;
    uint16_t vsyncWidth;
  };

  ErrorCode init(const LTDCConfig &ltdcConfig);

private:

  void setSynchronizationWidth(uint16_t hsyncWidth, uint16_t vsyncWidth);

  //! Pointer to USART peripheral
  LTDC_TypeDef *m_LTDCPeripheralPtr;

  //! Pointer to USART peripheral
  LTDC_Layer_TypeDef *m_LTDCPeripheralLayer1Ptr;

  //! Pointer to USART peripheral
  LTDC_Layer_TypeDef *m_LTDCPeripheralLayer2Ptr;
};

#endif // #ifndef LTDC_H