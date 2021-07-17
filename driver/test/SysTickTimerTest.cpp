#include "SysTickTimer.h"
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

  SysTick_Type virtualSysTick = 
  {
    .CTRL  = SYSTICK_CTRL_RESET_VALUE,
    .LOAD  = SYSTICK_LOAD_RESET_VALUE,
    .VAL   = SYSTICK_VAL_RESET_VALUE,
    .CALIB = SYSTICK_CALIB_RESET_VALUE
  };
  NiceMock<ClockControlMock> clockControlMock;
  SysTickTimer virtualSysTickTimer = SysTickTimer(&virtualSysTick, &clockControlMock);
  SysTickTimer::SysTickTimerConfig sysTickTimerConfig;

  void SetUp() override;
  void TearDown() override;
};

const uint32_t ASysTick::RANDOM_SYSCLOCK_FREQ    = 120000000u; // 120MHz
const uint32_t ASysTick::RANDOM_TICKS_PER_SECOND = 1000u;

void ASysTick::SetUp()
{
  DriverTest::SetUp();

  sysTickTimerConfig.ticksPerSecond  = 10000u;
  sysTickTimerConfig.enableInterrupt = false;
  sysTickTimerConfig.enableOnInit    = false;
}

void ASysTick::TearDown()
{
  DriverTest::TearDown();
}


TEST_F(ASysTick, InitSetsLOADRegisterAccordingToTicksPerSecondValueAndSystemClockFrequency)
{
  const uint32_t expectedLoadValue = RANDOM_SYSCLOCK_FREQ / RANDOM_TICKS_PER_SECOND;
  sysTickTimerConfig.ticksPerSecond = RANDOM_TICKS_PER_SECOND;
  clockControlMock.setReturnClockFrequency(RANDOM_SYSCLOCK_FREQ);
  expectRegisterSetOnlyOnce(&(virtualSysTick.LOAD), expectedLoadValue);
  
  const SysTickTimer::ErrorCode errorCode = virtualSysTickTimer.init(sysTickTimerConfig);

  ASSERT_THAT(errorCode, Eq(SysTickTimer::ErrorCode::OK));
  ASSERT_THAT(virtualSysTick.LOAD, Eq(expectedLoadValue));
}

TEST_F(ASysTick, InitSetsVALRegisterToZero)
{
  expectRegisterSetOnlyOnce(&(virtualSysTick.VAL), 0u);
  
  const SysTickTimer::ErrorCode errorCode = virtualSysTickTimer.init(sysTickTimerConfig);

  ASSERT_THAT(errorCode, Eq(SysTickTimer::ErrorCode::OK));
  ASSERT_THAT(virtualSysTick.VAL, Eq(0u));
}

TEST_F(ASysTick, InitSetsTICKINTBitInCTRLRegisterAccordingToEnableInterruptValueFromrConfig)
{
  constexpr uint32_t SYSTICK_CTRL_TICKINT_POSITION       = 1u;
  constexpr uint32_t EXPECTED_SYSTICK_CTRL_TICKINT_VALUE = 1u;
  auto bitValueMatcher = 
    BitHasValue(SYSTICK_CTRL_TICKINT_POSITION, EXPECTED_SYSTICK_CTRL_TICKINT_VALUE);
  sysTickTimerConfig.enableInterrupt = true;
  expectSpecificRegisterSetWithNoChangesAfter(&(virtualSysTick.CTRL), bitValueMatcher);

  const SysTickTimer::ErrorCode errorCode = virtualSysTickTimer.init(sysTickTimerConfig);

  ASSERT_THAT(errorCode, Eq(SysTickTimer::ErrorCode::OK));
  ASSERT_THAT(virtualSysTick.CTRL, bitValueMatcher);
}

TEST_F(ASysTick, InitSetsENABLEBitInCTRLRegisterAccordingToEnableOnInitValueFromConfig)
{
  constexpr uint32_t SYSTICK_CTRL_ENABLE_POSITION       = 0u;
  constexpr uint32_t EXPECTED_SYSTICK_CTRL_ENABLE_VALUE = 1u;
  auto bitValueMatcher = 
    BitHasValue(SYSTICK_CTRL_ENABLE_POSITION, EXPECTED_SYSTICK_CTRL_ENABLE_VALUE);
  sysTickTimerConfig.enableOnInit = true;
  expectSpecificRegisterSetWithNoChangesAfter(&(virtualSysTick.CTRL), bitValueMatcher);

  const SysTickTimer::ErrorCode errorCode = virtualSysTickTimer.init(sysTickTimerConfig);

  ASSERT_THAT(errorCode, Eq(SysTickTimer::ErrorCode::OK));
  ASSERT_THAT(virtualSysTick.CTRL, bitValueMatcher);
}

TEST_F(ASysTick, InitSetsCLKSOURCEBitInCTRLRegisterToOneAkaSysTickUsesCPUClock)
{
  constexpr uint32_t SYSTICK_CTRL_CLKSOURCE_POSITION       = 2u;
  constexpr uint32_t EXPECTED_SYSTICK_CTRL_CLKSOURCE_VALUE = 1u;
  auto bitValueMatcher = 
    BitHasValue(SYSTICK_CTRL_CLKSOURCE_POSITION, EXPECTED_SYSTICK_CTRL_CLKSOURCE_VALUE);
  expectSpecificRegisterSetWithNoChangesAfter(&(virtualSysTick.CTRL), bitValueMatcher);

  const SysTickTimer::ErrorCode errorCode = virtualSysTickTimer.init(sysTickTimerConfig);

  ASSERT_THAT(errorCode, Eq(SysTickTimer::ErrorCode::OK));
  ASSERT_THAT(virtualSysTick.CTRL, bitValueMatcher);
}

TEST_F(ASysTick, InitFailsIfGettingOfSystemClockFrequencyFails)
{
  clockControlMock.setReturnErrorCode(ClockControl::ErrorCode::INVALID_CLOCK_SOURCE);

  const SysTickTimer::ErrorCode errorCode = virtualSysTickTimer.init(sysTickTimerConfig);

  ASSERT_THAT(errorCode, Eq(SysTickTimer::ErrorCode::RELOAD_VALUE_CALCULATION_PROBLEM));
}

TEST_F(ASysTick, InitFailsIfReloadValueForGivenTicksPerSecondIsOutOfRange)
{
  clockControlMock.setReturnClockFrequency(120000000u); // 120 MHz
  sysTickTimerConfig.ticksPerSecond = 5u;

  const SysTickTimer::ErrorCode errorCode = virtualSysTickTimer.init(sysTickTimerConfig);

  ASSERT_THAT(errorCode, Eq(SysTickTimer::ErrorCode::RELOAD_VALUE_OUT_OF_RANGE));
}