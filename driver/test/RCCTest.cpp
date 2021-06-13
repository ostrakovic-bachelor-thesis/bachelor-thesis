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