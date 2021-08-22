#include "LTDC.h"
#include "DriverTest.h"
#include "gtest/gtest.h"
#include "gmock/gmock.h"


using namespace ::testing;


class ALTDC : public DriverTest
{
public:

  //! Based on real reset values for LTDC registers (source STM32L4R9 reference manual)
  static constexpr uint32_t LTDC_SSCR_RESET_VALUE  = 0x00000000;
  static constexpr uint32_t LTDC_BPCR_RESET_VALUE  = 0x00000000;
  static constexpr uint32_t LTDC_AWCR_RESET_VALUE  = 0x00000000;
  static constexpr uint32_t LTDC_TWCR_RESET_VALUE  = 0x00000000;
  static constexpr uint32_t LTDC_GCR_RESET_VALUE   = 0x00002220;
  static constexpr uint32_t LTDC_SRCR_RESET_VALUE  = 0x00000000;
  static constexpr uint32_t LTDC_BCCR_RESET_VALUE  = 0x00000000;
  static constexpr uint32_t LTDC_IER_RESET_VALUE   = 0x00000000;
  static constexpr uint32_t LTDC_ISR_RESET_VALUE   = 0x00000000;
  static constexpr uint32_t LTDC_ICR_RESET_VALUE   = 0x00000000;
  static constexpr uint32_t LTDC_LIPCR_RESET_VALUE = 0x00000000;
  static constexpr uint32_t LTDC_CPSR_RESET_VALUE  = 0x00000000;
  static constexpr uint32_t LTDC_CDSR_RESET_VALUE  = 0x0000000F;

  static constexpr uint32_t LTDC_LAYER_X_CR_RESET_VALUE     = 0x00000000;
  static constexpr uint32_t LTDC_LAYER_X_WHPCR_RESET_VALUE  = 0x00000000;
  static constexpr uint32_t LTDC_LAYER_X_WVPCR_RESET_VALUE  = 0x00000000;
  static constexpr uint32_t LTDC_LAYER_X_CKCR_RESET_VALUE   = 0x00000000;
  static constexpr uint32_t LTDC_LAYER_X_PFCR_RESET_VALUE   = 0x00000000;
  static constexpr uint32_t LTDC_LAYER_X_CACR_RESET_VALUE   = 0x000000FF;
  static constexpr uint32_t LTDC_LAYER_X_DCCR_RESET_VALUE   = 0x00000000;
  static constexpr uint32_t LTDC_LAYER_X_BFCR_RESET_VALUE   = 0x00000607;
  static constexpr uint32_t LTDC_LAYER_X_CFBAR_RESET_VALUE  = 0x00000000;
  static constexpr uint32_t LTDC_LAYER_X_CFBLR_RESET_VALUE  = 0x00000000;
  static constexpr uint32_t LTDC_LAYER_X_CFBLNR_RESET_VALUE = 0x00000000;
  static constexpr uint32_t LTDC_LAYER_X_CLUTWR_RESET_VALUE = 0x00000000;

  LTDC_TypeDef virtualLTDCPeripheral;
  LTDC_Layer_TypeDef virtualLTDCPeripheralLayer1;
  LTDC_Layer_TypeDef virtualLTDCPeripheralLayer2;
  LTDC virtualLTDC = LTDC(&virtualLTDCPeripheral, &virtualLTDCPeripheralLayer1, &virtualLTDCPeripheralLayer2);
  LTDC::LTDCConfig ltdcConfig;

  void SetUp() override;
  void TearDown() override;
};

void ALTDC::SetUp()
{
  DriverTest::SetUp();

  // based on real reset values for LTDC registers (source STM32L4R9 reference manual)
  virtualLTDCPeripheral.SSCR  = LTDC_SSCR_RESET_VALUE;
  virtualLTDCPeripheral.BPCR  = LTDC_BPCR_RESET_VALUE;
  virtualLTDCPeripheral.AWCR  = LTDC_AWCR_RESET_VALUE;
  virtualLTDCPeripheral.TWCR  = LTDC_TWCR_RESET_VALUE;
  virtualLTDCPeripheral.GCR   = LTDC_GCR_RESET_VALUE;
  virtualLTDCPeripheral.SRCR  = LTDC_SRCR_RESET_VALUE;
  virtualLTDCPeripheral.BCCR  = LTDC_BCCR_RESET_VALUE;
  virtualLTDCPeripheral.IER   = LTDC_IER_RESET_VALUE;
  virtualLTDCPeripheral.ISR   = LTDC_ISR_RESET_VALUE;
  virtualLTDCPeripheral.ICR   = LTDC_ICR_RESET_VALUE;
  virtualLTDCPeripheral.LIPCR = LTDC_LIPCR_RESET_VALUE;
  virtualLTDCPeripheral.CPSR  = LTDC_CPSR_RESET_VALUE;
  virtualLTDCPeripheral.CDSR  = LTDC_CDSR_RESET_VALUE;

  virtualLTDCPeripheralLayer1.CR     = LTDC_LAYER_X_CR_RESET_VALUE;
  virtualLTDCPeripheralLayer1.WHPCR  = LTDC_LAYER_X_WHPCR_RESET_VALUE;
  virtualLTDCPeripheralLayer1.WVPCR  = LTDC_LAYER_X_WVPCR_RESET_VALUE;
  virtualLTDCPeripheralLayer1.CKCR   = LTDC_LAYER_X_CKCR_RESET_VALUE;
  virtualLTDCPeripheralLayer1.PFCR   = LTDC_LAYER_X_PFCR_RESET_VALUE;
  virtualLTDCPeripheralLayer1.CACR   = LTDC_LAYER_X_CACR_RESET_VALUE;
  virtualLTDCPeripheralLayer1.DCCR   = LTDC_LAYER_X_DCCR_RESET_VALUE;
  virtualLTDCPeripheralLayer1.BFCR   = LTDC_LAYER_X_BFCR_RESET_VALUE;
  virtualLTDCPeripheralLayer1.CFBAR  = LTDC_LAYER_X_CFBAR_RESET_VALUE;
  virtualLTDCPeripheralLayer1.CFBLR  = LTDC_LAYER_X_CFBLR_RESET_VALUE;
  virtualLTDCPeripheralLayer1.CFBLNR = LTDC_LAYER_X_CFBLNR_RESET_VALUE;
  virtualLTDCPeripheralLayer1.CLUTWR = LTDC_LAYER_X_CLUTWR_RESET_VALUE;

  virtualLTDCPeripheralLayer2.CR     = LTDC_LAYER_X_CR_RESET_VALUE;
  virtualLTDCPeripheralLayer2.WHPCR  = LTDC_LAYER_X_WHPCR_RESET_VALUE;
  virtualLTDCPeripheralLayer2.WVPCR  = LTDC_LAYER_X_WVPCR_RESET_VALUE;
  virtualLTDCPeripheralLayer2.CKCR   = LTDC_LAYER_X_CKCR_RESET_VALUE;
  virtualLTDCPeripheralLayer2.PFCR   = LTDC_LAYER_X_PFCR_RESET_VALUE;
  virtualLTDCPeripheralLayer2.CACR   = LTDC_LAYER_X_CACR_RESET_VALUE;
  virtualLTDCPeripheralLayer2.DCCR   = LTDC_LAYER_X_DCCR_RESET_VALUE;
  virtualLTDCPeripheralLayer2.BFCR   = LTDC_LAYER_X_BFCR_RESET_VALUE;
  virtualLTDCPeripheralLayer2.CFBAR  = LTDC_LAYER_X_CFBAR_RESET_VALUE;
  virtualLTDCPeripheralLayer2.CFBLR  = LTDC_LAYER_X_CFBLR_RESET_VALUE;
  virtualLTDCPeripheralLayer2.CFBLNR = LTDC_LAYER_X_CFBLNR_RESET_VALUE;
  virtualLTDCPeripheralLayer2.CLUTWR = LTDC_LAYER_X_CLUTWR_RESET_VALUE;
}

void ALTDC::TearDown()
{
  DriverTest::TearDown();
}

TEST_F(ALTDC, InitSetsValueOfSSCRRegisterAccordingToHsyncWidthAndVSyncWidth)
{
  constexpr uint32_t LTDC_SSCR_VSH_POSITION = 0u;
  constexpr uint32_t LTDC_SSCR_HSW_POSITION = 16u;
  constexpr uint32_t LTDC_SSCR_VSH_SIZE     = 11u;
  constexpr uint32_t LTDC_SSCR_HSW_SIZE     = 12u;
  constexpr uint32_t EXPECTED_LTDC_SSCR_VSH_VALUE = 0u;
  constexpr uint32_t EXPECTED_LTDC_SSCR_HSW_VALUE = 0u;
  auto bitValueMatcher =
    AllOf(BitsHaveValue(LTDC_SSCR_VSH_POSITION, LTDC_SSCR_VSH_SIZE, EXPECTED_LTDC_SSCR_VSH_VALUE),
          BitsHaveValue(LTDC_SSCR_HSW_POSITION, LTDC_SSCR_HSW_SIZE, EXPECTED_LTDC_SSCR_HSW_VALUE));
  ltdcConfig.hsyncWidth = EXPECTED_LTDC_SSCR_HSW_VALUE + 1u;
  ltdcConfig.vsyncWidth = EXPECTED_LTDC_SSCR_VSH_VALUE + 1u;
  expectSpecificRegisterSetWithNoChangesAfter(&(virtualLTDCPeripheral.SSCR), bitValueMatcher);

  const LTDC::ErrorCode errorCode = virtualLTDC.init(ltdcConfig);

  ASSERT_THAT(errorCode, Eq(LTDC::ErrorCode::OK));
  ASSERT_THAT(virtualLTDCPeripheral.SSCR, bitValueMatcher);
}