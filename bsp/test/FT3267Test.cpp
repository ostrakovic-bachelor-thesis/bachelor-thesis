#include "FT3267.h"
#include "SysTickMock.h"
#include "I2CMock.h"
#include "MemoryUtility.h"
#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include <cstdint>
#include <memory>


using namespace ::testing;


class AFT3267 : public Test
{
public:

  static constexpr uint16_t FT3267_PERIPHERAL_ADDRESS = 0x70;

  struct CallbackMock
  {
    virtual ~CallbackMock() = default;

    // mock methods
    MOCK_METHOD(void, setFT3267ResetLineToHighCallback, (), ());
    MOCK_METHOD(void, setFT3267ResetLineToLowCallback, (), ());
  };

  static std::unique_ptr<CallbackMock> s_callbackMockPtr;
  NiceMock<I2CMock> i2cMock;
  NiceMock<SysTickMock> sysTickMock;
  FT3267 virtualFT3267 = FT3267(&i2cMock, &sysTickMock);

  FT3267::FT3267Config ft3267Config =
  {
    .peripheralAddress                = FT3267_PERIPHERAL_ADDRESS,
    .performPowerOn                   = true,
    .setFT3267ResetLineToLowCallback  = dummyCallback,
    .setFT3267ResetLineToHighCallback = dummyCallback
  };

  uint64_t m_sysTickElapsedMs;

  void expectOthersMemoryWrite(void);
  void expectOthersMemoryRead(void);
  void expectOthersMemoryReadAndMemoryWrite(void);

  template<typename GMockMatcher>
  void expectMemoryWriteOnlyOnce(uint8_t registerAddress, GMockMatcher matcher);

  template<typename GMockMatcher>
  void expectMemoryWrite(uint8_t registerAddress, GMockMatcher matcher);

  void expectMemoryReadOnlyOnce(uint8_t registerAddress, uint8_t registerValue);

  void expectMemoryRead(uint8_t registerAddress, uint8_t registerValue);

  void returnOnMemoryRead(uint8_t registerAddress, uint8_t registerValue);

  void expectSetFT3267ResetLineToLowCallbackCall(uint64_t timestamp, uint64_t afterTimestampInMs);
  void expectSetFT3267ResetLineToHighCallbackCall(uint64_t timestamp, uint64_t afterTimestampInMs);
  void expectThatStateOfFT3267ResetLineIsNotChanged(void);

  static void setFT3267ResetLineToLowCallback(void);
  static void setFT3267ResetLineToHighCallback(void);
  static void dummyCallback(void);

  void setupSysTickReadings(void);

  void SetUp() override;
  void TearDown() override;
};

std::unique_ptr<AFT3267::CallbackMock> AFT3267::s_callbackMockPtr;

void AFT3267::SetUp()
{
  s_callbackMockPtr = std::make_unique<CallbackMock>();

  m_sysTickElapsedMs = 0u;

  setupSysTickReadings();
}

void AFT3267::TearDown()
{
  s_callbackMockPtr.reset(nullptr);
}

void AFT3267::setupSysTickReadings(void)
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

void AFT3267::expectOthersMemoryRead(void)
{
  EXPECT_CALL(i2cMock, readMemory(_, Matcher<uint8_t>(_), Matcher<void*>(_), 1u))
    .Times(AnyNumber());
}

void AFT3267::expectOthersMemoryWrite(void)
{
  EXPECT_CALL(i2cMock, writeMemory(_, Matcher<uint8_t>(_), Matcher<const void*>(_), 1u))
    .Times(AnyNumber());
}

void AFT3267::expectOthersMemoryReadAndMemoryWrite(void)
{
  expectOthersMemoryRead();
  expectOthersMemoryWrite();
}

template<typename GMockMatcher>
void AFT3267::expectMemoryWriteOnlyOnce(uint8_t registerAddress, GMockMatcher matcher)
{
  EXPECT_CALL(i2cMock, writeMemory(_, registerAddress, Matcher<const void*>(_), 1u))
    .WillOnce([=](uint16_t slaveAddress, uint8_t memoryAddress, const void *messagePtr, uint32_t messageLen) -> I2C::ErrorCode
    {
      EXPECT_THAT(*reinterpret_cast<const uint8_t*>(messagePtr), matcher);
      return I2C::ErrorCode::OK;
    });

  EXPECT_CALL(i2cMock, writeMemory(_, Not(registerAddress), Matcher<const void*>(_), _))
    .Times(AnyNumber());
}

template<typename GMockMatcher>
void AFT3267::expectMemoryWrite(uint8_t registerAddress, GMockMatcher matcher)
{
  EXPECT_CALL(i2cMock, writeMemory(_, registerAddress, Matcher<const void*>(_), 1u))
    .WillOnce([=](uint16_t slaveAddress, uint8_t memoryAddress, const void *messagePtr, uint32_t messageLen) -> I2C::ErrorCode
    {
      EXPECT_THAT(*reinterpret_cast<const uint8_t*>(messagePtr), matcher);
      return I2C::ErrorCode::OK;
    });
}

void AFT3267::expectMemoryReadOnlyOnce(uint8_t registerAddress, uint8_t registerValue)
{
  EXPECT_CALL(i2cMock, readMemory(_, registerAddress, Matcher<void*>(_), 1u))
    .Times(1u)
    .WillOnce([=](uint16_t slaveAddress, uint8_t memoryAddress, void *messagePtr, uint32_t messageLen) -> I2C::ErrorCode
    {
      *reinterpret_cast<uint8_t*>(messagePtr) = registerValue;
      return I2C::ErrorCode::OK;
    });
}

void AFT3267::expectMemoryRead(uint8_t registerAddress, uint8_t registerValue)
{
  EXPECT_CALL(i2cMock, readMemory(_, registerAddress, Matcher<void*>(_), 1u))
    .Times(1u)
    .WillOnce([=](uint16_t slaveAddress, uint8_t memoryAddress, void *messagePtr, uint32_t messageLen) -> I2C::ErrorCode
    {
      *reinterpret_cast<uint8_t*>(messagePtr) = registerValue;
      return I2C::ErrorCode::OK;
    });
}

void AFT3267::returnOnMemoryRead(uint8_t registerAddress, uint8_t registerValue)
{
  EXPECT_CALL(i2cMock, readMemory(_, registerAddress, Matcher<void*>(_), 1u))
    .Times(AnyNumber())
    .WillRepeatedly([=](uint16_t slaveAddress, uint8_t memoryAddress, void *messagePtr, uint32_t messageLen) -> I2C::ErrorCode
    {
      *reinterpret_cast<uint8_t*>(messagePtr) = registerValue;
      return I2C::ErrorCode::OK;
    });
}

void AFT3267::expectSetFT3267ResetLineToLowCallbackCall(uint64_t timestamp, uint64_t afterTimestampInMs)
{
  EXPECT_CALL(*s_callbackMockPtr, setFT3267ResetLineToLowCallback())
    .Times(1u)
    .WillOnce([&, timestamp, afterTimestampInMs]()
    {
      EXPECT_THAT(sysTickMock.getElapsedTimeInMs(timestamp), AllOf(Ge(afterTimestampInMs), Le(afterTimestampInMs + 1u)));
    });
}

void AFT3267::expectSetFT3267ResetLineToHighCallbackCall(uint64_t timestamp, uint64_t afterTimestampInMs)
{
  EXPECT_CALL(*s_callbackMockPtr, setFT3267ResetLineToHighCallback())
    .Times(1u)
    .WillOnce([&, timestamp, afterTimestampInMs]()
    {
      EXPECT_THAT(sysTickMock.getElapsedTimeInMs(timestamp), AllOf(Ge(afterTimestampInMs), Le(afterTimestampInMs + 2u)));
    });
}

void AFT3267::expectThatStateOfFT3267ResetLineIsNotChanged(void)
{
  EXPECT_CALL(*s_callbackMockPtr, setFT3267ResetLineToLowCallback())
    .Times(0u);

  EXPECT_CALL(*s_callbackMockPtr, setFT3267ResetLineToHighCallback())
    .Times(0u);
}

void AFT3267::setFT3267ResetLineToLowCallback(void)
{
  s_callbackMockPtr->setFT3267ResetLineToLowCallback();
}

void AFT3267::setFT3267ResetLineToHighCallback(void)
{
  s_callbackMockPtr->setFT3267ResetLineToHighCallback();
}

void AFT3267::dummyCallback(void)
{

}


TEST_F(AFT3267, UsesPeripheralAddressGivenAtInitForAnyI2CTransferWithTheComponent)
{
  static constexpr uint16_t RANDOM_FT3267_PERIPHERAL_ADDRESS = 0x5A;
  ft3267Config.peripheralAddress = RANDOM_FT3267_PERIPHERAL_ADDRESS;
  virtualFT3267.init(ft3267Config);
  EXPECT_CALL(i2cMock, readMemory(RANDOM_FT3267_PERIPHERAL_ADDRESS, _, _, _))
    .Times(1u);

  uint8_t id;
  const FT3267::ErrorCode errorCode = virtualFT3267.getID(id);

  ASSERT_THAT(errorCode, Eq(FT3267::ErrorCode::OK));
}

TEST_F(AFT3267, InitDisablesGestureMode)
{
  static constexpr uint8_t FT3267_GESTURE_MODE_ENABLE_REG_ADDR           = 0xD0u;
  static constexpr uint8_t EXPECTED_FT3267_GESTURE_MODE_ENABLE_REG_VALUE = 0x0u;
  expectOthersMemoryReadAndMemoryWrite();
  expectMemoryWriteOnlyOnce(FT3267_GESTURE_MODE_ENABLE_REG_ADDR, EXPECTED_FT3267_GESTURE_MODE_ENABLE_REG_VALUE);

  const FT3267::ErrorCode errorCode = virtualFT3267.init(ft3267Config);

  ASSERT_THAT(errorCode, Eq(FT3267::ErrorCode::OK));
}

TEST_F(AFT3267, InitSetsOperationModeToInterrupt)
{
  static constexpr uint8_t FT3267_OPERATION_MODE_INTERRUPT = 0x1u;
  static constexpr uint8_t FT3267_OPERATION_MODE_REG_ADDR  = 0xA4u;
  static constexpr uint8_t EXPECTED_FT3267_OPERATION_MODE  = FT3267_OPERATION_MODE_INTERRUPT;
  expectOthersMemoryReadAndMemoryWrite();
  expectMemoryWriteOnlyOnce(FT3267_OPERATION_MODE_REG_ADDR, EXPECTED_FT3267_OPERATION_MODE);

  const FT3267::ErrorCode errorCode = virtualFT3267.init(ft3267Config);

  ASSERT_THAT(errorCode, Eq(FT3267::ErrorCode::OK));
}

TEST_F(AFT3267, InitReadsFirmwareVersionToDetermineIfTouchIsAlignedWithDisplayPlacement)
{
  static constexpr uint8_t FT3267_FIRMWARE_ID          = 0x0;
  static constexpr uint8_t FT3267_FIRMWARE_ID_REG_ADDR = 0xA6u;
  expectOthersMemoryReadAndMemoryWrite();
  expectMemoryReadOnlyOnce(FT3267_FIRMWARE_ID_REG_ADDR, FT3267_FIRMWARE_ID);

  const FT3267::ErrorCode errorCode = virtualFT3267.init(ft3267Config);

  ASSERT_THAT(errorCode, Eq(FT3267::ErrorCode::OK));
}

TEST_F(AFT3267, InitAtTheBegginingSetsFT3267ResetLineToLow)
{
  ft3267Config.setFT3267ResetLineToLowCallback = setFT3267ResetLineToLowCallback;
  const uint64_t timestamp = sysTickMock.getTicks();
  expectSetFT3267ResetLineToLowCallbackCall(timestamp, 0u);

  const FT3267::ErrorCode errorCode = virtualFT3267.init(ft3267Config);

  ASSERT_THAT(errorCode, Eq(FT3267::ErrorCode::OK));
}

TEST_F(AFT3267, InitWaits10MillisecondsAfterResetLineIsSetToLowBeforeSettingItAgainToHigh)
{
  constexpr uint64_t SET_FT3267_TO_HIGH_WAIT_PERIOD_MS = 10u;
  ft3267Config.setFT3267ResetLineToHighCallback = setFT3267ResetLineToHighCallback;
  const uint64_t timestamp = sysTickMock.getTicks();
  expectSetFT3267ResetLineToHighCallbackCall(timestamp, SET_FT3267_TO_HIGH_WAIT_PERIOD_MS);

  const FT3267::ErrorCode errorCode = virtualFT3267.init(ft3267Config);

  ASSERT_THAT(errorCode, Eq(FT3267::ErrorCode::OK));
}

TEST_F(AFT3267, InitDoesNotChangeStateOfFT3267ResetLineIfPerformPowerOnConfigParamIsEqualToFalse)
{
  ft3267Config.performPowerOn = false;
  ft3267Config.setFT3267ResetLineToLowCallback  = setFT3267ResetLineToLowCallback;
  ft3267Config.setFT3267ResetLineToHighCallback = setFT3267ResetLineToHighCallback;
  expectThatStateOfFT3267ResetLineIsNotChanged();

  const FT3267::ErrorCode errorCode = virtualFT3267.init(ft3267Config);

  ASSERT_THAT(errorCode, Eq(FT3267::ErrorCode::OK));
}