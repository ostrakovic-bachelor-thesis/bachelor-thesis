#ifndef LTDC_H
#define LTDC_H

#include "stm32l4r9xx.h"
#include "ResetControl.h"
#include "Peripheral.h"
#include <cstdint>


class LTDC
{
public:

  LTDC(LTDC_TypeDef *LTDCPeripheralPtr,
    LTDC_Layer_TypeDef *LTDCPeripheralLayer1Ptr,
    LTDC_Layer_TypeDef *LTDCPeripheralLayer2Ptr,
    ResetControl *resetControlPtr);

  //! This enum class represents errors which can happen during method calls
  enum class ErrorCode : uint8_t
  {
    OK                               = 0u,
    CAN_NOT_TURN_ON_PERIPHERAL_CLOCK = 1u
  };

  //! TODO
  enum class Polarity : uint8_t
  {
    ACTIVE_LOW  = 0u,
    ACTIVE_HIGH = 1u
  };

  struct Color
  {
    uint8_t red;
    uint8_t green;
    uint8_t blue;
  };

  struct LTDCConfig
  {
    uint16_t hsyncWidth;
    uint16_t vsyncWidth;
    uint16_t horizontalBackPorch;
    uint16_t verticalBackPorch;
    uint16_t displayWidth;
    uint16_t displayHeight;
    uint16_t horizontalFrontPorch;
    uint16_t verticalFrontPorch;
    Polarity hsyncPolarity;
    Polarity vsyncPolarity;
    Polarity notDataEnablePolarity;
    Polarity pixelClockPolarity;
    Color    backgroundColor;
  };

  ErrorCode init(const LTDCConfig &ltdcConfig);

  inline Peripheral getPeripheralTag(void) const
  {
    return static_cast<Peripheral>(reinterpret_cast<uintptr_t>(const_cast<LTDC_TypeDef*>(m_LTDCPeripheralPtr)));
  }

private:

  ErrorCode enablePeripheralClock(void);

  void setSynchronizationWidths(uint16_t hsyncWidth, uint16_t vsyncWidth);
  void setHorizontalSynchronizationWidth(uint32_t &registerValueSSCR, uint16_t hsyncWidth);
  void setVerticalSynchronizationWidth(uint32_t &registerValueSSCR, uint16_t vsyncWidth);
  void setAccumulatedBackPorches(uint16_t accumulatedHorizontalBackPorch, uint16_t accumulatedVerticalBackPorch);
  void setAccumulatedHorizontalBackPorch(uint32_t &registerValueBPCR, uint16_t accumulatedHorizontalBackPorch);
  void setAccumulatedVerticalBackPorch(uint32_t &registerValueBPCR, uint16_t accumulatedVerticalBackPorch);
  void setAccumulatedActiveWidthAndHeight(uint16_t accumulatedActiveWidth, uint16_t accumulatedActiveHeight);
  void setAccumulatedActiveWidth(uint32_t &registerValueAWCR, uint16_t accumulatedActiveWidth);
  void setAccumulatedActiveHeight(uint32_t &registerValueAWCR, uint16_t accumulatedActiveHeight);
  void setAccumulatedTotalWidthAndHeight(uint16_t accumulatedTotalWidth, uint16_t accumulatedTotalHeight);
  void setAccumulatedTotalWidth(uint32_t &registerValueTWCR, uint16_t accumulatedTotalWidth);
  void setAccumulatedTotalHeight(uint32_t &registerValueTWCR, uint16_t accumulatedTotalHeight);

  void setHorizontalSyncPolarity(uint32_t &registerValueGCR, Polarity hsyncPolarity);
  void setVerticalSyncPolarity(uint32_t &registerValueGCR, Polarity vsyncPolarity);
  void setNotDataEnablePolarity(uint32_t &registerValueGCR, Polarity notDataEnablePolarity);
  void setPixelClockPolarity(uint32_t &registerValueGCR, Polarity pixelClockPolarity);

  void setBackgroundColor(Color backgroundColor);
  void setBackgroundColorBlueComponenet(uint32_t &registerValueBCCR, uint8_t blue);
  void setBackgroundColorGreenComponenet(uint32_t &registerValueBCCR, uint8_t green);
  void setBackgroundColorRedComponenet(uint32_t &registerValueBCCR, uint8_t red);

  //! Pointer to USART peripheral
  LTDC_TypeDef *m_LTDCPeripheralPtr;

  //! Pointer to USART peripheral
  LTDC_Layer_TypeDef *m_LTDCPeripheralLayer1Ptr;

  //! Pointer to USART peripheral
  LTDC_Layer_TypeDef *m_LTDCPeripheralLayer2Ptr;

  //! Pointer to Reset Control module
  ResetControl *m_resetControlPtr;
};

#endif // #ifndef LTDC_H