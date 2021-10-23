#include "LTDC.h"
#include "MemoryAccess.h"
#include "MemoryUtility.h"
#include "RegisterUtility.h"


LTDC::LTDC(LTDC_TypeDef *LTDCPeripheralPtr, ResetControl *resetControlPtr):
  m_LTDCPeripheralPtr(LTDCPeripheralPtr),
  m_LTDCPeripheralLayer1Ptr(reinterpret_cast<LTDC_Layer_TypeDef*>(reinterpret_cast<uintptr_t>(LTDCPeripheralPtr) + LAYER1_OFFSET)),
  m_LTDCPeripheralLayer2Ptr(reinterpret_cast<LTDC_Layer_TypeDef*>(reinterpret_cast<uintptr_t>(LTDCPeripheralPtr) + LAYER2_OFFSET)),
  m_resetControlPtr(resetControlPtr)
{}

LTDC::ErrorCode LTDC::init(const LTDCConfig &ltdcConfig, const LTDCLayerConfig &ltdcLayer1Config)
{
  ErrorCode errorCode = enablePeripheralClock();
  if (ErrorCode::OK != errorCode)
  {
    return errorCode;
  }

  configureLTDC(ltdcConfig);
  enableLTDC();

  configureLTDCLayer(m_LTDCPeripheralLayer1Ptr, ltdcConfig, ltdcLayer1Config);
  enableLayer(m_LTDCPeripheralLayer1Ptr);

  disableLayer(m_LTDCPeripheralLayer2Ptr);

  forceReloadOfShadowRegisters();

  return ErrorCode::OK;
}

void LTDC::configureLTDC(const LTDCConfig &ltdcConfig)
{
  const uint16_t accumulatedHorizontalBackPorch = ltdcConfig.horizontalBackPorch + ltdcConfig.hsyncWidth;
  const uint16_t accumulatedVerticalBackPorch   = ltdcConfig.verticalBackPorch   + ltdcConfig.vsyncWidth;
  const uint16_t accumulatedActiveWidth  = ltdcConfig.displayWidth  + accumulatedHorizontalBackPorch;
  const uint16_t accumulatedActiveHeight = ltdcConfig.displayHeight + accumulatedVerticalBackPorch;
  const uint16_t accumulatedTotalWidth   = ltdcConfig.horizontalFrontPorch + accumulatedActiveWidth;
  const uint16_t accumulatedTotalHeight  = ltdcConfig.verticalFrontPorch   + accumulatedActiveHeight;

  setSynchronizationWidths(ltdcConfig.hsyncWidth, ltdcConfig.vsyncWidth);
  setAccumulatedBackPorches(accumulatedHorizontalBackPorch, accumulatedVerticalBackPorch);
  setAccumulatedActiveWidthAndHeight(accumulatedActiveWidth, accumulatedActiveHeight);
  setAccumulatedTotalWidthAndHeight(accumulatedTotalWidth, accumulatedTotalHeight);

  uint32_t registerValueGCR = 0u;

  setHorizontalSyncPolarity(registerValueGCR, ltdcConfig.hsyncPolarity);
  setVerticalSyncPolarity(registerValueGCR, ltdcConfig.vsyncPolarity);
  setNotDataEnablePolarity(registerValueGCR, ltdcConfig.notDataEnablePolarity);
  setPixelClockPolarity(registerValueGCR, ltdcConfig.pixelClockPolarity);

  MemoryAccess::setRegisterValue(&(m_LTDCPeripheralPtr->GCR), registerValueGCR);

  setBackgroundColor(ltdcConfig.backgroundColor);
}

void LTDC::configureLTDCLayer(
  LTDC_Layer_TypeDef *LTDCPeripheralLayerPtr,
  const LTDCConfig &ltdcConfig,
  const LTDCLayerConfig &ltdcLayerConfig)
{
  const uint16_t accumulatedHorizontalBackPorch = ltdcConfig.horizontalBackPorch + ltdcConfig.hsyncWidth;
  const uint16_t accumulatedVerticalBackPorch   = ltdcConfig.verticalBackPorch   + ltdcConfig.vsyncWidth;
  const uint16_t accumulatedActiveWidth  = ltdcConfig.displayWidth  + accumulatedHorizontalBackPorch;
  const uint16_t accumulatedActiveHeight = ltdcConfig.displayHeight + accumulatedVerticalBackPorch;

  setLayerWindowHorizontalPosition(LTDCPeripheralLayerPtr, accumulatedHorizontalBackPorch, accumulatedActiveWidth);
  setLayerWindowVerticalPosition(LTDCPeripheralLayerPtr, accumulatedVerticalBackPorch, accumulatedActiveHeight);
  setLayerFrameBufferColorFormat(LTDCPeripheralLayerPtr, ltdcLayerConfig.frameBufferConfig.colorFormat);
  setLayerConstantAlpha(LTDCPeripheralLayerPtr, ltdcLayerConfig.alpha);
  setLayerDefaultColor(LTDCPeripheralLayerPtr, ltdcLayerConfig.defaultColor);
  setLayerBlendingFactors(LTDCPeripheralLayerPtr,
    ltdcLayerConfig.currentLayerBlendingFactor,
    ltdcLayerConfig.subjacentLayerBlendingFactor);
  setLayerFrameBufferAddress(LTDCPeripheralLayerPtr, ltdcLayerConfig.frameBufferConfig.bufferPtr);
  setLayerFrameBufferWidth(LTDCPeripheralLayerPtr,
    ltdcLayerConfig.frameBufferConfig.bufferDimension.width,
    ltdcLayerConfig.frameBufferConfig.colorFormat);
  setLayerFrameBufferHeight(LTDCPeripheralLayerPtr, ltdcLayerConfig.frameBufferConfig.bufferDimension.height);
}

void LTDC::setSynchronizationWidths(uint16_t hsyncWidth, uint16_t vsyncWidth)
{
  uint32_t registerValueSSCR = 0u;

  setHorizontalSynchronizationWidth(registerValueSSCR, hsyncWidth);
  setVerticalSynchronizationWidth(registerValueSSCR, vsyncWidth);

  MemoryAccess::setRegisterValue(&(m_LTDCPeripheralPtr->SSCR), registerValueSSCR);
}

void LTDC::setAccumulatedBackPorches(uint16_t accumulatedHorizontalBackPorch, uint16_t accumulatedVerticalBackPorch)
{
  uint32_t registerValueBPCR = 0u;

  setAccumulatedHorizontalBackPorch(registerValueBPCR, accumulatedHorizontalBackPorch);
  setAccumulatedVerticalBackPorch(registerValueBPCR, accumulatedVerticalBackPorch);

  MemoryAccess::setRegisterValue(&(m_LTDCPeripheralPtr->BPCR), registerValueBPCR);
}

void LTDC::setAccumulatedActiveWidthAndHeight(uint16_t accumulatedActiveWidth, uint16_t accumulatedActiveHeight)
{
  uint32_t registerValueAWCR = 0u;

  setAccumulatedActiveWidth(registerValueAWCR, accumulatedActiveWidth);
  setAccumulatedActiveHeight(registerValueAWCR, accumulatedActiveHeight);

  MemoryAccess::setRegisterValue(&(m_LTDCPeripheralPtr->AWCR), registerValueAWCR);
}

void LTDC::setAccumulatedTotalWidthAndHeight(uint16_t accumulatedTotalWidth, uint16_t accumulatedTotalHeight)
{
  uint32_t registerValueTWCR = 0u;

  setAccumulatedTotalWidth(registerValueTWCR, accumulatedTotalWidth);
  setAccumulatedTotalHeight(registerValueTWCR, accumulatedTotalHeight);

  MemoryAccess::setRegisterValue(&(m_LTDCPeripheralPtr->TWCR), registerValueTWCR);
}

void LTDC::setBackgroundColor(Color backgroundColor)
{
  uint32_t registerValueBCCR = 0u;

  setBackgroundColorBlueComponent(registerValueBCCR, backgroundColor.blue);
  setBackgroundColorGreenComponent(registerValueBCCR, backgroundColor.green);
  setBackgroundColorRedComponent(registerValueBCCR, backgroundColor.red);

  MemoryAccess::setRegisterValue(&(m_LTDCPeripheralPtr->BCCR), registerValueBCCR);
}

void LTDC::enableLTDC(void)
{
  constexpr uint32_t LTDC_GCR_LTDCEN_POSITION = 0u;

  RegisterUtility<uint32_t>::setBitInRegister(&(m_LTDCPeripheralPtr->GCR), LTDC_GCR_LTDCEN_POSITION);
}

void LTDC::setLayerWindowHorizontalPosition(
  LTDC_Layer_TypeDef *LTDCPeripheralLayerPtr,
  uint16_t accumulatedHorizontalBackPorch,
  uint16_t accumulatedActiveWidth)
{
  uint32_t registerValueWHPCR = 0u;

  setWindowHorizontalStartPosition(registerValueWHPCR, accumulatedHorizontalBackPorch);
  setWindowHorizontalStopPosition(registerValueWHPCR, accumulatedActiveWidth - 1u);

  MemoryAccess::setRegisterValue(&(LTDCPeripheralLayerPtr->WHPCR), registerValueWHPCR);
}

void LTDC::setLayerWindowVerticalPosition(
  LTDC_Layer_TypeDef *LTDCPeripheralLayerPtr,
  uint16_t accumulatedVerticalBackPorch,
  uint16_t accumulatedActiveHeight)
{
  uint32_t registerValueWVPCR = 0u;

  setWindowVerticalStartPosition(registerValueWVPCR, accumulatedVerticalBackPorch);
  setWindowVerticalStopPosition(registerValueWVPCR, accumulatedActiveHeight - 1u);

  MemoryAccess::setRegisterValue(&(LTDCPeripheralLayerPtr->WVPCR), registerValueWVPCR);
}

void LTDC::setLayerFrameBufferColorFormat(LTDC_Layer_TypeDef *LTDCPeripheralLayerPtr, ColorFormat colorFormat)
{
  constexpr uint32_t LTDC_LAYER_PFCR_PF_POSITION = 0u;
  constexpr uint32_t LTDC_LAYER_PFCR_PF_SIZE     = 3u;

  RegisterUtility<uint32_t>::setBitsInRegister(
    &(LTDCPeripheralLayerPtr->PFCR),
    LTDC_LAYER_PFCR_PF_POSITION,
    LTDC_LAYER_PFCR_PF_SIZE,
    static_cast<uint32_t>(colorFormat));
}

void LTDC::setLayerConstantAlpha(LTDC_Layer_TypeDef *LTDCPeripheralLayerPtr, uint8_t alpha)
{
  constexpr uint32_t LTDC_LAYER_CACR_CONSTA_POSITION = 0u;
  constexpr uint32_t LTDC_LAYER_CACR_CONSTA_SIZE     = 8u;

  RegisterUtility<uint32_t>::setBitsInRegister(
    &(LTDCPeripheralLayerPtr->CACR),
    LTDC_LAYER_CACR_CONSTA_POSITION,
    LTDC_LAYER_CACR_CONSTA_SIZE,
    static_cast<uint32_t>(alpha));
}

void LTDC::setLayerDefaultColor(LTDC_Layer_TypeDef *LTDCPeripheralLayerPtr, Color defaultColor)
{
  uint32_t registerValueDCCR = 0u;

  setDefaultColorBlueComponent(registerValueDCCR, defaultColor.blue);
  setDefaultColorGreenComponent(registerValueDCCR, defaultColor.green);
  setDefaultColorRedComponent(registerValueDCCR, defaultColor.red);
  setDefaultColorAlphaComponent(registerValueDCCR, defaultColor.alpha);

  MemoryAccess::setRegisterValue(&(LTDCPeripheralLayerPtr->DCCR), registerValueDCCR);
}

void LTDC::setLayerBlendingFactors(
  LTDC_Layer_TypeDef *LTDCPeripheralLayerPtr,
  BlendingFactor currentLayerBlendingFactor,
  BlendingFactor subjacentLayerBlendingFactor)
{
  uint32_t registerValueBFCR = 0u;

  setCurrentLayerBlendingFactor(registerValueBFCR, currentLayerBlendingFactor);
  setSubjacentLayerBlendingFactor(registerValueBFCR, subjacentLayerBlendingFactor);

  MemoryAccess::setRegisterValue(&(LTDCPeripheralLayerPtr->BFCR), registerValueBFCR);
}

inline void LTDC::setLayerFrameBufferAddress(LTDC_Layer_TypeDef *LTDCPeripheralLayerPtr, void *frameBufferPtr)
{
  MemoryAccess::setRegisterValue(&(LTDCPeripheralLayerPtr->CFBAR),
    static_cast<uint32_t>(reinterpret_cast<uintptr_t>(frameBufferPtr)));
}

void LTDC::setLayerFrameBufferWidth(
  LTDC_Layer_TypeDef *LTDCPeripheralLayerPtr,
  uint16_t frameBufferWidth,
  ColorFormat frameBufferColorFormat)
{
  uint32_t registerValueCFBLR = 0u;

  setFrameBufferLineLength(registerValueCFBLR, frameBufferWidth, frameBufferColorFormat);
  setFrameBufferLinePitch(registerValueCFBLR, frameBufferWidth, frameBufferColorFormat);

  MemoryAccess::setRegisterValue(&(LTDCPeripheralLayerPtr->CFBLR), registerValueCFBLR);
}

inline void LTDC::setLayerFrameBufferHeight(LTDC_Layer_TypeDef *LTDCPeripheralLayerPtr, uint16_t frameBufferHeight)
{
  constexpr uint32_t LTDC_CFBLNR_CFBLNBR_POSITION = 0u;
  constexpr uint32_t LTDC_CFBLNR_CFBLNBR_SIZE     = 11u;

  RegisterUtility<uint32_t>::setBitsInRegister(
    &(LTDCPeripheralLayerPtr->CFBLNR),
    LTDC_CFBLNR_CFBLNBR_POSITION,
    LTDC_CFBLNR_CFBLNBR_SIZE,
    static_cast<uint32_t>(frameBufferHeight));
}

inline void LTDC::enableLayer(LTDC_Layer_TypeDef *LTDCPeripheralLayerPtr)
{
  constexpr uint32_t LTDC_CR_LEN_POSITION = 0u;

  RegisterUtility<uint32_t>::setBitInRegister(&(LTDCPeripheralLayerPtr->CR), LTDC_CR_LEN_POSITION);
}

inline void LTDC::disableLayer(LTDC_Layer_TypeDef *LTDCPeripheralLayerPtr)
{
  constexpr uint32_t LTDC_CR_LEN_POSITION = 0u;

  RegisterUtility<uint32_t>::resetBitInRegister(&(LTDCPeripheralLayerPtr->CR), LTDC_CR_LEN_POSITION);
}

inline void LTDC::forceReloadOfShadowRegisters(void)
{
  constexpr uint32_t LTDC_SRCR_IMR_POSITION = 0u;

  RegisterUtility<uint32_t>::resetBitInRegister(&(m_LTDCPeripheralPtr->SRCR), LTDC_SRCR_IMR_POSITION);
}

inline void LTDC::setHorizontalSynchronizationWidth(uint32_t &registerValueSSCR, uint16_t hsyncWidth)
{
  constexpr uint32_t LTDC_SSCR_HSW_POSITION = 16u;
  constexpr uint32_t LTDC_SSCR_HSW_SIZE     = 12u;

  registerValueSSCR = MemoryUtility<uint32_t>::setBits(
    registerValueSSCR,
    LTDC_SSCR_HSW_POSITION,
    LTDC_SSCR_HSW_SIZE,
    static_cast<uint32_t>(hsyncWidth - 1u));
}

inline void LTDC::setVerticalSynchronizationWidth(uint32_t &registerValueSSCR, uint16_t vsyncWidth)
{
  constexpr uint32_t LTDC_SSCR_VSH_POSITION = 0u;
  constexpr uint32_t LTDC_SSCR_VSH_SIZE     = 11u;

  registerValueSSCR = MemoryUtility<uint32_t>::setBits(
    registerValueSSCR,
    LTDC_SSCR_VSH_POSITION,
    LTDC_SSCR_VSH_SIZE,
    static_cast<uint32_t>(vsyncWidth - 1u));
}

inline void
LTDC::setAccumulatedHorizontalBackPorch(uint32_t &registerValueBPCR, uint16_t accumulatedHorizontalBackPorch)
{
  constexpr uint32_t LTDC_BPCR_AHBP_POSITION = 16u;
  constexpr uint32_t LTDC_BPCR_AHBP_SIZE     = 12u;

  registerValueBPCR = MemoryUtility<uint32_t>::setBits(
    registerValueBPCR,
    LTDC_BPCR_AHBP_POSITION,
    LTDC_BPCR_AHBP_SIZE,
    static_cast<uint32_t>(accumulatedHorizontalBackPorch - 1u));
}

inline void
LTDC::setAccumulatedVerticalBackPorch(uint32_t &registerValueBPCR, uint16_t accumulatedVerticalBackPorch)
{
  constexpr uint32_t LTDC_BPCR_AVBP_POSITION = 0u;
  constexpr uint32_t LTDC_BPCR_AVBP_SIZE     = 11u;

  registerValueBPCR = MemoryUtility<uint32_t>::setBits(
    registerValueBPCR,
    LTDC_BPCR_AVBP_POSITION,
    LTDC_BPCR_AVBP_SIZE,
    static_cast<uint32_t>(accumulatedVerticalBackPorch - 1u));
}

inline void LTDC::setAccumulatedActiveWidth(uint32_t &registerValueAWCR, uint16_t accumulatedActiveWidth)
{
  constexpr uint32_t LTDC_AWCR_AAW_POSITION = 16u;
  constexpr uint32_t LTDC_AWCR_AAW_SIZE     = 12u;

  registerValueAWCR = MemoryUtility<uint32_t>::setBits(
    registerValueAWCR,
    LTDC_AWCR_AAW_POSITION,
    LTDC_AWCR_AAW_SIZE,
    static_cast<uint32_t>(accumulatedActiveWidth - 1u));
}

inline void LTDC::setAccumulatedActiveHeight(uint32_t &registerValueAWCR, uint16_t accumulatedActiveHeight)
{
  constexpr uint32_t LTDC_AWCR_AAH_POSITION = 0u;
  constexpr uint32_t LTDC_AWCR_AAH_SIZE     = 11u;

  registerValueAWCR = MemoryUtility<uint32_t>::setBits(
    registerValueAWCR,
    LTDC_AWCR_AAH_POSITION,
    LTDC_AWCR_AAH_SIZE,
    static_cast<uint32_t>(accumulatedActiveHeight - 1u));
}

inline void LTDC::setAccumulatedTotalWidth(uint32_t &registerValueTWCR, uint16_t accumulatedTotalWidth)
{
  constexpr uint32_t LTDC_TWCR_TOTALW_POSITION = 16u;
  constexpr uint32_t LTDC_TWCR_TOTALW_SIZE     = 12u;

  registerValueTWCR = MemoryUtility<uint32_t>::setBits(
    registerValueTWCR,
    LTDC_TWCR_TOTALW_POSITION,
    LTDC_TWCR_TOTALW_SIZE,
    static_cast<uint32_t>(accumulatedTotalWidth - 1u));
}

inline void LTDC::setAccumulatedTotalHeight(uint32_t &registerValueTWCR, uint16_t accumulatedTotalHeight)
{
  constexpr uint32_t LTDC_TWCR_TOTALH_POSITION = 0u;
  constexpr uint32_t LTDC_TWCR_TOTALH_SIZE     = 11u;

  registerValueTWCR = MemoryUtility<uint32_t>::setBits(
    registerValueTWCR,
    LTDC_TWCR_TOTALH_POSITION,
    LTDC_TWCR_TOTALH_SIZE,
    static_cast<uint32_t>(accumulatedTotalHeight - 1u));
}

inline void LTDC::setHorizontalSyncPolarity(uint32_t &registerValueGCR, Polarity hsyncPolarity)
{
  constexpr uint32_t LTDC_GCR_HSPOL_POSITION = 31u;
  constexpr uint32_t LTDC_GCR_HSPOL_SIZE     = 1u;

  registerValueGCR = MemoryUtility<uint32_t>::setBits(
    registerValueGCR,
    LTDC_GCR_HSPOL_POSITION,
    LTDC_GCR_HSPOL_SIZE,
    static_cast<uint32_t>(hsyncPolarity));
}

inline void LTDC::setVerticalSyncPolarity(uint32_t &registerValueGCR, Polarity vsyncPolarity)
{
  constexpr uint32_t LTDC_GCR_VSPOL_POSITION = 30u;
  constexpr uint32_t LTDC_GCR_VSPOL_SIZE     = 1u;

  registerValueGCR = MemoryUtility<uint32_t>::setBits(
    registerValueGCR,
    LTDC_GCR_VSPOL_POSITION,
    LTDC_GCR_VSPOL_SIZE,
    static_cast<uint32_t>(vsyncPolarity));
}

inline void LTDC::setNotDataEnablePolarity(uint32_t &registerValueGCR, Polarity notDataEnablePolarity)
{
  constexpr uint32_t LTDC_GCR_DEPOL_POSITION = 29u;
  constexpr uint32_t LTDC_GCR_DEPOL_SIZE     = 1u;

  registerValueGCR = MemoryUtility<uint32_t>::setBits(
    registerValueGCR,
    LTDC_GCR_DEPOL_POSITION,
    LTDC_GCR_DEPOL_SIZE,
    static_cast<uint32_t>(notDataEnablePolarity));
}

inline void LTDC::setPixelClockPolarity(uint32_t &registerValueGCR, Polarity pixelClockPolarity)
{
  constexpr uint32_t LTDC_GCR_PCPOL_POSITION = 28u;
  constexpr uint32_t LTDC_GCR_PCPOL_SIZE     = 1u;

  registerValueGCR = MemoryUtility<uint32_t>::setBits(
    registerValueGCR,
    LTDC_GCR_PCPOL_POSITION,
    LTDC_GCR_PCPOL_SIZE,
    static_cast<uint32_t>(pixelClockPolarity));
}

inline void LTDC::setBackgroundColorBlueComponent(uint32_t &registerValueBCCR, uint8_t blue)
{
  constexpr uint32_t LTDC_BCCR_BCBLUE_POSITION = 0u;
  constexpr uint32_t LTDC_BCCR_BCBLUE_SIZE     = 8u;

  registerValueBCCR = MemoryUtility<uint32_t>::setBits(
    registerValueBCCR,
    LTDC_BCCR_BCBLUE_POSITION,
    LTDC_BCCR_BCBLUE_SIZE,
    static_cast<uint32_t>(blue));
}

inline void LTDC::setBackgroundColorGreenComponent(uint32_t &registerValueBCCR, uint8_t green)
{
  constexpr uint32_t LTDC_BCCR_BCGREEN_POSITION = 8u;
  constexpr uint32_t LTDC_BCCR_BCGREEN_SIZE     = 8u;

  registerValueBCCR = MemoryUtility<uint32_t>::setBits(
    registerValueBCCR,
    LTDC_BCCR_BCGREEN_POSITION,
    LTDC_BCCR_BCGREEN_SIZE,
    static_cast<uint32_t>(green));
}

inline void LTDC::setBackgroundColorRedComponent(uint32_t &registerValueBCCR, uint8_t red)
{
  constexpr uint32_t LTDC_BCCR_BCRED_POSITION = 16u;
  constexpr uint32_t LTDC_BCCR_BCRED_SIZE     = 8u;

  registerValueBCCR = MemoryUtility<uint32_t>::setBits(
    registerValueBCCR,
    LTDC_BCCR_BCRED_POSITION,
    LTDC_BCCR_BCRED_SIZE,
    static_cast<uint32_t>(red));
}

inline void
LTDC::setWindowHorizontalStartPosition(uint32_t &registerValueWHPCR, uint16_t windowHorizontalStartPosition)
{
  constexpr uint32_t LTDC_LAYER_WHPCR_WHSTPOS_POSITION = 0u;
  constexpr uint32_t LTDC_LAYER_WHPCR_WHSTPOS_SIZE     = 12u;

  registerValueWHPCR = MemoryUtility<uint32_t>::setBits(
    registerValueWHPCR,
    LTDC_LAYER_WHPCR_WHSTPOS_POSITION,
    LTDC_LAYER_WHPCR_WHSTPOS_SIZE,
    static_cast<uint32_t>(windowHorizontalStartPosition));
}

inline void
LTDC::setWindowHorizontalStopPosition(uint32_t &registerValueWHPCR, uint16_t windowHorizontalStopPosition)
{
  constexpr uint32_t LTDC_LAYER_WHPCR_WHSPPOS_POSITION = 16u;
  constexpr uint32_t LTDC_LAYER_WHPCR_WHSPPOS_SIZE     = 12u;

  registerValueWHPCR = MemoryUtility<uint32_t>::setBits(
    registerValueWHPCR,
    LTDC_LAYER_WHPCR_WHSPPOS_POSITION,
    LTDC_LAYER_WHPCR_WHSPPOS_SIZE,
    static_cast<uint32_t>(windowHorizontalStopPosition));
}

inline void
LTDC::setWindowVerticalStartPosition(uint32_t &registerValueWVPCR, uint16_t windowVerticalStartPosition)
{
  constexpr uint32_t LTDC_LAYER_WVPCR_WVSTPOS_POSITION = 0u;
  constexpr uint32_t LTDC_LAYER_WVPCR_WVSTPOS_SIZE     = 11u;

  registerValueWVPCR = MemoryUtility<uint32_t>::setBits(
    registerValueWVPCR,
    LTDC_LAYER_WVPCR_WVSTPOS_POSITION,
    LTDC_LAYER_WVPCR_WVSTPOS_SIZE,
    static_cast<uint32_t>(windowVerticalStartPosition));
}

inline void
LTDC::setWindowVerticalStopPosition(uint32_t &registerValueWVPCR, uint16_t windowVerticalStopPosition)
{
  constexpr uint32_t LTDC_LAYER_WVPCR_WVSPPOS_POSITION = 16u;
  constexpr uint32_t LTDC_LAYER_WVPCR_WVSPPOS_SIZE     = 11u;

  registerValueWVPCR = MemoryUtility<uint32_t>::setBits(
    registerValueWVPCR,
    LTDC_LAYER_WVPCR_WVSPPOS_POSITION,
    LTDC_LAYER_WVPCR_WVSPPOS_SIZE,
    static_cast<uint32_t>(windowVerticalStopPosition));
}

inline void LTDC::setDefaultColorBlueComponent(uint32_t &registerValueDCCR, uint8_t blue)
{
  constexpr uint32_t LTDC_DCCR_DCBLUE_POSITION = 0u;
  constexpr uint32_t LTDC_DCCR_DCBLUE_SIZE     = 8u;

  registerValueDCCR = MemoryUtility<uint32_t>::setBits(
    registerValueDCCR,
    LTDC_DCCR_DCBLUE_POSITION,
    LTDC_DCCR_DCBLUE_SIZE,
    static_cast<uint32_t>(blue));
}

inline void LTDC::setDefaultColorGreenComponent(uint32_t &registerValueDCCR, uint8_t green)
{
  constexpr uint32_t LTDC_DCCR_DCGREEN_POSITION = 8u;
  constexpr uint32_t LTDC_DCCR_DCGREEN_SIZE     = 8u;

  registerValueDCCR = MemoryUtility<uint32_t>::setBits(
    registerValueDCCR,
    LTDC_DCCR_DCGREEN_POSITION,
    LTDC_DCCR_DCGREEN_SIZE,
    static_cast<uint32_t>(green));
}

inline void LTDC::setDefaultColorRedComponent(uint32_t &registerValueDCCR, uint8_t red)
{
  constexpr uint32_t LTDC_DCCR_DCRED_POSITION = 16u;
  constexpr uint32_t LTDC_DCCR_DCRED_SIZE     = 8u;

  registerValueDCCR = MemoryUtility<uint32_t>::setBits(
    registerValueDCCR,
    LTDC_DCCR_DCRED_POSITION,
    LTDC_DCCR_DCRED_SIZE,
    static_cast<uint32_t>(red));
}

inline void LTDC::setDefaultColorAlphaComponent(uint32_t &registerValueDCCR, uint8_t alpha)
{
  constexpr uint32_t LTDC_DCCR_DCALPHA_POSITION = 24u;
  constexpr uint32_t LTDC_DCCR_DCALPHA_SIZE     = 8u;

  registerValueDCCR = MemoryUtility<uint32_t>::setBits(
    registerValueDCCR,
    LTDC_DCCR_DCALPHA_POSITION,
    LTDC_DCCR_DCALPHA_SIZE,
    static_cast<uint32_t>(alpha));
}

inline void
LTDC::setCurrentLayerBlendingFactor(uint32_t &registerValueBFCR, BlendingFactor currentLayerBlendingFactor)
{
  constexpr uint32_t LTDC_BFCR_BF1_POSITION = 8u;
  constexpr uint32_t LTDC_BFCR_BF1_SIZE     = 3u;

  registerValueBFCR = MemoryUtility<uint32_t>::setBits(
    registerValueBFCR,
    LTDC_BFCR_BF1_POSITION,
    LTDC_BFCR_BF1_SIZE,
    static_cast<uint32_t>(currentLayerBlendingFactor));
}

inline void
LTDC::setSubjacentLayerBlendingFactor(uint32_t &registerValueBFCR, BlendingFactor subjacentLayerBlendingFactor)
{
  constexpr uint32_t LTDC_BFCR_BF2_POSITION = 0u;
  constexpr uint32_t LTDC_BFCR_BF2_SIZE     = 3u;

  registerValueBFCR = MemoryUtility<uint32_t>::setBits(
    registerValueBFCR,
    LTDC_BFCR_BF2_POSITION,
    LTDC_BFCR_BF2_SIZE,
    static_cast<uint32_t>(subjacentLayerBlendingFactor));
}

void LTDC::setFrameBufferLineLength(
  uint32_t &registerValueCFBLR,
  uint16_t frameBufferWidth,
  ColorFormat frameBufferColorFormat)
{
  constexpr uint32_t LTDC_CFBLR_CFBLL_POSITION = 0u;
  constexpr uint32_t LTDC_CFBLR_CFBLL_SIZE     = 13u;

  registerValueCFBLR = MemoryUtility<uint32_t>::setBits(
    registerValueCFBLR,
    LTDC_CFBLR_CFBLL_POSITION,
    LTDC_CFBLR_CFBLL_SIZE,
    static_cast<uint32_t>(frameBufferWidth * getPixelSize(frameBufferColorFormat) + 3u));
}

void LTDC::setFrameBufferLinePitch(
  uint32_t &registerValueCFBLR,
  uint16_t frameBufferWidth,
  ColorFormat frameBufferColorFormat)
{
  constexpr uint32_t LTDC_CFBLR_CFBP_POSITION = 16u;
  constexpr uint32_t LTDC_CFBLR_CFBP_SIZE     = 13u;

  registerValueCFBLR = MemoryUtility<uint32_t>::setBits(
    registerValueCFBLR,
    LTDC_CFBLR_CFBP_POSITION ,
    LTDC_CFBLR_CFBP_SIZE,
    static_cast<uint32_t>(frameBufferWidth * getPixelSize(frameBufferColorFormat)));
}

inline LTDC::ErrorCode LTDC::enablePeripheralClock(void)
{
  ResetControl::ErrorCode errorCode = m_resetControlPtr->enablePeripheralClock(getPeripheralTag());

  return (ResetControl::ErrorCode::OK == errorCode) ? ErrorCode::OK : ErrorCode::CAN_NOT_TURN_ON_PERIPHERAL_CLOCK;
}

uint8_t LTDC::getPixelSize(ColorFormat colorFormat)
{
  uint8_t pixelSize;

  switch (colorFormat)
  {
    case ColorFormat::ARGB8888:
    {
      pixelSize = 4u;
    }
    break;

    case ColorFormat::RGB888:
    {
      pixelSize = 3u;
    }
    break;

    case ColorFormat::RGB565:
    case ColorFormat::ARGB1555:
    case ColorFormat::ARGB4444:
    case ColorFormat::AL88:
    {
      pixelSize = 2u;
    }
    break;

    case ColorFormat::L8:
    case ColorFormat::AL44:
    {
      pixelSize = 1u;
    }
    break;

    default:
    {
      pixelSize = 0u;
    }
    break;
  }

  return pixelSize;
}