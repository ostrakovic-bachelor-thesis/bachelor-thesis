#include "LTDC.h"
#include "MemoryAccess.h"
#include "MemoryUtility.h"
#include "RegisterUtility.h"


LTDC::LTDC(LTDC_TypeDef *LTDCPeripheralPtr,
    LTDC_Layer_TypeDef *LTDCPeripheralLayer1Ptr,
    LTDC_Layer_TypeDef *LTDCPeripheralLayer2Ptr,
    ResetControl *resetControlPtr):
  m_LTDCPeripheralPtr(LTDCPeripheralPtr),
  m_LTDCPeripheralLayer1Ptr(LTDCPeripheralLayer1Ptr),
  m_LTDCPeripheralLayer2Ptr(LTDCPeripheralLayer2Ptr),
  m_resetControlPtr(resetControlPtr)
{}

LTDC::ErrorCode LTDC::init(const LTDCConfig &ltdcConfig)
{
  ErrorCode errorCode = enablePeripheralClock();
  if (ErrorCode::OK != errorCode)
  {
    return errorCode;
  }

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

  return ErrorCode::OK;
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

  setBackgroundColorBlueComponenet(registerValueBCCR, backgroundColor.blue);
  setBackgroundColorGreenComponenet(registerValueBCCR, backgroundColor.green);
  setBackgroundColorRedComponenet(registerValueBCCR, backgroundColor.red);

  MemoryAccess::setRegisterValue(&(m_LTDCPeripheralPtr->BCCR), registerValueBCCR);
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

inline void LTDC::setBackgroundColorBlueComponenet(uint32_t &registerValueBCCR, uint8_t blue)
{
  constexpr uint32_t LTDC_BCCR_BCBLUE_POSITION = 0u;
  constexpr uint32_t LTDC_BCCR_BCBLUE_SIZE     = 8u;

  registerValueBCCR = MemoryUtility<uint32_t>::setBits(
    registerValueBCCR,
    LTDC_BCCR_BCBLUE_POSITION,
    LTDC_BCCR_BCBLUE_SIZE,
    static_cast<uint32_t>(blue));
}

inline void LTDC::setBackgroundColorGreenComponenet(uint32_t &registerValueBCCR, uint8_t green)
{
  constexpr uint32_t LTDC_BCCR_BCGREEN_POSITION = 8u;
  constexpr uint32_t LTDC_BCCR_BCGREEN_SIZE     = 8u;

  registerValueBCCR = MemoryUtility<uint32_t>::setBits(
    registerValueBCCR,
    LTDC_BCCR_BCGREEN_POSITION,
    LTDC_BCCR_BCGREEN_SIZE,
    static_cast<uint32_t>(green));
}

inline void LTDC::setBackgroundColorRedComponenet(uint32_t &registerValueBCCR, uint8_t red)
{
  constexpr uint32_t LTDC_BCCR_BCRED_POSITION = 16u;
  constexpr uint32_t LTDC_BCCR_BCRED_SIZE     = 8u;

  registerValueBCCR = MemoryUtility<uint32_t>::setBits(
    registerValueBCCR,
    LTDC_BCCR_BCRED_POSITION,
    LTDC_BCCR_BCRED_SIZE,
    static_cast<uint32_t>(red));
}

inline LTDC::ErrorCode LTDC::enablePeripheralClock(void)
{
  ResetControl::ErrorCode errorCode = m_resetControlPtr->enablePeripheralClock(getPeripheralTag());

  return (ResetControl::ErrorCode::OK == errorCode) ? ErrorCode::OK : ErrorCode::CAN_NOT_TURN_ON_PERIPHERAL_CLOCK;
}