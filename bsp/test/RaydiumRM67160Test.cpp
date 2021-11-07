#include "RaydiumRM67160.h"
#include "SysTickMock.h"
#include "MemoryUtility.h"
#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include <cstdint>
#include <memory>

#include <iostream>


using namespace ::testing;


class ARaydiumRM67160 : public Test
{
public:

  struct CallbackMock
  {
    virtual ~CallbackMock() = default;

    // mock methods
    MOCK_METHOD(void, enableDSI3V3Callback, (), ());
    MOCK_METHOD(void, enableDSI1V8Callback, (), ());
    MOCK_METHOD(void, setDSIResetLineToHighCallback, (), ());
    MOCK_METHOD(void, setDSIResetLineToLowCallback, (), ());
  };

  static std::unique_ptr<CallbackMock> s_callbackMockPtr;
  NiceMock<SysTickMock> sysTickMock;
  RaydiumRM67160 virtualRaydiumRM67160 = RaydiumRM67160(&sysTickMock);
  RaydiumRM67160::RaydiumRM67160Config raydiumRM67160Config;

  uint64_t m_sysTickElapsedMs;

  void setupSysTickReadings(void);
  void expectEnableDSI3V3CallbackCall(uint64_t timestamp, uint64_t afterTimestampInMs);
  void expectEnableDSI1V8CallbackCall(uint64_t timestamp, uint64_t afterTimestampInMs);
  void expectSetDSIResetLineToHighCallbackCall(uint64_t timestamp, uint64_t afterTimestampInMs);
  void expectSetDSIResetLineToLowCallbackCall(uint64_t timestamp, uint64_t afterTimestampInMs);

  static void enableDSI3V3Callback(void);
  static void enableDSI1V8Callback(void);
  static void setDSIResetLineToHighCallback(void);
  static void setDSIResetLineToLowCallback(void);
  static void dummyCallback(void);

  void SetUp() override;
  void TearDown() override;
};

std::unique_ptr<ARaydiumRM67160::CallbackMock> ARaydiumRM67160::s_callbackMockPtr;

void ARaydiumRM67160::SetUp()
{
  s_callbackMockPtr = std::make_unique<CallbackMock>();
  m_sysTickElapsedMs = 0u;

  raydiumRM67160Config.enableDSI3V3Callback          = dummyCallback;
  raydiumRM67160Config.enableDSI1V8Callback          = dummyCallback;
  raydiumRM67160Config.setDSIResetLineToLowCallback  = dummyCallback;
  raydiumRM67160Config.setDSIResetLineToHighCallback = dummyCallback;
}

void ARaydiumRM67160::TearDown()
{
  s_callbackMockPtr.reset(nullptr);
}

void ARaydiumRM67160::setupSysTickReadings(void)
{
  ON_CALL(sysTickMock, getElapsedTimeInMs(_))
    .WillByDefault([&](uint64_t timestamp)
    {
      return (m_sysTickElapsedMs++) - timestamp;
    });

  ON_CALL(sysTickMock, getTicks())
    .WillByDefault([&](void)
    {
      return m_sysTickElapsedMs;
    });
}

void ARaydiumRM67160::expectEnableDSI3V3CallbackCall(uint64_t timestamp, uint64_t afterTimestampInMs)
{
  EXPECT_CALL(*s_callbackMockPtr, enableDSI3V3Callback())
    .Times(1u)
    .WillOnce([&, timestamp, afterTimestampInMs]()
    {
      EXPECT_THAT(sysTickMock.getElapsedTimeInMs(timestamp), AllOf(Ge(afterTimestampInMs), Le(afterTimestampInMs + 1u)));
    });
}

void ARaydiumRM67160::expectEnableDSI1V8CallbackCall(uint64_t timestamp, uint64_t afterTimestampInMs)
{
  EXPECT_CALL(*s_callbackMockPtr, enableDSI1V8Callback())
    .Times(1u)
    .WillOnce([&, timestamp, afterTimestampInMs]()
    {
      EXPECT_THAT(sysTickMock.getElapsedTimeInMs(timestamp), AllOf(Ge(afterTimestampInMs), Le(afterTimestampInMs + 2u)));
    });
}

void ARaydiumRM67160::expectSetDSIResetLineToLowCallbackCall(uint64_t timestamp, uint64_t afterTimestampInMs)
{
  EXPECT_CALL(*s_callbackMockPtr, setDSIResetLineToLowCallback())
    .Times(1u)
    .WillOnce([&, timestamp, afterTimestampInMs]()
    {
      EXPECT_THAT(sysTickMock.getElapsedTimeInMs(timestamp), AllOf(Ge(afterTimestampInMs), Le(afterTimestampInMs + 3u)));
    });
}

void ARaydiumRM67160::expectSetDSIResetLineToHighCallbackCall(uint64_t timestamp, uint64_t afterTimestampInMs)
{
  EXPECT_CALL(*s_callbackMockPtr, setDSIResetLineToHighCallback())
    .Times(1u)
    .WillOnce([&, timestamp, afterTimestampInMs]()
    {
      EXPECT_THAT(sysTickMock.getElapsedTimeInMs(timestamp), AllOf(Ge(afterTimestampInMs), Le(afterTimestampInMs + 4u)));
    });
}

void ARaydiumRM67160::enableDSI3V3Callback(void)
{
  s_callbackMockPtr->enableDSI3V3Callback();
}

void ARaydiumRM67160::enableDSI1V8Callback(void)
{
  s_callbackMockPtr->enableDSI1V8Callback();
}

void ARaydiumRM67160::setDSIResetLineToHighCallback(void)
{
  s_callbackMockPtr->setDSIResetLineToHighCallback();
}

void ARaydiumRM67160::setDSIResetLineToLowCallback(void)
{
  s_callbackMockPtr->setDSIResetLineToLowCallback();
}

void ARaydiumRM67160::dummyCallback(void)
{

}


TEST_F(ARaydiumRM67160, InitAtTheBegginingSetDSIResetLineToLow)
{
  setupSysTickReadings();
  raydiumRM67160Config.setDSIResetLineToLowCallback = setDSIResetLineToLowCallback;
  const uint64_t timestamp = sysTickMock.getTicks();
  expectSetDSIResetLineToLowCallbackCall(timestamp, 0u);

  const RaydiumRM67160::ErrorCode errorCode = virtualRaydiumRM67160.init(raydiumRM67160Config);

  ASSERT_THAT(errorCode, Eq(RaydiumRM67160::ErrorCode::OK));
}

TEST_F(ARaydiumRM67160, InitWaits5MillisecondAfterResetLineIsSetToLowBeforeEnablingDSI3V3PowerSupply)
{
  constexpr uint64_t ENABLE_DSI_3V3_WAIT_PERIOD_MS = 5u;
  setupSysTickReadings();
  raydiumRM67160Config.enableDSI3V3Callback = enableDSI3V3Callback;
  const uint64_t timestamp = sysTickMock.getTicks();
  expectEnableDSI3V3CallbackCall(timestamp, ENABLE_DSI_3V3_WAIT_PERIOD_MS);

  const RaydiumRM67160::ErrorCode errorCode = virtualRaydiumRM67160.init(raydiumRM67160Config);

  ASSERT_THAT(errorCode, Eq(RaydiumRM67160::ErrorCode::OK));
}

TEST_F(ARaydiumRM67160, InitWaits1MillisecondAfterDSI3V3PowerSupplyIsEnabledBeforeEnablingDSI1V8PowerSupply)
{
  constexpr uint64_t ENABLE_DSI_3V3_WAIT_PERIOD_MS = 5u;
  constexpr uint64_t ENABLE_DSI_1V8_WAIT_PERIOD_MS = 1u;
  constexpr uint64_t TOTAL_WAIT_TIME_MS = ENABLE_DSI_3V3_WAIT_PERIOD_MS + ENABLE_DSI_1V8_WAIT_PERIOD_MS;
  setupSysTickReadings();
  raydiumRM67160Config.enableDSI1V8Callback = enableDSI1V8Callback;
  const uint64_t timestamp = sysTickMock.getTicks();
  expectEnableDSI1V8CallbackCall(timestamp, TOTAL_WAIT_TIME_MS);

  const RaydiumRM67160::ErrorCode errorCode = virtualRaydiumRM67160.init(raydiumRM67160Config);

  ASSERT_THAT(errorCode, Eq(RaydiumRM67160::ErrorCode::OK));
}

TEST_F(ARaydiumRM67160, InitWaits15MillisecondAfterDSI1V8PowerSupplyIsEnabledBeforeSettingDSIResetLineToHigh)
{
  constexpr uint64_t ENABLE_DSI_3V3_WAIT_PERIOD_MS        = 5u;
  constexpr uint64_t ENABLE_DSI_1V8_WAIT_PERIOD_MS        = 1u;
  constexpr uint64_t SET_DSI_RESET_TO_HIGH_WAIT_PERIOD_MS = 15u;
  constexpr uint64_t TOTAL_WAIT_TIME_MS =
    ENABLE_DSI_3V3_WAIT_PERIOD_MS +
    ENABLE_DSI_1V8_WAIT_PERIOD_MS +
    SET_DSI_RESET_TO_HIGH_WAIT_PERIOD_MS;
  setupSysTickReadings();
  raydiumRM67160Config.setDSIResetLineToHighCallback = setDSIResetLineToHighCallback;
  const uint64_t timestamp = sysTickMock.getTicks();
  expectSetDSIResetLineToHighCallbackCall(timestamp, TOTAL_WAIT_TIME_MS);

  const RaydiumRM67160::ErrorCode errorCode = virtualRaydiumRM67160.init(raydiumRM67160Config);

  ASSERT_THAT(errorCode, Eq(RaydiumRM67160::ErrorCode::OK));
}

TEST_F(ARaydiumRM67160, InitWaits120MilisecondsAfterDSIResetLineIsSetToHighBeforeReturning)
{
  constexpr uint64_t ENABLE_DSI_3V3_WAIT_PERIOD_MS        = 5u;
  constexpr uint64_t ENABLE_DSI_1V8_WAIT_PERIOD_MS        = 1u;
  constexpr uint64_t SET_DSI_RESET_TO_HIGH_WAIT_PERIOD_MS = 15u;
  constexpr uint64_t WAIT_BEFORE_RETURN_AFTER_RESET_LINE_IS_HIGH_PERIOD_MS = 120u;
  constexpr uint64_t TOTAL_INIT_TIME_MS =
    ENABLE_DSI_3V3_WAIT_PERIOD_MS +
    ENABLE_DSI_1V8_WAIT_PERIOD_MS +
    SET_DSI_RESET_TO_HIGH_WAIT_PERIOD_MS +
    WAIT_BEFORE_RETURN_AFTER_RESET_LINE_IS_HIGH_PERIOD_MS;
  setupSysTickReadings();
  const uint64_t timestamp = sysTickMock.getTicks();

  const RaydiumRM67160::ErrorCode errorCode = virtualRaydiumRM67160.init(raydiumRM67160Config);

  ASSERT_THAT(errorCode, Eq(RaydiumRM67160::ErrorCode::OK));
  EXPECT_THAT(sysTickMock.getElapsedTimeInMs(timestamp), AllOf(Ge(TOTAL_INIT_TIME_MS), Le(TOTAL_INIT_TIME_MS + 4u)));
}