#include "USART.h"
#include "MemoryUtility.h"
#include "MemoryAccess.h"
#include "DriverTest.h"
#include "ClockControlMock.h"
#include "gtest/gtest.h"
#include "gmock/gmock.h"


using namespace ::testing;


class AnUSART : public DriverTest
{
public:

  //! Based on real reset values for RCC register (source STM32L4R9 reference manual)
  static constexpr uint32_t USART_CR1_RESET_VALUE   = 0x00000000;
  static constexpr uint32_t USART_CR2_RESET_VALUE   = 0x00000000;
  static constexpr uint32_t USART_CR3_RESET_VALUE   = 0x00000000;
  static constexpr uint32_t USART_BRR_RESET_VALUE   = 0x00000400;
  static constexpr uint16_t USART_GTPR_RESET_VALUE  = 0x00000000;
  static constexpr uint32_t USART_RTOR_RESET_VALUE  = 0x00000000;
  static constexpr uint16_t USART_RQR_RESET_VALUE   = 0x00000000;
  static constexpr uint32_t USART_ISR_RESET_VALUE   = 0x000000C0;
  static constexpr uint32_t USART_ICR_RESET_VALUE   = 0x00000000;
  static constexpr uint16_t USART_RDR_RESET_VALUE   = 0x00000000;
  static constexpr uint16_t USART_TDR_RESET_VALUE   = 0x00000000;
  static constexpr uint32_t USART_PRESC_RESET_VALUE = 0x00000000;

  static const uint8_t RANDOM_MSG[];
  static const uint32_t RANDOM_MSG_LEN;

  USART_TypeDef virtualUSARTPeripheral;
  NiceMock<ClockControlMock> clockControlMock;
  USART virtualUSART = USART(&virtualUSARTPeripheral, &clockControlMock);
  USART::USARTConfig usartConfig;

  uint32_t m_txCounter;
  uint32_t m_messageIdx;

  void setUSARTInputClockFrequency(uint32_t clockFrequency);
  void setupISRRegisterReadings(void);
  void expectDataToBeWrittenInTDR(const void *messagePtr, uint32_t messageLen);

  void SetUp() override;
  void TearDown() override;
};

const uint8_t AnUSART::RANDOM_MSG[]    = "Random message.";
const uint32_t AnUSART::RANDOM_MSG_LEN = sizeof(AnUSART::RANDOM_MSG);

void AnUSART::SetUp()
{
  DriverTest::SetUp();

  m_txCounter = 1u;
  m_messageIdx = 0u;

  // set values of virtual RCC peripheral to reset values
  virtualUSARTPeripheral.CR1   = USART_CR1_RESET_VALUE;
  virtualUSARTPeripheral.CR2   = USART_CR2_RESET_VALUE;
  virtualUSARTPeripheral.CR3   = USART_CR3_RESET_VALUE;
  virtualUSARTPeripheral.BRR   = USART_BRR_RESET_VALUE;
  virtualUSARTPeripheral.GTPR  = USART_GTPR_RESET_VALUE;
  virtualUSARTPeripheral.RTOR  = USART_RTOR_RESET_VALUE;
  virtualUSARTPeripheral.RQR   = USART_RQR_RESET_VALUE;
  virtualUSARTPeripheral.ISR   = USART_ISR_RESET_VALUE;
  virtualUSARTPeripheral.ICR   = USART_ICR_RESET_VALUE;
  virtualUSARTPeripheral.RDR   = USART_RDR_RESET_VALUE;
  virtualUSARTPeripheral.TDR   = USART_TDR_RESET_VALUE;
  virtualUSARTPeripheral.PRESC = USART_PRESC_RESET_VALUE;

  usartConfig.frameFormat  = USART::FrameFormat::BITS_8_WITHOUT_PARITY;
  usartConfig.baudrate     = USART::Baudrate::BAUDRATE_115200;
  usartConfig.oversampling = USART::Oversampling::OVERSAMPLING_16;
  usartConfig.parity       = USART::Parity::EVEN;
  usartConfig.stopBits     = USART::StopBits::BIT_1_0;
}

void AnUSART::TearDown()
{
  DriverTest::TearDown();
}

void AnUSART::setUSARTInputClockFrequency(uint32_t clockFrequency)
{
  clockControlMock.setReturnClockFrequency(clockFrequency);
}

void AnUSART::setupISRRegisterReadings(void)
{
  ON_CALL(memoryAccessHook, getRegisterValue(&(virtualUSARTPeripheral.ISR)))
    .WillByDefault([&](volatile const uint32_t *registerPtr)
    {
      constexpr uint32_t USART_ISR_TXFNF_POSITION = 7u;
      virtualUSARTPeripheral.ISR =
        expectedRegVal(USART_ISR_RESET_VALUE, USART_ISR_TXFNF_POSITION, 1u, (((m_txCounter++) % 4u) != 0u));

      return virtualUSARTPeripheral.ISR;
    });
}

void AnUSART::expectDataToBeWrittenInTDR(const void *messagePtr, uint32_t messageLen)
{
  EXPECT_CALL(memoryAccessHook, setRegisterValue(&(virtualUSARTPeripheral.TDR), _))
    .Times(messageLen)
    .WillRepeatedly([=](volatile void *registerPtr, uint32_t registerValue)
    {
      ASSERT_THAT(registerValue, reinterpret_cast<const uint8_t*>(messagePtr)[m_messageIdx++]);
      if (m_messageIdx != messageLen)
      {
        virtualUSART.IRQHandler();
      }
    });
}

TEST_F(AnUSART, InitEnablesFIFOMode)
{
  constexpr uint32_t USART_CR1_FIFOEN_POSITION = 29u;
  constexpr uint32_t EXPECTED_USART_CR1_FIFOEN_VALUE = 1u;
  auto bitValueMatcher =
    BitHasValue(USART_CR1_FIFOEN_POSITION, EXPECTED_USART_CR1_FIFOEN_VALUE);
  expectSpecificRegisterSetWithNoChangesAfter(&(virtualUSARTPeripheral.CR1), bitValueMatcher);

  const USART::ErrorCode errorCode = virtualUSART.init(usartConfig);

  ASSERT_THAT(errorCode, Eq(USART::ErrorCode::OK));
  ASSERT_THAT(virtualUSARTPeripheral.CR1, bitValueMatcher);
}

TEST_F(AnUSART, InitSetsWordLengthAndParityControlBitsInCR1AccordingToChoosenFrameFormat)
{
  constexpr uint32_t USART_CR1_PCE_POSITION = 10u;
  constexpr uint32_t USART_CR1_M0_POSITION  = 12u;
  constexpr uint32_t USART_CR1_M1_POSITION  = 28u;
  constexpr uint32_t EXPECTED_USART_CR1_PCE_VALUE = 1u;
  constexpr uint32_t EXPECTED_USART_CR1_M0_VALUE  = 1u;
  constexpr uint32_t EXPECTED_USART_CR1_M1_VALUE  = 0u;
  auto bitValueMatcher =
    AllOf(BitHasValue(USART_CR1_M0_POSITION, EXPECTED_USART_CR1_M0_VALUE),
          BitHasValue(USART_CR1_M1_POSITION, EXPECTED_USART_CR1_M1_VALUE),
          BitHasValue(USART_CR1_PCE_POSITION, EXPECTED_USART_CR1_PCE_VALUE));
  usartConfig.frameFormat = USART::FrameFormat::BITS_8_WITH_PARITY;
  expectSpecificRegisterSetWithNoChangesAfter(&(virtualUSARTPeripheral.CR1), bitValueMatcher);

  const USART::ErrorCode errorCode = virtualUSART.init(usartConfig);

  ASSERT_THAT(errorCode, Eq(USART::ErrorCode::OK));
  ASSERT_THAT(virtualUSARTPeripheral.CR1, bitValueMatcher);
}

TEST_F(AnUSART, InitFailsIfFrameFormatIsOutOfAllowedRange)
{
  usartConfig.frameFormat = static_cast<USART::FrameFormat>(9u);
  expectNoRegisterToChange();

  const USART::ErrorCode errorCode = virtualUSART.init(usartConfig);

  ASSERT_THAT(errorCode, Eq(USART::ErrorCode::USART_CONFIG_PARAM_INVALID_VALUE));
}

TEST_F(AnUSART, InitSetsOversamplingModeBitInCR1AccordingToChoosenOversampling)
{
  constexpr uint32_t USART_CR1_OVER8_POSITION = 15u;
  constexpr uint32_t EXPECTED_USART_CR1_OVER8_VALUE = 1u;
  auto bitValueMatcher =
    BitHasValue(USART_CR1_OVER8_POSITION, EXPECTED_USART_CR1_OVER8_VALUE);
  usartConfig.oversampling = USART::Oversampling::OVERSAMPLING_8;
  expectSpecificRegisterSetWithNoChangesAfter(&(virtualUSARTPeripheral.CR1), bitValueMatcher);

  const USART::ErrorCode errorCode = virtualUSART.init(usartConfig);

  ASSERT_THAT(errorCode, Eq(USART::ErrorCode::OK));
  ASSERT_THAT(virtualUSARTPeripheral.CR1, bitValueMatcher);
}

TEST_F(AnUSART, InitFailsIfOversamplingIsOutOfAllowedRange)
{
  usartConfig.oversampling = static_cast<USART::Oversampling>(2u);
  expectNoRegisterToChange();

  const USART::ErrorCode errorCode = virtualUSART.init(usartConfig);

  ASSERT_THAT(errorCode, Eq(USART::ErrorCode::USART_CONFIG_PARAM_INVALID_VALUE));
}

TEST_F(AnUSART, InitSetsParityBitInCR1AccordingToChoosenParity)
{
  constexpr uint32_t USART_CR1_PS_POSITION = 9u;
  constexpr uint32_t EXPECTED_USART_CR1_PS_VALUE = 1u;
  auto bitValueMatcher =
    BitHasValue(USART_CR1_PS_POSITION, EXPECTED_USART_CR1_PS_VALUE);
  usartConfig.parity = USART::Parity::ODD;
  expectSpecificRegisterSetWithNoChangesAfter(&(virtualUSARTPeripheral.CR1), bitValueMatcher);

  const USART::ErrorCode errorCode = virtualUSART.init(usartConfig);

  ASSERT_THAT(errorCode, Eq(USART::ErrorCode::OK));
  ASSERT_THAT(virtualUSARTPeripheral.CR1, bitValueMatcher);
}

TEST_F(AnUSART, InitFailsIfParityIsOutOfAllowedRange)
{
  usartConfig.parity = static_cast<USART::Parity>(2u);
  expectNoRegisterToChange();

  const USART::ErrorCode errorCode = virtualUSART.init(usartConfig);

  ASSERT_THAT(errorCode, Eq(USART::ErrorCode::USART_CONFIG_PARAM_INVALID_VALUE));
}

TEST_F(AnUSART, InitSetsStopBitsInCR2AccordingToChoosenStopBitsValue)
{
  constexpr uint32_t USART_CR2_STOP_POSITION = 12u;
  constexpr uint32_t EXPECTED_USART_CR2_STOP_VALUE = 0x3;
  auto bitValueMatcher =
    BitHasValue(USART_CR2_STOP_POSITION, EXPECTED_USART_CR2_STOP_VALUE);
  usartConfig.stopBits = USART::StopBits::BIT_1_5;
  expectRegisterSetOnlyOnce(&(virtualUSARTPeripheral.CR2), bitValueMatcher);

  const USART::ErrorCode errorCode = virtualUSART.init(usartConfig);

  ASSERT_THAT(errorCode, Eq(USART::ErrorCode::OK));
  ASSERT_THAT(virtualUSARTPeripheral.CR2, bitValueMatcher);
}

TEST_F(AnUSART, InitFailsIfStopBitsIsOutOfAllowedRange)
{
  usartConfig.stopBits = static_cast<USART::StopBits>(4u);
  expectNoRegisterToChange();

  const USART::ErrorCode errorCode = virtualUSART.init(usartConfig);

  ASSERT_THAT(errorCode, Eq(USART::ErrorCode::USART_CONFIG_PARAM_INVALID_VALUE));
}

TEST_F(AnUSART, InitSetsPRESCRegisterAccordingToChoosenBaudrateValue)
{
  constexpr uint32_t EXPECTED_USART_PRESC_VALUE = 0x0u;
  usartConfig.oversampling = USART::Oversampling::OVERSAMPLING_8;
  usartConfig.baudrate = USART::Baudrate::BAUDRATE_921600;
  expectRegisterSetOnlyOnce(&(virtualUSARTPeripheral.PRESC), EXPECTED_USART_PRESC_VALUE);
  setUSARTInputClockFrequency(48000000u); // 48 MHz

  const USART::ErrorCode errorCode = virtualUSART.init(usartConfig);

  ASSERT_THAT(errorCode, Eq(USART::ErrorCode::OK));
  ASSERT_THAT(virtualUSARTPeripheral.PRESC, EXPECTED_USART_PRESC_VALUE);
}

TEST_F(AnUSART, InitSetsBRRRegisterAccordingToChoosenBaudrateValue)
{
  constexpr uint32_t EXPECTED_USART_BRR_VALUE   = 0x64u;
  usartConfig.oversampling = USART::Oversampling::OVERSAMPLING_8;
  usartConfig.baudrate = USART::Baudrate::BAUDRATE_921600;
  expectRegisterSetOnlyOnce(&(virtualUSARTPeripheral.BRR), EXPECTED_USART_BRR_VALUE);
  setUSARTInputClockFrequency(48000000u); // 48 MHz

  const USART::ErrorCode errorCode = virtualUSART.init(usartConfig);

  ASSERT_THAT(errorCode, Eq(USART::ErrorCode::OK));
  ASSERT_THAT(virtualUSARTPeripheral.BRR, EXPECTED_USART_BRR_VALUE);
}

TEST_F(AnUSART, InitFailsIfBaudrateIsOutOfAllowedRangeofValues)
{
  usartConfig.baudrate = static_cast<USART::Baudrate>(100000u);
  expectNoRegisterToChange();

  const USART::ErrorCode errorCode = virtualUSART.init(usartConfig);

  ASSERT_THAT(errorCode, Eq(USART::ErrorCode::USART_CONFIG_PARAM_INVALID_VALUE));
}

TEST_F(AnUSART, InitDisablesUsartOnTheBeginningOfInitFunctionCall)
{
  constexpr uint32_t USART_CR1_UE_POSITION = 0u;
  constexpr uint32_t EXPECTED_USART_CR1_UE_VALUE = 0u;
  auto bitValueMatcher =
    BitHasValue(USART_CR1_UE_POSITION, EXPECTED_USART_CR1_UE_VALUE);
  expectSpecificRegisterSetToBeCalledFirst(&(virtualUSARTPeripheral.CR1), bitValueMatcher);

  const USART::ErrorCode errorCode = virtualUSART.init(usartConfig);

  ASSERT_THAT(errorCode, Eq(USART::ErrorCode::OK));
}

TEST_F(AnUSART, InitEnablesUsartOnTheEndOfInitFunctionCall)
{
  constexpr uint32_t USART_CR1_UE_POSITION = 0u;
  constexpr uint32_t EXPECTED_USART_CR1_UE_VALUE = 1u;
  auto bitValueMatcher =
    BitHasValue(USART_CR1_UE_POSITION, EXPECTED_USART_CR1_UE_VALUE);
  expectSpecificRegisterSetToBeCalledLast(&(virtualUSARTPeripheral.CR1), bitValueMatcher);

  const USART::ErrorCode errorCode = virtualUSART.init(usartConfig);

  ASSERT_THAT(errorCode, Eq(USART::ErrorCode::OK));
}

TEST_F(AnUSART, WriteEnablesTransmitter)
{
  constexpr uint32_t USART_CR1_TE_POSITION = 3u;
  constexpr uint32_t EXPECTED_USART_CR1_TE_VALUE = 0x1;
  auto bitValueMatcher =
    BitHasValue(USART_CR1_TE_POSITION, EXPECTED_USART_CR1_TE_VALUE);
  expectSpecificRegisterSetToBeCalledLast(&(virtualUSARTPeripheral.CR1), bitValueMatcher);

  const USART::ErrorCode errorCode = virtualUSART.write(RANDOM_MSG, RANDOM_MSG_LEN);

  ASSERT_THAT(errorCode, Eq(USART::ErrorCode::OK));
  ASSERT_THAT(virtualUSARTPeripheral.CR1, bitValueMatcher);
}

TEST_F(AnUSART, WriteEnablesTxFifoThresholdInterrupt)
{
  constexpr uint32_t USART_CR3_TXFTIE_POSITION = 23u;
  constexpr uint32_t EXPECTED_USART_CR3_TXFTIE_VALUE = 0x1;
  auto bitValueMatcher =
    BitHasValue(USART_CR3_TXFTIE_POSITION, EXPECTED_USART_CR3_TXFTIE_VALUE);
  expectRegisterSetOnlyOnce(&(virtualUSARTPeripheral.CR3), bitValueMatcher);

  const USART::ErrorCode errorCode = virtualUSART.write(RANDOM_MSG, RANDOM_MSG_LEN);

  ASSERT_THAT(errorCode, Eq(USART::ErrorCode::OK));
  ASSERT_THAT(virtualUSARTPeripheral.CR3, bitValueMatcher);
}

TEST_F(AnUSART, WriteDisablesTransmissionCompletedInterrupt)
{
  constexpr uint32_t USART_CR1_TCIE_POSITION = 6u;
  constexpr uint32_t EXPECTED_USART_CR1_TCIE_VALUE = 0x0;
  auto bitValueMatcher =
    BitHasValue(USART_CR1_TCIE_POSITION, EXPECTED_USART_CR1_TCIE_VALUE);
  expectRegisterSet(&(virtualUSARTPeripheral.CR1), bitValueMatcher);

  const USART::ErrorCode errorCode = virtualUSART.write(RANDOM_MSG, RANDOM_MSG_LEN);

  ASSERT_THAT(errorCode, Eq(USART::ErrorCode::OK));
  ASSERT_THAT(virtualUSARTPeripheral.CR1, bitValueMatcher);
}

TEST_F(AnUSART, IRQHandlerWritesToTDROnlyIfTxFifoNotFullInterruptFlagIsSet)
{
  setupISRRegisterReadings();
  virtualUSART.write("a", 1u);
  expectRegisterSetOnlyOnce(&(virtualUSARTPeripheral.TDR), Matcher<uint16_t>(_));

  virtualUSART.IRQHandler();
}

TEST_F(AnUSART, IRQHandlerDoesNotWriteToTDRRegisterIfTxFifoThresholdInterruptIsNotEnabled)
{
  constexpr uint32_t USART_CR3_TXFTIE_POSITION = 23u;
  constexpr uint32_t EXPECTED_USART_CR3_TXFTIE_VALUE = 0x0;
  virtualUSARTPeripheral.CR3 =
    expectedRegVal(USART_CR3_RESET_VALUE, USART_CR3_TXFTIE_POSITION, 1u, EXPECTED_USART_CR3_TXFTIE_VALUE);
  setupISRRegisterReadings();
  expectRegisterNotToChange(&(virtualUSARTPeripheral.TDR));

  virtualUSART.IRQHandler();
}

TEST_F(AnUSART, IRQHandlerWritesDataToTransmitInTDR)
{
  setupISRRegisterReadings();
  virtualUSART.write(RANDOM_MSG, RANDOM_MSG_LEN);
  expectDataToBeWrittenInTDR(RANDOM_MSG, RANDOM_MSG_LEN);

  // trigger calling of IRQ handler and writting to TDR register
  virtualUSART.IRQHandler();
}

TEST_F(AnUSART, IRQHandlerDisablesTxFifoThresholdInterruptWhenThereIsNoMoreDataToTransmit)
{
  constexpr uint32_t USART_CR3_TXFTIE_POSITION = 23u;
  constexpr uint32_t EXPECTED_USART_CR3_TXFTIE_VALUE = 0x0;
  setupISRRegisterReadings();
  virtualUSART.write(RANDOM_MSG, RANDOM_MSG_LEN);
  expectDataToBeWrittenInTDR(RANDOM_MSG, RANDOM_MSG_LEN);
  auto bitValueMatcher =
    BitHasValue(USART_CR3_TXFTIE_POSITION, EXPECTED_USART_CR3_TXFTIE_VALUE);
  expectSpecificRegisterSetToBeCalledLast(&(virtualUSARTPeripheral.CR3), bitValueMatcher);

  // trigger calling of IRQ handler and writting to TDR register
  virtualUSART.IRQHandler();
}

TEST_F(AnUSART, IRQHandlerEnablesTransmissionCompleteInterruptWhenThereIsNoMoreDataToTransmit)
{
  constexpr uint32_t USART_CR1_TCIE_POSITION = 6u;
  constexpr uint32_t EXPECTED_USART_CR1_TCIE_VALUE = 0x1;
  setupISRRegisterReadings();
  virtualUSART.write(RANDOM_MSG, RANDOM_MSG_LEN);
  expectDataToBeWrittenInTDR(RANDOM_MSG, RANDOM_MSG_LEN);
  auto bitValueMatcher =
    BitHasValue(USART_CR1_TCIE_POSITION, EXPECTED_USART_CR1_TCIE_VALUE);
  expectSpecificRegisterSetToBeCalledFirst(&(virtualUSARTPeripheral.CR1), bitValueMatcher);

  // trigger calling of IRQ handler and writting to TDR register
  virtualUSART.IRQHandler();
}

TEST_F(AnUSART, IRQHandlerStopsTransmissionOnTranmissionCompleteInterrupt)
{
  constexpr uint32_t USART_CR1_TCIE_POSITION = 6u;
  constexpr uint32_t USART_ISR_TC_POSITION = 6u;
  virtualUSART.write(RANDOM_MSG, RANDOM_MSG_LEN);
  // force values as if tranmission compleate interrupt happened
  virtualUSARTPeripheral.CR1 =
    expectedRegVal(virtualUSARTPeripheral.CR1, USART_CR1_TCIE_POSITION, 1u, 1u);
  virtualUSARTPeripheral.ISR =
    expectedRegVal(virtualUSARTPeripheral.ISR, USART_ISR_TC_POSITION, 1u, 1u);

  virtualUSART.IRQHandler();

  ASSERT_THAT(virtualUSART.isWriteTransactionOngoing(), false);
}

TEST_F(AnUSART, IRQHandlerDisablesTransmissionCompletedInterruptWhenTransmissionIsCompleted)
{
  constexpr uint32_t USART_ISR_TC_POSITION = 6u;
  constexpr uint32_t USART_CR1_TCIE_POSITION = 6u;
  constexpr uint32_t EXPECTED_USART_CR1_TCIE_VALUE = 0x0;
  virtualUSART.write(RANDOM_MSG, RANDOM_MSG_LEN);
  // force values as if tranmission compleate interrupt happened
  virtualUSARTPeripheral.ISR =
    expectedRegVal(virtualUSARTPeripheral.ISR, USART_ISR_TC_POSITION, 1u, 1u);
  virtualUSARTPeripheral.CR1 =
    expectedRegVal(virtualUSARTPeripheral.CR1, USART_CR1_TCIE_POSITION, 1u, 1u);
  auto bitValueMatcher =
    BitHasValue(USART_CR1_TCIE_POSITION, EXPECTED_USART_CR1_TCIE_VALUE);
  expectSpecificRegisterSetWithNoChangesAfter(&(virtualUSARTPeripheral.CR1), bitValueMatcher);

  virtualUSART.IRQHandler();
}

TEST_F(AnUSART, IRQHandlerDisablesTransmitterWhenTransmissionIsCompleted)
{
  constexpr uint32_t USART_ISR_TC_POSITION = 6u;
  constexpr uint32_t USART_CR1_TCIE_POSITION = 6u;
  constexpr uint32_t USART_CR1_TE_POSITION = 3u;
  constexpr uint32_t EXPECTED_USART_CR1_TE_VALUE = 0x0;
  virtualUSART.write(RANDOM_MSG, RANDOM_MSG_LEN);
  // force values as if tranmission compleate interrupt happened
  virtualUSARTPeripheral.ISR =
    expectedRegVal(virtualUSARTPeripheral.ISR, USART_ISR_TC_POSITION, 1u, 1u);
  virtualUSARTPeripheral.CR1 =
    expectedRegVal(virtualUSARTPeripheral.CR1, USART_CR1_TCIE_POSITION, 1u, 1u);
  auto bitValueMatcher =
    BitHasValue(USART_CR1_TE_POSITION, EXPECTED_USART_CR1_TE_VALUE);
  expectSpecificRegisterSetToBeCalledLast(&(virtualUSARTPeripheral.CR1), bitValueMatcher);

  virtualUSART.IRQHandler();
}

TEST_F(AnUSART, WriteFailsIfAnotherWriteTransactionIsOngoing)
{
  ASSERT_THAT(virtualUSART.write(RANDOM_MSG, RANDOM_MSG_LEN), Eq(USART::ErrorCode::OK));
  ASSERT_THAT(virtualUSART.write(RANDOM_MSG, RANDOM_MSG_LEN), Eq(USART::ErrorCode::BUSY));
}

TEST_F(AnUSART, IsWriteTransacationOngoingReturnsTrueIfThereIsAnOngoingWrite)
{
  virtualUSART.write(RANDOM_MSG, RANDOM_MSG_LEN);

  ASSERT_THAT(virtualUSART.isWriteTransactionOngoing(), true);
}

TEST_F(AnUSART, IsWriteTransacationOngoingReturnsFalseIfAnotherTransactionIsNotStarted)
{
  ASSERT_THAT(virtualUSART.isWriteTransactionOngoing(), false);
}