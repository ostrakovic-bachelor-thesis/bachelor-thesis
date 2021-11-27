#include "RaydiumRM67160.h"
#include "DSIHostMock.h"
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
  NiceMock<DSIHostMock> dsiHostMock;
  NiceMock<SysTickMock> sysTickMock;
  RaydiumRM67160 virtualRaydiumRM67160 = RaydiumRM67160(&dsiHostMock, &sysTickMock);
  RaydiumRM67160::RaydiumRM67160Config raydiumRM67160Config;

  uint64_t m_sysTickElapsedMs;
  uint32_t m_dcsCommandIndex;
  uint64_t m_timestamp;
  bool m_isSpecificDcsCommandSetActivated;
  bool m_isExpectedDcsCommandIssued;
  bool m_isDSIResetLineSetToHigh;

  void setupSysTickReadings(void);
  void expectEnableDSI3V3CallbackCall(uint64_t timestamp, uint64_t afterTimestampInMs);
  void expectEnableDSI1V8CallbackCall(uint64_t timestamp, uint64_t afterTimestampInMs);
  void expectSetDSIResetLineToHighCallbackCall(uint64_t timestamp, uint64_t afterTimestampInMs);
  void expectSetDSIResetLineToLowCallbackCall(uint64_t timestamp, uint64_t afterTimestampInMs);
  void expectWaitPeriodAfterDSIResetLineIsSetToHigh(uint64_t waitPeriodMs);
  template<size_t N>
  void expectDCSShortWrites(uint8_t commandSet, const uint8_t (&dcsCommands)[N][2]);
  template<size_t N>
  void assertThatAllDCSShortWritesAreIssued(const uint8_t (&dcsCommands)[N][2]);
  void expectDCSShortWrite(uint8_t commandSet, uint8_t expectedDcsCommand, uint8_t expectedParameter);
  void expectDCSShortWrite(uint8_t commandSet, uint8_t expectedDcsCommand);
  void assertThatExpectedDCSShortWriteIsIssued(void);
  void expectDCSLongWrite(
    uint8_t commandSet,
    uint8_t expectedDcsCommand,
    const void *expectedDataPtr,
    uint16_t expectedDataSize);
  void assertThatExpectedDCSLongWriteIsIssued(void);
  void expectWaitPeriodAfterDCSCommand(
    uint8_t commandSet,
    uint8_t expectedDcsCommand,
    uint64_t waitPeriodMs);

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
  s_callbackMockPtr  = std::make_unique<CallbackMock>();
  m_sysTickElapsedMs = 0u;
  m_dcsCommandIndex  = 0u;
  m_isSpecificDcsCommandSetActivated = false;
  m_isExpectedDcsCommandIssued       = false;
  m_isDSIResetLineSetToHigh          = false;

  raydiumRM67160Config.enableDSI3V3Callback          = dummyCallback;
  raydiumRM67160Config.enableDSI1V8Callback          = dummyCallback;
  raydiumRM67160Config.setDSIResetLineToLowCallback  = dummyCallback;
  raydiumRM67160Config.setDSIResetLineToHighCallback = dummyCallback;

  setupSysTickReadings();
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

void ARaydiumRM67160::expectWaitPeriodAfterDSIResetLineIsSetToHigh(uint64_t waitPeriodMs)
{
  EXPECT_CALL(*s_callbackMockPtr, setDSIResetLineToHighCallback())
    .Times(1u)
    .WillOnce([&]()
    {
      m_isDSIResetLineSetToHigh = true;
      m_timestamp = sysTickMock.getTicks();
    });

  EXPECT_CALL(dsiHostMock, dcsShortWrite(_, _, _))
    .Times(AnyNumber())
    .WillRepeatedly([&, waitPeriodMs](DSIHost::VirtualChannelID, uint8_t dcsCommand, uint8_t parameter)
    {
      if (m_isDSIResetLineSetToHigh)
      {
        EXPECT_THAT(sysTickMock.getElapsedTimeInMs(m_timestamp), Ge(waitPeriodMs));
      }

      return DSIHost::ErrorCode::OK;
    });

  EXPECT_CALL(dsiHostMock, dcsShortWrite(_, _))
    .Times(AnyNumber())
    .WillRepeatedly([&, waitPeriodMs](DSIHost::VirtualChannelID, uint8_t dcsCommand)
    {
      if (m_isDSIResetLineSetToHigh)
      {
        EXPECT_THAT(sysTickMock.getElapsedTimeInMs(m_timestamp), Ge(waitPeriodMs));
      }

      return DSIHost::ErrorCode::OK;
    });

    EXPECT_CALL(dsiHostMock, dcsLongWrite(_, _, _, _))
    .Times(AnyNumber())
    .WillRepeatedly([&, waitPeriodMs]
      (DSIHost::VirtualChannelID, uint8_t dcsCommand, const void *dataPtr, uint16_t dataSize)
      {
        if (m_isDSIResetLineSetToHigh)
        {
          EXPECT_THAT(sysTickMock.getElapsedTimeInMs(m_timestamp), Ge(waitPeriodMs));
        }

        return DSIHost::ErrorCode::OK;
      });
}

template<size_t N>
void ARaydiumRM67160::expectDCSShortWrites(uint8_t commandSet, const uint8_t (&dcsCommands)[N][2])
{
  EXPECT_CALL(dsiHostMock, dcsShortWrite(_, _, _))
    .Times(AtLeast(N))
    .WillRepeatedly([&, commandSet](DSIHost::VirtualChannelID, uint8_t dcsCommand, uint8_t parameter)
    {
      constexpr uint8_t DCS_COMMAND_MAUCCTR = 0xFE;

      if (not m_isSpecificDcsCommandSetActivated)
      {
        if ((DCS_COMMAND_MAUCCTR == dcsCommand) && (commandSet == parameter))
        {
          m_isSpecificDcsCommandSetActivated = true;
        }
      }
      else if (m_dcsCommandIndex < N)
      {
        EXPECT_THAT(dcsCommand, dcsCommands[m_dcsCommandIndex][0]);
        EXPECT_THAT(parameter, dcsCommands[m_dcsCommandIndex][1]);
        ++m_dcsCommandIndex;
      }

      return DSIHost::ErrorCode::OK;
    });
}

template<size_t N>
void ARaydiumRM67160::assertThatAllDCSShortWritesAreIssued(const uint8_t (&dcsCommands)[N][2])
{
  ASSERT_THAT(m_isSpecificDcsCommandSetActivated, Eq(true));
  ASSERT_THAT(m_dcsCommandIndex, Eq(N));
}

void ARaydiumRM67160::expectDCSShortWrite(
  uint8_t commandSet,
  uint8_t expectedDcsCommand,
  uint8_t expectedParameter)
{
  EXPECT_CALL(dsiHostMock, dcsShortWrite(_, _, _))
    .Times(AtLeast(2u))
    .WillRepeatedly([&, commandSet, expectedDcsCommand, expectedParameter]
      (DSIHost::VirtualChannelID, uint8_t dcsCommand, uint8_t parameter)
      {
        constexpr uint8_t DCS_COMMAND_MAUCCTR = 0xFE;

        if (DCS_COMMAND_MAUCCTR == dcsCommand)
        {
          m_isSpecificDcsCommandSetActivated = (commandSet == parameter);
        }
        else if ((expectedDcsCommand == dcsCommand) && (true == m_isSpecificDcsCommandSetActivated))
        {
          EXPECT_THAT(parameter, expectedParameter);
          m_isExpectedDcsCommandIssued = true;
        }

        return DSIHost::ErrorCode::OK;
      });
}

void ARaydiumRM67160::expectDCSShortWrite(
  uint8_t commandSet,
  uint8_t expectedDcsCommand)
{
  EXPECT_CALL(dsiHostMock, dcsShortWrite(_, _, _))
    .Times(AtLeast(1u))
    .WillRepeatedly([&, commandSet] (DSIHost::VirtualChannelID, uint8_t dcsCommand, uint8_t parameter)
    {
      constexpr uint8_t DCS_COMMAND_MAUCCTR = 0xFE;

      if (DCS_COMMAND_MAUCCTR == dcsCommand)
      {
        m_isSpecificDcsCommandSetActivated = (commandSet == parameter);
      }

      return DSIHost::ErrorCode::OK;
    });

  EXPECT_CALL(dsiHostMock, dcsShortWrite(_, _))
    .Times(AtLeast(1u))
    .WillRepeatedly([&, expectedDcsCommand] (DSIHost::VirtualChannelID, uint8_t dcsCommand)
    {
      if ((expectedDcsCommand == dcsCommand) && (true == m_isSpecificDcsCommandSetActivated))
      {
        m_isExpectedDcsCommandIssued = true;
      }

      return DSIHost::ErrorCode::OK;
    });
}

void ARaydiumRM67160::assertThatExpectedDCSShortWriteIsIssued(void)
{
  EXPECT_THAT(m_isExpectedDcsCommandIssued, Eq(true));
}

void ARaydiumRM67160::expectDCSLongWrite(
  uint8_t commandSet,
  uint8_t expectedDcsCommand,
  const void *expectedDataPtr,
  uint16_t expectedDataSize)
{
  EXPECT_CALL(dsiHostMock, dcsShortWrite(_, _, _))
    .Times(AtLeast(1u))
    .WillRepeatedly([&, commandSet] (DSIHost::VirtualChannelID, uint8_t dcsCommand, uint8_t parameter)
    {
      constexpr uint8_t DCS_COMMAND_MAUCCTR = 0xFE;

      if (DCS_COMMAND_MAUCCTR == dcsCommand)
      {
        m_isSpecificDcsCommandSetActivated = (commandSet == parameter);
      }

      return DSIHost::ErrorCode::OK;
    });

  EXPECT_CALL(dsiHostMock, dcsLongWrite(_, _, _, _))
    .Times(AtLeast(1u))
    .WillRepeatedly([&, expectedDcsCommand, expectedDataPtr, expectedDataSize]
      (DSIHost::VirtualChannelID, uint8_t dcsCommand, const void *dataPtr, uint16_t dataSize)
      {
        if ((expectedDcsCommand == dcsCommand) && (true == m_isSpecificDcsCommandSetActivated))
        {
          EXPECT_THAT(dataSize, expectedDataSize);
          for (uint8_t i = 0u; i < expectedDataSize; ++i)
          {
            EXPECT_THAT(reinterpret_cast<const uint8_t*>(dataPtr)[i],
              Eq(reinterpret_cast<const uint8_t*>(expectedDataPtr)[i]));
          }
          m_isExpectedDcsCommandIssued = true;
        }

        return DSIHost::ErrorCode::OK;
      });

}

void ARaydiumRM67160::assertThatExpectedDCSLongWriteIsIssued(void)
{
  EXPECT_THAT(m_isExpectedDcsCommandIssued, Eq(true));
}

void ARaydiumRM67160::expectWaitPeriodAfterDCSCommand(
  uint8_t commandSet,
  uint8_t expectedDcsCommand,
  uint64_t waitPeriodMs)
{
  EXPECT_CALL(dsiHostMock, dcsShortWrite(_, _, _))
    .Times(AtLeast(1u))
    .WillRepeatedly([&, commandSet, waitPeriodMs] (DSIHost::VirtualChannelID, uint8_t dcsCommand, uint8_t parameter)
    {
      constexpr uint8_t DCS_COMMAND_MAUCCTR = 0xFE;

      if (DCS_COMMAND_MAUCCTR == dcsCommand)
      {
        m_isSpecificDcsCommandSetActivated = (commandSet == parameter);
      }

      if (m_isExpectedDcsCommandIssued)
      {
        EXPECT_THAT(sysTickMock.getElapsedTimeInMs(m_timestamp), Ge(waitPeriodMs));
      }

      return DSIHost::ErrorCode::OK;
    });

  EXPECT_CALL(dsiHostMock, dcsShortWrite(_, _))
    .Times(AtLeast(1u))
    .WillRepeatedly([&, expectedDcsCommand, waitPeriodMs] (DSIHost::VirtualChannelID, uint8_t dcsCommand)
    {
      if (m_isExpectedDcsCommandIssued)
      {
        EXPECT_THAT(sysTickMock.getElapsedTimeInMs(m_timestamp), Ge(waitPeriodMs));
      }

      if ((expectedDcsCommand == dcsCommand) && (true == m_isSpecificDcsCommandSetActivated))
      {
        m_isExpectedDcsCommandIssued = true;
        m_timestamp = sysTickMock.getTicks();
      }

      return DSIHost::ErrorCode::OK;
    });

    EXPECT_CALL(dsiHostMock, dcsLongWrite(_, _, _, _))
    .Times(AnyNumber())
    .WillRepeatedly([&, waitPeriodMs]
      (DSIHost::VirtualChannelID, uint8_t dcsCommand, const void *dataPtr, uint16_t dataSize)
      {
        if (m_isExpectedDcsCommandIssued)
        {
          EXPECT_THAT(sysTickMock.getElapsedTimeInMs(m_timestamp), Ge(waitPeriodMs));
        }

        return DSIHost::ErrorCode::OK;
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
  raydiumRM67160Config.setDSIResetLineToLowCallback = setDSIResetLineToLowCallback;
  const uint64_t timestamp = sysTickMock.getTicks();
  expectSetDSIResetLineToLowCallbackCall(timestamp, 0u);

  const RaydiumRM67160::ErrorCode errorCode = virtualRaydiumRM67160.init(raydiumRM67160Config);

  ASSERT_THAT(errorCode, Eq(RaydiumRM67160::ErrorCode::OK));
}

TEST_F(ARaydiumRM67160, InitWaits5MillisecondAfterResetLineIsSetToLowBeforeEnablingDSI3V3PowerSupply)
{
  constexpr uint64_t ENABLE_DSI_3V3_WAIT_PERIOD_MS = 5u;
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
  raydiumRM67160Config.setDSIResetLineToHighCallback = setDSIResetLineToHighCallback;
  const uint64_t timestamp = sysTickMock.getTicks();
  expectSetDSIResetLineToHighCallbackCall(timestamp, TOTAL_WAIT_TIME_MS);

  const RaydiumRM67160::ErrorCode errorCode = virtualRaydiumRM67160.init(raydiumRM67160Config);

  ASSERT_THAT(errorCode, Eq(RaydiumRM67160::ErrorCode::OK));
}

TEST_F(ARaydiumRM67160, InitWaits120MilisecondsAfterDSIResetLineIsSetToHighBeforeIssuingFirstDCSCommand)
{
  constexpr uint64_t WAIT_BEFORE_ISSUING_FIRST_DCS_COMMAND_PERIOD_MS = 120u;
  raydiumRM67160Config.setDSIResetLineToHighCallback = setDSIResetLineToHighCallback;
  expectWaitPeriodAfterDSIResetLineIsSetToHigh(WAIT_BEFORE_ISSUING_FIRST_DCS_COMMAND_PERIOD_MS);

  const RaydiumRM67160::ErrorCode errorCode = virtualRaydiumRM67160.init(raydiumRM67160Config);

  ASSERT_THAT(errorCode, Eq(RaydiumRM67160::ErrorCode::OK));
}

TEST_F(ARaydiumRM67160, InitIssuesASeriesOfManufactureCommandSetPage0Commands)
{
  constexpr uint8_t MANUF_COMMAND_SET_PAGE0 = 0x1;
  const uint8_t dcsCommandsManufCmdSetPage0[][2] =
  { { 0x06, 0x62 }, { 0x0E, 0x80 }, { 0x0F, 0x80 }, { 0x10, 0x71 }, { 0x13, 0x81 }, { 0x14, 0x81 },
	  { 0x15, 0x82 }, { 0x16, 0x82 }, { 0x18, 0x88 }, { 0x19, 0x55 }, { 0x1A, 0x10 }, { 0x1C, 0x99 },
	  { 0x1D, 0x03 }, { 0x1E, 0x03 }, { 0x1F, 0x03 }, { 0x20, 0x03 }, { 0x25, 0x03 }, { 0x26, 0x8D },
	  { 0x2A, 0x03 }, { 0x2B, 0x8D }, { 0x36, 0x00 }, { 0x37, 0x10 }, { 0x3A, 0x00 }, { 0x3B, 0x00 },
  	{ 0x3D, 0x20 }, { 0x3F, 0x3A }, { 0x40, 0x30 }, { 0x41, 0x1A }, { 0x42, 0x33 }, { 0x43, 0x22 },
	  { 0x44, 0x11 }, { 0x45, 0x66 }, { 0x46, 0x55 }, { 0x47, 0x44 }, { 0x4C, 0x33 }, { 0x4D, 0x22 },
	  { 0x4E, 0x11 }, { 0x4F, 0x66 }, { 0x50, 0x55 }, { 0x51, 0x44 }, { 0x57, 0x33 }, { 0x6B, 0x1B },
  	{ 0x70, 0x55 }, { 0x74, 0x0C }
  };
  expectDCSShortWrites(MANUF_COMMAND_SET_PAGE0, dcsCommandsManufCmdSetPage0);

  const RaydiumRM67160::ErrorCode errorCode = virtualRaydiumRM67160.init(raydiumRM67160Config);

  ASSERT_THAT(errorCode, Eq(RaydiumRM67160::ErrorCode::OK));
  assertThatAllDCSShortWritesAreIssued(dcsCommandsManufCmdSetPage0);
}

TEST_F(ARaydiumRM67160, InitIssuesASeriesOfManufactureCommandSetPage1Commands)
{
  constexpr uint8_t MANUF_COMMAND_SET_PAGE1 = 0x2;
  const uint8_t dcsCommandsManufCmdSetPage1[][2] = {{ 0x9B, 0x40 }, { 0x9C, 0x00 }, { 0x9D, 0x20 }};
  expectDCSShortWrites(MANUF_COMMAND_SET_PAGE1, dcsCommandsManufCmdSetPage1);

  const RaydiumRM67160::ErrorCode errorCode = virtualRaydiumRM67160.init(raydiumRM67160Config);

  ASSERT_THAT(errorCode, Eq(RaydiumRM67160::ErrorCode::OK));
  assertThatAllDCSShortWritesAreIssued(dcsCommandsManufCmdSetPage1);
}

TEST_F(ARaydiumRM67160, InitIssuesASeriesOfManufactureCommandSetPage2Commands)
{
  constexpr uint8_t MANUF_COMMAND_SET_PAGE2 = 0x3;
  const uint8_t dcsCommandsManufCmdSetPage2[][2] = {{ 0x9B, 0x40 }, { 0x9C, 0x00 }, { 0x9D, 0x20 }};
  expectDCSShortWrites(MANUF_COMMAND_SET_PAGE2, dcsCommandsManufCmdSetPage2);

  const RaydiumRM67160::ErrorCode errorCode = virtualRaydiumRM67160.init(raydiumRM67160Config);

  ASSERT_THAT(errorCode, Eq(RaydiumRM67160::ErrorCode::OK));
  assertThatAllDCSShortWritesAreIssued(dcsCommandsManufCmdSetPage2);
}

TEST_F(ARaydiumRM67160, InitIssuesASeriesOfManufactureCommandSetPage3Commands)
{
  constexpr uint8_t MANUF_COMMAND_SET_PAGE3 = 0x4;
  const uint8_t dcsCommandsManufCmdSetPage3[][2] =
  { { 0x5D, 0x10 }, { 0x00, 0x8D }, { 0x01, 0x00 }, { 0x02, 0x01 }, { 0x03, 0x01 }, { 0x04, 0x10 },
	  { 0x05, 0x01 }, { 0x06, 0xA7 }, { 0x07, 0x20 }, { 0x08, 0x00 }, { 0x09, 0xC2 }, { 0x0A, 0x00 },
	  { 0x0B, 0x02 }, { 0x0C, 0x01 }, { 0x0D, 0x40 }, { 0x0E, 0x06 }, { 0x0F, 0x01 }, { 0x10, 0xA7 },
	  { 0x11, 0x00 }, { 0x12, 0xC2 }, { 0x13, 0x00 }, { 0x14, 0x02 }, { 0x15, 0x01 }, { 0x16, 0x40 },
	  { 0x17, 0x07 }, { 0x18, 0x01 }, { 0x19, 0xA7 }, { 0x1A, 0x00 }, { 0x1B, 0x82 }, { 0x1C, 0x00 },
	  { 0x1D, 0xFF }, { 0x1E, 0x05 }, { 0x1F, 0x60 }, { 0x20, 0x02 }, { 0x21, 0x01 }, { 0x22, 0x7C },
	  { 0x23, 0x00 }, { 0x24, 0xC2 }, { 0x25, 0x00 }, { 0x26, 0x04 }, { 0x27, 0x02 }, { 0x28, 0x70 },
	  { 0x29, 0x05 }, { 0x2A, 0x74 }, { 0x2B, 0x8D }, { 0x2D, 0x00 }, { 0x2F, 0xC2 }, { 0x30, 0x00 },
	  { 0x31, 0x04 }, { 0x32, 0x02 }, { 0x33, 0x70 }, { 0x34, 0x07 }, { 0x35, 0x74 }, { 0x36, 0x8D },
	  { 0x37, 0x00 }, { 0x5E, 0x20 }, { 0x5F, 0x31 }, { 0x60, 0x54 }, { 0x61, 0x76 }, { 0x62, 0x98 }
  };
  expectDCSShortWrites(MANUF_COMMAND_SET_PAGE3, dcsCommandsManufCmdSetPage3);

  const RaydiumRM67160::ErrorCode errorCode = virtualRaydiumRM67160.init(raydiumRM67160Config);

  ASSERT_THAT(errorCode, Eq(RaydiumRM67160::ErrorCode::OK));
  assertThatAllDCSShortWritesAreIssued(dcsCommandsManufCmdSetPage3);
}

TEST_F(ARaydiumRM67160, InitIssuesASeriesOfManufactureCommandSetPage4Commands)
{
  constexpr uint8_t MANUF_COMMAND_SET_PAGE4 = 0x5;
  const uint8_t dcsCommandsManufCmdSetPage4[][2] = {{ 0x05, 0x17 }, { 0x2A, 0x04 }, { 0x91, 0x00 }};
  expectDCSShortWrites(MANUF_COMMAND_SET_PAGE4, dcsCommandsManufCmdSetPage4);

  const RaydiumRM67160::ErrorCode errorCode = virtualRaydiumRM67160.init(raydiumRM67160Config);

  ASSERT_THAT(errorCode, Eq(RaydiumRM67160::ErrorCode::OK));
  assertThatAllDCSShortWritesAreIssued(dcsCommandsManufCmdSetPage4);
}

TEST_F(ARaydiumRM67160, InitSetsDSIInterfaceColorFormatToRGB888)
{
  constexpr uint8_t USER_COMMAND_SET                = 0x0;
  constexpr uint8_t SET_INTERFACE_COLOR_FORMAT_CMD  = 0x3A;
  constexpr uint8_t EXPECTED_INTERFACE_COLOR_FORMAT = 0x7;
  expectDCSShortWrite(USER_COMMAND_SET, SET_INTERFACE_COLOR_FORMAT_CMD, EXPECTED_INTERFACE_COLOR_FORMAT);

  const RaydiumRM67160::ErrorCode errorCode = virtualRaydiumRM67160.init(raydiumRM67160Config);

  ASSERT_THAT(errorCode, Eq(RaydiumRM67160::ErrorCode::OK));
  assertThatExpectedDCSShortWriteIsIssued();
}

TEST_F(ARaydiumRM67160, InitDisablesTearingEffectLine)
{
  constexpr uint8_t USER_COMMAND_SET                = 0x0;
  constexpr uint8_t DISABLE_TEARING_EFFECT_LINE_CMD = 0x34;
  expectDCSShortWrite(USER_COMMAND_SET, DISABLE_TEARING_EFFECT_LINE_CMD);

  const RaydiumRM67160::ErrorCode errorCode = virtualRaydiumRM67160.init(raydiumRM67160Config);

  ASSERT_THAT(errorCode, Eq(RaydiumRM67160::ErrorCode::OK));
  assertThatExpectedDCSShortWriteIsIssued();
}

TEST_F(ARaydiumRM67160, InitSetsDSIModeToInternalTiming)
{
  constexpr uint8_t USER_COMMAND_SET  = 0x0;
  constexpr uint8_t SET_DSI_MODE_CMD  = 0xC2;
  constexpr uint8_t EXPECTED_DSI_MODE = 0x0;
  expectDCSShortWrite(USER_COMMAND_SET, SET_DSI_MODE_CMD, EXPECTED_DSI_MODE);

  const RaydiumRM67160::ErrorCode errorCode = virtualRaydiumRM67160.init(raydiumRM67160Config);

  ASSERT_THAT(errorCode, Eq(RaydiumRM67160::ErrorCode::OK));
  assertThatExpectedDCSShortWriteIsIssued();
}

TEST_F(ARaydiumRM67160, InitSetsStartAndEndColumnAddressAccordingToStartAndEndColumnAddressConfigParams)
{
  constexpr uint32_t BITS_IN_BYTE         = 8u;
  constexpr uint16_t COLUMN_START_ADDRESS = 4u;
  constexpr uint16_t COLUMN_END_ADDRESS   = 393u;
  constexpr uint8_t USER_COMMAND_SET      = 0x0u;
  constexpr uint8_t SET_COLUMN_START_END_ADDRESS_CMD = 0x2A;
  const uint8_t EXPECTED_COLUMN_START_END_ADDRESS[4] =
  {
    [0] = static_cast<uint8_t>(MemoryUtility<uint16_t>::getBits(COLUMN_START_ADDRESS, 8u, BITS_IN_BYTE)),
    [1] = static_cast<uint8_t>(MemoryUtility<uint16_t>::getBits(COLUMN_START_ADDRESS, 0u, BITS_IN_BYTE)),
    [2] = static_cast<uint8_t>(MemoryUtility<uint16_t>::getBits(COLUMN_END_ADDRESS, 8u, BITS_IN_BYTE)),
    [3] = static_cast<uint8_t>(MemoryUtility<uint16_t>::getBits(COLUMN_END_ADDRESS, 0u, BITS_IN_BYTE))
  };
  raydiumRM67160Config.startColumnAddress = COLUMN_START_ADDRESS;
  raydiumRM67160Config.endColumnAddress   = COLUMN_END_ADDRESS;
  expectDCSLongWrite(USER_COMMAND_SET,
    SET_COLUMN_START_END_ADDRESS_CMD,
    EXPECTED_COLUMN_START_END_ADDRESS,
    4u);

  const RaydiumRM67160::ErrorCode errorCode = virtualRaydiumRM67160.init(raydiumRM67160Config);

  ASSERT_THAT(errorCode, Eq(RaydiumRM67160::ErrorCode::OK));
  assertThatExpectedDCSLongWriteIsIssued();
}

TEST_F(ARaydiumRM67160, InitSetsStartAndEndRowAddressAccordingToStartAndEndRowAddressConfigParams)
{
  constexpr uint32_t BITS_IN_BYTE      = 8u;
  constexpr uint16_t ROW_START_ADDRESS = 0u;
  constexpr uint16_t ROW_END_ADDRESS   = 389u;
  constexpr uint8_t USER_COMMAND_SET   = 0x0u;
  constexpr uint8_t SET_ROW_START_END_ADDRESS_CMD = 0x2B;
  const uint8_t EXPECTED_ROW_START_END_ADDRESS[4] =
  {
    [0] = static_cast<uint8_t>(MemoryUtility<uint16_t>::getBits(ROW_START_ADDRESS, 8u, BITS_IN_BYTE)),
    [1] = static_cast<uint8_t>(MemoryUtility<uint16_t>::getBits(ROW_START_ADDRESS, 0u, BITS_IN_BYTE)),
    [2] = static_cast<uint8_t>(MemoryUtility<uint16_t>::getBits(ROW_END_ADDRESS, 8u, BITS_IN_BYTE)),
    [3] = static_cast<uint8_t>(MemoryUtility<uint16_t>::getBits(ROW_END_ADDRESS, 0u, BITS_IN_BYTE))
  };
  raydiumRM67160Config.startRowAddress = ROW_START_ADDRESS;
  raydiumRM67160Config.endRowAddress   = ROW_END_ADDRESS;
  expectDCSLongWrite(USER_COMMAND_SET,
    SET_ROW_START_END_ADDRESS_CMD,
    EXPECTED_ROW_START_END_ADDRESS,
    4u);

  const RaydiumRM67160::ErrorCode errorCode = virtualRaydiumRM67160.init(raydiumRM67160Config);

  ASSERT_THAT(errorCode, Eq(RaydiumRM67160::ErrorCode::OK));
  assertThatExpectedDCSLongWriteIsIssued();
}

TEST_F(ARaydiumRM67160, InitExitsSleepMode)
{
  constexpr uint8_t USER_COMMAND_SET    = 0x0;
  constexpr uint8_t EXIT_SLEEP_MODE_CMD = 0x11;
  expectDCSShortWrite(USER_COMMAND_SET, EXIT_SLEEP_MODE_CMD);

  const RaydiumRM67160::ErrorCode errorCode = virtualRaydiumRM67160.init(raydiumRM67160Config);

  ASSERT_THAT(errorCode, Eq(RaydiumRM67160::ErrorCode::OK));
  assertThatExpectedDCSShortWriteIsIssued();
}

TEST_F(ARaydiumRM67160, InitSetsDisplayBrightnessAccordingToDefaultBrightnessConfigParam)
{
  constexpr uint8_t DEFAULT_DISPLAY_BRIGHTNESS = 50;
  constexpr uint8_t USER_COMMAND_SET           = 0x0;
  constexpr uint8_t SET_DISPLAY_BRIGHTNESS_CMD = 0x51;
  raydiumRM67160Config.defaultBrightness = DEFAULT_DISPLAY_BRIGHTNESS;
  expectDCSShortWrite(USER_COMMAND_SET, SET_DISPLAY_BRIGHTNESS_CMD, DEFAULT_DISPLAY_BRIGHTNESS);

  const RaydiumRM67160::ErrorCode errorCode = virtualRaydiumRM67160.init(raydiumRM67160Config);

  ASSERT_THAT(errorCode, Eq(RaydiumRM67160::ErrorCode::OK));
  assertThatExpectedDCSShortWriteIsIssued();
}

TEST_F(ARaydiumRM67160, InitTurnsOnDisplaying)
{
  constexpr uint8_t USER_COMMAND_SET     = 0x0;
  constexpr uint8_t START_DISPLAYING_CMD = 0x29;
  expectDCSShortWrite(USER_COMMAND_SET, START_DISPLAYING_CMD);

  const RaydiumRM67160::ErrorCode errorCode = virtualRaydiumRM67160.init(raydiumRM67160Config);

  ASSERT_THAT(errorCode, Eq(RaydiumRM67160::ErrorCode::OK));
  assertThatExpectedDCSShortWriteIsIssued();
}

TEST_F(ARaydiumRM67160, InitWaits120MillisecondsAfterExitingFromSleepModeBeforeIssuingNextDCSCommand)
{
  constexpr uint8_t USER_COMMAND_SET    = 0x0;
  constexpr uint8_t EXIT_SLEEP_MODE_CMD = 0x11;
  constexpr uint64_t AFTER_EXIT_FROM_SLEEP_MODE_WAIT_PERIOD_MS = 120u;
  expectWaitPeriodAfterDCSCommand(
    USER_COMMAND_SET,
    EXIT_SLEEP_MODE_CMD,
    AFTER_EXIT_FROM_SLEEP_MODE_WAIT_PERIOD_MS);

  const RaydiumRM67160::ErrorCode errorCode = virtualRaydiumRM67160.init(raydiumRM67160Config);

  ASSERT_THAT(errorCode, Eq(RaydiumRM67160::ErrorCode::OK));
}

TEST_F(ARaydiumRM67160, SetDisplayBrightnessSetsBrightnessAccordingToBrightnessFunctionArgument)
{
  constexpr uint8_t USER_COMMAND_SET            = 0x0;
  constexpr uint8_t SET_DISPLAY_BRIGHTNESS_CMD  = 0x51;
  constexpr uint8_t EXPECTED_DISPLAY_BRIGHTNESS = 50;
  expectDCSShortWrite(USER_COMMAND_SET, SET_DISPLAY_BRIGHTNESS_CMD, EXPECTED_DISPLAY_BRIGHTNESS);

  const RaydiumRM67160::ErrorCode errorCode = virtualRaydiumRM67160.setDisplayBrightness(EXPECTED_DISPLAY_BRIGHTNESS);

  ASSERT_THAT(errorCode, Eq(RaydiumRM67160::ErrorCode::OK));
  assertThatExpectedDCSShortWriteIsIssued();
}