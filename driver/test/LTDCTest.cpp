#include "LTDC.h"
#include "DriverTest.h"
#include "ResetControlMock.h"
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
  NiceMock<ResetControlMock> resetControlMock;
  LTDC virtualLTDC =
    LTDC(&virtualLTDCPeripheral, &virtualLTDCPeripheralLayer1, &virtualLTDCPeripheralLayer2, &resetControlMock);
  LTDC::LTDCConfig ltdcConfig;
  LTDC::LTDCLayerConfig ltdcLayer1Config;

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


TEST_F(ALTDC, GetPeripheralTagReturnsPointerToUnderlayingLTDCPeripheralCastedToPeripheralType)
{
  ASSERT_THAT(virtualLTDC.getPeripheralTag(),
    Eq(static_cast<Peripheral>(reinterpret_cast<uintptr_t>(&virtualLTDCPeripheral))));
}

TEST_F(ALTDC, InitTurnsOnLTDCPeripheralClock)
{
  resetControlMock.setReturnErrorCode(ResetControl::ErrorCode::OK);
  EXPECT_CALL(resetControlMock, enablePeripheralClock(virtualLTDC.getPeripheralTag()))
    .Times(1u);

  const LTDC::ErrorCode errorCode = virtualLTDC.init(ltdcConfig, ltdcLayer1Config);

  ASSERT_THAT(errorCode, Eq(LTDC::ErrorCode::OK));
}

TEST_F(ALTDC, InitFailsIfTurningOnOfLTDCPeripheralClockFail)
{
  resetControlMock.setReturnErrorCode(ResetControl::ErrorCode::INTERNAL);
  EXPECT_CALL(resetControlMock, enablePeripheralClock(virtualLTDC.getPeripheralTag()))
    .Times(1u);

  const LTDC::ErrorCode errorCode = virtualLTDC.init(ltdcConfig, ltdcLayer1Config);

  ASSERT_THAT(errorCode, Eq(LTDC::ErrorCode::CAN_NOT_TURN_ON_PERIPHERAL_CLOCK));
}

TEST_F(ALTDC, InitSetsValueOfVSHInSSCRRegisterAccordingToVSyncWidth)
{
  constexpr uint32_t VSYNC_WIDTH = 230u;
  constexpr uint32_t LTDC_SSCR_VSH_POSITION = 0u;
  constexpr uint32_t LTDC_SSCR_VSH_SIZE     = 11u;
  constexpr uint32_t EXPECTED_LTDC_SSCR_VSH_VALUE = VSYNC_WIDTH - 1u;
  auto bitValueMatcher =
    BitsHaveValue(LTDC_SSCR_VSH_POSITION, LTDC_SSCR_VSH_SIZE, EXPECTED_LTDC_SSCR_VSH_VALUE);
  ltdcConfig.vsyncWidth = VSYNC_WIDTH;
  expectSpecificRegisterSetWithNoChangesAfter(&(virtualLTDCPeripheral.SSCR), bitValueMatcher);

  const LTDC::ErrorCode errorCode = virtualLTDC.init(ltdcConfig, ltdcLayer1Config);

  ASSERT_THAT(errorCode, Eq(LTDC::ErrorCode::OK));
  ASSERT_THAT(virtualLTDCPeripheral.SSCR, bitValueMatcher);
}

TEST_F(ALTDC, InitSetsValueOfHSWInSSCRRegisterAccordingToHSyncWidth)
{
  constexpr uint32_t HSYNC_WIDTH = 240u;
  constexpr uint32_t LTDC_SSCR_HSW_POSITION = 16u;
  constexpr uint32_t LTDC_SSCR_HSW_SIZE     = 12u;
  constexpr uint32_t EXPECTED_LTDC_SSCR_HSW_VALUE = HSYNC_WIDTH - 1u;
  auto bitValueMatcher =
    BitsHaveValue(LTDC_SSCR_HSW_POSITION, LTDC_SSCR_HSW_SIZE, EXPECTED_LTDC_SSCR_HSW_VALUE);
  ltdcConfig.hsyncWidth = HSYNC_WIDTH;
  expectSpecificRegisterSetWithNoChangesAfter(&(virtualLTDCPeripheral.SSCR), bitValueMatcher);

  const LTDC::ErrorCode errorCode = virtualLTDC.init(ltdcConfig, ltdcLayer1Config);

  ASSERT_THAT(errorCode, Eq(LTDC::ErrorCode::OK));
  ASSERT_THAT(virtualLTDCPeripheral.SSCR, bitValueMatcher);
}

TEST_F(ALTDC, InitSetsValueOfAVBPInBPCRRegisterAccordingToVsyncWidthAndVerticalBackPorch)
{
  constexpr uint32_t VSYNC_WIDTH = 30u;
  constexpr uint32_t VBP         = 10u;
  constexpr uint32_t LTDC_BPCR_AVBP_POSITION = 0u;
  constexpr uint32_t LTDC_BPCR_AVBP_SIZE = 11u;
  constexpr uint32_t EXPECTED_LTDC_BPCR_AVBP_VALUE = VBP + VSYNC_WIDTH - 1u;
  auto bitValueMatcher =
    BitsHaveValue(LTDC_BPCR_AVBP_POSITION, LTDC_BPCR_AVBP_SIZE, EXPECTED_LTDC_BPCR_AVBP_VALUE);
  ltdcConfig.vsyncWidth        = VSYNC_WIDTH;
  ltdcConfig.verticalBackPorch = VBP;
  expectSpecificRegisterSetWithNoChangesAfter(&(virtualLTDCPeripheral.BPCR), bitValueMatcher);

  const LTDC::ErrorCode errorCode = virtualLTDC.init(ltdcConfig, ltdcLayer1Config);

  ASSERT_THAT(errorCode, Eq(LTDC::ErrorCode::OK));
  ASSERT_THAT(virtualLTDCPeripheral.BPCR, bitValueMatcher);
}

TEST_F(ALTDC, InitSetsValueOfAHBPInBPCRRegisterAccordingToHsyncWidthAndHorizontalBackPorch)
{
  constexpr uint32_t HSYNC_WIDTH = 40u;
  constexpr uint32_t HBP         = 10u;
  constexpr uint32_t LTDC_BPCR_AHBP_POSITION = 16u;
  constexpr uint32_t LTDC_BPCR_AHBP_SIZE     = 12u;
  constexpr uint32_t EXPECTED_LTDC_BPCR_AHBP_VALUE = HBP + HSYNC_WIDTH - 1u;
  auto bitValueMatcher =
    BitsHaveValue(LTDC_BPCR_AHBP_POSITION, LTDC_BPCR_AHBP_SIZE, EXPECTED_LTDC_BPCR_AHBP_VALUE);
  ltdcConfig.hsyncWidth          = HSYNC_WIDTH;
  ltdcConfig.horizontalBackPorch = HBP;
  expectSpecificRegisterSetWithNoChangesAfter(&(virtualLTDCPeripheral.BPCR), bitValueMatcher);

  const LTDC::ErrorCode errorCode = virtualLTDC.init(ltdcConfig, ltdcLayer1Config);

  ASSERT_THAT(errorCode, Eq(LTDC::ErrorCode::OK));
  ASSERT_THAT(virtualLTDCPeripheral.BPCR, bitValueMatcher);
}

TEST_F(ALTDC, InitSetsValueOfAAHInAWCRRegisterAccordingToVsyncWidthAndVerticalBackPorchAndDisplayHeight)
{
  constexpr uint32_t VSYNC_WIDTH    = 20u;
  constexpr uint32_t VBP            = 10u;
  constexpr uint32_t DISPLAY_HEIGHT = 300u;
  constexpr uint32_t LTDC_AWCR_AAH_POSITION = 0u;
  constexpr uint32_t LTDC_AWCR_AAH_SIZE     = 11u;
  constexpr uint32_t EXPECTED_LTDC_AWCR_AAH_VALUE = DISPLAY_HEIGHT + VBP + VSYNC_WIDTH - 1u;
  auto bitValueMatcher =
    BitsHaveValue(LTDC_AWCR_AAH_POSITION, LTDC_AWCR_AAH_SIZE, EXPECTED_LTDC_AWCR_AAH_VALUE);
  ltdcConfig.vsyncWidth        = VSYNC_WIDTH;
  ltdcConfig.verticalBackPorch = VBP;
  ltdcConfig.displayHeight     = DISPLAY_HEIGHT;
  expectSpecificRegisterSetWithNoChangesAfter(&(virtualLTDCPeripheral.AWCR), bitValueMatcher);

  const LTDC::ErrorCode errorCode = virtualLTDC.init(ltdcConfig, ltdcLayer1Config);

  ASSERT_THAT(errorCode, Eq(LTDC::ErrorCode::OK));
  ASSERT_THAT(virtualLTDCPeripheral.AWCR, bitValueMatcher);
}

TEST_F(ALTDC, InitSetsValueOfAAWInAWCRRegisterAccordingToHsyncWidthAndHorizontalBackPorchAndDisplayWidth)
{
  constexpr uint32_t HSYNC_WIDTH   = 20u;
  constexpr uint32_t HBP           = 10u;
  constexpr uint32_t DISPLAY_WIDTH = 400u;
  constexpr uint32_t LTDC_AWCR_AAW_POSITION = 16u;
  constexpr uint32_t LTDC_AWCR_AAW_SIZE     = 12u;
  constexpr uint32_t EXPECTED_LTDC_AWCR_AAW_VALUE = DISPLAY_WIDTH  + HBP + HSYNC_WIDTH - 1u;
  auto bitValueMatcher =
    BitsHaveValue(LTDC_AWCR_AAW_POSITION, LTDC_AWCR_AAW_SIZE, EXPECTED_LTDC_AWCR_AAW_VALUE);
  ltdcConfig.hsyncWidth          = HSYNC_WIDTH;
  ltdcConfig.horizontalBackPorch = HBP;
  ltdcConfig.displayWidth        = DISPLAY_WIDTH;
  expectSpecificRegisterSetWithNoChangesAfter(&(virtualLTDCPeripheral.AWCR), bitValueMatcher);

  const LTDC::ErrorCode errorCode = virtualLTDC.init(ltdcConfig, ltdcLayer1Config);

  ASSERT_THAT(errorCode, Eq(LTDC::ErrorCode::OK));
  ASSERT_THAT(virtualLTDCPeripheral.AWCR, bitValueMatcher);
}

TEST_F(ALTDC, InitSetsValueOfTOTALHInTWCRRegisterAccordingToVsyncWidthAndVerticalBackPorchAndDisplayHeightAndVerticalFrontPorch)
{
  constexpr uint32_t VSYNC_WIDTH    = 20u;
  constexpr uint32_t VBP            = 10u;
  constexpr uint32_t DISPLAY_HEIGHT = 300u;
  constexpr uint32_t VFP            = 10u;
  constexpr uint32_t LTDC_TWCR_TOTALH_POSITION = 0u;
  constexpr uint32_t LTDC_TWCR_TOTALH_SIZE     = 11u;
  constexpr uint32_t EXPECTED_LTDC_TWCR_TOTALH_VALUE = VFP + DISPLAY_HEIGHT + VBP + VSYNC_WIDTH - 1u;
  auto bitValueMatcher =
    BitsHaveValue(LTDC_TWCR_TOTALH_POSITION, LTDC_TWCR_TOTALH_SIZE, EXPECTED_LTDC_TWCR_TOTALH_VALUE);
  ltdcConfig.vsyncWidth         = VSYNC_WIDTH;
  ltdcConfig.verticalBackPorch  = VBP;
  ltdcConfig.displayHeight      = DISPLAY_HEIGHT;
  ltdcConfig.verticalFrontPorch = VFP;
  expectSpecificRegisterSetWithNoChangesAfter(&(virtualLTDCPeripheral.TWCR), bitValueMatcher);

  const LTDC::ErrorCode errorCode = virtualLTDC.init(ltdcConfig, ltdcLayer1Config);

  ASSERT_THAT(errorCode, Eq(LTDC::ErrorCode::OK));
  ASSERT_THAT(virtualLTDCPeripheral.TWCR, bitValueMatcher);
}

TEST_F(ALTDC, InitSetsValueOfTOTALWInTWCRRegisterAccordingToHsyncWidthAndHorizontalBackPorchAndDisplayWidthAndHorizontalFrontPorch)
{
  constexpr uint32_t HSYNC_WIDTH   = 20u;
  constexpr uint32_t HBP           = 10u;
  constexpr uint32_t DISPLAY_WIDTH = 400u;
  constexpr uint32_t HFP           = 10u;
  constexpr uint32_t LTDC_TWCR_TOTALW_POSITION = 16u;
  constexpr uint32_t LTDC_TWCR_TOTALW_SIZE     = 12u;
  constexpr uint32_t EXPECTED_LTDC_TWCR_TOTALW_VALUE = HFP + DISPLAY_WIDTH  + HBP + HSYNC_WIDTH - 1u;
  auto bitValueMatcher =
    BitsHaveValue(LTDC_TWCR_TOTALW_POSITION, LTDC_TWCR_TOTALW_SIZE, EXPECTED_LTDC_TWCR_TOTALW_VALUE);
  ltdcConfig.hsyncWidth           = HSYNC_WIDTH;
  ltdcConfig.horizontalBackPorch  = HBP;
  ltdcConfig.displayWidth         = DISPLAY_WIDTH;
  ltdcConfig.horizontalFrontPorch = HFP;
  expectSpecificRegisterSetWithNoChangesAfter(&(virtualLTDCPeripheral.TWCR), bitValueMatcher);

  const LTDC::ErrorCode errorCode = virtualLTDC.init(ltdcConfig, ltdcLayer1Config);

  ASSERT_THAT(errorCode, Eq(LTDC::ErrorCode::OK));
  ASSERT_THAT(virtualLTDCPeripheral.TWCR, bitValueMatcher);
}

TEST_F(ALTDC, InitSetsValueOfHSPOLInGCRRegisterAccordingToHSyncPolarity)
{
  constexpr uint32_t LTDC_GCR_HSPOL_POSITION = 31u;
  constexpr uint32_t EXPECTED_LTDC_GCR_HSPOL_VALUE = 1u;
  auto bitValueMatcher =
    BitHasValue(LTDC_GCR_HSPOL_POSITION, EXPECTED_LTDC_GCR_HSPOL_VALUE);
  ltdcConfig.hsyncPolarity = LTDC::Polarity::ACTIVE_HIGH;
  expectSpecificRegisterSetWithNoChangesAfter(&(virtualLTDCPeripheral.GCR), bitValueMatcher);

  const LTDC::ErrorCode errorCode = virtualLTDC.init(ltdcConfig, ltdcLayer1Config);

  ASSERT_THAT(errorCode, Eq(LTDC::ErrorCode::OK));
  ASSERT_THAT(virtualLTDCPeripheral.GCR, bitValueMatcher);
}

TEST_F(ALTDC, InitSetsValueOfVSPOLInGCRRegisterAccordingToVSyncPolarity)
{
  constexpr uint32_t LTDC_GCR_VSPOL_POSITION = 30u;
  constexpr uint32_t EXPECTED_LTDC_GCR_VSPOL_VALUE = 1u;
  auto bitValueMatcher =
    BitHasValue(LTDC_GCR_VSPOL_POSITION, EXPECTED_LTDC_GCR_VSPOL_VALUE);
  ltdcConfig.vsyncPolarity = LTDC::Polarity::ACTIVE_HIGH;
  expectSpecificRegisterSetWithNoChangesAfter(&(virtualLTDCPeripheral.GCR), bitValueMatcher);

  const LTDC::ErrorCode errorCode = virtualLTDC.init(ltdcConfig, ltdcLayer1Config);

  ASSERT_THAT(errorCode, Eq(LTDC::ErrorCode::OK));
  ASSERT_THAT(virtualLTDCPeripheral.GCR, bitValueMatcher);
}

TEST_F(ALTDC, InitSetsValueOfDEPOLInGCRRegisterAccordingToNotDataEnablePolarity)
{
  constexpr uint32_t LTDC_GCR_DEPOL_POSITION = 29u;
  constexpr uint32_t EXPECTED_LTDC_GCR_DEPOL_VALUE = 1u;
  auto bitValueMatcher =
    BitHasValue(LTDC_GCR_DEPOL_POSITION, EXPECTED_LTDC_GCR_DEPOL_VALUE);
  ltdcConfig.notDataEnablePolarity = LTDC::Polarity::ACTIVE_HIGH;
  expectSpecificRegisterSetWithNoChangesAfter(&(virtualLTDCPeripheral.GCR), bitValueMatcher);

  const LTDC::ErrorCode errorCode = virtualLTDC.init(ltdcConfig, ltdcLayer1Config);

  ASSERT_THAT(errorCode, Eq(LTDC::ErrorCode::OK));
  ASSERT_THAT(virtualLTDCPeripheral.GCR, bitValueMatcher);
}

TEST_F(ALTDC, InitSetsValueOfPCPOLInGCRRegisterAccordingToPixelClockPolarity)
{
  constexpr uint32_t LTDC_GCR_PCPOL_POSITION = 28u;
  constexpr uint32_t EXPECTED_LTDC_GCR_PCPOL_VALUE = 1u;
  auto bitValueMatcher =
    BitHasValue(LTDC_GCR_PCPOL_POSITION, EXPECTED_LTDC_GCR_PCPOL_VALUE);
  ltdcConfig.pixelClockPolarity = LTDC::Polarity::ACTIVE_HIGH;
  expectSpecificRegisterSetWithNoChangesAfter(&(virtualLTDCPeripheral.GCR), bitValueMatcher);

  const LTDC::ErrorCode errorCode = virtualLTDC.init(ltdcConfig, ltdcLayer1Config);

  ASSERT_THAT(errorCode, Eq(LTDC::ErrorCode::OK));
  ASSERT_THAT(virtualLTDCPeripheral.GCR, bitValueMatcher);
}

TEST_F(ALTDC, InitSetsValueOfBCBLUEInBCCRRegisterAccordingToBackgroundColorBlueComponent)
{
  constexpr uint32_t LTDC_BCCR_BCBLUE_POSITION = 0u;
  constexpr uint32_t LTDC_BCCR_BCBLUE_SIZE     = 8u;
  constexpr uint32_t EXPECTED_LTDC_BCCR_BCBLUE_VALUE = 143u;
  auto bitValueMatcher =
    BitsHaveValue(LTDC_BCCR_BCBLUE_POSITION, LTDC_BCCR_BCBLUE_SIZE, EXPECTED_LTDC_BCCR_BCBLUE_VALUE);
  ltdcConfig.backgroundColor.blue = EXPECTED_LTDC_BCCR_BCBLUE_VALUE;
  expectSpecificRegisterSetWithNoChangesAfter(&(virtualLTDCPeripheral.BCCR), bitValueMatcher);

  const LTDC::ErrorCode errorCode = virtualLTDC.init(ltdcConfig, ltdcLayer1Config);

  ASSERT_THAT(errorCode, Eq(LTDC::ErrorCode::OK));
  ASSERT_THAT(virtualLTDCPeripheral.BCCR, bitValueMatcher);
}

TEST_F(ALTDC, InitSetsValueOfBCGREENInBCCRRegisterAccordingToBackgroundColorGreenComponent)
{
  constexpr uint32_t LTDC_BCCR_BCGREEN_POSITION = 8u;
  constexpr uint32_t LTDC_BCCR_BCGREEN_SIZE     = 8u;
  constexpr uint32_t EXPECTED_LTDC_BCCR_BCGREEN_VALUE = 31u;
  auto bitValueMatcher =
    BitsHaveValue(LTDC_BCCR_BCGREEN_POSITION, LTDC_BCCR_BCGREEN_SIZE, EXPECTED_LTDC_BCCR_BCGREEN_VALUE);
  ltdcConfig.backgroundColor.green = EXPECTED_LTDC_BCCR_BCGREEN_VALUE;
  expectSpecificRegisterSetWithNoChangesAfter(&(virtualLTDCPeripheral.BCCR), bitValueMatcher);

  const LTDC::ErrorCode errorCode = virtualLTDC.init(ltdcConfig, ltdcLayer1Config);

  ASSERT_THAT(errorCode, Eq(LTDC::ErrorCode::OK));
  ASSERT_THAT(virtualLTDCPeripheral.BCCR, bitValueMatcher);
}

TEST_F(ALTDC, InitSetsValueOfBCREDInBCCRRegisterAccordingToBackgroundColorRedComponent)
{
  constexpr uint32_t LTDC_BCCR_BCRED_POSITION = 16u;
  constexpr uint32_t LTDC_BCCR_BCRED_SIZE     = 8u;
  constexpr uint32_t EXPECTED_LTDC_BCCR_BCRED_VALUE = 254u;
  auto bitValueMatcher =
    BitsHaveValue(LTDC_BCCR_BCRED_POSITION, LTDC_BCCR_BCRED_SIZE, EXPECTED_LTDC_BCCR_BCRED_VALUE);
  ltdcConfig.backgroundColor.red = EXPECTED_LTDC_BCCR_BCRED_VALUE;
  expectSpecificRegisterSetWithNoChangesAfter(&(virtualLTDCPeripheral.BCCR), bitValueMatcher);

  const LTDC::ErrorCode errorCode = virtualLTDC.init(ltdcConfig, ltdcLayer1Config);

  ASSERT_THAT(errorCode, Eq(LTDC::ErrorCode::OK));
  ASSERT_THAT(virtualLTDCPeripheral.BCCR, bitValueMatcher);
}

TEST_F(ALTDC, InitEnablesLTDCPeripheralBySettingLTDCENBitInGCRRegister)
{
  constexpr uint32_t LTDC_GCR_LTDCEN_POSITION = 0u;
  constexpr uint32_t EXPECTED_LTDC_GCR_LTDCEN_VALUE = 1u;
  auto bitValueMatcher =
    BitHasValue(LTDC_GCR_LTDCEN_POSITION, EXPECTED_LTDC_GCR_LTDCEN_VALUE);
  expectSpecificRegisterSetWithNoChangesAfter(&(virtualLTDCPeripheral.GCR), bitValueMatcher);

  const LTDC::ErrorCode errorCode = virtualLTDC.init(ltdcConfig, ltdcLayer1Config);

  ASSERT_THAT(errorCode, Eq(LTDC::ErrorCode::OK));
  ASSERT_THAT(virtualLTDCPeripheral.GCR, bitValueMatcher);
}

TEST_F(ALTDC, InitSetsLayer1WindowHorizontalStartPositionAccordingToHsyncWidthAndHorizontalBackPorch)
{
  constexpr uint32_t HSYNC_WIDTH = 40u;
  constexpr uint32_t HBP         = 10u;
  constexpr uint32_t LTDC_LAYER_WHPCR_WHSTPOS_POSITION = 0u;
  constexpr uint32_t LTDC_LAYER_WHPCR_WHSTPOS_SIZE     = 12u;
  constexpr uint32_t EXPECTED_LTDC_LAYER_WHPCR_WHSTPOS_VALUE = HBP + HSYNC_WIDTH;
  auto bitValueMatcher = BitsHaveValue(LTDC_LAYER_WHPCR_WHSTPOS_POSITION,
    LTDC_LAYER_WHPCR_WHSTPOS_SIZE,
    EXPECTED_LTDC_LAYER_WHPCR_WHSTPOS_VALUE);
  ltdcConfig.hsyncWidth          = HSYNC_WIDTH;
  ltdcConfig.horizontalBackPorch = HBP;
  expectSpecificRegisterSetWithNoChangesAfter(&(virtualLTDCPeripheralLayer1.WHPCR), bitValueMatcher);

  const LTDC::ErrorCode errorCode = virtualLTDC.init(ltdcConfig, ltdcLayer1Config);

  ASSERT_THAT(errorCode, Eq(LTDC::ErrorCode::OK));
  ASSERT_THAT(virtualLTDCPeripheralLayer1.WHPCR, bitValueMatcher);
}

TEST_F(ALTDC, InitSetsLayer1WindowHorizontalStopPositionAccordingToHsyncWidthAndHorizontalBackPorchAndDisplayWidth)
{
  constexpr uint32_t HSYNC_WIDTH   = 20u;
  constexpr uint32_t HBP           = 10u;
  constexpr uint32_t DISPLAY_WIDTH = 400u;
  constexpr uint32_t LTDC_LAYER_WHPCR_WHSPPOS_POSITION = 16u;
  constexpr uint32_t LTDC_LAYER_WHPCR_WHSPPOS_SIZE     = 12u;
  constexpr uint32_t EXPECTED_LTDC_LAYER_WHPCR_WHSPPOS_VALUE = DISPLAY_WIDTH  + HBP + HSYNC_WIDTH - 1u;
  auto bitValueMatcher = BitsHaveValue(LTDC_LAYER_WHPCR_WHSPPOS_POSITION,
    LTDC_LAYER_WHPCR_WHSPPOS_SIZE,
    EXPECTED_LTDC_LAYER_WHPCR_WHSPPOS_VALUE);
  ltdcConfig.hsyncWidth          = HSYNC_WIDTH;
  ltdcConfig.horizontalBackPorch = HBP;
  ltdcConfig.displayWidth        = DISPLAY_WIDTH;
  expectSpecificRegisterSetWithNoChangesAfter(&(virtualLTDCPeripheralLayer1.WHPCR), bitValueMatcher);

  const LTDC::ErrorCode errorCode = virtualLTDC.init(ltdcConfig, ltdcLayer1Config);

  ASSERT_THAT(errorCode, Eq(LTDC::ErrorCode::OK));
  ASSERT_THAT(virtualLTDCPeripheralLayer1.WHPCR, bitValueMatcher);
}

TEST_F(ALTDC, InitSetsLayer1WindowVerticalStartPositionAccordingToVsyncWidthAndVerticalBackPorch)
{
  constexpr uint32_t VSYNC_WIDTH = 30u;
  constexpr uint32_t VBP         = 10u;
  constexpr uint32_t LTDC_LAYER_WVPCR_WVSTPOS_POSITION = 0u;
  constexpr uint32_t LTDC_LAYER_WVPCR_WVSTPOS_SIZE     = 11u;
  constexpr uint32_t EXPECTED_LTDC_LAYER_WVPCR_WVSTPOS_VALUE = VBP + VSYNC_WIDTH;
  auto bitValueMatcher = BitsHaveValue(LTDC_LAYER_WVPCR_WVSTPOS_POSITION,
    LTDC_LAYER_WVPCR_WVSTPOS_SIZE,
    EXPECTED_LTDC_LAYER_WVPCR_WVSTPOS_VALUE);
  ltdcConfig.vsyncWidth        = VSYNC_WIDTH;
  ltdcConfig.verticalBackPorch = VBP;
  expectSpecificRegisterSetWithNoChangesAfter(&(virtualLTDCPeripheralLayer1.WVPCR), bitValueMatcher);

  const LTDC::ErrorCode errorCode = virtualLTDC.init(ltdcConfig, ltdcLayer1Config);

  ASSERT_THAT(errorCode, Eq(LTDC::ErrorCode::OK));
  ASSERT_THAT(virtualLTDCPeripheralLayer1.WVPCR, bitValueMatcher);
}

TEST_F(ALTDC, InitSetsLayer1WindowVerticalStopPositionAccordingToVsyncWidthAndVerticalBackPorchAndDisplayHeight)
{
  constexpr uint32_t VSYNC_WIDTH    = 20u;
  constexpr uint32_t VBP            = 10u;
  constexpr uint32_t DISPLAY_HEIGHT = 300u;
  constexpr uint32_t LTDC_LAYER_WVPCR_WVSPPOS_POSITION = 16u;
  constexpr uint32_t LTDC_LAYER_WVPCR_WVSPPOS_SIZE     = 11u;
  constexpr uint32_t EXPECTED_LTDC_LAYER_WVPCR_WVSPPOS_VALUE = DISPLAY_HEIGHT + VBP + VSYNC_WIDTH - 1u;
  auto bitValueMatcher = BitsHaveValue(LTDC_LAYER_WVPCR_WVSPPOS_POSITION,
    LTDC_LAYER_WVPCR_WVSPPOS_SIZE,
    EXPECTED_LTDC_LAYER_WVPCR_WVSPPOS_VALUE);
  ltdcConfig.vsyncWidth        = VSYNC_WIDTH;
  ltdcConfig.verticalBackPorch = VBP;
  ltdcConfig.displayHeight     = DISPLAY_HEIGHT;
  expectSpecificRegisterSetWithNoChangesAfter(&(virtualLTDCPeripheralLayer1.WVPCR), bitValueMatcher);

  const LTDC::ErrorCode errorCode = virtualLTDC.init(ltdcConfig, ltdcLayer1Config);

  ASSERT_THAT(errorCode, Eq(LTDC::ErrorCode::OK));
  ASSERT_THAT(virtualLTDCPeripheralLayer1.WVPCR, bitValueMatcher);
}

TEST_F(ALTDC, InitSetsValueOfPFBitsInLayer1PFCRRegisterAccordingToLayer1FrameBufferPixelColorFormat)
{
  constexpr uint32_t LTDC_LAYER_PFCR_PF_POSITION = 0u;
  constexpr uint32_t LTDC_LAYER_PFCR_PF_SIZE     = 3u;
  constexpr uint32_t EXPECTED_LTDC_LAYER_PFCR_PF_VALUE = 0x1;
  auto bitValueMatcher = BitsHaveValue(LTDC_LAYER_PFCR_PF_POSITION,
    LTDC_LAYER_PFCR_PF_SIZE,
    EXPECTED_LTDC_LAYER_PFCR_PF_VALUE);
  ltdcLayer1Config.frameBufferColorFormat = LTDC::ColorFormat::RGB888;
  expectSpecificRegisterSetWithNoChangesAfter(&(virtualLTDCPeripheralLayer1.PFCR), bitValueMatcher);

  const LTDC::ErrorCode errorCode = virtualLTDC.init(ltdcConfig, ltdcLayer1Config);

  ASSERT_THAT(errorCode, Eq(LTDC::ErrorCode::OK));
  ASSERT_THAT(virtualLTDCPeripheralLayer1.PFCR, bitValueMatcher);
}

TEST_F(ALTDC, InitSetsValueOfCONSTABitsInLayer1CACRRegisterAccordingToLayer1Alpha)
{
  constexpr uint32_t LTDC_LAYER_CACR_CONSTA_POSITION = 0u;
  constexpr uint32_t LTDC_LAYER_CACR_CONSTA_SIZE     = 8u;
  constexpr uint32_t EXPECTED_LTDC_LAYER_CACR_CONSTA_VALUE = 241u;
  auto bitValueMatcher = BitsHaveValue(LTDC_LAYER_CACR_CONSTA_POSITION,
    LTDC_LAYER_CACR_CONSTA_SIZE,
    EXPECTED_LTDC_LAYER_CACR_CONSTA_VALUE);
  ltdcLayer1Config.alpha = EXPECTED_LTDC_LAYER_CACR_CONSTA_VALUE;
  expectSpecificRegisterSetWithNoChangesAfter(&(virtualLTDCPeripheralLayer1.CACR), bitValueMatcher);

  const LTDC::ErrorCode errorCode = virtualLTDC.init(ltdcConfig, ltdcLayer1Config);

  ASSERT_THAT(errorCode, Eq(LTDC::ErrorCode::OK));
  ASSERT_THAT(virtualLTDCPeripheralLayer1.CACR, bitValueMatcher);
}

TEST_F(ALTDC, InitSetsValueOfDCBLUEInInLayer1DCCRRegisterAccordingToLayer1DefaultColorBlueComponent)
{
  constexpr uint32_t LTDC_DCCR_DCBLUE_POSITION = 0u;
  constexpr uint32_t LTDC_DCCR_DCBLUE_SIZE     = 8u;
  constexpr uint32_t EXPECTED_LTDC_DCCR_DCBLUE_VALUE = 143u;
  auto bitValueMatcher =
    BitsHaveValue(LTDC_DCCR_DCBLUE_POSITION, LTDC_DCCR_DCBLUE_SIZE, EXPECTED_LTDC_DCCR_DCBLUE_VALUE);
  ltdcLayer1Config.defaultColor.blue = EXPECTED_LTDC_DCCR_DCBLUE_VALUE;
  expectSpecificRegisterSetWithNoChangesAfter(&(virtualLTDCPeripheralLayer1.DCCR), bitValueMatcher);

  const LTDC::ErrorCode errorCode = virtualLTDC.init(ltdcConfig, ltdcLayer1Config);

  ASSERT_THAT(errorCode, Eq(LTDC::ErrorCode::OK));
  ASSERT_THAT(virtualLTDCPeripheralLayer1.DCCR, bitValueMatcher);
}

TEST_F(ALTDC, InitSetsValueOfDCGREENInInLayer1DCCRRegisterAccordingToLayer1DefaultColorGreenComponent)
{
  constexpr uint32_t LTDC_DCCR_DCGREEN_POSITION = 8u;
  constexpr uint32_t LTDC_DCCR_DCGREEN_SIZE     = 8u;
  constexpr uint32_t EXPECTED_LTDC_DCCR_DCGREEN_VALUE = 81u;
  auto bitValueMatcher =
    BitsHaveValue(LTDC_DCCR_DCGREEN_POSITION, LTDC_DCCR_DCGREEN_SIZE, EXPECTED_LTDC_DCCR_DCGREEN_VALUE);
  ltdcLayer1Config.defaultColor.green = EXPECTED_LTDC_DCCR_DCGREEN_VALUE;
  expectSpecificRegisterSetWithNoChangesAfter(&(virtualLTDCPeripheralLayer1.DCCR), bitValueMatcher);

  const LTDC::ErrorCode errorCode = virtualLTDC.init(ltdcConfig, ltdcLayer1Config);

  ASSERT_THAT(errorCode, Eq(LTDC::ErrorCode::OK));
  ASSERT_THAT(virtualLTDCPeripheralLayer1.DCCR, bitValueMatcher);
}

TEST_F(ALTDC, InitSetsValueOfDCREDInInLayer1DCCRRegisterAccordingToLayer1DefaultColorRedComponent)
{
  constexpr uint32_t LTDC_DCCR_DCRED_POSITION = 16u;
  constexpr uint32_t LTDC_DCCR_DCRED_SIZE     = 8u;
  constexpr uint32_t EXPECTED_LTDC_DCCR_DCRED_VALUE = 7u;
  auto bitValueMatcher =
    BitsHaveValue(LTDC_DCCR_DCRED_POSITION, LTDC_DCCR_DCRED_SIZE, EXPECTED_LTDC_DCCR_DCRED_VALUE);
  ltdcLayer1Config.defaultColor.red = EXPECTED_LTDC_DCCR_DCRED_VALUE;
  expectSpecificRegisterSetWithNoChangesAfter(&(virtualLTDCPeripheralLayer1.DCCR), bitValueMatcher);

  const LTDC::ErrorCode errorCode = virtualLTDC.init(ltdcConfig, ltdcLayer1Config);

  ASSERT_THAT(errorCode, Eq(LTDC::ErrorCode::OK));
  ASSERT_THAT(virtualLTDCPeripheralLayer1.DCCR, bitValueMatcher);
}

TEST_F(ALTDC, InitSetsValueOfDCALPHAInInLayer1DCCRRegisterAccordingToLayer1DefaultColorAlphaComponent)
{
  constexpr uint32_t LTDC_DCCR_DCALPHA_POSITION = 24u;
  constexpr uint32_t LTDC_DCCR_DCALPHA_SIZE     = 8u;
  constexpr uint32_t EXPECTED_LTDC_DCCR_DCALPHA_VALUE = 30u;
  auto bitValueMatcher =
    BitsHaveValue(LTDC_DCCR_DCALPHA_POSITION, LTDC_DCCR_DCALPHA_SIZE, EXPECTED_LTDC_DCCR_DCALPHA_VALUE);
  ltdcLayer1Config.defaultColor.alpha = EXPECTED_LTDC_DCCR_DCALPHA_VALUE;
  expectSpecificRegisterSetWithNoChangesAfter(&(virtualLTDCPeripheralLayer1.DCCR), bitValueMatcher);

  const LTDC::ErrorCode errorCode = virtualLTDC.init(ltdcConfig, ltdcLayer1Config);

  ASSERT_THAT(errorCode, Eq(LTDC::ErrorCode::OK));
  ASSERT_THAT(virtualLTDCPeripheralLayer1.DCCR, bitValueMatcher);
}

TEST_F(ALTDC, InitSetsValueOfBF1InInLayer1BFCRRegisterAccordingToLayer1CurrentLayerBlendingFactor)
{
  constexpr uint32_t LTDC_BFCR_BF1_POSITION = 8u;
  constexpr uint32_t LTDC_BFCR_BF1_SIZE     = 3u;
  constexpr uint32_t EXPECTED_LTDC_BFCR_BF1_VALUE = 0b110;
  auto bitValueMatcher =
    BitsHaveValue(LTDC_BFCR_BF1_POSITION, LTDC_BFCR_BF1_SIZE, EXPECTED_LTDC_BFCR_BF1_VALUE);
  ltdcLayer1Config.currentLayerBlendingFactor = LTDC::BlendingFactor::PIXEL_ALPHA_X_CONST_ALPHA;
  expectSpecificRegisterSetWithNoChangesAfter(&(virtualLTDCPeripheralLayer1.BFCR), bitValueMatcher);

  const LTDC::ErrorCode errorCode = virtualLTDC.init(ltdcConfig, ltdcLayer1Config);

  ASSERT_THAT(errorCode, Eq(LTDC::ErrorCode::OK));
  ASSERT_THAT(virtualLTDCPeripheralLayer1.BFCR, bitValueMatcher);
}

TEST_F(ALTDC, InitSetsValueOfBF2InInLayer1BFCRRegisterAccordingToLayer1SubjacentLayerBlendingFactor)
{
  constexpr uint32_t LTDC_BFCR_BF2_POSITION = 0u;
  constexpr uint32_t LTDC_BFCR_BF2_SIZE     = 3u;
  constexpr uint32_t EXPECTED_LTDC_BFCR_BF2_VALUE = 0b100;
  auto bitValueMatcher =
    BitsHaveValue(LTDC_BFCR_BF2_POSITION, LTDC_BFCR_BF2_SIZE, EXPECTED_LTDC_BFCR_BF2_VALUE);
  ltdcLayer1Config.subjacentLayerBlendingFactor = LTDC::BlendingFactor::CONST_ALPHA;
  expectSpecificRegisterSetWithNoChangesAfter(&(virtualLTDCPeripheralLayer1.BFCR), bitValueMatcher);

  const LTDC::ErrorCode errorCode = virtualLTDC.init(ltdcConfig, ltdcLayer1Config);

  ASSERT_THAT(errorCode, Eq(LTDC::ErrorCode::OK));
  ASSERT_THAT(virtualLTDCPeripheralLayer1.BFCR, bitValueMatcher);
}

TEST_F(ALTDC, InitSetsLayer1FrameBufferAddressInLayer1CFBARRegister)
{
  constexpr uint32_t EXPECTED_LTDC_CFBAR_VALUE = 0xA0008000;
  ltdcLayer1Config.frameBufferPtr = reinterpret_cast<void*>(EXPECTED_LTDC_CFBAR_VALUE);
  expectSpecificRegisterSetWithNoChangesAfter(&(virtualLTDCPeripheralLayer1.CFBAR), EXPECTED_LTDC_CFBAR_VALUE);

  const LTDC::ErrorCode errorCode = virtualLTDC.init(ltdcConfig, ltdcLayer1Config);

  ASSERT_THAT(errorCode, Eq(LTDC::ErrorCode::OK));
  ASSERT_THAT(virtualLTDCPeripheralLayer1.CFBAR, EXPECTED_LTDC_CFBAR_VALUE);
}

TEST_F(ALTDC, InitSetsValueOfCFBLLInInLayer1CFBLRRegisterAccordingToFrameBufferWidthAndColorFormat)
{
  constexpr uint16_t FRAME_BUFFER_WIDTH = 800u;
  constexpr uint32_t PIXEL_SIZE_RGB888 = 3u;
  constexpr uint32_t LTDC_CFBLR_CFBLL_POSITION = 0u;
  constexpr uint32_t LTDC_CFBLR_CFBLL_SIZE     = 13u;
  constexpr uint32_t EXPECTED_LTDC_CFBLR_CFBLL_VALUE = FRAME_BUFFER_WIDTH * PIXEL_SIZE_RGB888 + 3u;
  auto bitValueMatcher =
    BitsHaveValue(LTDC_CFBLR_CFBLL_POSITION, LTDC_CFBLR_CFBLL_SIZE, EXPECTED_LTDC_CFBLR_CFBLL_VALUE);
  ltdcLayer1Config.frameBufferColorFormat     = LTDC::ColorFormat::RGB888;
  ltdcLayer1Config.frameBufferDimension.width = FRAME_BUFFER_WIDTH;
  expectSpecificRegisterSetWithNoChangesAfter(&(virtualLTDCPeripheralLayer1.CFBLR), bitValueMatcher);

  const LTDC::ErrorCode errorCode = virtualLTDC.init(ltdcConfig, ltdcLayer1Config);

  ASSERT_THAT(errorCode, Eq(LTDC::ErrorCode::OK));
  ASSERT_THAT(virtualLTDCPeripheralLayer1.CFBLR, bitValueMatcher);
}

TEST_F(ALTDC, InitSetsValueOfCFBPInInLayer1CFBLRRegisterAccordingToFrameBufferWidthAndColorFormat)
{
  constexpr uint16_t FRAME_BUFFER_WIDTH = 256u;
  constexpr uint32_t PIXEL_SIZE_RGB565 = 2u;
  constexpr uint32_t LTDC_CFBLR_CFBP_POSITION = 16u;
  constexpr uint32_t LTDC_CFBLR_CFBP_SIZE     = 13u;
  constexpr uint32_t EXPECTED_LTDC_CFBLR_CFBP_VALUE = FRAME_BUFFER_WIDTH * PIXEL_SIZE_RGB565;
  auto bitValueMatcher =
    BitsHaveValue(LTDC_CFBLR_CFBP_POSITION, LTDC_CFBLR_CFBP_SIZE, EXPECTED_LTDC_CFBLR_CFBP_VALUE);
  ltdcLayer1Config.frameBufferColorFormat     = LTDC::ColorFormat::RGB565;
  ltdcLayer1Config.frameBufferDimension.width = FRAME_BUFFER_WIDTH;
  expectSpecificRegisterSetWithNoChangesAfter(&(virtualLTDCPeripheralLayer1.CFBLR), bitValueMatcher);

  const LTDC::ErrorCode errorCode = virtualLTDC.init(ltdcConfig, ltdcLayer1Config);

  ASSERT_THAT(errorCode, Eq(LTDC::ErrorCode::OK));
  ASSERT_THAT(virtualLTDCPeripheralLayer1.CFBLR, bitValueMatcher);
}

TEST_F(ALTDC, InitSetsValueOfCFBLNBRInInLayer1CFBLNRRegisterAccordingToFrameBufferHeight)
{
  constexpr uint16_t FRAME_BUFFER_HEIGHT = 390u;
  constexpr uint32_t LTDC_CFBLNR_CFBLNBR_POSITION = 0u;
  constexpr uint32_t LTDC_CFBLNR_CFBLNBR_SIZE     = 11u;
  constexpr uint32_t EXPECTED_LTDC_CFBLNR_CFBLNBR_VALUE = FRAME_BUFFER_HEIGHT;
  auto bitValueMatcher =
    BitsHaveValue(LTDC_CFBLNR_CFBLNBR_POSITION, LTDC_CFBLNR_CFBLNBR_SIZE, EXPECTED_LTDC_CFBLNR_CFBLNBR_VALUE);
  ltdcLayer1Config.frameBufferDimension.height = FRAME_BUFFER_HEIGHT;
  expectSpecificRegisterSetWithNoChangesAfter(&(virtualLTDCPeripheralLayer1.CFBLNR), bitValueMatcher);

  const LTDC::ErrorCode errorCode = virtualLTDC.init(ltdcConfig, ltdcLayer1Config);

  ASSERT_THAT(errorCode, Eq(LTDC::ErrorCode::OK));
  ASSERT_THAT(virtualLTDCPeripheralLayer1.CFBLNR, bitValueMatcher);
}

TEST_F(ALTDC, InitEnablesLayer1BySettingToOneLENBitInLayer1CRRegister)
{
  constexpr uint32_t LTDC_CR_LEN_POSITION = 0u;
  constexpr uint32_t EXPECTED_LTDC_CR_LEN_VALUE = 0x1;
  auto bitValueMatcher =
    BitHasValue(LTDC_CR_LEN_POSITION, EXPECTED_LTDC_CR_LEN_VALUE);
  expectSpecificRegisterSetWithNoChangesAfter(&(virtualLTDCPeripheralLayer1.CR), bitValueMatcher);

  const LTDC::ErrorCode errorCode = virtualLTDC.init(ltdcConfig, ltdcLayer1Config);

  ASSERT_THAT(errorCode, Eq(LTDC::ErrorCode::OK));
  ASSERT_THAT(virtualLTDCPeripheralLayer1.CR, bitValueMatcher);
}

TEST_F(ALTDC, InitDisablesLayer2BySettingToZeroLENBitInLayer2CRRegister)
{
  constexpr uint32_t LTDC_CR_LEN_POSITION = 0u;
  constexpr uint32_t EXPECTED_LTDC_CR_LEN_VALUE = 0x0;
  auto bitValueMatcher =
    BitHasValue(LTDC_CR_LEN_POSITION, EXPECTED_LTDC_CR_LEN_VALUE);
  expectSpecificRegisterSetWithNoChangesAfter(&(virtualLTDCPeripheralLayer2.CR), bitValueMatcher);

  const LTDC::ErrorCode errorCode = virtualLTDC.init(ltdcConfig, ltdcLayer1Config);

  ASSERT_THAT(errorCode, Eq(LTDC::ErrorCode::OK));
  ASSERT_THAT(virtualLTDCPeripheralLayer2.CR, bitValueMatcher);
}
