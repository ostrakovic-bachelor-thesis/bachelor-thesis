#include "LTDC.h"
#include "MemoryAccess.h"
#include "MemoryUtility.h"
#include "RegisterUtility.h"


LTDC::LTDC(LTDC_TypeDef *LTDCPeripheralPtr,
    LTDC_Layer_TypeDef *LTDCPeripheralLayer1Ptr,
    LTDC_Layer_TypeDef *LTDCPeripheralLayer2Ptr):
  m_LTDCPeripheralPtr(LTDCPeripheralPtr),
  m_LTDCPeripheralLayer1Ptr(LTDCPeripheralLayer1Ptr),
  m_LTDCPeripheralLayer2Ptr(LTDCPeripheralLayer2Ptr)
{}

LTDC::ErrorCode LTDC::init(const LTDCConfig &ltdcConfig)
{
  setSynchronizationWidth(ltdcConfig.hsyncWidth, ltdcConfig.vsyncWidth);

  return ErrorCode::OK;
}

void LTDC::setSynchronizationWidth(uint16_t hsyncWidth, uint16_t vsyncWidth)
{
  constexpr uint32_t LTDC_SSCR_VSH_POSITION = 0u;
  constexpr uint32_t LTDC_SSCR_HSW_POSITION = 16u;
  constexpr uint32_t LTDC_SSCR_VSH_SIZE     = 11u;
  constexpr uint32_t LTDC_SSCR_HSW_SIZE     = 12u;

  uint32_t registerValueSSCR = 0u;

  registerValueSSCR = MemoryUtility<uint32_t>::setBits(
    registerValueSSCR,
    LTDC_SSCR_VSH_POSITION,
    LTDC_SSCR_VSH_SIZE,
    static_cast<uint32_t>(vsyncWidth - 1u));

  registerValueSSCR = MemoryUtility<uint32_t>::setBits(
    registerValueSSCR,
    LTDC_SSCR_HSW_POSITION,
    LTDC_SSCR_HSW_SIZE,
    static_cast<uint32_t>(hsyncWidth - 1u));

  MemoryAccess::setRegisterValue(&(m_LTDCPeripheralPtr->SSCR), registerValueSSCR);
}
