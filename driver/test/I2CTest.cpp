#include "I2C.h"
#include "MemoryUtility.h"
#include "MemoryAccess.h"
#include "DriverTest.h"
#include "ClockControlMock.h"
#include "ResetControlMock.h"
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

  static constexpr uint32_t RX_BUFFER_LEN = 128u;
  static uint8_t rxBufferPtr[RX_BUFFER_LEN];

  static const uint8_t  RANDOM_MSG[];
  static const uint32_t RANDOM_MSG_LEN;
  static const uint16_t RANDOM_SLAVE_ADDRESS;
  static const uint8_t  RANDOM_MEMORY_ADDRESS;

  I2C_TypeDef virtualI2CPeripheral;
  NiceMock<ClockControlMock> clockControlMock;
  NiceMock<ResetControlMock> resetControlMock;
  I2C virtualI2C = I2C(&virtualI2CPeripheral, &clockControlMock, &resetControlMock);
  I2C::I2CConfig i2cConfig;

  uint32_t m_messageIdx;
  bool m_isTransactionWrite;
  bool m_isMemoryAddressSent;
  bool m_isMemoryAddressTxReloadCompleted;

  void setI2CInputClockFrequency(uint32_t clockFrequency);
  void setupISRRegisterReadings(uint32_t messageLen);
  void setupISRAndCR2RegistersReadingsForWriteMemory(uint32_t messageLen);
  void setupRXDRRegisterReadings(const void *messagePtr, uint32_t messageLen);
  void expectDataToBeWrittenInTXDR(const void *messagePtr, uint32_t messageLen);
  void expectDataToBeWrittenInTXDR(uint8_t memoryAddress, const void *messagePtr, uint32_t messageLen);

  void SetUp() override;
  void TearDown() override;
};

const uint8_t  AnI2C::RANDOM_MSG[]   = "Random message.";
const uint32_t AnI2C::RANDOM_MSG_LEN = sizeof(AnI2C::RANDOM_MSG) - 1u;
const uint16_t AnI2C::RANDOM_SLAVE_ADDRESS  = 0x7F;
const uint8_t  AnI2C::RANDOM_MEMORY_ADDRESS = 0x80;

uint8_t AnI2C::rxBufferPtr[RX_BUFFER_LEN];

void AnI2C::SetUp()
{
  DriverTest::SetUp();

  m_messageIdx = 0u;
  m_isTransactionWrite  = true;
  m_isMemoryAddressSent = false;
  m_isMemoryAddressTxReloadCompleted = false;

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

void AnI2C::setupISRRegisterReadings(uint32_t messageLen)
{
  ON_CALL(memoryAccessHook, getRegisterValue(&(virtualI2CPeripheral.ISR)))
    .WillByDefault([&, messageLen](volatile const uint32_t *registerPtr)
    {
      constexpr uint32_t USART_ISR_TXIS_POSITION = 1u;
      constexpr uint32_t USART_ISR_RXNE_POSITION = 2u;

      const uint32_t bitPosition = m_isTransactionWrite ? USART_ISR_TXIS_POSITION : USART_ISR_RXNE_POSITION;

      virtualI2CPeripheral.ISR =
        expectedRegVal(I2C_ISR_RESET_VALUE, bitPosition, 1u, (m_messageIdx < messageLen));

      return virtualI2CPeripheral.ISR;
    });
}

void AnI2C::setupISRAndCR2RegistersReadingsForWriteMemory(uint32_t messageLen)
{
  ON_CALL(memoryAccessHook, getRegisterValue(&(virtualI2CPeripheral.ISR)))
    .WillByDefault([&, messageLen](volatile const uint32_t *registerPtr)
    {
      constexpr uint32_t USART_ISR_TXIS_POSITION = 1u;
      constexpr uint32_t USART_ISR_TCR_POSITION = 7u;

      if (m_isMemoryAddressSent && (not m_isMemoryAddressTxReloadCompleted) && (0u == m_messageIdx))
      {
        virtualI2CPeripheral.ISR =
          expectedRegVal(I2C_ISR_RESET_VALUE, USART_ISR_TCR_POSITION, 1u, 1u);
      }
      else
      {
        virtualI2CPeripheral.ISR =
          expectedRegVal(I2C_ISR_RESET_VALUE, USART_ISR_TXIS_POSITION, 1u, (m_messageIdx < messageLen));
      }

      return virtualI2CPeripheral.ISR;
    });

  ON_CALL(memoryAccessHook, getRegisterValue(&(virtualI2CPeripheral.CR2)))
    .WillByDefault([&, messageLen](volatile const uint32_t *registerPtr)
    {
      m_isMemoryAddressTxReloadCompleted = true;

      return virtualI2CPeripheral.CR2;
    });
}

void AnI2C::expectDataToBeWrittenInTXDR(const void *messagePtr, uint32_t messageLen)
{
  m_isTransactionWrite = true;

  EXPECT_CALL(memoryAccessHook, setRegisterValue(&(virtualI2CPeripheral.TXDR), _))
    .Times(messageLen)
    .WillRepeatedly([&, messagePtr, messageLen](volatile void *registerPtr, uint32_t registerValue)
    {
      ASSERT_THAT(registerValue, reinterpret_cast<const uint8_t*>(messagePtr)[m_messageIdx++]);
      if (m_messageIdx < messageLen)
      {
        virtualI2C.IRQHandler();
      }
    });
}

void AnI2C::expectDataToBeWrittenInTXDR(uint8_t memoryAddress, const void *messagePtr, uint32_t messageLen)
{
  m_isTransactionWrite = true;

  EXPECT_CALL(memoryAccessHook, setRegisterValue(&(virtualI2CPeripheral.TXDR), _))
    .Times(messageLen + sizeof(memoryAddress))
    .WillRepeatedly([&, messagePtr, messageLen](volatile void *registerPtr, uint32_t registerValue)
    {
      if (m_isMemoryAddressSent == false)
      {
        m_isMemoryAddressSent = true;
        ASSERT_THAT(registerValue, memoryAddress);
        virtualI2C.IRQHandler();
      }
      else
      {
        ASSERT_THAT(registerValue, reinterpret_cast<const uint8_t*>(messagePtr)[m_messageIdx++]);
      }

      if (m_messageIdx < messageLen)
      {
        virtualI2C.IRQHandler();
      }
    });

  EXPECT_CALL(memoryAccessHook, setRegisterValue(Not(&(virtualI2CPeripheral.TXDR)), Matcher<uint32_t>(_)))
    .Times(AnyNumber());
}

void AnI2C::setupRXDRRegisterReadings(const void *messagePtr, uint32_t messageLen)
{
  m_isTransactionWrite = false;

  ON_CALL(memoryAccessHook, getRegisterValue(&(virtualI2CPeripheral.RXDR)))
    .WillByDefault([&, messagePtr, messageLen](volatile const uint32_t *registerPtr)
    {
      ++m_messageIdx;

      if (m_messageIdx < messageLen)
      {
        virtualI2C.IRQHandler();
      }

      virtualI2CPeripheral.RXDR = reinterpret_cast<const uint8_t*>(messagePtr)[--m_messageIdx];

      return 0u;
    });
}


TEST_F(AnI2C, GetPeripheralTagReturnsPointerToUnderlayingI2CPeripheralCastedToPeripheralType)
{
  ASSERT_THAT(virtualI2C.getPeripheralTag(),
    Eq(static_cast<Peripheral>(reinterpret_cast<uintptr_t>(&virtualI2CPeripheral))));
}

TEST_F(AnI2C, InitTurnsOnI2CPeripheralClock)
{
  resetControlMock.setReturnErrorCode(ResetControl::ErrorCode::OK);
  EXPECT_CALL(resetControlMock, enablePeripheralClock(virtualI2C.getPeripheralTag()))
    .Times(1u);

  const I2C::ErrorCode errorCode = virtualI2C.init(i2cConfig);

  ASSERT_THAT(errorCode, Eq(I2C::ErrorCode::OK));
}

TEST_F(AnI2C, InitFailsIfTurningOnOfI2CPeripheralClockFail)
{
  resetControlMock.setReturnErrorCode(ResetControl::ErrorCode::INTERNAL);
  EXPECT_CALL(resetControlMock, enablePeripheralClock(virtualI2C.getPeripheralTag()))
    .Times(1u);

  const I2C::ErrorCode errorCode = virtualI2C.init(i2cConfig);

  ASSERT_THAT(errorCode, Eq(I2C::ErrorCode::CAN_NOT_TURN_ON_PERIPHERAL_CLOCK));
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

TEST_F(AnI2C, WriteEnablesStopDetectionInterrupt)
{
  constexpr uint32_t I2C_CR1_STOPIE_POSITION = 5u;
  constexpr uint32_t EXPECTED_I2C_CR1_STOPIE_VALUE = 0x1;
  auto bitValueMatcher =
    BitHasValue(I2C_CR1_STOPIE_POSITION, EXPECTED_I2C_CR1_STOPIE_VALUE);
  expectSpecificRegisterSetWithNoChangesAfter(&(virtualI2CPeripheral.CR1), bitValueMatcher);

  const I2C::ErrorCode errorCode = virtualI2C.write(RANDOM_SLAVE_ADDRESS, RANDOM_MSG, RANDOM_MSG_LEN);

  ASSERT_THAT(errorCode, Eq(I2C::ErrorCode::OK));
  ASSERT_THAT(virtualI2CPeripheral.CR1, bitValueMatcher);
}

TEST_F(AnI2C, WriteEnablesAutoEndModeBySettingAUTOENDBitInCR2Register)
{
  constexpr uint32_t I2C_CR2_AUTOEND_POSITION = 25u;
  constexpr uint32_t EXPECTED_I2C_CR2_AUTOEND_VALUE = 0x1;
  auto bitValueMatcher =
    BitHasValue(I2C_CR2_AUTOEND_POSITION, EXPECTED_I2C_CR2_AUTOEND_VALUE);
  expectSpecificRegisterSetWithNoChangesAfter(&(virtualI2CPeripheral.CR2), bitValueMatcher);

  const I2C::ErrorCode errorCode = virtualI2C.write(RANDOM_SLAVE_ADDRESS, RANDOM_MSG, RANDOM_MSG_LEN);

  ASSERT_THAT(errorCode, Eq(I2C::ErrorCode::OK));
  ASSERT_THAT(virtualI2CPeripheral.CR2, bitValueMatcher);
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

TEST_F(AnI2C, WriteFailsIfBUSYBitIsSetInISRRegister)
{
  constexpr uint32_t I2C_ISR_BUSY_POSITION = 15u;
  virtualI2CPeripheral.ISR =
    expectedRegVal(virtualI2CPeripheral.ISR, I2C_ISR_BUSY_POSITION, 1u, 1u);

  ASSERT_THAT(virtualI2C.write(RANDOM_SLAVE_ADDRESS, RANDOM_MSG, RANDOM_MSG_LEN), Eq(I2C::ErrorCode::BUSY));
}

TEST_F(AnI2C, WriteFailsIfAnotherTransactionIsOngoing)
{
  ASSERT_THAT(virtualI2C.write(RANDOM_SLAVE_ADDRESS, RANDOM_MSG, RANDOM_MSG_LEN), Eq(I2C::ErrorCode::OK));
  ASSERT_THAT(virtualI2C.write(RANDOM_SLAVE_ADDRESS, RANDOM_MSG, RANDOM_MSG_LEN), Eq(I2C::ErrorCode::BUSY));
}

TEST_F(AnI2C, IsTransactionOngoingReturnsTrueIfThereIsAnOngoingWriteTransaction)
{
  virtualI2C.write(RANDOM_SLAVE_ADDRESS, RANDOM_MSG, RANDOM_MSG_LEN);

  ASSERT_THAT(virtualI2C.isTransactionOngoing(), true);
}

TEST_F(AnI2C, IsTransactionOngoingReturnsFalseIfAnotherTransactionIsNotStarted)
{
  ASSERT_THAT(virtualI2C.isTransactionOngoing(), false);
}

TEST_F(AnI2C, IRQHandlerWritesToTXDROnlyIfTransmitInterruptStatusFlagIsSet)
{
  setupISRRegisterReadings(1u);
  virtualI2C.write(RANDOM_SLAVE_ADDRESS, "a", 1u);
  expectRegisterSetOnlyOnce(&(virtualI2CPeripheral.TXDR), Matcher<uint32_t>('a'));

  virtualI2C.IRQHandler();
}

TEST_F(AnI2C, IRQHandlerDoesNotWriteToTXDRRegisterIfTransmitInterruptIsNotEnabled)
{
  constexpr uint32_t I2C_CR1_TXIE_POSITION = 1u;
  constexpr uint32_t PRESET_I2C_CR1_TXIE_VALUE = 0x0;
  virtualI2CPeripheral.CR1 =
    expectedRegVal(I2C_CR1_RESET_VALUE, I2C_CR1_TXIE_POSITION, 1u, PRESET_I2C_CR1_TXIE_VALUE);
  setupISRRegisterReadings(1u);
  expectRegisterNotToChange(&(virtualI2CPeripheral.TXDR));

  virtualI2C.IRQHandler();
}

TEST_F(AnI2C, IRQHandlerWritesDataToTransmitInTXDRIfOngoingTransactionIsWrite)
{
  setupISRRegisterReadings(RANDOM_MSG_LEN);
  virtualI2C.write(RANDOM_SLAVE_ADDRESS, RANDOM_MSG, RANDOM_MSG_LEN);
  expectDataToBeWrittenInTXDR(RANDOM_MSG, RANDOM_MSG_LEN);

  // trigger calling of IRQ handler and writting to TXDR register
  virtualI2C.IRQHandler();
}

TEST_F(AnI2C, IRQHandlerWritesMemoryAddressToBeReadInTXDRIfOngoingTransactionIsReadMemory)
{
  setupISRRegisterReadings(sizeof(RANDOM_MEMORY_ADDRESS));
  virtualI2C.readMemory(RANDOM_SLAVE_ADDRESS, RANDOM_MEMORY_ADDRESS, rxBufferPtr, RANDOM_MSG_LEN);
  expectDataToBeWrittenInTXDR(&RANDOM_MEMORY_ADDRESS, sizeof(RANDOM_MEMORY_ADDRESS));

  // trigger calling of IRQ handler and writting to TXDR register
  virtualI2C.IRQHandler();
}

TEST_F(AnI2C, IRQHandlerWritesMemoryAddressToBeWrittenBeforeDataInTXDRIfOngoingTransactionIsWriteMemory)
{
  virtualI2C.writeMemory(RANDOM_SLAVE_ADDRESS, RANDOM_MEMORY_ADDRESS, RANDOM_MSG, RANDOM_MSG_LEN);
  setupISRAndCR2RegistersReadingsForWriteMemory(RANDOM_MSG_LEN);
  expectDataToBeWrittenInTXDR(RANDOM_MEMORY_ADDRESS, RANDOM_MSG, RANDOM_MSG_LEN);

  // trigger calling of IRQ handler and writting to TXDR register
  virtualI2C.IRQHandler();
}

TEST_F(AnI2C, IRQHandlerReadsDataReceivedDataFromRXDR)
{
  setupISRRegisterReadings(RANDOM_MSG_LEN);
  setupRXDRRegisterReadings(RANDOM_MSG, RANDOM_MSG_LEN);
  virtualI2C.read(RANDOM_SLAVE_ADDRESS, rxBufferPtr, RANDOM_MSG_LEN);

  // trigger calling of IRQ handler and reading from RXDR register
  virtualI2C.IRQHandler();

  ASSERT_STREQ(reinterpret_cast<const char*>(RANDOM_MSG), reinterpret_cast<char*>(&rxBufferPtr[0]));
}

TEST_F(AnI2C, IRQHandlerSetsTransferDirectionToReadWhenTransferIsCompletedIfOngoingTransactionIsReadMemory)
{
  constexpr uint32_t I2C_ISR_TC_POSITION = 6u;
  constexpr uint32_t I2C_CR1_TCIE_POSITION = 6u;
  constexpr uint32_t I2C_CR2_RD_WRN_POSITION = 10u;
  constexpr uint32_t EXPECTED_I2C_CR2_RD_WRN_VALUE = 0x1; // read direction
  virtualI2C.readMemory(RANDOM_SLAVE_ADDRESS, RANDOM_MEMORY_ADDRESS, rxBufferPtr, RANDOM_MSG_LEN);
  // force values as if transfer complete interrupt happened
  virtualI2CPeripheral.CR1 =
    expectedRegVal(virtualI2CPeripheral.CR1, I2C_CR1_TCIE_POSITION, 1u, 1u);
  virtualI2CPeripheral.ISR =
    expectedRegVal(virtualI2CPeripheral.ISR, I2C_ISR_TC_POSITION, 1u, 1u);
  auto bitValueMatcher =
    BitHasValue(I2C_CR2_RD_WRN_POSITION, EXPECTED_I2C_CR2_RD_WRN_VALUE);
  expectSpecificRegisterSetToBeCalledLast(&(virtualI2CPeripheral.CR2), bitValueMatcher);

  virtualI2C.IRQHandler();
}

TEST_F(AnI2C, IRQHandlerEnablesAutoEndModeWhenTransferIsCompletedIfOngoingTransactionIsReadMemory)
{
  constexpr uint32_t I2C_ISR_TC_POSITION = 6u;
  constexpr uint32_t I2C_CR1_TCIE_POSITION = 6u;
  constexpr uint32_t I2C_CR2_AUTOEND_POSITION = 25u;
  constexpr uint32_t EXPECTED_I2C_CR2_AUTOEND_VALUE = 0x1;
  virtualI2C.readMemory(RANDOM_SLAVE_ADDRESS, RANDOM_MEMORY_ADDRESS, rxBufferPtr, RANDOM_MSG_LEN);
  // force values as if transfer complete interrupt happened
  virtualI2CPeripheral.CR1 =
    expectedRegVal(virtualI2CPeripheral.CR1, I2C_CR1_TCIE_POSITION, 1u, 1u);
  virtualI2CPeripheral.ISR =
    expectedRegVal(virtualI2CPeripheral.ISR, I2C_ISR_TC_POSITION, 1u, 1u);
  auto bitValueMatcher =
    BitHasValue(I2C_CR2_AUTOEND_POSITION, EXPECTED_I2C_CR2_AUTOEND_VALUE);
  expectSpecificRegisterSetToBeCalledLast(&(virtualI2CPeripheral.CR2), bitValueMatcher);

  virtualI2C.IRQHandler();
}

TEST_F(AnI2C, IRQHandlerSetsInNBYTESNumberOfBytesToReadWhenTransferIsCompletedIfOngoingTransactionIsReadMemory)
{
  constexpr uint32_t I2C_ISR_TC_POSITION = 6u;
  constexpr uint32_t I2C_CR1_TCIE_POSITION = 6u;
  constexpr uint32_t I2C_CR2_NBYTES_POSITION = 16u;
  constexpr uint32_t I2C_CR2_NBYTES_SIZE = 8u;
  constexpr uint32_t EXPECTED_I2C_CR2_NBYTES_VALUE = RANDOM_MSG_LEN;
  virtualI2C.readMemory(RANDOM_SLAVE_ADDRESS, RANDOM_MEMORY_ADDRESS, rxBufferPtr, RANDOM_MSG_LEN);
  // force values as if transfer complete interrupt happened
  virtualI2CPeripheral.CR1 =
    expectedRegVal(virtualI2CPeripheral.CR1, I2C_CR1_TCIE_POSITION, 1u, 1u);
  virtualI2CPeripheral.ISR =
    expectedRegVal(virtualI2CPeripheral.ISR, I2C_ISR_TC_POSITION, 1u, 1u);
  auto bitValueMatcher =
    BitsHaveValue(I2C_CR2_NBYTES_POSITION, I2C_CR2_NBYTES_SIZE, EXPECTED_I2C_CR2_NBYTES_VALUE);
  expectSpecificRegisterSetToBeCalledLast(&(virtualI2CPeripheral.CR2), bitValueMatcher);

  virtualI2C.IRQHandler();
}

TEST_F(AnI2C, IRQHandlerStartsNewTransferWhenOngoingTransferIsCompletedIfOngoingTransactionIsReadMemory)
{
  constexpr uint32_t I2C_ISR_TC_POSITION = 6u;
  constexpr uint32_t I2C_CR1_TCIE_POSITION = 6u;
  constexpr uint32_t I2C_CR2_START_POSITION = 13u;
  constexpr uint32_t EXPECTED_I2C_CR2_START_VALUE = 0x1;
  virtualI2C.readMemory(RANDOM_SLAVE_ADDRESS, RANDOM_MEMORY_ADDRESS, rxBufferPtr, RANDOM_MSG_LEN);
  // clear START flag as real hardware does after transfer is started
  virtualI2CPeripheral.CR2 =
    expectedRegVal(virtualI2CPeripheral.CR2, I2C_CR2_START_POSITION, 1u, 0u);
  // force values as if transfer complete interrupt happened
  virtualI2CPeripheral.CR1 =
    expectedRegVal(virtualI2CPeripheral.CR1, I2C_CR1_TCIE_POSITION, 1u, 1u);
  virtualI2CPeripheral.ISR =
    expectedRegVal(virtualI2CPeripheral.ISR, I2C_ISR_TC_POSITION, 1u, 1u);
  auto bitValueMatcher =
    BitHasValue(I2C_CR2_START_POSITION, EXPECTED_I2C_CR2_START_VALUE);
  expectSpecificRegisterSetToBeCalledLast(&(virtualI2CPeripheral.CR2), bitValueMatcher);

  virtualI2C.IRQHandler();
}

TEST_F(AnI2C, IRQHandlerEnablesAutoEndModeWhenReloadIsNeededIfOngoingTransactionIsWriteMemory)
{
  constexpr uint32_t I2C_ISR_TCR_POSITION = 7u;
  constexpr uint32_t I2C_CR1_TCIE_POSITION = 6u;
  constexpr uint32_t I2C_CR2_AUTOEND_POSITION = 25u;
  constexpr uint32_t EXPECTED_I2C_CR2_AUTOEND_VALUE = 0x1;
  virtualI2C.writeMemory(RANDOM_SLAVE_ADDRESS, RANDOM_MEMORY_ADDRESS, RANDOM_MSG, RANDOM_MSG_LEN);
  // force values as if transfer complete interrupt happened
  virtualI2CPeripheral.CR1 =
    expectedRegVal(virtualI2CPeripheral.CR1, I2C_CR1_TCIE_POSITION, 1u, 1u);
  virtualI2CPeripheral.ISR =
    expectedRegVal(virtualI2CPeripheral.ISR, I2C_ISR_TCR_POSITION, 1u, 1u);
  auto bitValueMatcher =
    BitHasValue(I2C_CR2_AUTOEND_POSITION, EXPECTED_I2C_CR2_AUTOEND_VALUE);
  expectSpecificRegisterSetToBeCalledLast(&(virtualI2CPeripheral.CR2), bitValueMatcher);

  virtualI2C.IRQHandler();
}

TEST_F(AnI2C, IRQHandlerDisablesReloadModeWhenReloadIsNeededIfOngoingTransactionIsWriteMemory)
{
  constexpr uint32_t I2C_ISR_TCR_POSITION = 7u;
  constexpr uint32_t I2C_CR1_TCIE_POSITION = 6u;
  constexpr uint32_t I2C_CR2_RELOAD_POSITION = 24u;
  constexpr uint32_t EXPECTED_I2C_CR2_RELOAD_VALUE = 0x0;
  virtualI2C.writeMemory(RANDOM_SLAVE_ADDRESS, RANDOM_MEMORY_ADDRESS, RANDOM_MSG, RANDOM_MSG_LEN);
  // force values as if transfer complete interrupt happened
  virtualI2CPeripheral.CR1 =
    expectedRegVal(virtualI2CPeripheral.CR1, I2C_CR1_TCIE_POSITION, 1u, 1u);
  virtualI2CPeripheral.ISR =
    expectedRegVal(virtualI2CPeripheral.ISR, I2C_ISR_TCR_POSITION, 1u, 1u);
  auto bitValueMatcher =
    BitHasValue(I2C_CR2_RELOAD_POSITION, EXPECTED_I2C_CR2_RELOAD_VALUE);
  expectSpecificRegisterSetToBeCalledLast(&(virtualI2CPeripheral.CR2), bitValueMatcher);

  virtualI2C.IRQHandler();
}

TEST_F(AnI2C, IRQHandlerSetsInNBYTESNumberOfBytesToWriteWhenReloadIsNeededIfOngoingTransactionIsWriteMemory)
{
  constexpr uint32_t I2C_ISR_TCR_POSITION = 7u;
  constexpr uint32_t I2C_CR1_TCIE_POSITION = 6u;
  constexpr uint32_t I2C_CR2_NBYTES_POSITION = 16u;
  constexpr uint32_t I2C_CR2_NBYTES_SIZE = 8u;
  constexpr uint32_t EXPECTED_I2C_CR2_NBYTES_VALUE = RANDOM_MSG_LEN;
  virtualI2C.writeMemory(RANDOM_SLAVE_ADDRESS, RANDOM_MEMORY_ADDRESS, RANDOM_MSG, RANDOM_MSG_LEN);
  // force values as if transfer complete interrupt happened
  virtualI2CPeripheral.CR1 =
    expectedRegVal(virtualI2CPeripheral.CR1, I2C_CR1_TCIE_POSITION, 1u, 1u);
  virtualI2CPeripheral.ISR =
    expectedRegVal(virtualI2CPeripheral.ISR, I2C_ISR_TCR_POSITION, 1u, 1u);
  auto bitValueMatcher =
    BitsHaveValue(I2C_CR2_NBYTES_POSITION, I2C_CR2_NBYTES_SIZE, EXPECTED_I2C_CR2_NBYTES_VALUE);
  expectSpecificRegisterSetToBeCalledLast(&(virtualI2CPeripheral.CR2), bitValueMatcher);

  virtualI2C.IRQHandler();
}

TEST_F(AnI2C, IRQHandlerStopsAndCompletesTransactionOnStopDetectionInterrupt)
{
  constexpr uint32_t I2C_CR1_STOPIE_POSITION = 5u;
  constexpr uint32_t I2C_ISR_STOPF_POSITION = 5u;
  virtualI2C.write(RANDOM_SLAVE_ADDRESS, RANDOM_MSG, RANDOM_MSG_LEN);
  // force values as if stop detection interrupt happened
  virtualI2CPeripheral.CR1 =
    expectedRegVal(virtualI2CPeripheral.CR1, I2C_CR1_STOPIE_POSITION, 1u, 1u);
  virtualI2CPeripheral.ISR =
    expectedRegVal(virtualI2CPeripheral.ISR, I2C_ISR_STOPF_POSITION, 1u, 1u);

  virtualI2C.IRQHandler();

  ASSERT_THAT(virtualI2C.isTransactionOngoing(), false);
}

TEST_F(AnI2C, IRQHandlerDisablesStopDetectionInterruptWhenTransactionIsCompleted)
{
  constexpr uint32_t I2C_ISR_STOPF_POSITION = 5u;
  constexpr uint32_t I2C_CR1_STOPIE_POSITION = 5u;
  constexpr uint32_t EXPECTED_I2C_CR1_STOPIE_VALUE = 0x0;
  virtualI2C.write(RANDOM_SLAVE_ADDRESS, RANDOM_MSG, RANDOM_MSG_LEN);
  // force values as if stop detection interrupt happened
  virtualI2CPeripheral.CR1 =
    expectedRegVal(virtualI2CPeripheral.CR1, I2C_CR1_STOPIE_POSITION, 1u, 1u);
  virtualI2CPeripheral.ISR =
    expectedRegVal(virtualI2CPeripheral.ISR, I2C_ISR_STOPF_POSITION, 1u, 1u);
  auto bitValueMatcher =
    BitHasValue(I2C_CR1_STOPIE_POSITION, EXPECTED_I2C_CR1_STOPIE_VALUE);
  expectSpecificRegisterSetWithNoChangesAfter(&(virtualI2CPeripheral.CR1), bitValueMatcher);

  virtualI2C.IRQHandler();
}

TEST_F(AnI2C, IRQHandlerDisablesTransmitInterruptWhenTransactionIsCompleted)
{
  constexpr uint32_t I2C_ISR_STOPF_POSITION = 5u;
  constexpr uint32_t I2C_CR1_STOPIE_POSITION = 5u;
  constexpr uint32_t I2C_CR1_TXIS_POSITION = 1u;
  constexpr uint32_t EXPECTED_I2C_CR1_TXIS_VALUE = 0x0;
  virtualI2C.write(RANDOM_SLAVE_ADDRESS, RANDOM_MSG, RANDOM_MSG_LEN);
  // force values as if stop detection interrupt happened (aka. transfer is completed)
  virtualI2CPeripheral.ISR =
    expectedRegVal(virtualI2CPeripheral.ISR, I2C_ISR_STOPF_POSITION, 1u, 1u);
  virtualI2CPeripheral.CR1 =
    expectedRegVal(virtualI2CPeripheral.CR1, I2C_CR1_STOPIE_POSITION, 1u, 1u);
  virtualI2CPeripheral.CR1 =
    expectedRegVal(virtualI2CPeripheral.CR1, I2C_CR1_TXIS_POSITION, 1u, 1u);
  auto bitValueMatcher =
    BitHasValue(I2C_CR1_TXIS_POSITION, EXPECTED_I2C_CR1_TXIS_VALUE);
  expectSpecificRegisterSetWithNoChangesAfter(&(virtualI2CPeripheral.CR1), bitValueMatcher);

  virtualI2C.IRQHandler();
}

TEST_F(AnI2C, IRQHandlerDisablesReceiveInterruptWhenTransactionIsCompleted)
{
  constexpr uint32_t I2C_ISR_STOPF_POSITION = 5u;
  constexpr uint32_t I2C_CR1_STOPIE_POSITION = 5u;
  constexpr uint32_t I2C_CR1_RXIE_POSITION = 2u;
  constexpr uint32_t EXPECTED_I2C_CR1_RXIE_VALUE = 0x0;
  virtualI2C.write(RANDOM_SLAVE_ADDRESS, RANDOM_MSG, RANDOM_MSG_LEN);
  // force values as if stop detection interrupt happened (aka. transfer is completed)
  virtualI2CPeripheral.ISR =
    expectedRegVal(virtualI2CPeripheral.ISR, I2C_ISR_STOPF_POSITION, 1u, 1u);
  virtualI2CPeripheral.CR1 =
    expectedRegVal(virtualI2CPeripheral.CR1, I2C_CR1_STOPIE_POSITION, 1u, 1u);
  virtualI2CPeripheral.CR1 =
    expectedRegVal(virtualI2CPeripheral.CR1, I2C_CR1_RXIE_POSITION, 1u, 1u);
  auto bitValueMatcher =
    BitHasValue(I2C_CR1_RXIE_POSITION, EXPECTED_I2C_CR1_RXIE_VALUE);
  expectSpecificRegisterSetWithNoChangesAfter(&(virtualI2CPeripheral.CR1), bitValueMatcher);

  virtualI2C.IRQHandler();
}

TEST_F(AnI2C, IRQHandlerDisablesTransferCompleteInterruptWhenTransactionIsCompleted)
{
  constexpr uint32_t I2C_ISR_STOPF_POSITION = 5u;
  constexpr uint32_t I2C_CR1_STOPIE_POSITION = 5u;
  constexpr uint32_t I2C_CR1_TCIE_POSITION = 6u;
  constexpr uint32_t EXPECTED_I2C_CR1_TCIE_VALUE = 0x0;
  virtualI2C.write(RANDOM_SLAVE_ADDRESS, RANDOM_MSG, RANDOM_MSG_LEN);
  // force values as if stop detection interrupt happened (aka. transfer is completed)
  virtualI2CPeripheral.ISR =
    expectedRegVal(virtualI2CPeripheral.ISR, I2C_ISR_STOPF_POSITION, 1u, 1u);
  virtualI2CPeripheral.CR1 =
    expectedRegVal(virtualI2CPeripheral.CR1, I2C_CR1_STOPIE_POSITION, 1u, 1u);
  virtualI2CPeripheral.CR1 =
    expectedRegVal(virtualI2CPeripheral.CR1, I2C_CR1_TCIE_POSITION, 1u, 1u);
  auto bitValueMatcher =
    BitHasValue(I2C_CR1_TCIE_POSITION, EXPECTED_I2C_CR1_TCIE_VALUE);
  expectSpecificRegisterSetWithNoChangesAfter(&(virtualI2CPeripheral.CR1), bitValueMatcher);

  virtualI2C.IRQHandler();
}

TEST_F(AnI2C, IRQHandlerClearsStopDetectionFlagWhenTransactionIsCompleted)
{
  constexpr uint32_t I2C_CR1_STOPIE_POSITION = 5u;
  constexpr uint32_t I2C_ISR_STOPF_POSITION  = 5u;
  constexpr uint32_t I2C_ICR_STOPCF_POSITION = 5u;
  constexpr uint32_t EXPECTED_I2C_ICR_STOPCF_VALUE = 0x1;
  virtualI2C.write(RANDOM_SLAVE_ADDRESS, RANDOM_MSG, RANDOM_MSG_LEN);
  // force values as if stop detection interrupt happened (aka. transfer is completed)
  virtualI2CPeripheral.CR1 =
    expectedRegVal(virtualI2CPeripheral.CR1, I2C_CR1_STOPIE_POSITION, 1u, 1u);
  virtualI2CPeripheral.ISR =
    expectedRegVal(virtualI2CPeripheral.ISR, I2C_ISR_STOPF_POSITION, 1u, 1u);
  auto bitValueMatcher =
    BitHasValue(I2C_ICR_STOPCF_POSITION, EXPECTED_I2C_ICR_STOPCF_VALUE);
  expectSpecificRegisterSetWithNoChangesAfter(&(virtualI2CPeripheral.ICR), bitValueMatcher);

  virtualI2C.IRQHandler();
}

TEST_F(AnI2C, IRQHandlerWritesDummyDataToTXDRIfTXISFlagIsSetInOrderToCleanItWhenTransactionIsCompleted)
{
  constexpr uint32_t I2C_TXDR_DUMMY_VALUE = 0x0;
  constexpr uint32_t I2C_CR1_STOPIE_POSITION = 5u;
  constexpr uint32_t I2C_ISR_STOPF_POSITION  = 5u;
  constexpr uint32_t I2C_ISR_TXIS_POSITION   = 1u;
  constexpr uint32_t EXPECTED_I2C_TXDR_VALUE = I2C_TXDR_DUMMY_VALUE;
  // force values as if stop detection interrupt happened (aka. transfer is completed)
  virtualI2CPeripheral.CR1 =
    expectedRegVal(virtualI2CPeripheral.CR1, I2C_CR1_STOPIE_POSITION, 1u, 1u);
  virtualI2CPeripheral.ISR =
    expectedRegVal(virtualI2CPeripheral.ISR, I2C_ISR_STOPF_POSITION, 1u, 1u);
  // force value of TXIS flag to be set
  virtualI2CPeripheral.ISR =
    expectedRegVal(virtualI2CPeripheral.ISR, I2C_ISR_TXIS_POSITION, 1u, 1u);
  expectSpecificRegisterSetToBeCalledLast(&(virtualI2CPeripheral.TXDR), EXPECTED_I2C_TXDR_VALUE);

  virtualI2C.IRQHandler();
}

TEST_F(AnI2C, IRQHandlerFlushesTXDRRegisterIfThereIsStillDataInItWhenTransactionIsCompleted)
{
  constexpr uint32_t I2C_CR1_STOPIE_POSITION = 5u;
  constexpr uint32_t I2C_ISR_STOPF_POSITION  = 5u;
  constexpr uint32_t I2C_ISR_TXE_POSITION = 0u;
  constexpr uint32_t EXPECTED_I2C_ISR_TXE_VALUE = 0x1;
  virtualI2C.write(RANDOM_SLAVE_ADDRESS, RANDOM_MSG, RANDOM_MSG_LEN);
  // force values as if stop detection interrupt happened (aka. transfer is completed)
  virtualI2CPeripheral.CR1 =
    expectedRegVal(virtualI2CPeripheral.CR1, I2C_CR1_STOPIE_POSITION, 1u, 1u);
  virtualI2CPeripheral.ISR =
    expectedRegVal(virtualI2CPeripheral.ISR, I2C_ISR_STOPF_POSITION, 1u, 1u);
  // force value as if TXDR register is not empty
  virtualI2CPeripheral.ISR =
    expectedRegVal(virtualI2CPeripheral.ISR, I2C_ISR_TXE_POSITION, 1u, 0u);
  auto bitValueMatcher =
    BitHasValue(I2C_ISR_TXE_POSITION, EXPECTED_I2C_ISR_TXE_VALUE);
  expectSpecificRegisterSetWithNoChangesAfter(&(virtualI2CPeripheral.ISR), bitValueMatcher);

  virtualI2C.IRQHandler();
}

TEST_F(AnI2C, ReadEnablesReceiveInterrupt)
{
  constexpr uint32_t I2C_CR1_RXIE_POSITION = 2u;
  constexpr uint32_t EXPECTED_I2C_CR1_RXIE_VALUE = 0x1;
  auto bitValueMatcher =
    BitHasValue(I2C_CR1_RXIE_POSITION, EXPECTED_I2C_CR1_RXIE_VALUE);
  expectSpecificRegisterSetWithNoChangesAfter(&(virtualI2CPeripheral.CR1), bitValueMatcher);

  const I2C::ErrorCode errorCode = virtualI2C.read(RANDOM_SLAVE_ADDRESS, rxBufferPtr, RANDOM_MSG_LEN);

  ASSERT_THAT(errorCode, Eq(I2C::ErrorCode::OK));
  ASSERT_THAT(virtualI2CPeripheral.CR1, bitValueMatcher);
}

TEST_F(AnI2C, ReadEnablesStopDetectionInterrupt)
{
  constexpr uint32_t I2C_CR1_STOPIE_POSITION = 5u;
  constexpr uint32_t EXPECTED_I2C_CR1_STOPIE_VALUE = 0x1;
  auto bitValueMatcher =
    BitHasValue(I2C_CR1_STOPIE_POSITION, EXPECTED_I2C_CR1_STOPIE_VALUE);
  expectSpecificRegisterSetWithNoChangesAfter(&(virtualI2CPeripheral.CR1), bitValueMatcher);

  const I2C::ErrorCode errorCode = virtualI2C.read(RANDOM_SLAVE_ADDRESS, rxBufferPtr, RANDOM_MSG_LEN);

  ASSERT_THAT(errorCode, Eq(I2C::ErrorCode::OK));
  ASSERT_THAT(virtualI2CPeripheral.CR1, bitValueMatcher);
}

TEST_F(AnI2C, ReadEnablesAutoEndModeBySettingAUTOENDBitInCR2Register)
{
  constexpr uint32_t I2C_CR2_AUTOEND_POSITION = 25u;
  constexpr uint32_t EXPECTED_I2C_CR2_AUTOEND_VALUE = 0x1;
  auto bitValueMatcher =
    BitHasValue(I2C_CR2_AUTOEND_POSITION, EXPECTED_I2C_CR2_AUTOEND_VALUE);
  expectSpecificRegisterSetWithNoChangesAfter(&(virtualI2CPeripheral.CR2), bitValueMatcher);

  const I2C::ErrorCode errorCode = virtualI2C.read(RANDOM_SLAVE_ADDRESS, rxBufferPtr, RANDOM_MSG_LEN);

  ASSERT_THAT(errorCode, Eq(I2C::ErrorCode::OK));
  ASSERT_THAT(virtualI2CPeripheral.CR2, bitValueMatcher);
}

TEST_F(AnI2C, ReadSetsInNBYTESBitsInCR2RegisterNumberOfBytesToRead)
{
  constexpr uint32_t I2C_CR2_NBYTES_POSITION = 16u;
  constexpr uint32_t I2C_CR2_NBYTES_SIZE = 8u;
  constexpr uint32_t EXPECTED_I2C_CR2_NBYTES_VALUE = RANDOM_MSG_LEN;
  auto bitValueMatcher =
    BitsHaveValue(I2C_CR2_NBYTES_POSITION, I2C_CR2_NBYTES_SIZE, EXPECTED_I2C_CR2_NBYTES_VALUE);
  expectRegisterSetOnlyOnce(&(virtualI2CPeripheral.CR2), bitValueMatcher);

  const I2C::ErrorCode errorCode = virtualI2C.read(RANDOM_SLAVE_ADDRESS, rxBufferPtr, RANDOM_MSG_LEN);

  ASSERT_THAT(errorCode, Eq(I2C::ErrorCode::OK));
  ASSERT_THAT(virtualI2CPeripheral.CR2, bitValueMatcher);
}

TEST_F(AnI2C, ReadSetsSlaveAddressInSADDBits1To7InCR2RegisterIfChoosenAddressingModeIs7Bits)
{
  constexpr uint32_t I2C_CR2_SADD_POSITION = 1u;
  constexpr uint32_t I2C_CR2_SADD_SIZE = 7u;
  constexpr uint32_t EXPECTED_I2C_CR2_SADD_VALUE = RANDOM_SLAVE_ADDRESS;
  auto bitValueMatcher =
    BitsHaveValue(I2C_CR2_SADD_POSITION, I2C_CR2_SADD_SIZE, EXPECTED_I2C_CR2_SADD_VALUE);
  i2cConfig.addressingMode = I2C::AddressingMode::ADDRESS_7_BITS;
  virtualI2C.init(i2cConfig);
  expectRegisterSetOnlyOnce(&(virtualI2CPeripheral.CR2), bitValueMatcher);

  const I2C::ErrorCode errorCode = virtualI2C.read(RANDOM_SLAVE_ADDRESS, rxBufferPtr, RANDOM_MSG_LEN);

  ASSERT_THAT(errorCode, Eq(I2C::ErrorCode::OK));
  ASSERT_THAT(virtualI2CPeripheral.CR2, bitValueMatcher);
}

TEST_F(AnI2C, ReadSetsSlaveAddressInSADDBits0To9InCR2RegisterIfChoosenAddressingModeIs10Bits)
{
  constexpr uint32_t I2C_CR2_SADD_POSITION = 0u;
  constexpr uint32_t I2C_CR2_SADD_SIZE = 10u;
  constexpr uint32_t EXPECTED_I2C_CR2_SADD_VALUE = RANDOM_SLAVE_ADDRESS;
  auto bitValueMatcher =
    BitsHaveValue(I2C_CR2_SADD_POSITION, I2C_CR2_SADD_SIZE, EXPECTED_I2C_CR2_SADD_VALUE);
  i2cConfig.addressingMode = I2C::AddressingMode::ADDRESS_10_BITS;
  virtualI2C.init(i2cConfig);
  expectRegisterSetOnlyOnce(&(virtualI2CPeripheral.CR2), bitValueMatcher);

  const I2C::ErrorCode errorCode = virtualI2C.read(RANDOM_SLAVE_ADDRESS, rxBufferPtr, RANDOM_MSG_LEN);

  ASSERT_THAT(errorCode, Eq(I2C::ErrorCode::OK));
  ASSERT_THAT(virtualI2CPeripheral.CR2, bitValueMatcher);
}

TEST_F(AnI2C, ReadSetsTransferDirectionToReadBySettingRD_WRNBitInCR2RegisterToOne)
{
  constexpr uint32_t I2C_CR2_RD_WRN_POSITION = 10u;
  constexpr uint32_t EXPECTED_I2C_CR2_RD_WRN_VALUE = 0x1;
  auto bitValueMatcher =
    BitHasValue(I2C_CR2_RD_WRN_POSITION, EXPECTED_I2C_CR2_RD_WRN_VALUE);
  expectRegisterSetOnlyOnce(&(virtualI2CPeripheral.CR2), bitValueMatcher);

  const I2C::ErrorCode errorCode = virtualI2C.read(RANDOM_SLAVE_ADDRESS, rxBufferPtr, RANDOM_MSG_LEN);

  ASSERT_THAT(errorCode, Eq(I2C::ErrorCode::OK));
  ASSERT_THAT(virtualI2CPeripheral.CR2, bitValueMatcher);
}

TEST_F(AnI2C, ReadStartsTransferBySettingSTARTBitInCR2Register)
{
  constexpr uint32_t I2C_CR2_START_POSITION = 13u;
  constexpr uint32_t EXPECTED_I2C_CR2_START_VALUE = 0x1;
  auto bitValueMatcher =
    BitHasValue(I2C_CR2_START_POSITION, EXPECTED_I2C_CR2_START_VALUE);
  expectRegisterSetOnlyOnce(&(virtualI2CPeripheral.CR2), bitValueMatcher);

  const I2C::ErrorCode errorCode = virtualI2C.read(RANDOM_SLAVE_ADDRESS, rxBufferPtr, RANDOM_MSG_LEN);

  ASSERT_THAT(errorCode, Eq(I2C::ErrorCode::OK));
  ASSERT_THAT(virtualI2CPeripheral.CR2, bitValueMatcher);
}

TEST_F(AnI2C, ReadClearsSTOPFlagInCR2Register)
{
  constexpr uint32_t I2C_CR2_STOP_POSITION = 14u;
  constexpr uint32_t EXPECTED_I2C_CR2_STOP_VALUE = 0x0;
  auto bitValueMatcher =
    BitHasValue(I2C_CR2_STOP_POSITION, EXPECTED_I2C_CR2_STOP_VALUE);
  expectRegisterSetOnlyOnce(&(virtualI2CPeripheral.CR2), bitValueMatcher);

  const I2C::ErrorCode errorCode = virtualI2C.read(RANDOM_SLAVE_ADDRESS, rxBufferPtr, RANDOM_MSG_LEN);

  ASSERT_THAT(errorCode, Eq(I2C::ErrorCode::OK));
  ASSERT_THAT(virtualI2CPeripheral.CR2, bitValueMatcher);
}

TEST_F(AnI2C, ReadFailsIfBUSYBitIsSetInISRRegister)
{
  constexpr uint32_t I2C_ISR_BUSY_POSITION = 15u;
  virtualI2CPeripheral.ISR =
    expectedRegVal(virtualI2CPeripheral.ISR, I2C_ISR_BUSY_POSITION, 1u, 1u);

  ASSERT_THAT(virtualI2C.read(RANDOM_SLAVE_ADDRESS, rxBufferPtr, RANDOM_MSG_LEN), Eq(I2C::ErrorCode::BUSY));
}

TEST_F(AnI2C, ReadFailsIfAnotherTransactionIsOngoing)
{
  ASSERT_THAT(virtualI2C.read(RANDOM_SLAVE_ADDRESS, rxBufferPtr, RANDOM_MSG_LEN), Eq(I2C::ErrorCode::OK));
  ASSERT_THAT(virtualI2C.read(RANDOM_SLAVE_ADDRESS, rxBufferPtr, RANDOM_MSG_LEN), Eq(I2C::ErrorCode::BUSY));
}

TEST_F(AnI2C, IsTransactionOngoingReturnsTrueIfThereIsAnOngoingReadTransaction)
{
  virtualI2C.read(RANDOM_SLAVE_ADDRESS, rxBufferPtr, RANDOM_MSG_LEN);

  ASSERT_THAT(virtualI2C.isTransactionOngoing(), true);
}

TEST_F(AnI2C, IsTransactionOngoingReturnsTrueIfBUSYBitIsSetInISRRegister)
{
  constexpr uint32_t I2C_ISR_BUSY_POSITION = 15u;
  virtualI2CPeripheral.ISR =
    expectedRegVal(virtualI2CPeripheral.ISR, I2C_ISR_BUSY_POSITION, 1u, 1u);

  ASSERT_THAT(virtualI2C.isTransactionOngoing(), true);
}

TEST_F(AnI2C, ReadMemoryEnablesReceiveInterrupt)
{
  constexpr uint32_t I2C_CR1_RXIE_POSITION = 2u;
  constexpr uint32_t EXPECTED_I2C_CR1_RXIE_VALUE = 0x1;
  auto bitValueMatcher =
    BitHasValue(I2C_CR1_RXIE_POSITION, EXPECTED_I2C_CR1_RXIE_VALUE);
  expectSpecificRegisterSetWithNoChangesAfter(&(virtualI2CPeripheral.CR1), bitValueMatcher);

  const I2C::ErrorCode errorCode =
    virtualI2C.readMemory(RANDOM_SLAVE_ADDRESS, RANDOM_MEMORY_ADDRESS, rxBufferPtr, RANDOM_MSG_LEN);

  ASSERT_THAT(errorCode, Eq(I2C::ErrorCode::OK));
  ASSERT_THAT(virtualI2CPeripheral.CR1, bitValueMatcher);
}

TEST_F(AnI2C, ReadMemoryEnablesTransmitInterrupt)
{
  constexpr uint32_t I2C_CR1_TXIE_POSITION = 1u;
  constexpr uint32_t EXPECTED_I2C_CR1_TXIE_VALUE = 0x1;
  auto bitValueMatcher =
    BitHasValue(I2C_CR1_TXIE_POSITION, EXPECTED_I2C_CR1_TXIE_VALUE);
  expectSpecificRegisterSetWithNoChangesAfter(&(virtualI2CPeripheral.CR1), bitValueMatcher);

  const I2C::ErrorCode errorCode =
    virtualI2C.readMemory(RANDOM_SLAVE_ADDRESS, RANDOM_MEMORY_ADDRESS, rxBufferPtr, RANDOM_MSG_LEN);

  ASSERT_THAT(errorCode, Eq(I2C::ErrorCode::OK));
  ASSERT_THAT(virtualI2CPeripheral.CR1, bitValueMatcher);
}

TEST_F(AnI2C, ReadMemoryEnablesStopDetectionInterrupt)
{
  constexpr uint32_t I2C_CR1_STOPIE_POSITION = 5u;
  constexpr uint32_t EXPECTED_I2C_CR1_STOPIE_VALUE = 0x1;
  auto bitValueMatcher =
    BitHasValue(I2C_CR1_STOPIE_POSITION, EXPECTED_I2C_CR1_STOPIE_VALUE);
  expectSpecificRegisterSetWithNoChangesAfter(&(virtualI2CPeripheral.CR1), bitValueMatcher);

  const I2C::ErrorCode errorCode =
    virtualI2C.readMemory(RANDOM_SLAVE_ADDRESS, RANDOM_MEMORY_ADDRESS, rxBufferPtr, RANDOM_MSG_LEN);

  ASSERT_THAT(errorCode, Eq(I2C::ErrorCode::OK));
  ASSERT_THAT(virtualI2CPeripheral.CR1, bitValueMatcher);
}

TEST_F(AnI2C, ReadMemoryEnablesTransferCompleteInterrupt)
{
  constexpr uint32_t I2C_CR1_TCIE_POSITION = 6u;
  constexpr uint32_t EXPECTED_I2C_CR1_TCIE_VALUE = 0x1;
  auto bitValueMatcher =
    BitHasValue(I2C_CR1_TCIE_POSITION, EXPECTED_I2C_CR1_TCIE_VALUE);
  expectSpecificRegisterSetWithNoChangesAfter(&(virtualI2CPeripheral.CR1), bitValueMatcher);

  const I2C::ErrorCode errorCode =
    virtualI2C.readMemory(RANDOM_SLAVE_ADDRESS, RANDOM_MEMORY_ADDRESS, rxBufferPtr, RANDOM_MSG_LEN);

  ASSERT_THAT(errorCode, Eq(I2C::ErrorCode::OK));
  ASSERT_THAT(virtualI2CPeripheral.CR1, bitValueMatcher);
}

TEST_F(AnI2C, ReadMemoryDisablesAutoEndModeByClearingAUTOENDBitInCR2Register)
{
  constexpr uint32_t I2C_CR2_AUTOEND_POSITION = 25u;
  constexpr uint32_t EXPECTED_I2C_CR2_AUTOEND_VALUE = 0x0;
  auto bitValueMatcher =
    BitHasValue(I2C_CR2_AUTOEND_POSITION, EXPECTED_I2C_CR2_AUTOEND_VALUE);
  expectSpecificRegisterSetWithNoChangesAfter(&(virtualI2CPeripheral.CR2), bitValueMatcher);

  const I2C::ErrorCode errorCode =
    virtualI2C.readMemory(RANDOM_SLAVE_ADDRESS, RANDOM_MEMORY_ADDRESS, rxBufferPtr, RANDOM_MSG_LEN);

  ASSERT_THAT(errorCode, Eq(I2C::ErrorCode::OK));
  ASSERT_THAT(virtualI2CPeripheral.CR2, bitValueMatcher);
}

TEST_F(AnI2C, ReadMemoryDisablesReloadModeByClearingRELOADBitInCR2Register)
{
  constexpr uint32_t I2C_CR2_RELOAD_POSITION = 24u;
  constexpr uint32_t EXPECTED_I2C_CR2_RELOAD_VALUE = 0x0;
  auto bitValueMatcher =
    BitHasValue(I2C_CR2_RELOAD_POSITION, EXPECTED_I2C_CR2_RELOAD_VALUE);
  expectSpecificRegisterSetWithNoChangesAfter(&(virtualI2CPeripheral.CR2), bitValueMatcher);

  const I2C::ErrorCode errorCode =
    virtualI2C.readMemory(RANDOM_SLAVE_ADDRESS, RANDOM_MEMORY_ADDRESS, rxBufferPtr, RANDOM_MSG_LEN);

  ASSERT_THAT(errorCode, Eq(I2C::ErrorCode::OK));
  ASSERT_THAT(virtualI2CPeripheral.CR2, bitValueMatcher);
}

TEST_F(AnI2C, ReadMemorySetsNBYTESBitsInCR2RegisterToOneInOrderToSendAddressOfMemoryToRead)
{
  constexpr uint32_t I2C_CR2_NBYTES_POSITION = 16u;
  constexpr uint32_t I2C_CR2_NBYTES_SIZE = 8u;
  constexpr uint32_t EXPECTED_I2C_CR2_NBYTES_VALUE = sizeof(uint8_t);
  auto bitValueMatcher =
    BitsHaveValue(I2C_CR2_NBYTES_POSITION, I2C_CR2_NBYTES_SIZE, EXPECTED_I2C_CR2_NBYTES_VALUE);
  expectRegisterSetOnlyOnce(&(virtualI2CPeripheral.CR2), bitValueMatcher);

  const I2C::ErrorCode errorCode =
    virtualI2C.readMemory(RANDOM_SLAVE_ADDRESS, RANDOM_MEMORY_ADDRESS, rxBufferPtr, RANDOM_MSG_LEN);

  ASSERT_THAT(errorCode, Eq(I2C::ErrorCode::OK));
  ASSERT_THAT(virtualI2CPeripheral.CR2, bitValueMatcher);
}

TEST_F(AnI2C, ReadMemorySetsSlaveAddressInSADDBits1To7InCR2RegisterIfChoosenAddressingModeIs7Bits)
{
  constexpr uint32_t I2C_CR2_SADD_POSITION = 1u;
  constexpr uint32_t I2C_CR2_SADD_SIZE = 7u;
  constexpr uint32_t EXPECTED_I2C_CR2_SADD_VALUE = RANDOM_SLAVE_ADDRESS;
  auto bitValueMatcher =
    BitsHaveValue(I2C_CR2_SADD_POSITION, I2C_CR2_SADD_SIZE, EXPECTED_I2C_CR2_SADD_VALUE);
  i2cConfig.addressingMode = I2C::AddressingMode::ADDRESS_7_BITS;
  virtualI2C.init(i2cConfig);
  expectRegisterSetOnlyOnce(&(virtualI2CPeripheral.CR2), bitValueMatcher);

  const I2C::ErrorCode errorCode =
    virtualI2C.readMemory(RANDOM_SLAVE_ADDRESS, RANDOM_MEMORY_ADDRESS, rxBufferPtr, RANDOM_MSG_LEN);

  ASSERT_THAT(errorCode, Eq(I2C::ErrorCode::OK));
  ASSERT_THAT(virtualI2CPeripheral.CR2, bitValueMatcher);
}

TEST_F(AnI2C, ReadMemorySetsSlaveAddressInSADDBits0To9InCR2RegisterIfChoosenAddressingModeIs10Bits)
{
  constexpr uint32_t I2C_CR2_SADD_POSITION = 0u;
  constexpr uint32_t I2C_CR2_SADD_SIZE = 10u;
  constexpr uint32_t EXPECTED_I2C_CR2_SADD_VALUE = RANDOM_SLAVE_ADDRESS;
  auto bitValueMatcher =
    BitsHaveValue(I2C_CR2_SADD_POSITION, I2C_CR2_SADD_SIZE, EXPECTED_I2C_CR2_SADD_VALUE);
  i2cConfig.addressingMode = I2C::AddressingMode::ADDRESS_10_BITS;
  virtualI2C.init(i2cConfig);
  expectRegisterSetOnlyOnce(&(virtualI2CPeripheral.CR2), bitValueMatcher);

  const I2C::ErrorCode errorCode =
    virtualI2C.readMemory(RANDOM_SLAVE_ADDRESS, RANDOM_MEMORY_ADDRESS, rxBufferPtr, RANDOM_MSG_LEN);

  ASSERT_THAT(errorCode, Eq(I2C::ErrorCode::OK));
  ASSERT_THAT(virtualI2CPeripheral.CR2, bitValueMatcher);
}

TEST_F(AnI2C, ReadMemorySetsTransferDirectionToWriteInOrderToSendAddressOfMemoryToRead)
{
  constexpr uint32_t I2C_CR2_RD_WRN_POSITION = 10u;
  constexpr uint32_t EXPECTED_I2C_CR2_RD_WRN_VALUE = 0x0; // write direction
  auto bitValueMatcher =
    BitHasValue(I2C_CR2_RD_WRN_POSITION, EXPECTED_I2C_CR2_RD_WRN_VALUE);
  expectRegisterSetOnlyOnce(&(virtualI2CPeripheral.CR2), bitValueMatcher);

  const I2C::ErrorCode errorCode =
    virtualI2C.readMemory(RANDOM_SLAVE_ADDRESS, RANDOM_MEMORY_ADDRESS, rxBufferPtr, RANDOM_MSG_LEN);

  ASSERT_THAT(errorCode, Eq(I2C::ErrorCode::OK));
  ASSERT_THAT(virtualI2CPeripheral.CR2, bitValueMatcher);
}

TEST_F(AnI2C, ReadMemoryStartsTransferBySettingSTARTBitInCR2Register)
{
  constexpr uint32_t I2C_CR2_START_POSITION = 13u;
  constexpr uint32_t EXPECTED_I2C_CR2_START_VALUE = 0x1;
  auto bitValueMatcher =
    BitHasValue(I2C_CR2_START_POSITION, EXPECTED_I2C_CR2_START_VALUE);
  expectRegisterSetOnlyOnce(&(virtualI2CPeripheral.CR2), bitValueMatcher);

  const I2C::ErrorCode errorCode =
    virtualI2C.readMemory(RANDOM_SLAVE_ADDRESS, RANDOM_MEMORY_ADDRESS, rxBufferPtr, RANDOM_MSG_LEN);

  ASSERT_THAT(errorCode, Eq(I2C::ErrorCode::OK));
  ASSERT_THAT(virtualI2CPeripheral.CR2, bitValueMatcher);
}

TEST_F(AnI2C, ReadMemoryClearsSTOPFlagInCR2Register)
{
  constexpr uint32_t I2C_CR2_STOP_POSITION = 14u;
  constexpr uint32_t EXPECTED_I2C_CR2_STOP_VALUE = 0x0;
  auto bitValueMatcher =
    BitHasValue(I2C_CR2_STOP_POSITION, EXPECTED_I2C_CR2_STOP_VALUE);
  expectRegisterSetOnlyOnce(&(virtualI2CPeripheral.CR2), bitValueMatcher);

  const I2C::ErrorCode errorCode =
    virtualI2C.readMemory(RANDOM_SLAVE_ADDRESS, RANDOM_MEMORY_ADDRESS, rxBufferPtr, RANDOM_MSG_LEN);

  ASSERT_THAT(errorCode, Eq(I2C::ErrorCode::OK));
  ASSERT_THAT(virtualI2CPeripheral.CR2, bitValueMatcher);
}

TEST_F(AnI2C, ReadMemoryFailsIfBUSYBitIsSetInISRRegister)
{
  constexpr uint32_t I2C_ISR_BUSY_POSITION = 15u;
  virtualI2CPeripheral.ISR =
    expectedRegVal(virtualI2CPeripheral.ISR, I2C_ISR_BUSY_POSITION, 1u, 1u);

  ASSERT_THAT(virtualI2C.readMemory(RANDOM_SLAVE_ADDRESS, RANDOM_MEMORY_ADDRESS, rxBufferPtr, RANDOM_MSG_LEN),
    Eq(I2C::ErrorCode::BUSY));
}

TEST_F(AnI2C, ReadMemoryFailsIfAnotherTransactionIsOngoing)
{
  ASSERT_THAT(virtualI2C.read(RANDOM_SLAVE_ADDRESS, rxBufferPtr, RANDOM_MSG_LEN), Eq(I2C::ErrorCode::OK));
  ASSERT_THAT(virtualI2C.readMemory(RANDOM_SLAVE_ADDRESS, RANDOM_MEMORY_ADDRESS, rxBufferPtr, RANDOM_MSG_LEN),
    Eq(I2C::ErrorCode::BUSY));
}

TEST_F(AnI2C, IsTransactionOngoingReturnsTrueIfThereIsAnOngoingReadMemoryTransaction)
{
  virtualI2C.readMemory(RANDOM_SLAVE_ADDRESS, RANDOM_MEMORY_ADDRESS, rxBufferPtr, RANDOM_MSG_LEN);

  ASSERT_THAT(virtualI2C.isTransactionOngoing(), true);
}

TEST_F(AnI2C, WriteMemoryEnablesTransmitInterrupt)
{
  constexpr uint32_t I2C_CR1_TXIE_POSITION = 1u;
  constexpr uint32_t EXPECTED_I2C_CR1_TXIE_VALUE = 0x1;
  auto bitValueMatcher =
    BitHasValue(I2C_CR1_TXIE_POSITION, EXPECTED_I2C_CR1_TXIE_VALUE);
  expectSpecificRegisterSetWithNoChangesAfter(&(virtualI2CPeripheral.CR1), bitValueMatcher);

  const I2C::ErrorCode errorCode =
    virtualI2C.writeMemory(RANDOM_SLAVE_ADDRESS, RANDOM_MEMORY_ADDRESS, RANDOM_MSG, RANDOM_MSG_LEN);

  ASSERT_THAT(errorCode, Eq(I2C::ErrorCode::OK));
  ASSERT_THAT(virtualI2CPeripheral.CR1, bitValueMatcher);
}

TEST_F(AnI2C, WriteMemoryEnablesStopDetectionInterrupt)
{
  constexpr uint32_t I2C_CR1_STOPIE_POSITION = 5u;
  constexpr uint32_t EXPECTED_I2C_CR1_STOPIE_VALUE = 0x1;
  auto bitValueMatcher =
    BitHasValue(I2C_CR1_STOPIE_POSITION, EXPECTED_I2C_CR1_STOPIE_VALUE);
  expectSpecificRegisterSetWithNoChangesAfter(&(virtualI2CPeripheral.CR1), bitValueMatcher);

  const I2C::ErrorCode errorCode =
    virtualI2C.writeMemory(RANDOM_SLAVE_ADDRESS, RANDOM_MEMORY_ADDRESS, RANDOM_MSG, RANDOM_MSG_LEN);

  ASSERT_THAT(errorCode, Eq(I2C::ErrorCode::OK));
  ASSERT_THAT(virtualI2CPeripheral.CR1, bitValueMatcher);
}

TEST_F(AnI2C, WriteMemoryEnablesTransferCompleteInterrupt)
{
  constexpr uint32_t I2C_CR1_TCIE_POSITION = 6u;
  constexpr uint32_t EXPECTED_I2C_CR1_TCIE_VALUE = 0x1;
  auto bitValueMatcher =
    BitHasValue(I2C_CR1_TCIE_POSITION, EXPECTED_I2C_CR1_TCIE_VALUE);
  expectSpecificRegisterSetWithNoChangesAfter(&(virtualI2CPeripheral.CR1), bitValueMatcher);

  const I2C::ErrorCode errorCode =
    virtualI2C.writeMemory(RANDOM_SLAVE_ADDRESS, RANDOM_MEMORY_ADDRESS, RANDOM_MSG, RANDOM_MSG_LEN);

  ASSERT_THAT(errorCode, Eq(I2C::ErrorCode::OK));
  ASSERT_THAT(virtualI2CPeripheral.CR1, bitValueMatcher);
}

TEST_F(AnI2C, WriteMemoryDisablesAutoEndModeByClearingAUTOENDBitInCR2Register)
{
  constexpr uint32_t I2C_CR2_AUTOEND_POSITION = 25u;
  constexpr uint32_t EXPECTED_I2C_CR2_AUTOEND_VALUE = 0x0;
  auto bitValueMatcher =
    BitHasValue(I2C_CR2_AUTOEND_POSITION, EXPECTED_I2C_CR2_AUTOEND_VALUE);
  expectSpecificRegisterSetWithNoChangesAfter(&(virtualI2CPeripheral.CR2), bitValueMatcher);

  const I2C::ErrorCode errorCode =
    virtualI2C.writeMemory(RANDOM_SLAVE_ADDRESS, RANDOM_MEMORY_ADDRESS, RANDOM_MSG, RANDOM_MSG_LEN);

  ASSERT_THAT(errorCode, Eq(I2C::ErrorCode::OK));
  ASSERT_THAT(virtualI2CPeripheral.CR2, bitValueMatcher);
}

TEST_F(AnI2C, WriteMemoryEnablesReloadModeBySettingRELOADBitInCR2Register)
{
  constexpr uint32_t I2C_CR2_RELOAD_POSITION = 24u;
  constexpr uint32_t EXPECTED_I2C_CR2_RELOAD_VALUE = 0x1;
  auto bitValueMatcher =
    BitHasValue(I2C_CR2_RELOAD_POSITION, EXPECTED_I2C_CR2_RELOAD_VALUE);
  expectSpecificRegisterSetWithNoChangesAfter(&(virtualI2CPeripheral.CR2), bitValueMatcher);

  const I2C::ErrorCode errorCode =
    virtualI2C.writeMemory(RANDOM_SLAVE_ADDRESS, RANDOM_MEMORY_ADDRESS, RANDOM_MSG, RANDOM_MSG_LEN);

  ASSERT_THAT(errorCode, Eq(I2C::ErrorCode::OK));
  ASSERT_THAT(virtualI2CPeripheral.CR2, bitValueMatcher);
}

TEST_F(AnI2C, WriteMemorySetsNBYTESBitsInCR2RegisterToOneInOrderToSendAddressOfMemoryToRead)
{
  constexpr uint32_t I2C_CR2_NBYTES_POSITION = 16u;
  constexpr uint32_t I2C_CR2_NBYTES_SIZE = 8u;
  constexpr uint32_t EXPECTED_I2C_CR2_NBYTES_VALUE = sizeof(uint8_t);
  auto bitValueMatcher =
    BitsHaveValue(I2C_CR2_NBYTES_POSITION, I2C_CR2_NBYTES_SIZE, EXPECTED_I2C_CR2_NBYTES_VALUE);
  expectRegisterSetOnlyOnce(&(virtualI2CPeripheral.CR2), bitValueMatcher);

  const I2C::ErrorCode errorCode =
    virtualI2C.writeMemory(RANDOM_SLAVE_ADDRESS, RANDOM_MEMORY_ADDRESS, RANDOM_MSG, RANDOM_MSG_LEN);

  ASSERT_THAT(errorCode, Eq(I2C::ErrorCode::OK));
  ASSERT_THAT(virtualI2CPeripheral.CR2, bitValueMatcher);
}

TEST_F(AnI2C, WriteMemorySetsSlaveAddressInSADDBits1To7InCR2RegisterIfChoosenAddressingModeIs7Bits)
{
  constexpr uint32_t I2C_CR2_SADD_POSITION = 1u;
  constexpr uint32_t I2C_CR2_SADD_SIZE = 7u;
  constexpr uint32_t EXPECTED_I2C_CR2_SADD_VALUE = RANDOM_SLAVE_ADDRESS;
  auto bitValueMatcher =
    BitsHaveValue(I2C_CR2_SADD_POSITION, I2C_CR2_SADD_SIZE, EXPECTED_I2C_CR2_SADD_VALUE);
  i2cConfig.addressingMode = I2C::AddressingMode::ADDRESS_7_BITS;
  virtualI2C.init(i2cConfig);
  expectRegisterSetOnlyOnce(&(virtualI2CPeripheral.CR2), bitValueMatcher);

  const I2C::ErrorCode errorCode =
    virtualI2C.writeMemory(RANDOM_SLAVE_ADDRESS, RANDOM_MEMORY_ADDRESS, RANDOM_MSG, RANDOM_MSG_LEN);

  ASSERT_THAT(errorCode, Eq(I2C::ErrorCode::OK));
  ASSERT_THAT(virtualI2CPeripheral.CR2, bitValueMatcher);
}

TEST_F(AnI2C, WriteMemorySetsSlaveAddressInSADDBits0To9InCR2RegisterIfChoosenAddressingModeIs10Bits)
{
  constexpr uint32_t I2C_CR2_SADD_POSITION = 0u;
  constexpr uint32_t I2C_CR2_SADD_SIZE = 10u;
  constexpr uint32_t EXPECTED_I2C_CR2_SADD_VALUE = RANDOM_SLAVE_ADDRESS;
  auto bitValueMatcher =
    BitsHaveValue(I2C_CR2_SADD_POSITION, I2C_CR2_SADD_SIZE, EXPECTED_I2C_CR2_SADD_VALUE);
  i2cConfig.addressingMode = I2C::AddressingMode::ADDRESS_10_BITS;
  virtualI2C.init(i2cConfig);
  expectRegisterSetOnlyOnce(&(virtualI2CPeripheral.CR2), bitValueMatcher);

  const I2C::ErrorCode errorCode =
    virtualI2C.writeMemory(RANDOM_SLAVE_ADDRESS, RANDOM_MEMORY_ADDRESS, RANDOM_MSG, RANDOM_MSG_LEN);

  ASSERT_THAT(errorCode, Eq(I2C::ErrorCode::OK));
  ASSERT_THAT(virtualI2CPeripheral.CR2, bitValueMatcher);
}

TEST_F(AnI2C, WriteMemorySetsTransferDirectionToWrite)
{
  constexpr uint32_t I2C_CR2_RD_WRN_POSITION = 10u;
  constexpr uint32_t EXPECTED_I2C_CR2_RD_WRN_VALUE = 0x0; // write direction
  auto bitValueMatcher =
    BitHasValue(I2C_CR2_RD_WRN_POSITION, EXPECTED_I2C_CR2_RD_WRN_VALUE);
  expectRegisterSetOnlyOnce(&(virtualI2CPeripheral.CR2), bitValueMatcher);

  const I2C::ErrorCode errorCode =
    virtualI2C.writeMemory(RANDOM_SLAVE_ADDRESS, RANDOM_MEMORY_ADDRESS, RANDOM_MSG, RANDOM_MSG_LEN);

  ASSERT_THAT(errorCode, Eq(I2C::ErrorCode::OK));
  ASSERT_THAT(virtualI2CPeripheral.CR2, bitValueMatcher);
}

TEST_F(AnI2C, WriteMemoryStartsTransferBySettingSTARTBitInCR2Register)
{
  constexpr uint32_t I2C_CR2_START_POSITION = 13u;
  constexpr uint32_t EXPECTED_I2C_CR2_START_VALUE = 0x1;
  auto bitValueMatcher =
    BitHasValue(I2C_CR2_START_POSITION, EXPECTED_I2C_CR2_START_VALUE);
  expectRegisterSetOnlyOnce(&(virtualI2CPeripheral.CR2), bitValueMatcher);

  const I2C::ErrorCode errorCode =
    virtualI2C.writeMemory(RANDOM_SLAVE_ADDRESS, RANDOM_MEMORY_ADDRESS, RANDOM_MSG, RANDOM_MSG_LEN);

  ASSERT_THAT(errorCode, Eq(I2C::ErrorCode::OK));
  ASSERT_THAT(virtualI2CPeripheral.CR2, bitValueMatcher);
}

TEST_F(AnI2C, WriteMemoryClearsSTOPFlagInCR2Register)
{
  constexpr uint32_t I2C_CR2_STOP_POSITION = 14u;
  constexpr uint32_t EXPECTED_I2C_CR2_STOP_VALUE = 0x0;
  auto bitValueMatcher =
    BitHasValue(I2C_CR2_STOP_POSITION, EXPECTED_I2C_CR2_STOP_VALUE);
  expectRegisterSetOnlyOnce(&(virtualI2CPeripheral.CR2), bitValueMatcher);

  const I2C::ErrorCode errorCode =
    virtualI2C.writeMemory(RANDOM_SLAVE_ADDRESS, RANDOM_MEMORY_ADDRESS, RANDOM_MSG, RANDOM_MSG_LEN);

  ASSERT_THAT(errorCode, Eq(I2C::ErrorCode::OK));
  ASSERT_THAT(virtualI2CPeripheral.CR2, bitValueMatcher);
}

TEST_F(AnI2C, WriteMemoryFailsIfAnotherTransactionIsOngoing)
{
  ASSERT_THAT(virtualI2C.write(RANDOM_SLAVE_ADDRESS, RANDOM_MSG, RANDOM_MSG_LEN), Eq(I2C::ErrorCode::OK));
  ASSERT_THAT(virtualI2C.writeMemory(RANDOM_SLAVE_ADDRESS, RANDOM_MEMORY_ADDRESS, RANDOM_MSG, RANDOM_MSG_LEN),
    Eq(I2C::ErrorCode::BUSY));
}

TEST_F(AnI2C, IsTransactionOngoingReturnsTrueIfThereIsAnOngoingWriteMemoryTransaction)
{
  virtualI2C.writeMemory(RANDOM_SLAVE_ADDRESS, RANDOM_MEMORY_ADDRESS, RANDOM_MSG, RANDOM_MSG_LEN);

  ASSERT_THAT(virtualI2C.isTransactionOngoing(), true);
}
