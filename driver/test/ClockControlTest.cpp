#include "ClockControl.h"
#include "MemoryUtility.h"
#include "MemoryAccess.h"
#include "DriverTest.h"
#include "gtest/gtest.h"
#include "gmock/gmock.h"


using namespace ::testing;


class AClockControl : public DriverTest
{
public:

  //! Based on real reset values for RCC register (source STM32L4R9 reference manual)
  static constexpr uint32_t RCC_AHB1ENR_RESET_VALUE  = 0x00000100;
  static constexpr uint32_t RCC_AHB2ENR_RESET_VALUE  = 0x00000000;
  static constexpr uint32_t RCC_AHB3ENR_RESET_VALUE  = 0x00000000;
  static constexpr uint32_t RCC_APB1ENR1_RESET_VALUE = 0x00000400;
  static constexpr uint32_t RCC_APB1ENR2_RESET_VALUE = 0x00000000;
  static constexpr uint32_t RCC_APB2ENR_RESET_VALUE  = 0x00000000;

  RCC_TypeDef virtualRCCPeripheral;
  ClockControl virtualClockControl = ClockControl(&virtualRCCPeripheral);
  ClockControl::PLLConfiguration pllConfig;

  void setPLL(uint32_t PLLM, uint32_t PLLN, uint32_t PLLR, uint32_t PLLP, uint32_t PLLQ, uint32_t PLLSRC);
  void setMSI(uint32_t msiRange, bool isMsiRangeInCR);
  void setSysClockSource(uint32_t SWS);
  void setAHBPrescaler(uint32_t prescaler);
  void setAPB1Prescaler(uint32_t prescaler);
  void setAPB2Prescaler(uint32_t prescaler);
  void setUSART1ClockSource(uint32_t clockSource);
  void setUSART2ClockSource(uint32_t clockSource);
  void setI2C1ClockSource(uint32_t clockSource);

  void SetUp() override;
  void TearDown() override;
};

void AClockControl::SetUp()
{
  DriverTest::SetUp();

  // set values of virtual RCC peripheral to reset values
}

void AClockControl::TearDown()
{
  DriverTest::TearDown();
}

void AClockControl::setPLL(uint32_t PLLM, uint32_t PLLN, uint32_t PLLR, uint32_t PLLP, uint32_t PLLQ, uint32_t PLLSRC)
{
  virtualRCCPeripheral.PLLCFGR =
    (((PLLM - 1) << RCC_PLLCFGR_PLLM_Pos) & RCC_PLLCFGR_PLLM_Msk) |
    ((PLLN << RCC_PLLCFGR_PLLN_Pos) & RCC_PLLCFGR_PLLN_Msk) |
    (PLLSRC) |
    ((((PLLQ >> 1) - 1) << RCC_PLLCFGR_PLLQ_Pos) & RCC_PLLCFGR_PLLQ_Msk) |
    ((((PLLR >> 1) - 1) << RCC_PLLCFGR_PLLR_Pos) & RCC_PLLCFGR_PLLR_Msk)|
    ((PLLP << RCC_PLLCFGR_PLLP_Pos) & RCC_PLLCFGR_PLLP_Msk);
}

void AClockControl::setMSI(uint32_t msiRange, bool isMsiRangeInCR)
{
  virtualRCCPeripheral.CR = isMsiRangeInCR ? RCC_CR_MSIRGSEL : 0u;

  if (isMsiRangeInCR)
  {
    virtualRCCPeripheral.CR |= (msiRange << RCC_CR_MSIRANGE_Pos) & RCC_CR_MSIRANGE_Msk;
  }
  else
  {
    virtualRCCPeripheral.CSR = (msiRange << RCC_CSR_MSISRANGE_Pos) & RCC_CSR_MSISRANGE_Msk;
  }
}

void AClockControl::setSysClockSource(uint32_t SWS)
{
  virtualRCCPeripheral.CFGR = (SWS & RCC_CFGR_SWS_Msk);
}

void AClockControl::setAHBPrescaler(uint32_t prescaler)
{
  virtualRCCPeripheral.CFGR |= (prescaler & RCC_CFGR_HPRE_Msk);
}

void AClockControl::setAPB1Prescaler(uint32_t prescaler)
{
  virtualRCCPeripheral.CFGR |= (prescaler & RCC_CFGR_PPRE1_Msk);
}

void AClockControl::setAPB2Prescaler(uint32_t prescaler)
{
  virtualRCCPeripheral.CFGR |= (prescaler & RCC_CFGR_PPRE2_Msk);
}

void AClockControl::setUSART1ClockSource(uint32_t clockSource)
{
  virtualRCCPeripheral.CCIPR= (clockSource << RCC_CCIPR_USART1SEL_Pos) & RCC_CCIPR_USART1SEL_Msk;
}

void AClockControl::setUSART2ClockSource(uint32_t clockSource)
{
  virtualRCCPeripheral.CCIPR= (clockSource << RCC_CCIPR_USART2SEL_Pos) & RCC_CCIPR_USART2SEL_Msk;
}

void AClockControl::setI2C1ClockSource(uint32_t clockSource)
{
  virtualRCCPeripheral.CCIPR= (clockSource << RCC_CCIPR_I2C1SEL_Pos) & RCC_CCIPR_I2C1SEL_Msk;
}

TEST_F(AClockControl, GetsHSIClockFrequency)
{
  constexpr uint32_t EXPECTED_HSI_CLOCK_FREQUENCY = 16000000u; // 16 MHz
  expectNoRegisterToChange();

  uint32_t clockFrequency;
  const ClockControl::ErrorCode errorCode =
    virtualClockControl.getClockFrequency(ClockControl::Clock::HSI, clockFrequency);

  ASSERT_THAT(errorCode, Eq(ClockControl::ErrorCode::OK));
  ASSERT_THAT(clockFrequency, Eq(EXPECTED_HSI_CLOCK_FREQUENCY));
}

TEST_F(AClockControl, GetsHSEClockFrequency)
{
  constexpr uint32_t EXPECTED_HSE_CLOCK_FREQUENCY = 16000000u; // 16 MHz
  expectNoRegisterToChange();

  uint32_t clockFrequency;
  const ClockControl::ErrorCode errorCode =
    virtualClockControl.getClockFrequency(ClockControl::Clock::HSE, clockFrequency);

  ASSERT_THAT(errorCode, Eq(ClockControl::ErrorCode::OK));
  ASSERT_THAT(clockFrequency, Eq(EXPECTED_HSE_CLOCK_FREQUENCY));
}

TEST_F(AClockControl, GetsLSEClockFrequency)
{
  constexpr uint32_t EXPECTED_LSE_CLOCK_FREQUENCY = 32768u; // Hz
  expectNoRegisterToChange();

  uint32_t clockFrequency;
  const ClockControl::ErrorCode errorCode =
    virtualClockControl.getClockFrequency(ClockControl::Clock::LSE, clockFrequency);

  ASSERT_THAT(errorCode, Eq(ClockControl::ErrorCode::OK));
  ASSERT_THAT(clockFrequency, Eq(EXPECTED_LSE_CLOCK_FREQUENCY));
}

TEST_F(AClockControl, GetMSIClockFrequencyFromCR)
{
  constexpr uint32_t MSI_RANGE_VALUE = 0b1001;
  constexpr uint32_t EXPECTED_MSI_CLOCK_FREQUENCY = 24000000u; // 24 MHz
  setMSI(MSI_RANGE_VALUE, true);
  expectNoRegisterToChange();

  uint32_t clockFrequency;
  const ClockControl::ErrorCode errorCode =
    virtualClockControl.getClockFrequency(ClockControl::Clock::MSI, clockFrequency);

  ASSERT_THAT(errorCode, Eq(ClockControl::ErrorCode::OK));
  ASSERT_THAT(clockFrequency, Eq(EXPECTED_MSI_CLOCK_FREQUENCY));
}

TEST_F(AClockControl, GetMSIClockFrequencyFromCSR)
{
  constexpr uint32_t MSI_RANGE_VALUE = 0b0101; // corresponding with 2 MHz frequency
  constexpr uint32_t EXPECTED_MSI_CLOCK_FREQUENCY = 2000000u; // 2 MHz
  setMSI(MSI_RANGE_VALUE, false);
  expectNoRegisterToChange();

  uint32_t clockFrequency;
  const ClockControl::ErrorCode errorCode =
    virtualClockControl.getClockFrequency(ClockControl::Clock::MSI, clockFrequency);

  ASSERT_THAT(errorCode, Eq(ClockControl::ErrorCode::OK));
  ASSERT_THAT(clockFrequency, Eq(EXPECTED_MSI_CLOCK_FREQUENCY));
}

TEST_F(AClockControl, GetPLLClockFrequency)
{
  constexpr uint32_t EXPECTED_PLL_CLOCK_FREQUENCY = 120000000u; // 120 MHz
  setPLL(1, 15, 2, 2, 0, RCC_PLLCFGR_PLLSRC_HSE);
  expectNoRegisterToChange();

  uint32_t clockFrequency;
  const ClockControl::ErrorCode errorCode =
    virtualClockControl.getClockFrequency(ClockControl::Clock::PLL, clockFrequency);

  ASSERT_THAT(errorCode, Eq(ClockControl::ErrorCode::OK));
  ASSERT_THAT(clockFrequency, Eq(EXPECTED_PLL_CLOCK_FREQUENCY));
}

TEST_F(AClockControl, GetsSystemClockFrequency)
{
  constexpr uint32_t EXPECTED_SYSTEM_CLOCK_FREQUENCY = 120000000u; // 120 MHz
  setPLL(1, 15, 2, 7, 4, RCC_PLLCFGR_PLLSRC_HSE);
  setSysClockSource(RCC_CFGR_SWS_PLL);
  expectNoRegisterToChange();

  uint32_t clockFrequency;
  const ClockControl::ErrorCode errorCode =
    virtualClockControl.getClockFrequency(ClockControl::Clock::SYSTEM_CLOCK, clockFrequency);

  ASSERT_THAT(errorCode, Eq(ClockControl::ErrorCode::OK));
  ASSERT_THAT(clockFrequency, Eq(EXPECTED_SYSTEM_CLOCK_FREQUENCY));
}

TEST_F(AClockControl, GetsAHBClockFrequency)
{
  constexpr uint32_t EXPECTED_AHB_CLOCK_FREQUENCY = 8000000u; // 8 MHz
  setSysClockSource(RCC_CFGR_SWS_HSI);
  setAHBPrescaler(RCC_CFGR_HPRE_DIV2);
  expectNoRegisterToChange();

  uint32_t clockFrequency;
  const ClockControl::ErrorCode errorCode =
    virtualClockControl.getClockFrequency(ClockControl::Clock::AHB, clockFrequency);

  ASSERT_THAT(errorCode, Eq(ClockControl::ErrorCode::OK));
  ASSERT_THAT(clockFrequency, Eq(EXPECTED_AHB_CLOCK_FREQUENCY));
}

TEST_F(AClockControl, GetsAPB1ClockFrequency)
{
  constexpr uint32_t EXPECTED_APB1_CLOCK_FREQUENCY = 4000000u; // 4 MHz
  setSysClockSource(RCC_CFGR_SWS_HSI);
  setAHBPrescaler(RCC_CFGR_HPRE_DIV1);
  setAPB1Prescaler(RCC_CFGR_PPRE1_DIV4);
  expectNoRegisterToChange();

  uint32_t clockFrequency;
  const ClockControl::ErrorCode errorCode =
    virtualClockControl.getClockFrequency(ClockControl::Clock::APB1, clockFrequency);

  ASSERT_THAT(errorCode, Eq(ClockControl::ErrorCode::OK));
  ASSERT_THAT(clockFrequency, Eq(EXPECTED_APB1_CLOCK_FREQUENCY));
}

TEST_F(AClockControl, GetsAPB2ClockFrequency)
{
  constexpr uint32_t EXPECTED_APB2_CLOCK_FREQUENCY = 8000000u; // 8 MHz
  setSysClockSource(RCC_CFGR_SWS_HSI);
  setAHBPrescaler(RCC_CFGR_HPRE_DIV1);
  setAPB2Prescaler(RCC_CFGR_PPRE2_DIV2);
  expectNoRegisterToChange();

  uint32_t clockFrequency;
  const ClockControl::ErrorCode errorCode =
    virtualClockControl.getClockFrequency(ClockControl::Clock::APB2, clockFrequency);

  ASSERT_THAT(errorCode, Eq(ClockControl::ErrorCode::OK));
  ASSERT_THAT(clockFrequency, Eq(EXPECTED_APB2_CLOCK_FREQUENCY));
}

TEST_F(AClockControl, GetsUSART1ClockFrequency)
{
  constexpr uint32_t USART1_CLOCK_SOURCE_HSI = 0b10;
  constexpr uint32_t EXPECTED_USART1_CLOCK_FREQUENCY = 16000000u; // 16 MHz
  setUSART1ClockSource(USART1_CLOCK_SOURCE_HSI);
  expectNoRegisterToChange();

  uint32_t clockFrequency;
  const ClockControl::ErrorCode errorCode = virtualClockControl.getClockFrequency(Peripheral::USART1, clockFrequency);

  ASSERT_THAT(errorCode, Eq(ClockControl::ErrorCode::OK));
  ASSERT_THAT(clockFrequency, Eq(EXPECTED_USART1_CLOCK_FREQUENCY));
}

TEST_F(AClockControl, GetsUSART2ClockFrequency)
{
  constexpr uint32_t USART2_CLOCK_SOURCE_PCLK = 0b00;
  constexpr uint32_t EXPECTED_USART2_CLOCK_FREQUENCY = 8000000u; // 8 MHz
  setSysClockSource(RCC_CFGR_SWS_HSI);
  setAHBPrescaler(RCC_CFGR_HPRE_DIV1);
  setAPB1Prescaler(RCC_CFGR_PPRE1_DIV2);
  setUSART2ClockSource(USART2_CLOCK_SOURCE_PCLK);
  expectNoRegisterToChange();

  uint32_t clockFrequency;
  const ClockControl::ErrorCode errorCode = virtualClockControl.getClockFrequency(Peripheral::USART2, clockFrequency);

  ASSERT_THAT(errorCode, Eq(ClockControl::ErrorCode::OK));
  ASSERT_THAT(clockFrequency, Eq(EXPECTED_USART2_CLOCK_FREQUENCY));
}

TEST_F(AClockControl, GetsI2C1ClockFrequency)
{
  constexpr uint32_t I2C1_CLOCK_SOURCE_PCLK = 0b00;
  constexpr uint32_t EXPECTED_I2C1_CLOCK_FREQUENCY = 8000000u; // 8 MHz
  setSysClockSource(RCC_CFGR_SWS_HSI);
  setAHBPrescaler(RCC_CFGR_HPRE_DIV1);
  setAPB1Prescaler(RCC_CFGR_PPRE1_DIV2);
  setI2C1ClockSource(I2C1_CLOCK_SOURCE_PCLK);
  expectNoRegisterToChange();

  uint32_t clockFrequency;
  const ClockControl::ErrorCode errorCode = virtualClockControl.getClockFrequency(Peripheral::I2C1, clockFrequency);

  ASSERT_THAT(errorCode, Eq(ClockControl::ErrorCode::OK));
  ASSERT_THAT(clockFrequency, Eq(EXPECTED_I2C1_CLOCK_FREQUENCY));
}

TEST_F(AClockControl, SetsPLLClockSource)
{
  constexpr uint32_t RCC_PLLCFGR_PLLSRC_POSITION = 0u;
  constexpr uint32_t RCC_PLLCFGR_PLLSRC_SIZE     = 2u;
  constexpr uint32_t EXPECTED_RCC_PLLCFGR_PLLSRC_VALUE = 0b01;
  auto bitsValueMatcher =
    BitsHaveValue(RCC_PLLCFGR_PLLSRC_POSITION, RCC_PLLCFGR_PLLSRC_SIZE, EXPECTED_RCC_PLLCFGR_PLLSRC_VALUE);
  expectSpecificRegisterSetWithNoChangesAfter(&(virtualRCCPeripheral.PLLCFGR), bitsValueMatcher);

  const ClockControl::ErrorCode errorCode =
    virtualClockControl.setClockSource(ClockControl::Clock::PLL, ClockControl::Clock::MSI);

  ASSERT_THAT(errorCode, Eq(ClockControl::ErrorCode::OK));
  ASSERT_THAT(virtualRCCPeripheral.PLLCFGR, bitsValueMatcher);
}

TEST_F(AClockControl, SetsPLLClockSourceFailsIfGivenClockCanNotBeUsedAsPLLClockSource)
{
  void expectNoRegisterToChange();

  const ClockControl::ErrorCode errorCode =
    virtualClockControl.setClockSource(ClockControl::Clock::PLL, ClockControl::Clock::LSE);

  ASSERT_THAT(errorCode, Eq(ClockControl::ErrorCode::INVALID_CLOCK_SOURCE));
}

TEST_F(AClockControl, ConfigurePLLSAI2SetsPLLSAI2MInPLLSAI2CFGRAccordingToInputClockDivider)
{
  constexpr uint8_t PLLSAI2_INPUT_CLOCK_DIVIDER = 15u;
  constexpr uint32_t RCC_PLLSAI2CFGR_PLLSAI2M_POSITION = 4u;
  constexpr uint32_t RCC_PLLSAI2CFGR_PLLSAI2M_SIZE     = 4u;
  constexpr uint32_t EXPECTED_RCC_PLLSAI2CFGR_PLLSAI2M_VALUE = PLLSAI2_INPUT_CLOCK_DIVIDER - 1u;
  pllConfig.inputClockDivider = PLLSAI2_INPUT_CLOCK_DIVIDER;
  auto bitsValueMatcher = BitsHaveValue(RCC_PLLSAI2CFGR_PLLSAI2M_POSITION,
    RCC_PLLSAI2CFGR_PLLSAI2M_SIZE,
    EXPECTED_RCC_PLLSAI2CFGR_PLLSAI2M_VALUE);
  expectSpecificRegisterSetWithNoChangesAfter(&(virtualRCCPeripheral.PLLSAI2CFGR), bitsValueMatcher);

  const ClockControl::ErrorCode errorCode =
    virtualClockControl.configurePLL(ClockControl::Clock::PLLSAI2, pllConfig);

  ASSERT_THAT(errorCode, Eq(ClockControl::ErrorCode::OK));
  ASSERT_THAT(virtualRCCPeripheral.PLLSAI2CFGR, bitsValueMatcher);
}