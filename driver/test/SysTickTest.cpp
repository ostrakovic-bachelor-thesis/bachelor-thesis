#include "SysTick.h"
#include "MemoryUtility.h"
#include "MemoryAccess.h"
#include "DriverTest.h"
#include "ClockControlMock.h"
#include "gtest/gtest.h"
#include "gmock/gmock.h"


using namespace ::testing;


class ASysTick : public DriverTest
{
public:
  //! Based on real reset values for SysTick register
  static constexpr uint32_t SYSTICK_CTRL_RESET_VALUE  = 0x00000000;
  static constexpr uint32_t SYSTICK_LOAD_RESET_VALUE  = 0x00000000;
  static constexpr uint32_t SYSTICK_VAL_RESET_VALUE   = 0xABCDABCD;
  static constexpr uint32_t SYSTICK_CALIB_RESET_VALUE = 0x00000000;

  static const uint32_t RANDOM_SYSCLOCK_FREQ;
  static const uint32_t RANDOM_TICKS_PER_SECOND;

  SysTick_Type virtualSysTickCoreHardware =
  {
    .CTRL  = SYSTICK_CTRL_RESET_VALUE,
    .LOAD  = SYSTICK_LOAD_RESET_VALUE,
    .VAL   = SYSTICK_VAL_RESET_VALUE,
    .CALIB = SYSTICK_CALIB_RESET_VALUE
  };
  NiceMock<ClockControlMock> clockControlMock;
  // TODO NiceMock<InterruptControllerMock> interruptControllerMock;
  InterruptController interruptControllerMock = InterruptController(nullptr, nullptr);
  SysTick virtualSysTick = SysTick(&virtualSysTickCoreHardware, &clockControlMock, &interruptControllerMock);
  SysTick::SysTickConfig sysTickConfig;

  void SetUp() override;
  void TearDown() override;
};

const uint32_t ASysTick::RANDOM_SYSCLOCK_FREQ    = 120000000u; // 120MHz
const uint32_t ASysTick::RANDOM_TICKS_PER_SECOND = 1000u;

void ASysTick::SetUp()
{
  DriverTest::SetUp();

  sysTickConfig.ticksPerSecond  = 10000u;
  sysTickConfig.enableInterrupt = false;
  sysTickConfig.enableOnInit    = true;
}

void ASysTick::TearDown()
{
  DriverTest::TearDown();
}


TEST_F(ASysTick, InitSetsLOADRegisterAccordingToTicksPerSecondValueAndSystemClockFrequency)
{
  const uint32_t expectedLoadValue = RANDOM_SYSCLOCK_FREQ / RANDOM_TICKS_PER_SECOND;
  sysTickConfig.ticksPerSecond = RANDOM_TICKS_PER_SECOND;
  clockControlMock.setReturnClockFrequency(RANDOM_SYSCLOCK_FREQ);
  expectRegisterSetOnlyOnce(&(virtualSysTickCoreHardware.LOAD), expectedLoadValue);

  const SysTick::ErrorCode errorCode = virtualSysTick.init(sysTickConfig);

  ASSERT_THAT(errorCode, Eq(SysTick::ErrorCode::OK));
  ASSERT_THAT(virtualSysTickCoreHardware.LOAD, Eq(expectedLoadValue));
}

TEST_F(ASysTick, InitSetsVALRegisterToZero)
{
  expectRegisterSetOnlyOnce(&(virtualSysTickCoreHardware.VAL), 0u);

  const SysTick::ErrorCode errorCode = virtualSysTick.init(sysTickConfig);

  ASSERT_THAT(errorCode, Eq(SysTick::ErrorCode::OK));
  ASSERT_THAT(virtualSysTickCoreHardware.VAL, Eq(0u));
}

TEST_F(ASysTick, InitSetsTICKINTBitInCTRLRegisterAccordingToEnableInterruptValueFromrConfig)
{
  constexpr uint32_t SYSTICK_CTRL_TICKINT_POSITION       = 1u;
  constexpr uint32_t EXPECTED_SYSTICK_CTRL_TICKINT_VALUE = 1u;
  auto bitValueMatcher =
    BitHasValue(SYSTICK_CTRL_TICKINT_POSITION, EXPECTED_SYSTICK_CTRL_TICKINT_VALUE);
  sysTickConfig.enableInterrupt = true;
  expectSpecificRegisterSetWithNoChangesAfter(&(virtualSysTickCoreHardware.CTRL), bitValueMatcher);

  const SysTick::ErrorCode errorCode = virtualSysTick.init(sysTickConfig);

  ASSERT_THAT(errorCode, Eq(SysTick::ErrorCode::OK));
  ASSERT_THAT(virtualSysTickCoreHardware.CTRL, bitValueMatcher);
}

TEST_F(ASysTick, InitSetsENABLEBitInCTRLRegisterAccordingToEnableOnInitValueFromConfig)
{
  constexpr uint32_t SYSTICK_CTRL_ENABLE_POSITION       = 0u;
  constexpr uint32_t EXPECTED_SYSTICK_CTRL_ENABLE_VALUE = 1u;
  auto bitValueMatcher =
    BitHasValue(SYSTICK_CTRL_ENABLE_POSITION, EXPECTED_SYSTICK_CTRL_ENABLE_VALUE);
  sysTickConfig.enableOnInit = true;
  expectSpecificRegisterSetWithNoChangesAfter(&(virtualSysTickCoreHardware.CTRL), bitValueMatcher);

  const SysTick::ErrorCode errorCode = virtualSysTick.init(sysTickConfig);

  ASSERT_THAT(errorCode, Eq(SysTick::ErrorCode::OK));
  ASSERT_THAT(virtualSysTickCoreHardware.CTRL, bitValueMatcher);
}

TEST_F(ASysTick, InitSetsCLKSOURCEBitInCTRLRegisterToOneAkaSysTickUsesCPUClock)
{
  constexpr uint32_t SYSTICK_CTRL_CLKSOURCE_POSITION       = 2u;
  constexpr uint32_t EXPECTED_SYSTICK_CTRL_CLKSOURCE_VALUE = 1u;
  auto bitValueMatcher =
    BitHasValue(SYSTICK_CTRL_CLKSOURCE_POSITION, EXPECTED_SYSTICK_CTRL_CLKSOURCE_VALUE);
  expectSpecificRegisterSetWithNoChangesAfter(&(virtualSysTickCoreHardware.CTRL), bitValueMatcher);

  const SysTick::ErrorCode errorCode = virtualSysTick.init(sysTickConfig);

  ASSERT_THAT(errorCode, Eq(SysTick::ErrorCode::OK));
  ASSERT_THAT(virtualSysTickCoreHardware.CTRL, bitValueMatcher);
}

TEST_F(ASysTick, InitFailsIfGettingOfSystemClockFrequencyFails)
{
  clockControlMock.setReturnErrorCode(ClockControl::ErrorCode::INVALID_CLOCK_SOURCE);

  const SysTick::ErrorCode errorCode = virtualSysTick.init(sysTickConfig);

  ASSERT_THAT(errorCode, Eq(SysTick::ErrorCode::RELOAD_VALUE_CALCULATION_PROBLEM));
}

TEST_F(ASysTick, InitFailsIfReloadValueForGivenTicksPerSecondIsOutOfRange)
{
  clockControlMock.setReturnClockFrequency(120000000u); // 120 MHz
  sysTickConfig.ticksPerSecond = 5u;

  const SysTick::ErrorCode errorCode = virtualSysTick.init(sysTickConfig);

  ASSERT_THAT(errorCode, Eq(SysTick::ErrorCode::RELOAD_VALUE_OUT_OF_RANGE));
}

TEST_F(ASysTick, getTicksReturnsZeroIfIRQHandlerHasNotBeenCalledSoFar)
{
  const uint64_t ticks = virtualSysTick.getTicks();

  ASSERT_THAT(ticks, Eq(0ull));
}

TEST_F(ASysTick, IRQHandlerCallIncreasesTickCountByOne)
{
  const uint64_t startTicks = virtualSysTick.getTicks();
  virtualSysTick.IRQHandler();
  const uint64_t endTicks = virtualSysTick.getTicks();

  ASSERT_THAT(endTicks - startTicks, Eq(1ull));
}

TEST_F(ASysTick, GetsTicksPerSecondReturnsNumberOfTicksPerSecondIfInitializationOfSysTickWasSuccessful)
{
  virtualSysTick.init(sysTickConfig);

  ASSERT_THAT(virtualSysTick.getTicksPerSecond(), Eq(sysTickConfig.ticksPerSecond));
}

TEST_F(ASysTick, GetsTicksPerSecondReturnZeroIfSysTickIsNotInitialized)
{
  ASSERT_THAT(virtualSysTick.getTicksPerSecond(), Eq(0u));
}

TEST_F(ASysTick, GetElapsedTimeInMsGetsElapsedTimeinMillisecondsSinceProvidedTimestamp)
{
  constexpr uint64_t EXPECTED_ELAPSED_TIME_IN_MS = 500u;
  constexpr uint64_t MILLISECONDS_IN_SECOND = 1000u;
  virtualSysTick.init(sysTickConfig);
  const uint32_t numberOfIRQHandlerCalls =
    (EXPECTED_ELAPSED_TIME_IN_MS * virtualSysTick.getTicksPerSecond()) / MILLISECONDS_IN_SECOND;
  const uint64_t timestamp = virtualSysTick.getTicks();
  for (uint32_t i = 0u; i < numberOfIRQHandlerCalls; ++i)
  {
    virtualSysTick.IRQHandler();
  }

  ASSERT_THAT(virtualSysTick.getElapsedTimeInMs(timestamp), Eq(EXPECTED_ELAPSED_TIME_IN_MS));
}

TEST_F(ASysTick, GetElapsedTimeInUsGetsElapsedTimeinMicrosecondsSinceProvidedTimestamp)
{
  constexpr uint64_t EXPECTED_ELAPSED_TIME_IN_US = 2500u;
  constexpr uint64_t MICROSECONDS_IN_SECOND = 1000000u;
  virtualSysTick.init(sysTickConfig);
  const uint32_t numberOfIRQHandlerCalls =
    (EXPECTED_ELAPSED_TIME_IN_US * virtualSysTick.getTicksPerSecond()) / MICROSECONDS_IN_SECOND;
  const uint64_t timestamp = virtualSysTick.getTicks();
  for (uint32_t i = 0u; i < numberOfIRQHandlerCalls; ++i)
  {
    virtualSysTick.IRQHandler();
  }

  ASSERT_THAT(virtualSysTick.getElapsedTimeInUs(timestamp), Eq(EXPECTED_ELAPSED_TIME_IN_US));
}
