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
  NiceMock<ClockControlMock> clockControlMock;
  I2C virtualI2C = I2C(&virtualI2CPeripheral, &clockControlMock);
  I2C::I2CConfig i2cConfig;

  void setI2CInputClockFrequency(uint32_t clockFrequency);

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

void AnI2C::setI2CInputClockFrequency(uint32_t clockFrequency)
{
  clockControlMock.setReturnClockFrequency(clockFrequency);
}

TEST_F(AnI2C, InitDisablesI2COnTheBeginningOfInitFunctionCall)
{
  constexpr uint32_t I2C_CR1_PE_POSITION = 0u;
  constexpr uint32_t EXPECTED_I2C_CR1_PE_VALUE = 0u;
  auto bitValueMatcher =
    BitHasValue(I2C_CR1_PE_POSITION, EXPECTED_I2C_CR1_PE_VALUE);
  expectSpecificRegisterSetToBeCalledFirst(&(virtualI2CPeripheral.CR1), bitValueMatcher);

  const I2C::ErrorCode errorCode = virtualI2C.init(i2cConfig);

  ASSERT_THAT(errorCode, Eq(I2C::ErrorCode::OK));
}

TEST_F(AnI2C, InitEnablesI2COnTheEndOfInitFunctionCall)
{
  constexpr uint32_t I2C_CR1_PE_POSITION = 0u;
  constexpr uint32_t EXPECTED_I2C_CR1_PE_VALUE = 1u;
  auto bitValueMatcher =
    BitHasValue(I2C_CR1_PE_POSITION, EXPECTED_I2C_CR1_PE_VALUE);
  expectSpecificRegisterSetToBeCalledLast(&(virtualI2CPeripheral.CR1), bitValueMatcher);

  const I2C::ErrorCode errorCode = virtualI2C.init(i2cConfig);

  ASSERT_THAT(errorCode, Eq(I2C::ErrorCode::OK));
}

TEST_F(AnI2C, InitSetsADD10BitInCR2RegisterAccordingToChosenAddressingMode)
{
  constexpr uint32_t I2C_CR2_ADDR10_POSITION = 11u;
  constexpr uint32_t EXPECTED_I2C_CR2_ADDR10_VALUE = 0x1;
  auto bitValueMatcher =
    BitHasValue(I2C_CR2_ADDR10_POSITION, EXPECTED_I2C_CR2_ADDR10_VALUE);
  i2cConfig.addressingMode = I2C::AddressingMode::ADDRESS_10_BITS;
  expectSpecificRegisterSetWithNoChangesAfter(&(virtualI2CPeripheral.CR2), bitValueMatcher);

  const I2C::ErrorCode errorCode = virtualI2C.init(i2cConfig);

  ASSERT_THAT(errorCode, Eq(I2C::ErrorCode::OK));
  ASSERT_THAT(virtualI2CPeripheral.CR2, bitValueMatcher);
}

TEST_F(AnI2C, InitEnablesAutoEndModeBySettingAUTOENDBitInCR2Register)
{
  constexpr uint32_t I2C_CR2_AUTOEND_POSITION = 25u;
  constexpr uint32_t EXPECTED_I2C_CR2_AUTOEND_VALUE = 0x1;
  auto bitValueMatcher =
    BitHasValue(I2C_CR2_AUTOEND_POSITION, EXPECTED_I2C_CR2_AUTOEND_VALUE);
  expectSpecificRegisterSetWithNoChangesAfter(&(virtualI2CPeripheral.CR2), bitValueMatcher);

  const I2C::ErrorCode errorCode = virtualI2C.init(i2cConfig);

  ASSERT_THAT(errorCode, Eq(I2C::ErrorCode::OK));
  ASSERT_THAT(virtualI2CPeripheral.CR2, bitValueMatcher);
}

TEST_F(AnI2C, InitSetsPRESCInTIMINGRRegisterAccordingToChoosenSCLClockFrequency)
{
  constexpr uint32_t I2C_TIMINGR_PRESC_POSITION = 28u;
  constexpr uint32_t I2C_TIMINGR_PRESC_SIZE = 4u;
  constexpr uint32_t EXPECTED_I2C_TIMINGR_PRESC_VALUE = 0x3;
  i2cConfig.clockFrequencySCL = 100000u; // 100 kHz
  auto bitsValueMatcher =
    BitsHaveValue(I2C_TIMINGR_PRESC_POSITION, I2C_TIMINGR_PRESC_SIZE, EXPECTED_I2C_TIMINGR_PRESC_VALUE);
  expectRegisterSetOnlyOnce(&(virtualI2CPeripheral.TIMINGR), bitsValueMatcher);
  setI2CInputClockFrequency(16000000u); // 16 MHz

  const I2C::ErrorCode errorCode = virtualI2C.init(i2cConfig);

  ASSERT_THAT(errorCode, Eq(I2C::ErrorCode::OK));
  ASSERT_THAT(virtualI2CPeripheral.TIMINGR, bitsValueMatcher);
}

TEST_F(AnI2C, InitSetsSCLDELInTIMINGRRegisterAccordingToChoosenSCLClockFrequency)
{
  constexpr uint32_t I2C_TIMINGR_SCLDEL_POSITION = 20u;
  constexpr uint32_t I2C_TIMINGR_SCLDEL_SIZE = 4u;
  constexpr uint32_t EXPECTED_I2C_TIMINGR_SCLDEL_VALUE = 0x4;
  i2cConfig.clockFrequencySCL = 100000u; // 100 kHz
  auto bitsValueMatcher =
    BitsHaveValue(I2C_TIMINGR_SCLDEL_POSITION, I2C_TIMINGR_SCLDEL_SIZE, EXPECTED_I2C_TIMINGR_SCLDEL_VALUE);
  expectRegisterSetOnlyOnce(&(virtualI2CPeripheral.TIMINGR), bitsValueMatcher);
  setI2CInputClockFrequency(16000000u); // 16 MHz

  const I2C::ErrorCode errorCode = virtualI2C.init(i2cConfig);

  ASSERT_THAT(errorCode, Eq(I2C::ErrorCode::OK));
  ASSERT_THAT(virtualI2CPeripheral.TIMINGR, bitsValueMatcher);
}

TEST_F(AnI2C, InitSetsSDADELInTIMINGRRegisterAccordingToChoosenSCLClockFrequency)
{
  constexpr uint32_t I2C_TIMINGR_SDADEL_POSITION = 16u;
  constexpr uint32_t I2C_TIMINGR_SDADEL_SIZE = 4u;
  constexpr uint32_t EXPECTED_I2C_TIMINGR_SDADEL_VALUE = 0x2;
  i2cConfig.clockFrequencySCL = 100000u; // 100 kHz
  auto bitsValueMatcher =
    BitsHaveValue(I2C_TIMINGR_SDADEL_POSITION, I2C_TIMINGR_SDADEL_SIZE, EXPECTED_I2C_TIMINGR_SDADEL_VALUE);
  expectRegisterSetOnlyOnce(&(virtualI2CPeripheral.TIMINGR), bitsValueMatcher);
  setI2CInputClockFrequency(16000000u); // 16 MHz

  const I2C::ErrorCode errorCode = virtualI2C.init(i2cConfig);

  ASSERT_THAT(errorCode, Eq(I2C::ErrorCode::OK));
  ASSERT_THAT(virtualI2CPeripheral.TIMINGR, bitsValueMatcher);
}

TEST_F(AnI2C, InitSetsSCLHInTIMINGRRegisterAccordingToChoosenSCLClockFrequency)
{
  constexpr uint32_t I2C_TIMINGR_SCLH_POSITION = 8u;
  constexpr uint32_t I2C_TIMINGR_SCLH_SIZE = 8u;
  constexpr uint32_t EXPECTED_I2C_TIMINGR_SCLH_VALUE = 0xF;
  i2cConfig.clockFrequencySCL = 100000u; // 100 kHz
  auto bitsValueMatcher =
    BitsHaveValue(I2C_TIMINGR_SCLH_POSITION, I2C_TIMINGR_SCLH_SIZE, EXPECTED_I2C_TIMINGR_SCLH_VALUE);
  expectRegisterSetOnlyOnce(&(virtualI2CPeripheral.TIMINGR), bitsValueMatcher);
  setI2CInputClockFrequency(16000000u); // 16 MHz

  const I2C::ErrorCode errorCode = virtualI2C.init(i2cConfig);

  ASSERT_THAT(errorCode, Eq(I2C::ErrorCode::OK));
  ASSERT_THAT(virtualI2CPeripheral.TIMINGR, bitsValueMatcher);
}

TEST_F(AnI2C, InitSetsSCLLInTIMINGRRegisterAccordingToChoosenSCLClockFrequency)
{
  constexpr uint32_t I2C_TIMINGR_SCLL_POSITION = 0u;
  constexpr uint32_t I2C_TIMINGR_SCLL_SIZE = 8u;
  constexpr uint32_t EXPECTED_I2C_TIMINGR_SCLL_VALUE = 0x13;
  i2cConfig.clockFrequencySCL = 100000u; // 100 kHz
  auto bitsValueMatcher =
    BitsHaveValue(I2C_TIMINGR_SCLL_POSITION, I2C_TIMINGR_SCLL_SIZE, EXPECTED_I2C_TIMINGR_SCLL_VALUE);
  expectRegisterSetOnlyOnce(&(virtualI2CPeripheral.TIMINGR), bitsValueMatcher);
  setI2CInputClockFrequency(16000000u); // 16 MHz

  const I2C::ErrorCode errorCode = virtualI2C.init(i2cConfig);

  ASSERT_THAT(errorCode, Eq(I2C::ErrorCode::OK));
  ASSERT_THAT(virtualI2CPeripheral.TIMINGR, bitsValueMatcher);
}