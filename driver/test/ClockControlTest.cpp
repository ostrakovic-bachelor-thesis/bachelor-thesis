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

  void setPLL(uint32_t PLLM, uint32_t PLLN, uint32_t PLLR, uint32_t PLLP, uint32_t PLLQ, uint32_t PLLSRC);
  void setMSI(uint32_t msiRange, bool isMsiRangeInCR);
  void setSysClockSource(uint32_t SWS);
  void setAHBPrescaler(uint32_t prescaler);
  void setAPB1Prescaler(uint32_t prescaler);
  void setAPB2Prescaler(uint32_t prescaler);
  void setUSART1ClockSource(uint32_t clockSource);

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


TEST_F(AClockControl, GetsHSIClockFrequency)
{
  constexpr uint32_t EXPECTED_HSI_CLOCK_FREQUENCY = 16000000u; // 16 MHz
  expectNoRegisterToChange();

  uint32_t clockFrequency;
  const ClockControl::ErrorCode errorCode = 
    virtualClockControl.getClockFrequency(ClockControl::ClockSource::HSI, clockFrequency);

  ASSERT_THAT(errorCode, Eq(ClockControl::ErrorCode::OK));
  ASSERT_THAT(clockFrequency, Eq(EXPECTED_HSI_CLOCK_FREQUENCY));
}

TEST_F(AClockControl, GetsHSEClockFrequency)
{
  constexpr uint32_t EXPECTED_HSE_CLOCK_FREQUENCY = 16000000u; // 16 MHz
  expectNoRegisterToChange();

  uint32_t clockFrequency;
  const ClockControl::ErrorCode errorCode = 
    virtualClockControl.getClockFrequency(ClockControl::ClockSource::HSE, clockFrequency);

  ASSERT_THAT(errorCode, Eq(ClockControl::ErrorCode::OK));
  ASSERT_THAT(clockFrequency, Eq(EXPECTED_HSE_CLOCK_FREQUENCY));
}

TEST_F(AClockControl, GetsLSEClockFrequency)
{
  constexpr uint32_t EXPECTED_LSE_CLOCK_FREQUENCY = 32768u; // Hz
  expectNoRegisterToChange();

  uint32_t clockFrequency;
  const ClockControl::ErrorCode errorCode = 
    virtualClockControl.getClockFrequency(ClockControl::ClockSource::LSE, clockFrequency);

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
    virtualClockControl.getClockFrequency(ClockControl::ClockSource::MSI, clockFrequency);

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
    virtualClockControl.getClockFrequency(ClockControl::ClockSource::MSI, clockFrequency);

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
    virtualClockControl.getClockFrequency(ClockControl::ClockSource::PLL, clockFrequency);

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
    virtualClockControl.getClockFrequency(ClockControl::ClockSource::SYSTEM_CLOCK, clockFrequency);

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
    virtualClockControl.getClockFrequency(ClockControl::ClockSource::AHB, clockFrequency);

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
    virtualClockControl.getClockFrequency(ClockControl::ClockSource::APB1, clockFrequency);

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
    virtualClockControl.getClockFrequency(ClockControl::ClockSource::APB2, clockFrequency);

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