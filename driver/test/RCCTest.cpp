#include "RCC.h"
#include "MemoryUtility.h"
#include "MemoryAccess.h"
#include "DriverTest.h"
#include "gtest/gtest.h"
#include "gmock/gmock.h"


using namespace ::testing;


class ARCC : public DriverTest
{
public:

  //! Based on real reset values for RCC register (soruce STM32L4R9 reference manual)
  static constexpr uint32_t RCC_AHB1ENR_RESET_VALUE  = 0x00000100;
  static constexpr uint32_t RCC_AHB2ENR_RESET_VALUE  = 0x00000000;
  static constexpr uint32_t RCC_AHB3ENR_RESET_VALUE  = 0x00000000;
  static constexpr uint32_t RCC_APB1ENR1_RESET_VALUE = 0x00000400;
  static constexpr uint32_t RCC_APB1ENR2_RESET_VALUE = 0x00000000;
  static constexpr uint32_t RCC_APB2ENR_RESET_VALUE  = 0x00000000;

  RCC_TypeDef virtualRCCPeripheral;
  RCC virtualRCC = RCC(&virtualRCCPeripheral);

  void setPLL(uint32_t PLLM, uint32_t PLLN, uint32_t PLLR, uint32_t PLLP, uint32_t PLLQ, uint32_t PLLSRC);
  void setMSI(uint32_t msiRange, bool isMsiRangeInCR);
  void setSysClockSource(uint32_t SWS);

  void SetUp() override;
  void TearDown() override;
};

void ARCC::SetUp()
{
  DriverTest::SetUp();

  // set values of virtual RCC peripheral to reset values
  virtualRCCPeripheral.AHB1ENR  = RCC_AHB1ENR_RESET_VALUE;
  virtualRCCPeripheral.AHB2ENR  = RCC_AHB2ENR_RESET_VALUE;
  virtualRCCPeripheral.AHB3ENR  = RCC_AHB3ENR_RESET_VALUE;
  virtualRCCPeripheral.APB1ENR1 = RCC_APB1ENR1_RESET_VALUE;
  virtualRCCPeripheral.APB1ENR2 = RCC_APB1ENR2_RESET_VALUE;
  virtualRCCPeripheral.APB2ENR  = RCC_APB2ENR_RESET_VALUE;
}

void ARCC::TearDown()
{
  DriverTest::TearDown();
}

void ARCC::setPLL(uint32_t PLLM, uint32_t PLLN, uint32_t PLLR, uint32_t PLLP, uint32_t PLLQ, uint32_t PLLSRC)
{
  virtualRCCPeripheral.PLLCFGR = 
    (((PLLM - 1) << RCC_PLLCFGR_PLLM_Pos) & RCC_PLLCFGR_PLLM_Msk) |
    ((PLLN << RCC_PLLCFGR_PLLN_Pos) & RCC_PLLCFGR_PLLN_Msk) | 
    (PLLSRC) | 
    ((((PLLQ >> 1) - 1) << RCC_PLLCFGR_PLLQ_Pos) & RCC_PLLCFGR_PLLQ_Msk) | 
    ((((PLLR >> 1) - 1) << RCC_PLLCFGR_PLLR_Pos) & RCC_PLLCFGR_PLLR_Msk)| 
    ((PLLP << RCC_PLLCFGR_PLLP_Pos) & RCC_PLLCFGR_PLLP_Msk);
}

void ARCC::setMSI(uint32_t msiRange, bool isMsiRangeInCR)
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

void ARCC::setSysClockSource(uint32_t SWS)
{
  virtualRCCPeripheral.CFGR = (SWS & RCC_CFGR_SWS_Msk);
}


TEST_F(ARCC, EnablesPeripheralClock)
{
  constexpr uint32_t CLOCK_ENABLED    = 1u;
  constexpr uint32_t POSITION_GPIOCEN = 2u;
  const uint32_t expectedAHB2ENRValue = 
    expectedRegVal(RCC_AHB2ENR_RESET_VALUE, POSITION_GPIOCEN, 1u, CLOCK_ENABLED);
  expectRegisterSetOnlyOnce(&(virtualRCCPeripheral.AHB2ENR), expectedAHB2ENRValue);
  
  const RCC::ErrorCode errorCode = virtualRCC.enablePeripheralClock(Peripheral::GPIOC);

  ASSERT_THAT(errorCode, Eq(RCC::ErrorCode::OK));
  ASSERT_THAT(virtualRCCPeripheral.AHB2ENR, Eq(expectedAHB2ENRValue));
}

TEST_F(ARCC, EnablesPeripheralClockFailsIfEnablingIsNotImplementedForPeripheral)
{
  expectNoRegisterToChange();
  
  const RCC::ErrorCode errorCode = virtualRCC.enablePeripheralClock(Peripheral::INVALID_PERIPHERAL);

  ASSERT_THAT(errorCode, Eq(RCC::ErrorCode::NOT_IMPLEMENTED));
}

TEST_F(ARCC, DisablesPeripheralClock)
{
  constexpr uint32_t CLOCK_DISABLED   = 0u;
  constexpr uint32_t POSITION_GPIOEEN = 4u;
  const uint32_t expectedAHB2ENRValue = 
    expectedRegVal(RCC_AHB2ENR_RESET_VALUE, POSITION_GPIOEEN, 1u, CLOCK_DISABLED);
  virtualRCC.enablePeripheralClock(Peripheral::GPIOE);
  expectRegisterSetOnlyOnce(&(virtualRCCPeripheral.AHB2ENR), expectedAHB2ENRValue);
  
  const RCC::ErrorCode errorCode = virtualRCC.disablePeripheralClock(Peripheral::GPIOE);

  ASSERT_THAT(errorCode, Eq(RCC::ErrorCode::OK));
  ASSERT_THAT(virtualRCCPeripheral.AHB2ENR, Eq(expectedAHB2ENRValue));
}

TEST_F(ARCC, DisablesPeripheralClockFailsIfDisablingIsNotImplementedForPeripheral)
{
  expectNoRegisterToChange();
  
  const RCC::ErrorCode errorCode = virtualRCC.disablePeripheralClock(Peripheral::INVALID_PERIPHERAL);

  ASSERT_THAT(errorCode, Eq(RCC::ErrorCode::NOT_IMPLEMENTED));
}

TEST_F(ARCC, ChecksIsPeripheralClockEnabled)
{
  virtualRCC.enablePeripheralClock(Peripheral::GPIOF);
  expectNoRegisterToChange();
  
  bool isEnabledGPIOFClock;
  const RCC::ErrorCode errorCode = virtualRCC.isPeripheralClockEnabled(Peripheral::GPIOF, isEnabledGPIOFClock);

  ASSERT_THAT(errorCode, Eq(RCC::ErrorCode::OK));
  ASSERT_THAT(isEnabledGPIOFClock, Eq(true));
}

TEST_F(ARCC, ChecksIsPeripheralClockEnabledFailsIfEnablingIsNotImplementedForPeripheral)
{
  expectNoRegisterToChange();
  
  bool isClockEnabled;
  const RCC::ErrorCode errorCode = virtualRCC.isPeripheralClockEnabled(Peripheral::INVALID_PERIPHERAL, isClockEnabled);

  ASSERT_THAT(errorCode, Eq(RCC::ErrorCode::NOT_IMPLEMENTED));
}

TEST_F(ARCC, GetSystemClockSource)
{
  setSysClockSource(RCC_CFGR_SWS_PLL);
  expectNoRegisterToChange();
  
  ASSERT_THAT(virtualRCC.getSystemClockSource(), Eq(RCC::ClockSource::PLL));
}

TEST_F(ARCC, GetHSIClockFrequency)
{
  constexpr uint32_t EXPECTED_HSI_CLOCK_FREQUENCY = 16000000u; // 16 MHz
  expectNoRegisterToChange();
  
  ASSERT_THAT(virtualRCC.getHSIClockFrequency(), Eq(EXPECTED_HSI_CLOCK_FREQUENCY));
}

TEST_F(ARCC, GetHSEClockFrequency)
{
  constexpr uint32_t EXPECTED_HSE_CLOCK_FREQUENCY = 16000000u; // 16 MHz
  expectNoRegisterToChange();
  
  ASSERT_THAT(virtualRCC.getHSEClockFrequency(), Eq(EXPECTED_HSE_CLOCK_FREQUENCY));
}

TEST_F(ARCC, GetMSIClockFrequencyFromCR)
{
  constexpr uint32_t MSI_RANGE_VALUE = 0b1001;
  constexpr uint32_t EXPECTED_MSI_CLOCK_FREQUENCY = 24000000u; // 24 MHz
  setMSI(MSI_RANGE_VALUE, true);
  expectNoRegisterToChange();
  
  ASSERT_THAT(virtualRCC.getMSIClockFrequency(), Eq(EXPECTED_MSI_CLOCK_FREQUENCY));
}

TEST_F(ARCC, GetMSIClockFrequencyFromCSR)
{
  constexpr uint32_t MSI_RANGE_VALUE = 0b0101; // corresponding with 2 MHz frequency
  constexpr uint32_t EXPECTED_MSI_CLOCK_FREQUENCY = 2000000u; // 2 MHz
  setMSI(MSI_RANGE_VALUE, false);
  expectNoRegisterToChange();
  
  ASSERT_THAT(virtualRCC.getMSIClockFrequency(), Eq(EXPECTED_MSI_CLOCK_FREQUENCY));
}

TEST_F(ARCC, GetPLLClockFrequency)
{
  constexpr uint32_t EXPECTED_PLL_CLOCK_FREQUENCY = 120000000u; // 120 MHz
  setPLL(1, 15, 2, 2, 0, RCC_PLLCFGR_PLLSRC_HSE);
  expectNoRegisterToChange();
  
  ASSERT_THAT(virtualRCC.getPLLClockFrequency(), Eq(EXPECTED_PLL_CLOCK_FREQUENCY));
}

TEST_F(ARCC, GetsSystemClockFrequency)
{
  constexpr uint32_t EXPECTED_SYSTEM_CLOCK_FREQUENCY = 120000000u; // 120 MHz
  setPLL(1, 15, 2, 7, 4, RCC_PLLCFGR_PLLSRC_HSE);
  setSysClockSource(RCC_CFGR_SWS_PLL);
  expectNoRegisterToChange();

  ASSERT_THAT(virtualRCC.getSystemClockFrequency(), Eq(EXPECTED_SYSTEM_CLOCK_FREQUENCY));
}