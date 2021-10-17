#include "MFXSTM32L152.h"
#include "I2C.h"
#include "I2CMock.h"
#include "MemoryUtility.h"
#include "GPIOMock.h"
#include "SysTickMock.h"
#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include <cstdint>


using namespace ::testing;


MATCHER_P2(BitHasValue, bit, value, "")
{
  return ((arg >> bit) & 0x1) == (value & 0x1);
}

MATCHER_P3(BitsHaveValue, startBit, numberOfBit, value, "")
{
  const uint32_t mask = 0xFFFFFFFFu >> (sizeof(uint32_t) * 8u - numberOfBit);
  return ((arg >> startBit) & mask) == (value & mask);
}


class AMFXSTM32L152 : public Test
{
public:

  static constexpr uint16_t MFXSTM32L152_PERIPHERAL_ADDRESS = 0x84;
  static constexpr MFXSTM32L152::GPIOPin RANDOM_GPIO_PIN = MFXSTM32L152::GPIOPin::PIN10;

  NiceMock<I2CMock> i2cMock;
  NiceMock<GPIOMock> mockGPIO;
  NiceMock<SysTickMock> sysTickMock;
  MFXSTM32L152 virtualMFXSTM32L152 = MFXSTM32L152(&i2cMock, &sysTickMock);
  MFXSTM32L152::GPIOPinConfiguration gpioPinConfig;
  MFXSTM32L152::IRQPinConfiguration irqPinConfig;

  MFXSTM32L152::MFXSTM32L152Config mfxstm32l152Config =
  {
    .peripheralAddress    = MFXSTM32L152_PERIPHERAL_ADDRESS,
    .wakeUpPinGPIOPortPtr = &mockGPIO,
    .wakeUpPin            = GPIO::Pin::PIN2
  };

  uint32_t m_isTransactionOngoingCallCounter;
  uint64_t m_sysTickElapsedMs;

  void expectWakeUpPinSetState(GPIO::PinState pinState, uint64_t timestamp, uint64_t afterTimestampInMs);
  void setupSysTickReadings(void);

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

  void SetUp() override;
  void TearDown() override;
};

void AMFXSTM32L152::SetUp()
{
  m_isTransactionOngoingCallCounter = 0u;
  m_sysTickElapsedMs = 0u;

  // is transaction ongoing will return false once in N calls
  EXPECT_CALL(i2cMock, isTransactionOngoing)
    .WillRepeatedly([&]() -> bool {
      constexpr uint32_t EACH_N_CHECK_IF_FALSE = 10u;
      return ((++m_isTransactionOngoingCallCounter) % EACH_N_CHECK_IF_FALSE) != 0u;
    });

  virtualMFXSTM32L152.init(mfxstm32l152Config);

  gpioPinConfig.mode = MFXSTM32L152::GPIOPinMode::INTERRUPT;
}

void AMFXSTM32L152::TearDown()
{

}

void AMFXSTM32L152::setupSysTickReadings(void)
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

void AMFXSTM32L152::expectWakeUpPinSetState(GPIO::PinState pinState, uint64_t timestamp, uint64_t afterTimestampInMs)
{
  EXPECT_CALL(mockGPIO, setPinState(mfxstm32l152Config.wakeUpPin, _))
    .Times(AnyNumber());

  EXPECT_CALL(mockGPIO, setPinState(mfxstm32l152Config.wakeUpPin, pinState))
    .WillRepeatedly([&](GPIO::Pin pin, GPIO::PinState pinState) -> GPIO::ErrorCode
    {
      EXPECT_THAT(sysTickMock.getElapsedTimeInMs(timestamp), AllOf(Ge(afterTimestampInMs), Le(afterTimestampInMs + 1u)));
      return GPIO::ErrorCode::OK;
    });
}

void AMFXSTM32L152::expectOthersMemoryRead(void)
{
  EXPECT_CALL(i2cMock, readMemory(_, Matcher<uint8_t>(_), Matcher<void*>(_), 1u))
    .Times(AnyNumber());
}

void AMFXSTM32L152::expectOthersMemoryWrite(void)
{
  EXPECT_CALL(i2cMock, writeMemory(_, Matcher<uint8_t>(_), Matcher<const void*>(_), 1u))
    .Times(AnyNumber());
}

void AMFXSTM32L152::expectOthersMemoryReadAndMemoryWrite(void)
{
  expectOthersMemoryRead();
  expectOthersMemoryWrite();
}

template<typename GMockMatcher>
void AMFXSTM32L152::expectMemoryWriteOnlyOnce(uint8_t registerAddress, GMockMatcher matcher)
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
void AMFXSTM32L152::expectMemoryWrite(uint8_t registerAddress, GMockMatcher matcher)
{
  EXPECT_CALL(i2cMock, writeMemory(_, registerAddress, Matcher<const void*>(_), 1u))
    .WillOnce([=](uint16_t slaveAddress, uint8_t memoryAddress, const void *messagePtr, uint32_t messageLen) -> I2C::ErrorCode
    {
      EXPECT_THAT(*reinterpret_cast<const uint8_t*>(messagePtr), matcher);
      return I2C::ErrorCode::OK;
    });
}

void AMFXSTM32L152::expectMemoryReadOnlyOnce(uint8_t registerAddress, uint8_t registerValue)
{
  EXPECT_CALL(i2cMock, readMemory(_, registerAddress, Matcher<void*>(_), 1u))
    .Times(1u)
    .WillOnce([=](uint16_t slaveAddress, uint8_t memoryAddress, void *messagePtr, uint32_t messageLen) -> I2C::ErrorCode
    {
      *reinterpret_cast<uint8_t*>(messagePtr) = registerValue;
      return I2C::ErrorCode::OK;
    });
}

void AMFXSTM32L152::expectMemoryRead(uint8_t registerAddress, uint8_t registerValue)
{
  EXPECT_CALL(i2cMock, readMemory(_, registerAddress, Matcher<void*>(_), 1u))
    .Times(1u)
    .WillOnce([=](uint16_t slaveAddress, uint8_t memoryAddress, void *messagePtr, uint32_t messageLen) -> I2C::ErrorCode
    {
      *reinterpret_cast<uint8_t*>(messagePtr) = registerValue;
      return I2C::ErrorCode::OK;
    });
}

void AMFXSTM32L152::returnOnMemoryRead(uint8_t registerAddress, uint8_t registerValue)
{
  EXPECT_CALL(i2cMock, readMemory(_, registerAddress, Matcher<void*>(_), 1u))
    .Times(AnyNumber())
    .WillRepeatedly([=](uint16_t slaveAddress, uint8_t memoryAddress, void *messagePtr, uint32_t messageLen) -> I2C::ErrorCode
    {
      *reinterpret_cast<uint8_t*>(messagePtr) = registerValue;
      return I2C::ErrorCode::OK;
    });
}


TEST_F(AMFXSTM32L152, UsesPeripheralAddressGivenAtInitForAnyI2CTransferWithTheComponent)
{
  static constexpr uint16_t RANDOM_MFXSTM32L152_PERIPHERAL_ADDRESS = 0x34;
  mfxstm32l152Config.peripheralAddress = RANDOM_MFXSTM32L152_PERIPHERAL_ADDRESS;
  virtualMFXSTM32L152.init(mfxstm32l152Config);
  EXPECT_CALL(i2cMock, readMemory(RANDOM_MFXSTM32L152_PERIPHERAL_ADDRESS, _, _, _))
    .Times(1u);

  uint8_t id;
  const MFXSTM32L152::ErrorCode errorCode = virtualMFXSTM32L152.getID(id);

  ASSERT_THAT(errorCode, Eq(MFXSTM32L152::ErrorCode::OK));
}

TEST_F(AMFXSTM32L152, GetIDReadsIdFromMFXSTM32L152IdRegister)
{
  static constexpr uint8_t MFXSTM32L152_ID_REG_ADDR = 0x00u;
  static constexpr uint8_t MFXSTM32L152_ID_REG_INIT_VALUE = 0x7B;
  uint8_t id = 0x0;
  expectMemoryReadOnlyOnce(MFXSTM32L152_ID_REG_ADDR, MFXSTM32L152_ID_REG_INIT_VALUE);

  const MFXSTM32L152::ErrorCode errorCode = virtualMFXSTM32L152.getID(id);

  ASSERT_THAT(errorCode, Eq(MFXSTM32L152::ErrorCode::OK));
  ASSERT_THAT(id, Eq(MFXSTM32L152_ID_REG_INIT_VALUE));
}

TEST_F(AMFXSTM32L152, EnableGPIOEnablesBothGPIOandAGPIOInSystemControlRegister)
{
  static constexpr uint8_t MFXSTM32L152_SYS_CTRL_REG_ADDR = 0x40u;
  static constexpr uint8_t MFXSTM32L152_SYS_CTRL_INIT_VALUE = 0x0u;
  constexpr uint32_t MFXSTM32L152_SYS_CTRL_GPIO_EN_POSITION = 0u;
  constexpr uint32_t MFXSTM32L152_SYS_CTRL_AGPIO_EN_POSITION = 3u;
  constexpr uint32_t MFXSTM32L152_SYS_CTRL_GPIO_EN_VALUE = 1u;
  constexpr uint32_t MFXSTM32L152_SYS_CTRL_AGPIO_EN_VALUE = 1u;
  auto bitValueMatcher =
    AllOf(BitHasValue(MFXSTM32L152_SYS_CTRL_GPIO_EN_POSITION, MFXSTM32L152_SYS_CTRL_GPIO_EN_VALUE),
          BitHasValue(MFXSTM32L152_SYS_CTRL_AGPIO_EN_POSITION, MFXSTM32L152_SYS_CTRL_AGPIO_EN_VALUE));
  expectOthersMemoryReadAndMemoryWrite();
  expectMemoryReadOnlyOnce(MFXSTM32L152_SYS_CTRL_REG_ADDR, MFXSTM32L152_SYS_CTRL_INIT_VALUE);
  expectMemoryWriteOnlyOnce(MFXSTM32L152_SYS_CTRL_REG_ADDR, bitValueMatcher);

  const MFXSTM32L152::ErrorCode errorCode = virtualMFXSTM32L152.enableGPIO();

  ASSERT_THAT(errorCode, Eq(MFXSTM32L152::ErrorCode::OK));
}

TEST_F(AMFXSTM32L152, ConfigurePinSetsPinDirectionInGPIODirectionRegisterAccordingToChoosenMode)
{
  static constexpr uint8_t MFXSTM32L152_GPIO_DIR_REG_BASE_ADDR = 0x60u;
  static constexpr uint8_t MFXSTM32L152_GPIO_DIR_REG_ADDR =
    MFXSTM32L152_GPIO_DIR_REG_BASE_ADDR + (static_cast<uint8_t>(RANDOM_GPIO_PIN) / 8u);
  static constexpr uint8_t MFXSTM32L152_GPIO_DIR_INIT_VALUE = 0x0u;
  gpioPinConfig.mode = MFXSTM32L152::GPIOPinMode::OUTPUT;
  auto bitValueMatcher =
    BitHasValue((static_cast<uint32_t>(RANDOM_GPIO_PIN) % 8u), static_cast<uint32_t>(MFXSTM32L152::GPIOPinMode::OUTPUT));
  expectOthersMemoryReadAndMemoryWrite();
  expectMemoryReadOnlyOnce(MFXSTM32L152_GPIO_DIR_REG_ADDR, MFXSTM32L152_GPIO_DIR_INIT_VALUE);
  expectMemoryWriteOnlyOnce(MFXSTM32L152_GPIO_DIR_REG_ADDR, bitValueMatcher);

  const MFXSTM32L152::ErrorCode errorCode = virtualMFXSTM32L152.configureGPIOPin(RANDOM_GPIO_PIN, gpioPinConfig);

  ASSERT_THAT(errorCode, Eq(MFXSTM32L152::ErrorCode::OK));
}

TEST_F(AMFXSTM32L152, ConfigurePinSetsPinTypeInGPIOTypeRegisterAccordingToOutputTypeWhenPinIsInOutputMode)
{
  static constexpr uint8_t MFXSTM32L152_GPIO_TYPE_REG_BASE_ADDR = 0x64u;
  static constexpr uint8_t MFXSTM32L152_GPIO_TYPE_REG_ADDR =
    MFXSTM32L152_GPIO_TYPE_REG_BASE_ADDR + (static_cast<uint8_t>(RANDOM_GPIO_PIN) / 8u);
  static constexpr uint8_t MFXSTM32L152_GPIO_TYPE_INIT_VALUE = 0x0u;
  gpioPinConfig.mode = MFXSTM32L152::GPIOPinMode::OUTPUT;
  gpioPinConfig.outputType = MFXSTM32L152::GPIOOutputType::OPEN_DRAIN;
  auto bitValueMatcher =
    BitHasValue((static_cast<uint32_t>(RANDOM_GPIO_PIN) % 8u), static_cast<uint32_t>(MFXSTM32L152::GPIOOutputType::OPEN_DRAIN));
  expectOthersMemoryReadAndMemoryWrite();
  expectMemoryReadOnlyOnce(MFXSTM32L152_GPIO_TYPE_REG_ADDR, MFXSTM32L152_GPIO_TYPE_INIT_VALUE);
  expectMemoryWriteOnlyOnce(MFXSTM32L152_GPIO_TYPE_REG_ADDR, bitValueMatcher);

  const MFXSTM32L152::ErrorCode errorCode = virtualMFXSTM32L152.configureGPIOPin(RANDOM_GPIO_PIN, gpioPinConfig);

  ASSERT_THAT(errorCode, Eq(MFXSTM32L152::ErrorCode::OK));
}

TEST_F(AMFXSTM32L152, ConfigurePinSetsChoosenPinPullConfigInGPIOPUPDRegister)
{
  static constexpr uint8_t MFXSTM32L152_GPIO_PUPD_REG_BASE_ADDR = 0x68u;
  static constexpr uint8_t MFXSTM32L152_GPIO_PUPD_REG_ADDR =
    MFXSTM32L152_GPIO_PUPD_REG_BASE_ADDR + (static_cast<uint8_t>(RANDOM_GPIO_PIN) / 8u);
  static constexpr uint8_t MFXSTM32L152_GPIO_PUPD_INIT_VALUE = 0x0u;
  gpioPinConfig.mode = MFXSTM32L152::GPIOPinMode::INPUT;
  gpioPinConfig.pullConfig = MFXSTM32L152::GPIOPullConfig::PULL_UP;
  auto bitValueMatcher =
    BitHasValue((static_cast<uint32_t>(RANDOM_GPIO_PIN) % 8u), static_cast<uint32_t>(MFXSTM32L152::GPIOPullConfig::PULL_UP));
  expectOthersMemoryReadAndMemoryWrite();
  expectMemoryReadOnlyOnce(MFXSTM32L152_GPIO_PUPD_REG_ADDR, MFXSTM32L152_GPIO_PUPD_INIT_VALUE);
  expectMemoryWriteOnlyOnce(MFXSTM32L152_GPIO_PUPD_REG_ADDR, bitValueMatcher);

  const MFXSTM32L152::ErrorCode errorCode = virtualMFXSTM32L152.configureGPIOPin(RANDOM_GPIO_PIN, gpioPinConfig);

  ASSERT_THAT(errorCode, Eq(MFXSTM32L152::ErrorCode::OK));
}

TEST_F(AMFXSTM32L152, ConfigurePinSetsPinTypeInGPIOTypeToZeroIfPullConfigIsNoPullWhenPinIsInInputMode)
{
  static constexpr uint8_t MFXSTM32L152_GPIO_TYPE_REG_BASE_ADDR = 0x64u;
  static constexpr uint8_t MFXSTM32L152_GPIO_TYPE_REG_ADDR =
    MFXSTM32L152_GPIO_TYPE_REG_BASE_ADDR + (static_cast<uint8_t>(RANDOM_GPIO_PIN) / 8u);
  static constexpr uint8_t MFXSTM32L152_GPIO_TYPE_INIT_VALUE = 0x0u;
  gpioPinConfig.mode = MFXSTM32L152::GPIOPinMode::INPUT;
  gpioPinConfig.pullConfig = MFXSTM32L152::GPIOPullConfig::NO_PULL;
  auto bitValueMatcher =
    BitHasValue((static_cast<uint32_t>(RANDOM_GPIO_PIN) % 8u), 0u);
  expectOthersMemoryReadAndMemoryWrite();
  expectMemoryReadOnlyOnce(MFXSTM32L152_GPIO_TYPE_REG_ADDR, MFXSTM32L152_GPIO_TYPE_INIT_VALUE);
  expectMemoryWriteOnlyOnce(MFXSTM32L152_GPIO_TYPE_REG_ADDR, bitValueMatcher);

  const MFXSTM32L152::ErrorCode errorCode = virtualMFXSTM32L152.configureGPIOPin(RANDOM_GPIO_PIN, gpioPinConfig);

  ASSERT_THAT(errorCode, Eq(MFXSTM32L152::ErrorCode::OK));
}

TEST_F(AMFXSTM32L152, ConfigurePinDisablesPinInterruptIfPinModeIsNotInterrupt)
{
  static constexpr uint8_t MFXSTM32L152_GPIO_IRQ_EN_REG_BASE_ADDR = 0x48u;
  static constexpr uint8_t MFXSTM32L152_GPIO_IRQ_EN_REG_ADDR =
    MFXSTM32L152_GPIO_IRQ_EN_REG_BASE_ADDR + (static_cast<uint8_t>(RANDOM_GPIO_PIN) / 8u);
  static constexpr uint8_t MFXSTM32L152_GPIO_IRQ_EN_INIT_VALUE = 0xFFu;
  gpioPinConfig.mode = MFXSTM32L152::GPIOPinMode::INPUT;
  auto bitValueMatcher =
    BitHasValue((static_cast<uint32_t>(RANDOM_GPIO_PIN) % 8u), 0u);
  expectOthersMemoryReadAndMemoryWrite();
  expectMemoryReadOnlyOnce(MFXSTM32L152_GPIO_IRQ_EN_REG_ADDR, MFXSTM32L152_GPIO_IRQ_EN_INIT_VALUE);
  expectMemoryWriteOnlyOnce(MFXSTM32L152_GPIO_IRQ_EN_REG_ADDR, bitValueMatcher);

  const MFXSTM32L152::ErrorCode errorCode = virtualMFXSTM32L152.configureGPIOPin(RANDOM_GPIO_PIN, gpioPinConfig);

  ASSERT_THAT(errorCode, Eq(MFXSTM32L152::ErrorCode::OK));
}

TEST_F(AMFXSTM32L152, ConfigurePinSetsInterruptEventTypeIfChosenPinModeIsInterruptAccordingToInterruptTrigger)
{
  static constexpr uint8_t MFXSTM32L152_GPIO_IRQ_EVT_REG_BASE_ADDR = 0x4Cu;
  static constexpr uint8_t MFXSTM32L152_GPIO_IRQ_EVT_REG_ADDR =
    MFXSTM32L152_GPIO_IRQ_EVT_REG_BASE_ADDR + (static_cast<uint8_t>(RANDOM_GPIO_PIN) / 8u);
  static constexpr uint8_t MFXSTM32L152_GPIO_IRQ_EVT_INIT_VALUE = 0x00u;
  gpioPinConfig.mode = MFXSTM32L152::GPIOPinMode::INTERRUPT;
  gpioPinConfig.interruptTrigger = MFXSTM32L152::GPIOInterruptTrigger::RISING_EDGE;
  auto bitValueMatcher =
    BitHasValue((static_cast<uint32_t>(RANDOM_GPIO_PIN) % 8u), 1u);
  expectOthersMemoryReadAndMemoryWrite();
  expectMemoryReadOnlyOnce(MFXSTM32L152_GPIO_IRQ_EVT_REG_ADDR, MFXSTM32L152_GPIO_IRQ_EVT_INIT_VALUE);
  expectMemoryWriteOnlyOnce(MFXSTM32L152_GPIO_IRQ_EVT_REG_ADDR, bitValueMatcher);

  const MFXSTM32L152::ErrorCode errorCode = virtualMFXSTM32L152.configureGPIOPin(RANDOM_GPIO_PIN, gpioPinConfig);

  ASSERT_THAT(errorCode, Eq(MFXSTM32L152::ErrorCode::OK));
}

TEST_F(AMFXSTM32L152, ConfigurePinSetsInterruptActivityTypeIfChosenPinModeIsInterruptAccordingToInterruptTrigger)
{
  static constexpr uint8_t MFXSTM32L152_GPIO_IRQ_ATYPE_REG_BASE_ADDR = 0x50u;
  static constexpr uint8_t MFXSTM32L152_GPIO_IRQ_ATYPE_REG_ADDR =
    MFXSTM32L152_GPIO_IRQ_ATYPE_REG_BASE_ADDR + (static_cast<uint8_t>(RANDOM_GPIO_PIN) / 8u);
  static constexpr uint8_t MFXSTM32L152_GPIO_IRQ_ATYPE_INIT_VALUE = 0x00u;
  gpioPinConfig.mode = MFXSTM32L152::GPIOPinMode::INTERRUPT;
  gpioPinConfig.interruptTrigger = MFXSTM32L152::GPIOInterruptTrigger::HIGH_LEVEL;
  auto bitValueMatcher =
    BitHasValue((static_cast<uint32_t>(RANDOM_GPIO_PIN) % 8u), 1u);
  expectOthersMemoryReadAndMemoryWrite();
  expectMemoryReadOnlyOnce(MFXSTM32L152_GPIO_IRQ_ATYPE_REG_ADDR, MFXSTM32L152_GPIO_IRQ_ATYPE_INIT_VALUE);
  expectMemoryWriteOnlyOnce(MFXSTM32L152_GPIO_IRQ_ATYPE_REG_ADDR, bitValueMatcher);

  const MFXSTM32L152::ErrorCode errorCode = virtualMFXSTM32L152.configureGPIOPin(RANDOM_GPIO_PIN, gpioPinConfig);

  ASSERT_THAT(errorCode, Eq(MFXSTM32L152::ErrorCode::OK));
}

TEST_F(AMFXSTM32L152, ConfigurePinEnablesPinInterruptIfPinModeIsInterrupt)
{
  static constexpr uint8_t MFXSTM32L152_GPIO_IRQ_EN_REG_BASE_ADDR = 0x48u;
  static constexpr uint8_t MFXSTM32L152_GPIO_IRQ_EN_REG_ADDR =
    MFXSTM32L152_GPIO_IRQ_EN_REG_BASE_ADDR + (static_cast<uint8_t>(RANDOM_GPIO_PIN) / 8u);
  static constexpr uint8_t MFXSTM32L152_GPIO_IRQ_EN_INIT_VALUE = 0x00u;
  gpioPinConfig.mode = MFXSTM32L152::GPIOPinMode::INTERRUPT;
  auto bitValueMatcher =
    BitHasValue((static_cast<uint32_t>(RANDOM_GPIO_PIN) % 8u), 1u);
  expectOthersMemoryReadAndMemoryWrite();
  expectMemoryReadOnlyOnce(MFXSTM32L152_GPIO_IRQ_EN_REG_ADDR, MFXSTM32L152_GPIO_IRQ_EN_INIT_VALUE);
  expectMemoryWriteOnlyOnce(MFXSTM32L152_GPIO_IRQ_EN_REG_ADDR, bitValueMatcher);

  const MFXSTM32L152::ErrorCode errorCode = virtualMFXSTM32L152.configureGPIOPin(RANDOM_GPIO_PIN, gpioPinConfig);

  ASSERT_THAT(errorCode, Eq(MFXSTM32L152::ErrorCode::OK));
}

TEST_F(AMFXSTM32L152, ConfigureIRQPinSetsIRQPinOutputTypeAccordingToChosenOutputType)
{
  constexpr uint8_t MFXSTM32L152_MFX_IRQ_OUT_REG_ADDR = 0x41u;
  constexpr uint32_t MFXSTM32L152_MFX_IRQ_OUT_OTYPE_POSITION = 0u;
  constexpr uint32_t MFXSTM32L152_MFX_IRQ_OUT_OTYPE_VALUE = 1u;
  irqPinConfig.outputType = MFXSTM32L152::GPIOOutputType::PUSH_PULL;
  auto bitValueMatcher =
    BitHasValue(MFXSTM32L152_MFX_IRQ_OUT_OTYPE_POSITION, MFXSTM32L152_MFX_IRQ_OUT_OTYPE_VALUE);
  expectMemoryWriteOnlyOnce(MFXSTM32L152_MFX_IRQ_OUT_REG_ADDR, bitValueMatcher);

  const MFXSTM32L152::ErrorCode errorCode = virtualMFXSTM32L152.configureIRQPin(irqPinConfig);

  ASSERT_THAT(errorCode, Eq(MFXSTM32L152::ErrorCode::OK));
}

TEST_F(AMFXSTM32L152, ConfigureIRQPinSetsIRQPinPolarityAccordingToChosenPolarity)
{
  constexpr uint8_t MFXSTM32L152_MFX_IRQ_OUT_REG_ADDR = 0x41u;
  constexpr uint32_t MFXSTM32L152_MFX_IRQ_OUT_POLARITY_POSITION = 1u;
  constexpr uint32_t MFXSTM32L152_MFX_IRQ_OUT_POLARITY_VALUE = 1u;
  irqPinConfig.polarity = MFXSTM32L152::IRQPinPolarity::HIGH;
  auto bitValueMatcher =
    BitHasValue(MFXSTM32L152_MFX_IRQ_OUT_POLARITY_POSITION, MFXSTM32L152_MFX_IRQ_OUT_POLARITY_VALUE);
  expectMemoryWriteOnlyOnce(MFXSTM32L152_MFX_IRQ_OUT_REG_ADDR, bitValueMatcher);

  const MFXSTM32L152::ErrorCode errorCode = virtualMFXSTM32L152.configureIRQPin(irqPinConfig);

  ASSERT_THAT(errorCode, Eq(MFXSTM32L152::ErrorCode::OK));
}

TEST_F(AMFXSTM32L152, EnableInterruptEnablesGivenInterruptAsMFXSTM32L152IRQSource)
{
  static constexpr uint8_t MFXSTM32L152_IRQ_SRC_EN_REG_ADDR = 0x42u;
  static constexpr uint8_t MFXSTM32L152_IRQ_SRC_EN_INIT_VALUE = 0x0u;
  auto bitValueMatcher =
    BitHasValue(static_cast<uint8_t>(MFXSTM32L152::Interrupt::GPIO), 1u);
  expectMemoryReadOnlyOnce(MFXSTM32L152_IRQ_SRC_EN_REG_ADDR, MFXSTM32L152_IRQ_SRC_EN_INIT_VALUE);
  expectMemoryWriteOnlyOnce(MFXSTM32L152_IRQ_SRC_EN_REG_ADDR, bitValueMatcher);

  const MFXSTM32L152::ErrorCode errorCode = virtualMFXSTM32L152.enableInterrupt(MFXSTM32L152::Interrupt::GPIO);

  ASSERT_THAT(errorCode, Eq(MFXSTM32L152::ErrorCode::OK));
}

TEST_F(AMFXSTM32L152, SetGPIOPinStateSetsPinStateToHighBySettingCorrespondingBitInGPIOSetHighRegister)
{
  static constexpr uint8_t MFXSTM32L152_GPIO_SET_HIGH_BASE_REG_ADDR = 0x6Cu;
  static constexpr uint8_t MFXSTM32L152_GPIO_SET_HIGH_REG_ADDR =
    MFXSTM32L152_GPIO_SET_HIGH_BASE_REG_ADDR + (static_cast<uint8_t>(RANDOM_GPIO_PIN) / 8u);
  static constexpr uint8_t MFXSTM32L152_GPIO_SET_HIGH_INIT_VALUE = 0x0u;
  auto bitValueMatcher =
    BitHasValue((static_cast<uint32_t>(RANDOM_GPIO_PIN) % 8u), 1u);
  expectMemoryReadOnlyOnce(MFXSTM32L152_GPIO_SET_HIGH_REG_ADDR, MFXSTM32L152_GPIO_SET_HIGH_INIT_VALUE);
  expectMemoryWriteOnlyOnce(MFXSTM32L152_GPIO_SET_HIGH_REG_ADDR, bitValueMatcher);

  const MFXSTM32L152::ErrorCode errorCode =
    virtualMFXSTM32L152.setGPIOPinState(RANDOM_GPIO_PIN, MFXSTM32L152::GPIOPinState::HIGH);

  ASSERT_THAT(errorCode, Eq(MFXSTM32L152::ErrorCode::OK));
}

TEST_F(AMFXSTM32L152, SetGPIOPinStateSetsPinStateToLowBySettingCorrespondingBitInGPIOSetLowRegister)
{
  static constexpr uint8_t MFXSTM32L152_GPIO_SET_LOW_BASE_REG_ADDR = 0x70u;
  static constexpr uint8_t MFXSTM32L152_GPIO_SET_LOW_REG_ADDR =
    MFXSTM32L152_GPIO_SET_LOW_BASE_REG_ADDR + (static_cast<uint8_t>(RANDOM_GPIO_PIN) / 8u);
  static constexpr uint8_t MFXSTM32L152_GPIO_SET_LOW_INIT_VALUE = 0x0u;
  auto bitValueMatcher =
    BitHasValue((static_cast<uint32_t>(RANDOM_GPIO_PIN) % 8u), 1u);
  expectMemoryReadOnlyOnce(MFXSTM32L152_GPIO_SET_LOW_REG_ADDR, MFXSTM32L152_GPIO_SET_LOW_INIT_VALUE);
  expectMemoryWriteOnlyOnce(MFXSTM32L152_GPIO_SET_LOW_REG_ADDR, bitValueMatcher);

  const MFXSTM32L152::ErrorCode errorCode =
    virtualMFXSTM32L152.setGPIOPinState(RANDOM_GPIO_PIN, MFXSTM32L152::GPIOPinState::LOW);

  ASSERT_THAT(errorCode, Eq(MFXSTM32L152::ErrorCode::OK));
}

TEST_F(AMFXSTM32L152, GetGPIOPinStateReadsPinStateValueFromGPIOStateRegister)
{
  static constexpr uint8_t MFXSTM32L152_GPIO_STATE_BASE_REG_ADDR = 0x10u;
  static constexpr uint8_t MFXSTM32L152_GPIO_STATE_REG_ADDR =
    MFXSTM32L152_GPIO_STATE_BASE_REG_ADDR + (static_cast<uint8_t>(RANDOM_GPIO_PIN) / 8u);
  static constexpr uint8_t MFXSTM32L152_GPIO_STATE_INIT_VALUE =
    static_cast<uint8_t>(MFXSTM32L152::GPIOPinState::HIGH) << (static_cast<uint8_t>(RANDOM_GPIO_PIN) % 8u);
  expectMemoryReadOnlyOnce(MFXSTM32L152_GPIO_STATE_REG_ADDR, MFXSTM32L152_GPIO_STATE_INIT_VALUE);

  MFXSTM32L152::GPIOPinState state;
  const MFXSTM32L152::ErrorCode errorCode = virtualMFXSTM32L152.getGPIOPinState(RANDOM_GPIO_PIN, state);

  ASSERT_THAT(errorCode, Eq(MFXSTM32L152::ErrorCode::OK));
  ASSERT_THAT(state, Eq(MFXSTM32L152::GPIOPinState::HIGH));
}

TEST_F(AMFXSTM32L152, RuntimeTaskGetsAllGlobalPendingInterruptsByReadingIRQPengingRegister)
{
  static constexpr uint8_t MFXSTM32L152_IRQ_PENDING_REG_ADDR = 0x08u;
  expectOthersMemoryRead();
  expectMemoryReadOnlyOnce(MFXSTM32L152_IRQ_PENDING_REG_ADDR, 0xFF);

  const MFXSTM32L152::ErrorCode errorCode = virtualMFXSTM32L152.runtimeTask();

  ASSERT_THAT(errorCode, Eq(MFXSTM32L152::ErrorCode::OK));
}

TEST_F(AMFXSTM32L152, RuntimeTaskClearsAllGlobalPendingInterruptsBySettingCorrespondingBitsInIRQAckRegister)
{
  static constexpr uint8_t MFXSTM32L152_IRQ_PENDING_REG_ADDR = 0x08u;
  static constexpr uint8_t MFXSTM32L152_IRQ_ACK_REG_ADDR = 0x44u;
  static constexpr uint8_t MFXSTM32L152_IRQ_PENDING_VALUE = 0xA9;
  expectOthersMemoryReadAndMemoryWrite();
  expectMemoryReadOnlyOnce(MFXSTM32L152_IRQ_PENDING_REG_ADDR, MFXSTM32L152_IRQ_PENDING_VALUE);
  expectMemoryWriteOnlyOnce(MFXSTM32L152_IRQ_ACK_REG_ADDR, MFXSTM32L152_IRQ_PENDING_VALUE);

  const MFXSTM32L152::ErrorCode errorCode = virtualMFXSTM32L152.runtimeTask();

  ASSERT_THAT(errorCode, Eq(MFXSTM32L152::ErrorCode::OK));
}

TEST_F(AMFXSTM32L152, RuntimeTaskGetsPendingGPIOInterruptsByReadingGPIOIRQPendingRegistersIfGlobalGPIOInterruptIsPending)
{
  static constexpr uint8_t MFXSTM32L152_GPIO_IRQ_PENDING_REG1_ADDR = 0x0Cu;
  static constexpr uint8_t MFXSTM32L152_GPIO_IRQ_PENDING_REG2_ADDR = 0x0Du;
  static constexpr uint8_t MFXSTM32L152_GPIO_IRQ_PENDING_REG3_ADDR = 0x0Eu;
  static constexpr uint8_t MFXSTM32L152_IRQ_PENDING_REG_ADDR = 0x08u;
  static constexpr uint8_t MFXSTM32L152_IRQ_PENDING_VALUE =
    MemoryUtility<uint8_t>::setBit(0u, static_cast<uint8_t>(MFXSTM32L152::Interrupt::GPIO));
  expectOthersMemoryRead();
  returnOnMemoryRead(MFXSTM32L152_IRQ_PENDING_REG_ADDR, MFXSTM32L152_IRQ_PENDING_VALUE);
  expectMemoryRead(MFXSTM32L152_GPIO_IRQ_PENDING_REG1_ADDR, 0x00u);
  expectMemoryRead(MFXSTM32L152_GPIO_IRQ_PENDING_REG2_ADDR, 0xFFu);
  expectMemoryRead(MFXSTM32L152_GPIO_IRQ_PENDING_REG3_ADDR, 0x00u);

  const MFXSTM32L152::ErrorCode errorCode = virtualMFXSTM32L152.runtimeTask();

  ASSERT_THAT(errorCode, Eq(MFXSTM32L152::ErrorCode::OK));
}

TEST_F(AMFXSTM32L152, RuntimeTaskClearsAllPendingGPIOInterruptsInGPIOIRQAckRegistersIfGlobalGPIOInterruptIsPending)
{
  static constexpr uint8_t MFXSTM32L152_GPIO_IRQ_PENDING_REG1_ADDR = 0x0Cu;
  static constexpr uint8_t MFXSTM32L152_GPIO_IRQ_PENDING_REG2_ADDR = 0x0Du;
  static constexpr uint8_t MFXSTM32L152_GPIO_IRQ_PENDING_REG3_ADDR = 0x0Eu;
  static constexpr uint8_t MFXSTM32L152_GPIO_IRQ_ACK_REG1_ADDR = 0x54u;
  static constexpr uint8_t MFXSTM32L152_GPIO_IRQ_ACK_REG2_ADDR = 0x55u;
  static constexpr uint8_t MFXSTM32L152_GPIO_IRQ_ACK_REG3_ADDR = 0x56u;
  static constexpr uint8_t MFXSTM32L152_IRQ_PENDING_REG_ADDR = 0x08u;
  static constexpr uint8_t MFXSTM32L152_IRQ_PENDING_VALUE =
    MemoryUtility<uint8_t>::setBit(0u, static_cast<uint8_t>(MFXSTM32L152::Interrupt::GPIO));
  returnOnMemoryRead(MFXSTM32L152_IRQ_PENDING_REG_ADDR, MFXSTM32L152_IRQ_PENDING_VALUE);
  returnOnMemoryRead(MFXSTM32L152_GPIO_IRQ_PENDING_REG1_ADDR, 0xFAu);
  returnOnMemoryRead(MFXSTM32L152_GPIO_IRQ_PENDING_REG2_ADDR, 0xB0u);
  returnOnMemoryRead(MFXSTM32L152_GPIO_IRQ_PENDING_REG3_ADDR, 0xD5u);
  expectOthersMemoryWrite();
  expectMemoryWrite(MFXSTM32L152_GPIO_IRQ_ACK_REG1_ADDR, 0xFAu);
  expectMemoryWrite(MFXSTM32L152_GPIO_IRQ_ACK_REG2_ADDR, 0xB0u);
  expectMemoryWrite(MFXSTM32L152_GPIO_IRQ_ACK_REG3_ADDR, 0xD5u);

  const MFXSTM32L152::ErrorCode errorCode = virtualMFXSTM32L152.runtimeTask();

  ASSERT_THAT(errorCode, Eq(MFXSTM32L152::ErrorCode::OK));
}

TEST_F(AMFXSTM32L152, RuntimeTaskCallsRegisteredGPIOInterruptCallbackIfGPIOInterruptOccurredForCorrespondingPin)
{
  static constexpr uint8_t MFXSTM32L152_GPIO_IRQ_PENDING_REG1_ADDR = 0x0Cu;
  static constexpr uint8_t MFXSTM32L152_IRQ_PENDING_REG_ADDR = 0x08u;
  static constexpr uint8_t MFXSTM32L152_IRQ_PENDING_VALUE =
    MemoryUtility<uint8_t>::setBit(0u, static_cast<uint8_t>(MFXSTM32L152::Interrupt::GPIO));
  expectOthersMemoryRead();
  returnOnMemoryRead(MFXSTM32L152_IRQ_PENDING_REG_ADDR, MFXSTM32L152_IRQ_PENDING_VALUE);
  returnOnMemoryRead(MFXSTM32L152_GPIO_IRQ_PENDING_REG1_ADDR, 0xFAu);
  bool isCallbackCalled = false;
  auto callback = [](void *isCallbackCalledPtr) { *reinterpret_cast<bool*>(isCallbackCalledPtr) = true; };
  virtualMFXSTM32L152.registerGPIOInterruptCallback(MFXSTM32L152::GPIOPin::PIN5, callback, &isCallbackCalled);

  const MFXSTM32L152::ErrorCode errorCode = virtualMFXSTM32L152.runtimeTask();

  ASSERT_THAT(errorCode, Eq(MFXSTM32L152::ErrorCode::OK));
  ASSERT_THAT(isCallbackCalled, Eq(true));
}

TEST_F(AMFXSTM32L152, WakeUpAtTheBegginingSetsWakeUpPinStateToHigh)
{
  setupSysTickReadings();
  const uint64_t timestamp = sysTickMock.getTicks();
  expectWakeUpPinSetState(GPIO::PinState::HIGH, timestamp, 0u);

  const MFXSTM32L152::ErrorCode errorCode = virtualMFXSTM32L152.wakeUp();

  ASSERT_THAT(errorCode, Eq(MFXSTM32L152::ErrorCode::OK));
}

TEST_F(AMFXSTM32L152, WakeUpHoldsWakeUpPinInHighStateFor100MillisecondsBeforeSettingItToLowState)
{
  setupSysTickReadings();
  const uint64_t timestamp = sysTickMock.getTicks();
  expectWakeUpPinSetState(GPIO::PinState::LOW, timestamp, 100u);

  const MFXSTM32L152::ErrorCode errorCode = virtualMFXSTM32L152.wakeUp();

  ASSERT_THAT(errorCode, Eq(MFXSTM32L152::ErrorCode::OK));
}

TEST_F(AMFXSTM32L152, WakeUpHoldsWakeUpPinInLowStateFor10MillisecondsBeforeReturning)
{
  constexpr uint64_t WAKEUP_PIN_HIGH_PERIOD_MS = 100u;
  constexpr uint64_t WAKEUP_PIN_LOW_PERIOD_MS = 10u;
  constexpr uint64_t WAKEUP_TIME_TOTAL = WAKEUP_PIN_HIGH_PERIOD_MS + WAKEUP_PIN_LOW_PERIOD_MS;
  setupSysTickReadings();
  const uint64_t timestamp = sysTickMock.getTicks();

  const MFXSTM32L152::ErrorCode errorCode = virtualMFXSTM32L152.wakeUp();

  ASSERT_THAT(errorCode, Eq(MFXSTM32L152::ErrorCode::OK));
  EXPECT_THAT(sysTickMock.getElapsedTimeInMs(timestamp), AllOf(Ge(WAKEUP_TIME_TOTAL), Le(WAKEUP_TIME_TOTAL + 2u)));
}