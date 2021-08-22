#include "I2C.h"
#include "MemoryUtility.h"
#include "MemoryAccess.h"
#include "DriverTest.h"
#include "ClockControlMock.h"
#include "gtest/gtest.h"
#include "gmock/gmock.h"


using namespace ::testing;


class AnI2C : public DriverTest
{
public:

  //! Based on real reset values for I2C registers (source STM32L4R9 reference manual)
  static constexpr uint32_t I2C_CR1_RESET_VALUE      = 0x00000000;
  static constexpr uint32_t I2C_CR2_RESET_VALUE      = 0x00000000;
  static constexpr uint32_t I2C_OAR1_RESET_VALUE     = 0x00000000;
  static constexpr uint32_t I2C_OAR2_RESET_VALUE     = 0x00000000;
  static constexpr uint16_t I2C_TIMINGR_RESET_VALUE  = 0x00000000;
  static constexpr uint32_t I2C_TIMEOUTR_RESET_VALUE = 0x00000000;
  static constexpr uint16_t I2C_ISR_RESET_VALUE      = 0x00000000;
  static constexpr uint32_t I2C_ICR_RESET_VALUE      = 0x00000000;
  static constexpr uint16_t I2C_PECR_RESET_VALUE     = 0x00000000;
  static constexpr uint16_t I2C_RXDR_RESET_VALUE     = 0x00000000;
  static constexpr uint32_t I2C_TXDR_RESET_VALUE     = 0x00000000;

  I2C_TypeDef virtualI2CPeripheral;
  I2C virtualI2C = I2C(&virtualI2CPeripheral);

  void SetUp() override;
  void TearDown() override;
};

void AnI2C::SetUp()
{
  DriverTest::SetUp();

  // set values of virtual I2C peripheral to reset values
  virtualI2CPeripheral.CR1      = I2C_CR1_RESET_VALUE;
  virtualI2CPeripheral.CR2      = I2C_CR2_RESET_VALUE;
  virtualI2CPeripheral.OAR1     = I2C_OAR1_RESET_VALUE;
  virtualI2CPeripheral.OAR2     = I2C_OAR2_RESET_VALUE;
  virtualI2CPeripheral.TIMINGR  = I2C_TIMINGR_RESET_VALUE;
  virtualI2CPeripheral.TIMEOUTR = I2C_TIMEOUTR_RESET_VALUE;
  virtualI2CPeripheral.ISR      = I2C_ISR_RESET_VALUE;
  virtualI2CPeripheral.ICR      = I2C_ICR_RESET_VALUE;
  virtualI2CPeripheral.PECR     = I2C_PECR_RESET_VALUE;
  virtualI2CPeripheral.RXDR     = I2C_RXDR_RESET_VALUE;
  virtualI2CPeripheral.TXDR     = I2C_TXDR_RESET_VALUE;
}

void AnI2C::TearDown()
{
  DriverTest::TearDown();
}

TEST_F(AnI2C, InitDisablesI2COnTheBeginningOfInitFunctionCall)
{
  constexpr uint32_t I2C_CR1_PE_POSITION = 0u;
  constexpr uint32_t EXPECTED_I2C_CR1_PE_VALUE = 0u;
  auto bitValueMatcher =
    BitHasValue(I2C_CR1_PE_POSITION, EXPECTED_I2C_CR1_PE_VALUE);
  expectSpecificRegisterSetToBeCalledFirst(&(virtualI2CPeripheral.CR1), bitValueMatcher);

  const I2C::ErrorCode errorCode = virtualI2C.init();

  ASSERT_THAT(errorCode, Eq(I2C::ErrorCode::OK));
}

TEST_F(AnI2C, InitEnablesI2COnTheEndOfInitFunctionCall)
{
  constexpr uint32_t I2C_CR1_PE_POSITION = 0u;
  constexpr uint32_t EXPECTED_I2C_CR1_PE_VALUE = 1u;
  auto bitValueMatcher =
    BitHasValue(I2C_CR1_PE_POSITION, EXPECTED_I2C_CR1_PE_VALUE);
  expectSpecificRegisterSetToBeCalledLast(&(virtualI2CPeripheral.CR1), bitValueMatcher);

  const I2C::ErrorCode errorCode = virtualI2C.init();

  ASSERT_THAT(errorCode, Eq(I2C::ErrorCode::OK));
}