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
  return ErrorCode::OK;
}
