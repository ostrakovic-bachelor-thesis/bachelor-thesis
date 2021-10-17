#include "EXTI.h"
#include "MemoryUtility.h"
#include "MemoryAccess.h"
#include "DriverTest.h"
#include <memory>
#include "gtest/gtest.h"
#include "gmock/gmock.h"


using namespace ::testing;


class AnEXTI : public DriverTest
{
public:

  class IRQHandlerCallbackMock
  {
  public:
    virtual ~IRQHandlerCallbackMock() = default;

    // mock methods
    MOCK_METHOD(void, interruptCallback, (EXTI::EXTILine));
  };

  //! Based on real reset values for EXTI registers
  static constexpr uint32_t EXTI_IMR1_RESET_VALUE   = 0xFF820000;
  static constexpr uint32_t EXTI_EMR1_RESET_VALUE   = 0x00000000;
  static constexpr uint32_t EXTI_RTSR1_RESET_VALUE  = 0x00000000;
  static constexpr uint32_t EXTI_FTSR1_RESET_VALUE  = 0x00000000;
  static constexpr uint32_t EXTI_SWIER1_RESET_VALUE = 0x00000000;
  static constexpr uint32_t EXTI_PR1_RESET_VALUE    = 0x00000000;
  static constexpr uint32_t EXTI_IMR2_RESET_VALUE   = 0x00000187;
  static constexpr uint32_t EXTI_EMR2_RESET_VALUE   = 0x00000000;
  static constexpr uint32_t EXTI_RTSR2_RESET_VALUE  = 0x00000000;
  static constexpr uint32_t EXTI_FTSR2_RESET_VALUE  = 0x00000000;
  static constexpr uint32_t EXTI_SWIER2_RESET_VALUE = 0x00000000;
  static constexpr uint32_t EXTI_PR2_RESET_VALUE    = 0x00000000;

  EXTI_TypeDef virtualEXTIPeripheral;
  EXTI virtualEXTI= EXTI(&virtualEXTIPeripheral);
  EXTI::EXTIConfig extiConfig;

  static std::unique_ptr<IRQHandlerCallbackMock> irqHandlerCallbackMockPtr;
  static void interruptCallback(EXTI::EXTILine extiLine);

  void SetUp() override;
  void TearDown() override;
};

std::unique_ptr<AnEXTI::IRQHandlerCallbackMock> AnEXTI::irqHandlerCallbackMockPtr;

void AnEXTI::SetUp()
{
  DriverTest::SetUp();

  // set values of virtual EXTI peripheral registers to reset values
  virtualEXTIPeripheral.IMR1   = EXTI_IMR1_RESET_VALUE;
  virtualEXTIPeripheral.EMR1   = EXTI_EMR1_RESET_VALUE;
  virtualEXTIPeripheral.RTSR1  = EXTI_RTSR1_RESET_VALUE;
  virtualEXTIPeripheral.FTSR1  = EXTI_FTSR1_RESET_VALUE;
  virtualEXTIPeripheral.SWIER1 = EXTI_SWIER1_RESET_VALUE;
  virtualEXTIPeripheral.PR1    = EXTI_PR1_RESET_VALUE;
  virtualEXTIPeripheral.IMR2   = EXTI_IMR2_RESET_VALUE;
  virtualEXTIPeripheral.EMR2   = EXTI_EMR2_RESET_VALUE;
  virtualEXTIPeripheral.RTSR2  = EXTI_RTSR2_RESET_VALUE;
  virtualEXTIPeripheral.FTSR2  = EXTI_FTSR2_RESET_VALUE;
  virtualEXTIPeripheral.SWIER2 = EXTI_SWIER2_RESET_VALUE;
  virtualEXTIPeripheral.PR2    = EXTI_PR2_RESET_VALUE;

  irqHandlerCallbackMockPtr = std::make_unique<AnEXTI::IRQHandlerCallbackMock>();
}

void AnEXTI::TearDown()
{
  DriverTest::TearDown();

  irqHandlerCallbackMockPtr.reset();
}

void AnEXTI::interruptCallback(EXTI::EXTILine extiLine)
{
  irqHandlerCallbackMockPtr->interruptCallback(extiLine);
}


TEST_F(AnEXTI, GetPeripheralTagReturnsPointerToUnderlayingEXTIPeripheralCastedToPeripheralType)
{
  ASSERT_THAT(virtualEXTI.getPeripheralTag(),
    Eq(static_cast<Peripheral>(reinterpret_cast<uintptr_t>(&virtualEXTIPeripheral))));
}

TEST_F(AnEXTI, ConfigureEXTILineSetsIMxValueToOneInIMRxRegisterIfConfigParameterIsInterruptMaskedIsEqualToFalse)
{
  constexpr uint32_t EXTI_IMR1_IMX_POSITION = static_cast<uint32_t>(EXTI::EXTILine::LINE2);
  constexpr uint32_t EXPECTED_EXTI_IMR1_IMX_VALUE = 1u;
  extiConfig.isInterruptMasked = false;
  auto bitValueMatcher =
    BitHasValue(EXTI_IMR1_IMX_POSITION, EXPECTED_EXTI_IMR1_IMX_VALUE);
  expectSpecificRegisterSetToBeCalledFirst(&(virtualEXTIPeripheral.IMR1), bitValueMatcher);

  const EXTI::ErrorCode errorCode = virtualEXTI.configureEXTILine(EXTI::EXTILine::LINE2, extiConfig);

  ASSERT_THAT(errorCode, Eq(EXTI::ErrorCode::OK));
}

TEST_F(AnEXTI, ConfigureEXTILineSetsIMxValueToZeroInIMRxRegisterIfConfigParameterIsInterruptMaskedIsEqualToTrue)
{
  static constexpr uint32_t NUMBER_OF_BITS_IN_UINT32_T = 32u;
  constexpr uint32_t EXTI_IMR2_IMX_POSITION =
    static_cast<uint32_t>(EXTI::EXTILine::LINE33) % NUMBER_OF_BITS_IN_UINT32_T;
  constexpr uint32_t EXPECTED_EXTI_IMR2_IMX_VALUE = 0u;
  extiConfig.isInterruptMasked = true;
  auto bitValueMatcher =
    BitHasValue(EXTI_IMR2_IMX_POSITION, EXPECTED_EXTI_IMR2_IMX_VALUE);
  expectSpecificRegisterSetToBeCalledFirst(&(virtualEXTIPeripheral.IMR2), bitValueMatcher);

  const EXTI::ErrorCode errorCode = virtualEXTI.configureEXTILine(EXTI::EXTILine::LINE33, extiConfig);

  ASSERT_THAT(errorCode, Eq(EXTI::ErrorCode::OK));
}

TEST_F(AnEXTI, ConfigureEXTILineDoesNotChangeValuesOfRTSR1RegisterIfConfigParameterIsInterruptMaskedIsEqualToTrue)
{
  extiConfig.isInterruptMasked = true;
  extiConfig.interruptTrigger  = EXTI::InterruptTrigger::RISING_EDGE;
  expectRegisterNotToChange(&(virtualEXTIPeripheral.RTSR1));

  const EXTI::ErrorCode errorCode = virtualEXTI.configureEXTILine(EXTI::EXTILine::LINE22, extiConfig);

  ASSERT_THAT(errorCode, Eq(EXTI::ErrorCode::OK));
}

TEST_F(AnEXTI, ConfigureEXTILineDoesNotChangeValuesOfRTSR2RegisterIfConfigParameterIsInterruptMaskedIsEqualToTrue)
{
  extiConfig.isInterruptMasked = true;
  extiConfig.interruptTrigger  = EXTI::InterruptTrigger::RISING_EDGE;
  expectRegisterNotToChange(&(virtualEXTIPeripheral.RTSR2));

  const EXTI::ErrorCode errorCode = virtualEXTI.configureEXTILine(EXTI::EXTILine::LINE30, extiConfig);

  ASSERT_THAT(errorCode, Eq(EXTI::ErrorCode::OK));
}

TEST_F(AnEXTI, ConfigureEXTILineDoesNotChangeValuesOfFTSR1RegisterIfConfigParameterIsInterruptMaskedIsEqualToTrue)
{
  extiConfig.isInterruptMasked = true;
  extiConfig.interruptTrigger  = EXTI::InterruptTrigger::BOTH_EDGE;
  expectRegisterNotToChange(&(virtualEXTIPeripheral.FTSR1));

  const EXTI::ErrorCode errorCode = virtualEXTI.configureEXTILine(EXTI::EXTILine::LINE2, extiConfig);

  ASSERT_THAT(errorCode, Eq(EXTI::ErrorCode::OK));
}

TEST_F(AnEXTI, ConfigureEXTILineDoesNotChangeValuesOfFTSR2RegisterIfConfigParameterIsInterruptMaskedIsEqualToTrue)
{
  extiConfig.isInterruptMasked = true;
  extiConfig.interruptTrigger  = EXTI::InterruptTrigger::FALLING_EDGE;
  expectRegisterNotToChange(&(virtualEXTIPeripheral.FTSR2));

  const EXTI::ErrorCode errorCode = virtualEXTI.configureEXTILine(EXTI::EXTILine::LINE32, extiConfig);

  ASSERT_THAT(errorCode, Eq(EXTI::ErrorCode::OK));
}

TEST_F(AnEXTI, ConfigureEXTILineSetsRTxValueToOneInRTSRxRegisterIfInterruptTriggerIsRisingEdge)
{
  static constexpr uint32_t NUMBER_OF_BITS_IN_UINT32_T = 32u;
  constexpr uint32_t EXTI_RTSR2_RTX_POSITION =
    static_cast<uint32_t>(EXTI::EXTILine::LINE36) % NUMBER_OF_BITS_IN_UINT32_T;
  constexpr uint32_t EXPECTED_EXTI_RTSR2_RTX_VALUE = 1u;
  extiConfig.isInterruptMasked = false;
  extiConfig.interruptTrigger  = EXTI::InterruptTrigger::RISING_EDGE;
  auto bitValueMatcher =
    BitHasValue(EXTI_RTSR2_RTX_POSITION, EXPECTED_EXTI_RTSR2_RTX_VALUE);
  expectSpecificRegisterSetToBeCalledFirst(&(virtualEXTIPeripheral.RTSR2), bitValueMatcher);

  const EXTI::ErrorCode errorCode = virtualEXTI.configureEXTILine(EXTI::EXTILine::LINE36, extiConfig);

  ASSERT_THAT(errorCode, Eq(EXTI::ErrorCode::OK));
}

TEST_F(AnEXTI, ConfigureEXTILineSetsRTxValueToOneInRTSRxRegisterIfInterruptTriggerIsBothEdge)
{
  static constexpr uint32_t NUMBER_OF_BITS_IN_UINT32_T = 32u;
  constexpr uint32_t EXTI_RTSR2_RTX_POSITION =
    static_cast<uint32_t>(EXTI::EXTILine::LINE37) % NUMBER_OF_BITS_IN_UINT32_T;
  constexpr uint32_t EXPECTED_EXTI_RTSR2_RTX_VALUE = 1u;
  extiConfig.isInterruptMasked = false;
  extiConfig.interruptTrigger  = EXTI::InterruptTrigger::BOTH_EDGE;
  auto bitValueMatcher =
    BitHasValue(EXTI_RTSR2_RTX_POSITION, EXPECTED_EXTI_RTSR2_RTX_VALUE);
  expectSpecificRegisterSetToBeCalledFirst(&(virtualEXTIPeripheral.RTSR2), bitValueMatcher);

  const EXTI::ErrorCode errorCode = virtualEXTI.configureEXTILine(EXTI::EXTILine::LINE37, extiConfig);

  ASSERT_THAT(errorCode, Eq(EXTI::ErrorCode::OK));
}

TEST_F(AnEXTI, ConfigureEXTILineSetsRTxValueToZeroInRTSRxRegisterIfInterruptTriggerIsFallingEdge)
{
  constexpr uint32_t EXTI_RTSR1_RTX_POSITION = static_cast<uint32_t>(EXTI::EXTILine::LINE20);
  constexpr uint32_t EXPECTED_EXTI_RTSR1_RTX_VALUE = 0u;
  extiConfig.isInterruptMasked = false;
  extiConfig.interruptTrigger  = EXTI::InterruptTrigger::FALLING_EDGE;
  auto bitValueMatcher =
    BitHasValue(EXTI_RTSR1_RTX_POSITION, EXPECTED_EXTI_RTSR1_RTX_VALUE);
  expectSpecificRegisterSetToBeCalledFirst(&(virtualEXTIPeripheral.RTSR1), bitValueMatcher);

  const EXTI::ErrorCode errorCode = virtualEXTI.configureEXTILine(EXTI::EXTILine::LINE20, extiConfig);

  ASSERT_THAT(errorCode, Eq(EXTI::ErrorCode::OK));
}

TEST_F(AnEXTI, ConfigureEXTILineSetsFTxValueToOneInFTSRxRegisterIfInterruptTriggerIsFallingEdge)
{
  constexpr uint32_t EXTI_FTSR1_FTX_POSITION = static_cast<uint32_t>(EXTI::EXTILine::LINE20);
  constexpr uint32_t EXPECTED_EXTI_FTSR1_FTX_VALUE = 1u;
  extiConfig.isInterruptMasked = false;
  extiConfig.interruptTrigger  = EXTI::InterruptTrigger::FALLING_EDGE;
  auto bitValueMatcher =
    BitHasValue(EXTI_FTSR1_FTX_POSITION, EXPECTED_EXTI_FTSR1_FTX_VALUE);
  expectSpecificRegisterSetToBeCalledFirst(&(virtualEXTIPeripheral.FTSR1), bitValueMatcher);

  const EXTI::ErrorCode errorCode = virtualEXTI.configureEXTILine(EXTI::EXTILine::LINE20, extiConfig);

  ASSERT_THAT(errorCode, Eq(EXTI::ErrorCode::OK));
}

TEST_F(AnEXTI, ConfigureEXTILineSetsFTxValueToOneInFTSRxRegisterIfInterruptTriggerIsBothEdge)
{
  constexpr uint32_t EXTI_FTSR1_FTX_POSITION = static_cast<uint32_t>(EXTI::EXTILine::LINE20);
  constexpr uint32_t EXPECTED_EXTI_FTSR1_FTX_VALUE = 1u;
  extiConfig.isInterruptMasked = false;
  extiConfig.interruptTrigger  = EXTI::InterruptTrigger::BOTH_EDGE;
  auto bitValueMatcher =
    BitHasValue(EXTI_FTSR1_FTX_POSITION, EXPECTED_EXTI_FTSR1_FTX_VALUE);
  expectSpecificRegisterSetToBeCalledFirst(&(virtualEXTIPeripheral.FTSR1), bitValueMatcher);

  const EXTI::ErrorCode errorCode = virtualEXTI.configureEXTILine(EXTI::EXTILine::LINE20, extiConfig);

  ASSERT_THAT(errorCode, Eq(EXTI::ErrorCode::OK));
}

TEST_F(AnEXTI, ConfigureEXTILineSetsFTxValueToZeroInFTSRxRegisterIfInterruptTriggerIsRisingEdge)
{
  static constexpr uint32_t NUMBER_OF_BITS_IN_UINT32_T = 32u;
  constexpr uint32_t EXTI_FTSR2_FTX_POSITION =
    static_cast<uint32_t>(EXTI::EXTILine::LINE38) % NUMBER_OF_BITS_IN_UINT32_T;
  constexpr uint32_t EXPECTED_EXTI_FTSR2_FTX_VALUE = 0u;
  extiConfig.isInterruptMasked = false;
  extiConfig.interruptTrigger  = EXTI::InterruptTrigger::RISING_EDGE;
  auto bitValueMatcher =
    BitHasValue(EXTI_FTSR2_FTX_POSITION, EXPECTED_EXTI_FTSR2_FTX_VALUE);
  expectSpecificRegisterSetToBeCalledFirst(&(virtualEXTIPeripheral.FTSR2), bitValueMatcher);

  const EXTI::ErrorCode errorCode = virtualEXTI.configureEXTILine(EXTI::EXTILine::LINE38, extiConfig);

  ASSERT_THAT(errorCode, Eq(EXTI::ErrorCode::OK));
}

TEST_F(AnEXTI, IRQHandlerClearsPendingInterruptsWhichAreInProvidenRange)
{
  constexpr uint32_t EXTI_PR1_PIF7_POSITION = 7u;
  constexpr uint32_t EXPECTED_EXTI_PR1_PIF7_VALUE = 1u;
  // set value of pending register as if interrupt on EXTI line 7 happened
  virtualEXTIPeripheral.PR1 =
    expectedRegVal(EXTI_PR1_RESET_VALUE, EXTI_PR1_PIF7_POSITION, 1u, 1u);
  auto bitValueMatcher =
    BitHasValue(EXTI_PR1_PIF7_POSITION, EXPECTED_EXTI_PR1_PIF7_VALUE);
  expectRegisterSetOnlyOnce(&(virtualEXTIPeripheral.PR1), bitValueMatcher);

  virtualEXTI.IRQHandler(EXTI::EXTILine::LINE5, EXTI::EXTILine::LINE9);
}

TEST_F(AnEXTI, IRQHandlerDoesNotClearPendingInterruptsWhichAreNotInProvidenRange)
{
  constexpr uint32_t EXTI_PR1_PIF3_POSITION = 3u;
  constexpr uint32_t EXPECTED_EXTI_PR1_PIF3_VALUE = 1u;
  // set value of pending register as if interrupt on EXTI line 3 happened
  virtualEXTIPeripheral.PR1 =
    expectedRegVal(EXTI_PR1_RESET_VALUE, EXTI_PR1_PIF3_POSITION, 1u, 1u);
  auto bitValueMatcher =
    BitHasValue(EXTI_PR1_PIF3_POSITION, EXPECTED_EXTI_PR1_PIF3_VALUE);
  doesNotExpectRegisterSet(&(virtualEXTIPeripheral.PR1), bitValueMatcher);

  virtualEXTI.IRQHandler(EXTI::EXTILine::LINE5, EXTI::EXTILine::LINE9);
}

TEST_F(AnEXTI, RuntimeTaskCallsInterruptCallbacksForAllInterruptsPreviouslyClearedInIRQHandler)
{
  constexpr uint32_t EXTI_PR1_PIF3_POSITION = 3u;
  // configure interrupt callback for the given line
  extiConfig.interruptCallback = interruptCallback;
  virtualEXTI.configureEXTILine(EXTI::EXTILine::LINE3, extiConfig);
  // set value of pending register as if interrupt on EXTI line 3 happened
  virtualEXTIPeripheral.PR1 =
    expectedRegVal(EXTI_PR1_RESET_VALUE, EXTI_PR1_PIF3_POSITION, 1u, 1u);
  EXPECT_CALL(*irqHandlerCallbackMockPtr, interruptCallback(EXTI::EXTILine::LINE3))
    .Times(1u);
  virtualEXTI.IRQHandler(EXTI::EXTILine::LINE3, EXTI::EXTILine::LINE3);

  const EXTI::ErrorCode errorCode = virtualEXTI.runtimeTask();

  ASSERT_THAT(errorCode, Eq(EXTI::ErrorCode::OK));
}

TEST_F(AnEXTI, RuntimeTaskDoesNotDoAnythingWhenInterruptOnEXTILineOccursIfInterruptCallbacksIsNotDefined)
{
  constexpr uint32_t EXTI_PR1_PIF3_POSITION = 3u;
  // configure interrupt callback for the given line
  extiConfig.interruptCallback = nullptr;
  virtualEXTI.configureEXTILine(EXTI::EXTILine::LINE3, extiConfig);
  // set value of pending register as if interrupt on EXTI line 3 happened
  virtualEXTIPeripheral.PR1 =
    expectedRegVal(EXTI_PR1_RESET_VALUE, EXTI_PR1_PIF3_POSITION, 1u, 1u);
  virtualEXTI.IRQHandler(EXTI::EXTILine::LINE3, EXTI::EXTILine::LINE3);

  const EXTI::ErrorCode errorCode = virtualEXTI.runtimeTask();

  ASSERT_THAT(errorCode, Eq(EXTI::ErrorCode::OK));
}