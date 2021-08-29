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

  static const uint8_t  RANDOM_MSG[];
  static const uint32_t RANDOM_MSG_LEN;
  static const uint16_t RANDOM_SLAVE_ADDRESS;

  I2C_TypeDef virtualI2CPeripheral;
  NiceMock<ClockControlMock> clockControlMock;
  I2C virtualI2C = I2C(&virtualI2CPeripheral, &clockControlMock);
  I2C::I2CConfig i2cConfig;

  void setI2CInputClockFrequency(uint32_t clockFrequency);
  void setupISRRegisterReadings(void);

  void SetUp() override;
  void TearDown() override;
};

const uint8_t  AnI2C::RANDOM_MSG[]   = "Random message.";
const uint32_t AnI2C::RANDOM_MSG_LEN = sizeof(AnI2C::RANDOM_MSG);
const uint16_t AnI2C::RANDOM_SLAVE_ADDRESS = 0x7F;

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

  setI2CInputClockFrequency(12000000u); // 12 MHz

  i2cConfig.clockFrequencySCL = 100000u; // 100 kHz
}

void AnI2C::TearDown()
{
  DriverTest::TearDown();
}

void AnI2C::setI2CInputClockFrequency(uint32_t clockFrequency)
{
  clockControlMock.setReturnClockFrequency(clockFrequency);
}

void AnI2C::setupISRRegisterReadings(void)
{
  ON_CALL(memoryAccessHook, getRegisterValue(&(virtualI2CPeripheral.ISR)))
    .WillByDefault([&](volatile const uint32_t *registerPtr)
    {
      //constexpr uint32_t USART_ISR_TXFNF_POSITION = 7u;
      //virtualUSARTPeripheral.ISR =
      //  expectedRegVal(USART_ISR_RESET_VALUE, USART_ISR_TXFNF_POSITION, 1u, (((m_txCounter++) % 4u) != 0u));

      return 0u; //virtualUSARTPeripheral.ISR;
    });
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

TEST_F(AnI2C, InitDisablesReloadModeByClearingRELOADBitInCR2Register)
{
  constexpr uint32_t I2C_CR2_RELOAD_POSITION = 24u;
  constexpr uint32_t EXPECTED_I2C_CR2_RELOAD_VALUE = 0x0;
  auto bitValueMatcher =
    BitHasValue(I2C_CR2_RELOAD_POSITION, EXPECTED_I2C_CR2_RELOAD_VALUE);
  expectSpecificRegisterSetWithNoChangesAfter(&(virtualI2CPeripheral.CR2), bitValueMatcher);

  const I2C::ErrorCode errorCode = virtualI2C.init(i2cConfig);

  ASSERT_THAT(errorCode, Eq(I2C::ErrorCode::OK));
  ASSERT_THAT(virtualI2CPeripheral.CR2, bitValueMatcher);
}

TEST_F(AnI2C, InitDisablesAnalogNoiseFilterBySettingANFOFFBitInCR1RegisterToZero)
{
  constexpr uint32_t I2C_CR1_ANFOFF_POSITION = 12u;
  constexpr uint32_t EXPECTED_I2C_CR1_ANFOFF_VALUE = 0x0;
  auto bitValueMatcher =
    BitHasValue(I2C_CR1_ANFOFF_POSITION, EXPECTED_I2C_CR1_ANFOFF_VALUE);
  expectSpecificRegisterSetWithNoChangesAfter(&(virtualI2CPeripheral.CR1), bitValueMatcher);

  const I2C::ErrorCode errorCode = virtualI2C.init(i2cConfig);

  ASSERT_THAT(errorCode, Eq(I2C::ErrorCode::OK));
  ASSERT_THAT(virtualI2CPeripheral.CR1, bitValueMatcher);
}

TEST_F(AnI2C, InitDisablesDigitalNoiseFilterBySettingDNFBitsInCR1RegisterToZeros)
{
  constexpr uint32_t I2C_CR1_DNF_POSITION = 8u;
  constexpr uint32_t I2C_CR1_DNF_SIZE = 4u;
  constexpr uint32_t EXPECTED_I2C_CR1_DNF_VALUE = 0x0;
  auto bitValueMatcher =
    BitsHaveValue(I2C_CR1_DNF_POSITION, I2C_CR1_DNF_SIZE, EXPECTED_I2C_CR1_DNF_VALUE);
  expectSpecificRegisterSetWithNoChangesAfter(&(virtualI2CPeripheral.CR1), bitValueMatcher);

  const I2C::ErrorCode errorCode = virtualI2C.init(i2cConfig);

  ASSERT_THAT(errorCode, Eq(I2C::ErrorCode::OK));
  ASSERT_THAT(virtualI2CPeripheral.CR1, bitValueMatcher);
}

TEST_F(AnI2C, InitDisablesClockStretchingBySettingNOSTRETCHBitInCR1RegisterToOne)
{
  constexpr uint32_t I2C_CR1_NOSTRETCH_POSITION = 17u;
  constexpr uint32_t EXPECTED_I2C_CR1_NOSTRETCH_VALUE = 0x1;
  auto bitValueMatcher =
    BitHasValue(I2C_CR1_NOSTRETCH_POSITION, EXPECTED_I2C_CR1_NOSTRETCH_VALUE);
  expectSpecificRegisterSetWithNoChangesAfter(&(virtualI2CPeripheral.CR1), bitValueMatcher);

  const I2C::ErrorCode errorCode = virtualI2C.init(i2cConfig);

  ASSERT_THAT(errorCode, Eq(I2C::ErrorCode::OK));
  ASSERT_THAT(virtualI2CPeripheral.CR1, bitValueMatcher);
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
  constexpr uint32_t EXPECTED_I2C_TIMINGR_SCLDEL_VALUE = 0x7;
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
  constexpr uint32_t EXPECTED_I2C_TIMINGR_SCLH_VALUE = 0x11;
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

TEST_F(AnI2C, WriteEnablesTransmitInterrupt)
{
  constexpr uint32_t I2C_CR1_TXIE_POSITION = 1u;
  constexpr uint32_t EXPECTED_I2C_CR1_TXIE_VALUE = 0x1;
  auto bitValueMatcher =
    BitHasValue(I2C_CR1_TXIE_POSITION, EXPECTED_I2C_CR1_TXIE_VALUE);
  expectSpecificRegisterSetWithNoChangesAfter(&(virtualI2CPeripheral.CR1), bitValueMatcher);

  const I2C::ErrorCode errorCode = virtualI2C.write(RANDOM_SLAVE_ADDRESS, RANDOM_MSG, RANDOM_MSG_LEN);

  ASSERT_THAT(errorCode, Eq(I2C::ErrorCode::OK));
  ASSERT_THAT(virtualI2CPeripheral.CR1, bitValueMatcher);
}

TEST_F(AnI2C, WriteEnablesTransferCompleteInterrupt)
{
  constexpr uint32_t I2C_CR1_TCIE_POSITION = 6u;
  constexpr uint32_t EXPECTED_I2C_CR1_TCIE_VALUE = 0x1;
  auto bitValueMatcher =
    BitHasValue(I2C_CR1_TCIE_POSITION, EXPECTED_I2C_CR1_TCIE_VALUE);
  expectSpecificRegisterSetWithNoChangesAfter(&(virtualI2CPeripheral.CR1), bitValueMatcher);

  const I2C::ErrorCode errorCode = virtualI2C.write(RANDOM_SLAVE_ADDRESS, RANDOM_MSG, RANDOM_MSG_LEN);

  ASSERT_THAT(errorCode, Eq(I2C::ErrorCode::OK));
  ASSERT_THAT(virtualI2CPeripheral.CR1, bitValueMatcher);
}

TEST_F(AnI2C, WriteSetsInNBYTESBitsInCR2RegisterNumberOfBytesToWrite)
{
  constexpr uint32_t I2C_CR2_NBYTES_POSITION = 16u;
  constexpr uint32_t I2C_CR2_NBYTES_SIZE = 8u;
  constexpr uint32_t EXPECTED_I2C_CR2_NBYTES_VALUE = RANDOM_MSG_LEN;
  auto bitValueMatcher =
    BitsHaveValue(I2C_CR2_NBYTES_POSITION, I2C_CR2_NBYTES_SIZE, EXPECTED_I2C_CR2_NBYTES_VALUE);
  expectRegisterSetOnlyOnce(&(virtualI2CPeripheral.CR2), bitValueMatcher);

  const I2C::ErrorCode errorCode = virtualI2C.write(RANDOM_SLAVE_ADDRESS, RANDOM_MSG, RANDOM_MSG_LEN);

  ASSERT_THAT(errorCode, Eq(I2C::ErrorCode::OK));
  ASSERT_THAT(virtualI2CPeripheral.CR2, bitValueMatcher);
}

TEST_F(AnI2C, WriteSetsSlaveAddressInSADDBits1To7InCR2RegisterIfChoosenAddressingModeIs7Bits)
{
  constexpr uint32_t I2C_CR2_SADD_POSITION = 1u;
  constexpr uint32_t I2C_CR2_SADD_SIZE = 7u;
  constexpr uint32_t EXPECTED_I2C_CR2_SADD_VALUE = RANDOM_SLAVE_ADDRESS;
  auto bitValueMatcher =
    BitsHaveValue(I2C_CR2_SADD_POSITION, I2C_CR2_SADD_SIZE, EXPECTED_I2C_CR2_SADD_VALUE);
  i2cConfig.addressingMode = I2C::AddressingMode::ADDRESS_7_BITS;
  virtualI2C.init(i2cConfig);
  expectRegisterSetOnlyOnce(&(virtualI2CPeripheral.CR2), bitValueMatcher);

  const I2C::ErrorCode errorCode = virtualI2C.write(RANDOM_SLAVE_ADDRESS, RANDOM_MSG, RANDOM_MSG_LEN);

  ASSERT_THAT(errorCode, Eq(I2C::ErrorCode::OK));
  ASSERT_THAT(virtualI2CPeripheral.CR2, bitValueMatcher);
}

TEST_F(AnI2C, WriteSetsSlaveAddressInSADDBits0To9InCR2RegisterIfChoosenAddressingModeIs10Bits)
{
  constexpr uint32_t I2C_CR2_SADD_POSITION = 0u;
  constexpr uint32_t I2C_CR2_SADD_SIZE = 10u;
  constexpr uint32_t EXPECTED_I2C_CR2_SADD_VALUE = RANDOM_SLAVE_ADDRESS;
  auto bitValueMatcher =
    BitsHaveValue(I2C_CR2_SADD_POSITION, I2C_CR2_SADD_SIZE, EXPECTED_I2C_CR2_SADD_VALUE);
  i2cConfig.addressingMode = I2C::AddressingMode::ADDRESS_10_BITS;
  virtualI2C.init(i2cConfig);
  expectRegisterSetOnlyOnce(&(virtualI2CPeripheral.CR2), bitValueMatcher);

  const I2C::ErrorCode errorCode = virtualI2C.write(RANDOM_SLAVE_ADDRESS, RANDOM_MSG, RANDOM_MSG_LEN);

  ASSERT_THAT(errorCode, Eq(I2C::ErrorCode::OK));
  ASSERT_THAT(virtualI2CPeripheral.CR2, bitValueMatcher);
}

TEST_F(AnI2C, WriteSetsTransferDirectionToWriteBySettingRD_WRNBitInCR2RegisterToZero)
{
  constexpr uint32_t I2C_CR2_RD_WRN_POSITION = 10u;
  constexpr uint32_t EXPECTED_I2C_CR2_RD_WRN_VALUE = 0x0;
  auto bitValueMatcher =
    BitHasValue(I2C_CR2_RD_WRN_POSITION, EXPECTED_I2C_CR2_RD_WRN_VALUE);
  expectRegisterSetOnlyOnce(&(virtualI2CPeripheral.CR2), bitValueMatcher);

  const I2C::ErrorCode errorCode = virtualI2C.write(RANDOM_SLAVE_ADDRESS, RANDOM_MSG, RANDOM_MSG_LEN);

  ASSERT_THAT(errorCode, Eq(I2C::ErrorCode::OK));
  ASSERT_THAT(virtualI2CPeripheral.CR2, bitValueMatcher);
}

TEST_F(AnI2C, WriteStartsTransferBySettingSTARTBitInCR2Register)
{
  constexpr uint32_t I2C_CR2_START_POSITION = 13u;
  constexpr uint32_t EXPECTED_I2C_CR2_START_VALUE = 0x1;
  auto bitValueMatcher =
    BitHasValue(I2C_CR2_START_POSITION, EXPECTED_I2C_CR2_START_VALUE);
  expectRegisterSetOnlyOnce(&(virtualI2CPeripheral.CR2), bitValueMatcher);

  const I2C::ErrorCode errorCode = virtualI2C.write(RANDOM_SLAVE_ADDRESS, RANDOM_MSG, RANDOM_MSG_LEN);

  ASSERT_THAT(errorCode, Eq(I2C::ErrorCode::OK));
  ASSERT_THAT(virtualI2CPeripheral.CR2, bitValueMatcher);
}

TEST_F(AnI2C, WriteClearsSTOPFlagInCR2Register)
{
  constexpr uint32_t I2C_CR2_STOP_POSITION = 14u;
  constexpr uint32_t EXPECTED_I2C_CR2_STOP_VALUE = 0x0;
  auto bitValueMatcher =
    BitHasValue(I2C_CR2_STOP_POSITION, EXPECTED_I2C_CR2_STOP_VALUE);
  expectRegisterSetOnlyOnce(&(virtualI2CPeripheral.CR2), bitValueMatcher);

  const I2C::ErrorCode errorCode = virtualI2C.write(RANDOM_SLAVE_ADDRESS, RANDOM_MSG, RANDOM_MSG_LEN);

  ASSERT_THAT(errorCode, Eq(I2C::ErrorCode::OK));
  ASSERT_THAT(virtualI2CPeripheral.CR2, bitValueMatcher);
}

TEST_F(AnI2C, WriteFailsIfAnotherWriteTransactionIsOngoing)
{
  ASSERT_THAT(virtualI2C.write(RANDOM_SLAVE_ADDRESS, RANDOM_MSG, RANDOM_MSG_LEN), Eq(I2C::ErrorCode::OK));
  ASSERT_THAT(virtualI2C.write(RANDOM_SLAVE_ADDRESS, RANDOM_MSG, RANDOM_MSG_LEN), Eq(I2C::ErrorCode::BUSY));
}

TEST_F(AnI2C, IsWriteTransactionOngoingReturnsTrueIfThereIsAnOngoingWrite)
{
  virtualI2C.write(RANDOM_SLAVE_ADDRESS, RANDOM_MSG, RANDOM_MSG_LEN);

  ASSERT_THAT(virtualI2C.isWriteTransactionOngoing(), true);
}

TEST_F(AnI2C, IsWriteTransactionOngoingReturnsFalseIfAnotherTransactionIsNotStarted)
{
  ASSERT_THAT(virtualI2C.isWriteTransactionOngoing(), false);
}

TEST_F(AnI2C, IRQHandlerWritesToTXDROnlyIfTransmitInterruptStatusFlagIsSet)
{
  setupISRRegisterReadings();
  virtualI2C.write(RANDOM_SLAVE_ADDRESS, "a", 1u);
  expectRegisterSetOnlyOnce(&(virtualI2CPeripheral.TXDR), Matcher<uint32_t>('a'));

  virtualI2C.IRQHandler();
}