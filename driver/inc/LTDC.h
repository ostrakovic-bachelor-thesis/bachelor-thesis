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
    uint8_t alpha;
    uint8_t red;
    uint8_t green;
    uint8_t blue;
  };

  enum class ColorFormat : uint8_t
  {
    ARGB8888 = 0b000,
    RGB888   = 0b001,
    RGB565   = 0b010,
    ARGB1555 = 0b011,
    ARGB4444 = 0b100,
    L8       = 0b101,
    AL44     = 0b110,
    AL88     = 0b111
  };

  enum class BlendingFactor : uint8_t
  {
    CONST_ALPHA               = 0b100,
    PIXEL_ALPHA_X_CONST_ALPHA = 0b110
  };

  struct Position
  {
    uint16_t x;
    uint16_t y;
  };

  struct Dimension
  {
    uint16_t width;
    uint16_t height;
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

  struct LTDCLayerConfig
  {
    ColorFormat frameBufferColorFormat;
    uint8_t alpha;
    Color defaultColor;
    BlendingFactor currentLayerBlendingFactor;
    BlendingFactor subjacentLayerBlendingFactor;
    void *frameBufferPtr;
    Dimension frameBufferDimension;
  };

  ErrorCode init(const LTDCConfig &ltdcConfig, LTDCLayerConfig &ltdcLayer1Config);

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
  void setBackgroundColorBlueComponent(uint32_t &registerValueBCCR, uint8_t blue);
  void setBackgroundColorGreenComponent(uint32_t &registerValueBCCR, uint8_t green);
  void setBackgroundColorRedComponent(uint32_t &registerValueBCCR, uint8_t red);

  void enableLTDC(void);

  void setLayerWindowHorizontalPosition(
    LTDC_Layer_TypeDef *LTDCPeripheralLayerPtr,
    uint16_t accumulatedHorizontalBackPorch,
    uint16_t accumulatedActiveWidth);

  void setLayerWindowVerticalPosition(
    LTDC_Layer_TypeDef *LTDCPeripheralLayerPtr,
    uint16_t accumulatedVerticalBackPorch,
    uint16_t accumulatedActiveHeight);

  void setWindowHorizontalStartPosition(uint32_t &registerValueWHPCR, uint16_t windowHorizontalStartPosition);
  void setWindowHorizontalStopPosition(uint32_t &registerValueWHPCR, uint16_t windowHorizontalStopPosition);
  void setWindowVerticalStartPosition(uint32_t &registerValueWVPCR, uint16_t windowVerticalStartPosition);
  void setWindowVerticalStopPosition(uint32_t &registerValueWVPCR, uint16_t windowVerticalStopPosition);

  void setLayerFrameBufferColorFormat(LTDC_Layer_TypeDef *LTDCPeripheralLayerPtr, ColorFormat colorFormat);
  void setLayerConstantAlpha(LTDC_Layer_TypeDef *LTDCPeripheralLayerPtr, uint8_t alpha);

  void setLayerDefaultColor(LTDC_Layer_TypeDef *LTDCPeripheralLayerPtr, Color defaultColor);

  void setDefaultColorBlueComponent(uint32_t &registerValueDCCR, uint8_t blue);
  void setDefaultColorGreenComponent(uint32_t &registerValueDCCR, uint8_t green);
  void setDefaultColorRedComponent(uint32_t &registerValueDCCR, uint8_t red);
  void setDefaultColorAlphaComponent(uint32_t &registerValueDCCR, uint8_t alpha);

  void setLayerBlendingFactors(
    LTDC_Layer_TypeDef *LTDCPeripheralLayerPtr,
    BlendingFactor currentLayerBlendingFactor,
    BlendingFactor subjacentLayerBlendingFactor);

  void setCurrentLayerBlendingFactor(uint32_t &registerValueBFCR, BlendingFactor currentLayerBlendingFactor);
  void setSubjacentLayerBlendingFactor(uint32_t &registerValueBFCR, BlendingFactor subjacentLayerBlendingFactor);

  void setLayerFrameBufferAddress(LTDC_Layer_TypeDef *LTDCPeripheralLayerPtr, void *frameBufferPtr);

  void setLayerFrameBufferWidth(
    LTDC_Layer_TypeDef *LTDCPeripheralLayerPtr,
    uint16_t frameBufferWidth,
    ColorFormat frameBufferColorFormat);

  void setFrameBufferLineLength(
    uint32_t &registerValueCFBLR,
    uint16_t frameBufferWidth,
    ColorFormat frameBufferColorFormat);

  void setFrameBufferLinePitch(
    uint32_t &registerValueCFBLR,
    uint16_t frameBufferWidth,
    ColorFormat frameBufferColorFormat);

  void setLayerFrameBufferHeight(LTDC_Layer_TypeDef *LTDCPeripheralLayerPtr, uint16_t frameBufferHeight);

  void enableLayer(LTDC_Layer_TypeDef *LTDCPeripheralLayerPtr);
  void disableLayer(LTDC_Layer_TypeDef *LTDCPeripheralLayerPtr);

  static uint8_t getPixelSize(ColorFormat colorFormat);

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