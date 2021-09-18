#include "MFXSTM32L152.h"
#include "I2C.h"
#include <cstdint>
#include "gtest/gtest.h"
#include "gmock/gmock.h"


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

class I2CMock : public I2C
{
public:

  I2CMock():
    I2C(nullptr, nullptr)
  {}

  virtual ~I2CMock() = default;

  // Mock methods
  MOCK_METHOD(ErrorCode, init, (const I2CConfig &), (override));
  MOCK_METHOD(ErrorCode, write, (uint16_t, const void *, uint32_t), (override));
  MOCK_METHOD(ErrorCode, read, (uint16_t, void *, uint32_t), (override));
  MOCK_METHOD(ErrorCode, writeMemory, (uint16_t, uint8_t, const void *, uint32_t), (override));
  MOCK_METHOD(ErrorCode, readMemory, (uint16_t, uint8_t, void *, uint32_t), (override));
  MOCK_METHOD(bool, isTransactionOngoing, (), (const, override));
  MOCK_METHOD(void, IRQHandler, (), (override));
};


class AMFXSTM32L152 : public Test
{
public:

  static constexpr uint16_t MFXSTM32L152_PERIPHERAL_ADDRESS = 0x84;
  static constexpr MFXSTM32L152::GPIOPin RANDOM_GPIO_PIN = MFXSTM32L152::GPIOPin::PIN10;

  NiceMock<I2CMock> i2cMock;
  MFXSTM32L152 virtualMFXSTM32L152 = MFXSTM32L152(&i2cMock);
  MFXSTM32L152::MFXSTM32L152Config mfxstm32l152Config;
  MFXSTM32L152::GPIOPinConfiguration gpioPinConfig;

  template<typename GMockMatcher>
  void expectMemoryWriteOnlyOnce(uint8_t registerAddress, GMockMatcher matcher);

  void expectMemoryReadOnlyOnce(uint8_t registerAddress, uint8_t registerValue);

  void SetUp() override;
  void TearDown() override;
};

void AMFXSTM32L152::SetUp()
{
  mfxstm32l152Config.peripheralAddress = MFXSTM32L152_PERIPHERAL_ADDRESS;
  virtualMFXSTM32L152.init(mfxstm32l152Config);

  gpioPinConfig.mode = MFXSTM32L152::GPIOPinMode::INTERRUPT;
}

void AMFXSTM32L152::TearDown()
{

}

template<typename GMockMatcher>
void AMFXSTM32L152::expectMemoryWriteOnlyOnce(uint8_t registerAddress, GMockMatcher matcher)
{
  EXPECT_CALL(i2cMock, writeMemory(_, registerAddress, Matcher<const void*>(_), 1u))
    .WillOnce([=](uint16_t slaveAddress, uint8_t memoryAddress, const void *messagePtr, uint32_t messageLen) -> I2C::ErrorCode {
      EXPECT_THAT(*reinterpret_cast<const uint8_t*>(messagePtr), matcher);
      return I2C::ErrorCode::OK;
    });

  EXPECT_CALL(i2cMock, writeMemory(_, Not(registerAddress), Matcher<const void*>(_), _))
    .Times(AnyNumber());
}

void AMFXSTM32L152::expectMemoryReadOnlyOnce(uint8_t registerAddress, uint8_t registerValue)
{
  EXPECT_CALL(i2cMock, readMemory(_, registerAddress, Matcher<void*>(_), 1u))
    .Times(1u)
    .WillOnce([=](uint16_t slaveAddress, uint8_t memoryAddress, void *messagePtr, uint32_t messageLen) -> I2C::ErrorCode {
      *reinterpret_cast<uint8_t*>(messagePtr) = registerValue;
      return I2C::ErrorCode::OK;
    });

  EXPECT_CALL(i2cMock, readMemory(_, Not(registerAddress), Matcher<void*>(_), 1u))
    .Times(AnyNumber());
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
  uint8_t id;
  EXPECT_CALL(i2cMock, readMemory(_, MFXSTM32L152_ID_REG_ADDR, &id, 1u))
    .Times(1u);

  const MFXSTM32L152::ErrorCode errorCode = virtualMFXSTM32L152.getID(id);

  ASSERT_THAT(errorCode, Eq(MFXSTM32L152::ErrorCode::OK));
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
  expectMemoryReadOnlyOnce(MFXSTM32L152_GPIO_IRQ_EN_REG_ADDR, MFXSTM32L152_GPIO_IRQ_EN_INIT_VALUE);
  expectMemoryWriteOnlyOnce(MFXSTM32L152_GPIO_IRQ_EN_REG_ADDR, bitValueMatcher);

  const MFXSTM32L152::ErrorCode errorCode = virtualMFXSTM32L152.configureGPIOPin(RANDOM_GPIO_PIN, gpioPinConfig);

  ASSERT_THAT(errorCode, Eq(MFXSTM32L152::ErrorCode::OK));
}

TEST_F(AMFXSTM32L152, EnableGPIOInterruptEnablesGPIOAsMFXSTM32L152IRQSource)
{
  static constexpr uint8_t MFXSTM32L152_IRQ_SRC_EN_REG_ADDR = 0x42u;
  static constexpr uint8_t MFXSTM32L152_IRQ_SRC_EN_INIT_VALUE = 0x0u;
  static constexpr uint8_t MFXSTM32L152_IRQ_SRC_EN_GPIO_EN_POS = 0u;
  auto bitValueMatcher =
    BitHasValue(MFXSTM32L152_IRQ_SRC_EN_GPIO_EN_POS, 1u);
  expectMemoryReadOnlyOnce(MFXSTM32L152_IRQ_SRC_EN_REG_ADDR, MFXSTM32L152_IRQ_SRC_EN_INIT_VALUE);
  expectMemoryWriteOnlyOnce(MFXSTM32L152_IRQ_SRC_EN_REG_ADDR, bitValueMatcher);

  const MFXSTM32L152::ErrorCode errorCode = virtualMFXSTM32L152.enableGPIOInterrupt();

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
  const MFXSTM32L152::ErrorCode errorCode =
    virtualMFXSTM32L152.getGPIOPinState(RANDOM_GPIO_PIN, state);

  ASSERT_THAT(errorCode, Eq(MFXSTM32L152::ErrorCode::OK));
  ASSERT_THAT(state, Eq(MFXSTM32L152::GPIOPinState::HIGH));
}