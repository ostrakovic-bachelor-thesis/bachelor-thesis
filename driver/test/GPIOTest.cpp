#include "GPIO.h"
#include "MemoryUtility.h"
#include "MemoryAccess.h"
#include "DriverTest.h"
#include "ResetControlMock.h"
#include "gtest/gtest.h"
#include "gmock/gmock.h"


using namespace ::testing;


class AGPIO : public DriverTest
{
public:

  //! Based on real reset values for GPIO register (source STM32L4R9 reference manual)
  static constexpr uint32_t GPIO_PORT_MODER_RESET_VALUE   = 0xFFFFFFFF;
  static constexpr uint32_t GPIO_PORT_OTYPER_RESET_VALUE  = 0x00000000;
  static constexpr uint32_t GPIO_PORT_OSPEEDR_RESET_VALUE = 0x00000000;
  static constexpr uint32_t GPIO_PORT_PUPDR_RESET_VALUE   = 0x00000000;
  static constexpr uint32_t GPIO_PORT_IDR_RESET_VALUE     = 0x00000000;
  static constexpr uint32_t GPIO_PORT_ODR_RESET_VALUE     = 0x00000000;
  static constexpr uint32_t GPIO_PORT_BSRR_RESET_VALUE    = 0x00000000;
  static constexpr uint32_t GPIO_PORT_LCKR_RESET_VALUE    = 0x00000000;
  static constexpr uint32_t GPIO_PORT_AFRL_RESET_VALUE    = 0x00000000;
  static constexpr uint32_t GPIO_PORT_AFRH_RESET_VALUE    = 0x00000000;
  static constexpr uint32_t GPIO_PORT_BRR_RESET_VALUE     = 0x00000000;

  static constexpr GPIO::Pin RANDOM_GPIO_PIN = GPIO::Pin::PIN10;

  GPIO_TypeDef virtualGPIOPort;
  NiceMock<ResetControlMock> resetControlMock;
  GPIO virtualGPIO = GPIO(&virtualGPIOPort, &resetControlMock);
  GPIO::PinConfiguration pinConfig;

  uint32_t expectedRegVal(uint32_t initialRegVal, GPIO::Pin pin, uint32_t valueSize, uint32_t value);

  void SetUp() override;
  void TearDown() override;
};

uint32_t AGPIO::expectedRegVal(uint32_t initialRegVal, GPIO::Pin pin, uint32_t valueSize, uint32_t value)
{
  return DriverTest::expectedRegVal(initialRegVal, static_cast<uint32_t>(pin), valueSize, value);
}

void AGPIO::SetUp()
{
  DriverTest::SetUp();

  // set values of virtual GPIO port to reset values
  virtualGPIOPort.MODER   = GPIO_PORT_MODER_RESET_VALUE;
  virtualGPIOPort.OTYPER  = GPIO_PORT_OTYPER_RESET_VALUE;
  virtualGPIOPort.OSPEEDR = GPIO_PORT_OSPEEDR_RESET_VALUE;
  virtualGPIOPort.PUPDR   = GPIO_PORT_PUPDR_RESET_VALUE;
  virtualGPIOPort.IDR     = GPIO_PORT_IDR_RESET_VALUE;
  virtualGPIOPort.ODR     = GPIO_PORT_ODR_RESET_VALUE;
  virtualGPIOPort.BSRR    = GPIO_PORT_BSRR_RESET_VALUE;
  virtualGPIOPort.LCKR    = GPIO_PORT_AFRL_RESET_VALUE;
  virtualGPIOPort.AFR[0]  = GPIO_PORT_AFRL_RESET_VALUE;
  virtualGPIOPort.AFR[1]  = GPIO_PORT_AFRH_RESET_VALUE;
  virtualGPIOPort.BRR     = GPIO_PORT_BRR_RESET_VALUE;

  // config is initailized to random valid parameters
  pinConfig.mode              = GPIO::PinMode::ANALOG;
  pinConfig.pullConfig        = GPIO::PullConfig::NO_PULL;
  pinConfig.outputSpeed       = GPIO::OutputSpeed::LOW;
  pinConfig.outputType        = GPIO::OutputType::PUSH_PULL;
  pinConfig.alternateFunction = GPIO::AlternateFunction::AF9;
}

void AGPIO::TearDown()
{
  DriverTest::TearDown();
}


TEST_F(AGPIO, GetPeripheralTagReturnsPointerToUnderlayingGPIOPortCastedToPeripheralType)
{
  ASSERT_THAT(virtualGPIO.getPeripheralTag(),
    Eq(static_cast<Peripheral>(reinterpret_cast<uintptr_t>(&virtualGPIOPort))));
}

TEST_F(AGPIO, ConfigurePinTurnsOnGPIOPortPeripheralClock)
{
  EXPECT_CALL(resetControlMock, enablePeripheralClock(virtualGPIO.getPeripheralTag()))
    .Times(1u);

  const GPIO::ErrorCode errorCode = virtualGPIO.configurePin(RANDOM_GPIO_PIN, pinConfig);

  ASSERT_THAT(errorCode, Eq(GPIO::ErrorCode::OK));
}

TEST_F(AGPIO, ConfigurePinFailsIfTurningOnOfGPIOPortPeripheralClockFail)
{
  resetControlMock.setReturnErrorCode(ResetControl::ErrorCode::INTERNAL);

  const GPIO::ErrorCode errorCode = virtualGPIO.configurePin(RANDOM_GPIO_PIN, pinConfig);

  ASSERT_THAT(errorCode, Eq(GPIO::ErrorCode::CAN_NOT_TURN_ON_PERIPHERAL_CLOCK));
}

TEST_F(AGPIO, ConfigurePinSetsMODERRegisterAccordingToChoosenMode)
{
  const uint32_t expectedModerValue =
    expectedRegVal(GPIO_PORT_MODER_RESET_VALUE, RANDOM_GPIO_PIN, 2u, static_cast<uint32_t>(GPIO::PinMode::OUTPUT));
  pinConfig.mode = GPIO::PinMode::OUTPUT;
  expectRegisterSetOnlyOnce(&(virtualGPIOPort.MODER), expectedModerValue);

  const GPIO::ErrorCode errorCode = virtualGPIO.configurePin(RANDOM_GPIO_PIN, pinConfig);

  ASSERT_THAT(errorCode, Eq(GPIO::ErrorCode::OK));
  ASSERT_THAT(virtualGPIOPort.MODER, Eq(expectedModerValue));
}

TEST_F(AGPIO, ConfiguresPinSetsPUPDRRegisterAccordingToChoosenPullConfig)
{
  const uint32_t expectedPupdrValue =
    expectedRegVal(GPIO_PORT_PUPDR_RESET_VALUE, RANDOM_GPIO_PIN, 2u, static_cast<uint32_t>(GPIO::PullConfig::PULL_UP));
  pinConfig.mode       = GPIO::PinMode::INPUT;
  pinConfig.pullConfig = GPIO::PullConfig::PULL_UP;
  expectRegisterSetOnlyOnce(&(virtualGPIOPort.PUPDR), expectedPupdrValue);

  const GPIO::ErrorCode errorCode = virtualGPIO.configurePin(RANDOM_GPIO_PIN, pinConfig);

  ASSERT_THAT(errorCode, Eq(GPIO::ErrorCode::OK));
  ASSERT_THAT(virtualGPIOPort.PUPDR, Eq(expectedPupdrValue));
}

TEST_F(AGPIO, ConfiguresPinAlwaysSetsPUPDRegisterToNoPullForAnalogMode)
{
  const uint32_t expectedPupdrValue =
    expectedRegVal(GPIO_PORT_PUPDR_RESET_VALUE, RANDOM_GPIO_PIN, 2u, static_cast<uint32_t>(GPIO::PullConfig::NO_PULL));
  pinConfig.mode       = GPIO::PinMode::ANALOG;
  pinConfig.pullConfig = GPIO::PullConfig::PULL_UP;
  expectRegisterSetOnlyOnce(&(virtualGPIOPort.PUPDR), expectedPupdrValue);

  const GPIO::ErrorCode errorCode = virtualGPIO.configurePin(RANDOM_GPIO_PIN, pinConfig);

  ASSERT_THAT(errorCode, Eq(GPIO::ErrorCode::OK));
  ASSERT_THAT(virtualGPIOPort.PUPDR, Eq(expectedPupdrValue));
}

TEST_F(AGPIO, ConfiguresPinSetsOSPEEDRRegisterAccordingToChoosenOutputSpeed)
{
  const uint32_t expectedOspeedrValue =
    expectedRegVal(GPIO_PORT_OSPEEDR_RESET_VALUE, RANDOM_GPIO_PIN, 2u, static_cast<uint32_t>(GPIO::OutputSpeed::HIGH));
  pinConfig.mode        = GPIO::PinMode::OUTPUT;
  pinConfig.outputSpeed = GPIO::OutputSpeed::HIGH;
  expectRegisterSetOnlyOnce(&(virtualGPIOPort.OSPEEDR), expectedOspeedrValue);

  const GPIO::ErrorCode errorCode = virtualGPIO.configurePin(RANDOM_GPIO_PIN, pinConfig);

  ASSERT_THAT(errorCode, Eq(GPIO::ErrorCode::OK));
  ASSERT_THAT(virtualGPIOPort.OSPEEDR, Eq(expectedOspeedrValue));
}

TEST_F(AGPIO, ConfiguresPinDoesNotChangeValueOfOSPEEDRRegisterInInputMode)
{
  pinConfig.mode        = GPIO::PinMode::INPUT;
  pinConfig.outputSpeed = GPIO::OutputSpeed::HIGH;
  expectRegisterNotToChange(&(virtualGPIOPort.OSPEEDR));

  const GPIO::ErrorCode errorCode = virtualGPIO.configurePin(RANDOM_GPIO_PIN, pinConfig);

  ASSERT_THAT(errorCode, Eq(GPIO::ErrorCode::OK));
  ASSERT_THAT(virtualGPIOPort.OSPEEDR, Eq(GPIO_PORT_OSPEEDR_RESET_VALUE));
}

TEST_F(AGPIO, ConfiguresPinDoesNotChangeValueOfOSPEEDRRegisterInAnalogMode)
{
  pinConfig.mode        = GPIO::PinMode::ANALOG;
  pinConfig.outputSpeed = GPIO::OutputSpeed::HIGH;
  expectRegisterNotToChange(&(virtualGPIOPort.OSPEEDR));

  const GPIO::ErrorCode errorCode = virtualGPIO.configurePin(RANDOM_GPIO_PIN, pinConfig);

  ASSERT_THAT(errorCode, Eq(GPIO::ErrorCode::OK));
  ASSERT_THAT(virtualGPIOPort.OSPEEDR, Eq(GPIO_PORT_OSPEEDR_RESET_VALUE));
}

TEST_F(AGPIO, ConfiguresPinSetsOTYPERRegisterAccordingToChoosenOutputType)
{
  const uint32_t expectedOtyperValue =
    expectedRegVal(GPIO_PORT_OTYPER_RESET_VALUE, RANDOM_GPIO_PIN, 1u, static_cast<uint32_t>(GPIO::OutputType::OPEN_DRAIN));
  pinConfig.mode       = GPIO::PinMode::OUTPUT;
  pinConfig.outputType = GPIO::OutputType::OPEN_DRAIN;
  expectRegisterSetOnlyOnce(&(virtualGPIOPort.OTYPER), expectedOtyperValue);

  const GPIO::ErrorCode errorCode = virtualGPIO.configurePin(RANDOM_GPIO_PIN, pinConfig);

  ASSERT_THAT(errorCode, Eq(GPIO::ErrorCode::OK));
  ASSERT_THAT(virtualGPIOPort.OTYPER, Eq(expectedOtyperValue));
}

TEST_F(AGPIO, ConfiguresPinSetsAFRLegisterAccordingToChoosenAlternateFunction)
{
  constexpr GPIO::Pin LOW_GPIO_PIN = GPIO::Pin::PIN6;
  const uint32_t expectedAfrlValue =
    expectedRegVal(GPIO_PORT_AFRL_RESET_VALUE, LOW_GPIO_PIN, 4u, static_cast<uint32_t>(GPIO::AlternateFunction::AF7));
  pinConfig.mode              = GPIO::PinMode::AF;
  pinConfig.alternateFunction = GPIO::AlternateFunction::AF7;
  expectRegisterSetOnlyOnce(&(virtualGPIOPort.AFR[0]), expectedAfrlValue);

  const GPIO::ErrorCode errorCode = virtualGPIO.configurePin(LOW_GPIO_PIN, pinConfig);

  ASSERT_THAT(errorCode, Eq(GPIO::ErrorCode::OK));
  ASSERT_THAT(virtualGPIOPort.AFR[0], Eq(expectedAfrlValue));
}

TEST_F(AGPIO, ConfiguresPinSetsAFRHegisterAccordingToChoosenAlternateFunction)
{
  constexpr GPIO::Pin HIGH_GPIO_PIN = GPIO::Pin::PIN14;
  constexpr GPIO::Pin RELATIVE_HIGH_GPIO_PIN_POS = GPIO::Pin::PIN6;
  const uint32_t expectedAfrhValue =
    expectedRegVal(GPIO_PORT_AFRH_RESET_VALUE, RELATIVE_HIGH_GPIO_PIN_POS, 4u, static_cast<uint32_t>(GPIO::AlternateFunction::AF13));
  pinConfig.mode              = GPIO::PinMode::AF;
  pinConfig.alternateFunction = GPIO::AlternateFunction::AF13;
  expectRegisterSetOnlyOnce(&(virtualGPIOPort.AFR[1]), expectedAfrhValue);

  const GPIO::ErrorCode errorCode = virtualGPIO.configurePin(HIGH_GPIO_PIN, pinConfig);

  ASSERT_THAT(errorCode, Eq(GPIO::ErrorCode::OK));
  ASSERT_THAT(virtualGPIOPort.AFR[1], Eq(expectedAfrhValue));
}

TEST_F(AGPIO, ConfiguresPinDoesNotChangeValueOfOTYPERRegisterInAnalogMode)
{
  pinConfig.mode       = GPIO::PinMode::ANALOG;
  pinConfig.outputType = GPIO::OutputType::OPEN_DRAIN;
  expectRegisterNotToChange(&(virtualGPIOPort.OTYPER));

  const GPIO::ErrorCode errorCode = virtualGPIO.configurePin(RANDOM_GPIO_PIN, pinConfig);

  ASSERT_THAT(errorCode, Eq(GPIO::ErrorCode::OK));
  ASSERT_THAT(virtualGPIOPort.OTYPER, Eq(GPIO_PORT_OTYPER_RESET_VALUE));
}

TEST_F(AGPIO, ConfiguresPinDoesNotChangeValueOfOTYPERRegisterInInputMode)
{
  pinConfig.mode       = GPIO::PinMode::INPUT;
  pinConfig.outputType = GPIO::OutputType::OPEN_DRAIN;
  expectRegisterNotToChange(&(virtualGPIOPort.OTYPER));

  const GPIO::ErrorCode errorCode = virtualGPIO.configurePin(RANDOM_GPIO_PIN, pinConfig);

  ASSERT_THAT(errorCode, Eq(GPIO::ErrorCode::OK));
  ASSERT_THAT(virtualGPIOPort.OTYPER, Eq(GPIO_PORT_OTYPER_RESET_VALUE));
}

TEST_F(AGPIO, ConfiguresPinDoesNotChangeValueOfAFRegistersInInputMode)
{
  constexpr GPIO::Pin LOW_GPIO_PIN = GPIO::Pin::PIN6;
  pinConfig.mode              = GPIO::PinMode::INPUT;
  pinConfig.alternateFunction = GPIO::AlternateFunction::AF10;
  expectRegisterNotToChange(&(virtualGPIOPort.AFR[0]));

  const GPIO::ErrorCode errorCode = virtualGPIO.configurePin(LOW_GPIO_PIN, pinConfig);

  ASSERT_THAT(errorCode, Eq(GPIO::ErrorCode::OK));
  ASSERT_THAT(virtualGPIOPort.AFR[0], Eq(GPIO_PORT_AFRL_RESET_VALUE));
}

TEST_F(AGPIO, ConfiguresPinDoesNotChangeValueOfARRegistersInOutputMode)
{
  constexpr GPIO::Pin HIGH_GPIO_PIN = GPIO::Pin::PIN10;
  pinConfig.mode              = GPIO::PinMode::OUTPUT;
  pinConfig.alternateFunction = GPIO::AlternateFunction::AF10;
  expectRegisterNotToChange(&(virtualGPIOPort.AFR[1]));

  const GPIO::ErrorCode errorCode = virtualGPIO.configurePin(HIGH_GPIO_PIN, pinConfig);

  ASSERT_THAT(errorCode, Eq(GPIO::ErrorCode::OK));
  ASSERT_THAT(virtualGPIOPort.AFR[1], Eq(GPIO_PORT_AFRH_RESET_VALUE));
}

TEST_F(AGPIO, ConfiguresPinDoesNotChangeValueOfARRegistersInAnalogMode)
{
  constexpr GPIO::Pin HIGH_GPIO_PIN = GPIO::Pin::PIN14;
  pinConfig.mode              = GPIO::PinMode::ANALOG;
  pinConfig.alternateFunction = GPIO::AlternateFunction::AF3;
  expectRegisterNotToChange(&(virtualGPIOPort.AFR[1]));

  const GPIO::ErrorCode errorCode = virtualGPIO.configurePin(HIGH_GPIO_PIN, pinConfig);

  ASSERT_THAT(errorCode, Eq(GPIO::ErrorCode::OK));
  ASSERT_THAT(virtualGPIOPort.AFR[1], Eq(GPIO_PORT_AFRH_RESET_VALUE));
}

TEST_F(AGPIO, ConfiguresPinFailsIfPinIsOutOfAllowedRange)
{
  const GPIO::Pin invalidPin = static_cast<GPIO::Pin>(16u);
  expectNoRegisterToChange();

  const GPIO::ErrorCode errorCode = virtualGPIO.configurePin(invalidPin, pinConfig);

  ASSERT_THAT(errorCode, Eq(GPIO::ErrorCode::INVALID_PIN_VALUE));
}

TEST_F(AGPIO, ConfiguresPinFailsIfModeIsOutOfAllowedRange)
{
  pinConfig.mode  = static_cast<GPIO::PinMode>(7u);
  expectNoRegisterToChange();

  const GPIO::ErrorCode errorCode = virtualGPIO.configurePin(RANDOM_GPIO_PIN, pinConfig);

  ASSERT_THAT(errorCode, Eq(GPIO::ErrorCode::PIN_CONFIG_PARAM_INVALID_VALUE));
}

TEST_F(AGPIO, ConfiguresPinFailsIfPullConfigIsOutOfAllowedRange)
{
  pinConfig.pullConfig  = static_cast<GPIO::PullConfig>(4u);
  expectNoRegisterToChange();

  const GPIO::ErrorCode errorCode = virtualGPIO.configurePin(RANDOM_GPIO_PIN, pinConfig);

  ASSERT_THAT(errorCode, Eq(GPIO::ErrorCode::PIN_CONFIG_PARAM_INVALID_VALUE));
}

TEST_F(AGPIO, ConfiguresPinFailsIfOutputSpeedIsOutOfAllowedRangeInOutputMode)
{
  pinConfig.mode        = GPIO::PinMode::OUTPUT;
  pinConfig.outputSpeed  = static_cast<GPIO::OutputSpeed>(4u);
  expectNoRegisterToChange();

  const GPIO::ErrorCode errorCode = virtualGPIO.configurePin(RANDOM_GPIO_PIN, pinConfig);

  ASSERT_THAT(errorCode, Eq(GPIO::ErrorCode::PIN_CONFIG_PARAM_INVALID_VALUE));
}

TEST_F(AGPIO, ConfiguresPinFailsIfOutputSpeedIsOutOfAllowedRangeInAFMode)
{
  pinConfig.mode        = GPIO::PinMode::AF;
  pinConfig.outputSpeed  = static_cast<GPIO::OutputSpeed>(4u);
  expectNoRegisterToChange();

  const GPIO::ErrorCode errorCode = virtualGPIO.configurePin(RANDOM_GPIO_PIN, pinConfig);

  ASSERT_THAT(errorCode, Eq(GPIO::ErrorCode::PIN_CONFIG_PARAM_INVALID_VALUE));
}

TEST_F(AGPIO, ConfiguresPinFailsIfOutputTypeIsOutOfAllowedRangeInOutputMode)
{
  pinConfig.mode       = GPIO::PinMode::OUTPUT;
  pinConfig.outputType = static_cast<GPIO::OutputType>(2u);
  expectNoRegisterToChange();

  const GPIO::ErrorCode errorCode = virtualGPIO.configurePin(RANDOM_GPIO_PIN, pinConfig);

  ASSERT_THAT(errorCode, Eq(GPIO::ErrorCode::PIN_CONFIG_PARAM_INVALID_VALUE));
}

TEST_F(AGPIO, ConfiguresPinFailsIfOutputTypeIsOutOfAllowedRangeInAFMode)
{
  pinConfig.mode        = GPIO::PinMode::AF;
  pinConfig.outputType = static_cast<GPIO::OutputType>(4u);
  expectNoRegisterToChange();

  const GPIO::ErrorCode errorCode = virtualGPIO.configurePin(RANDOM_GPIO_PIN, pinConfig);

  ASSERT_THAT(errorCode, Eq(GPIO::ErrorCode::PIN_CONFIG_PARAM_INVALID_VALUE));
}

TEST_F(AGPIO, ConfiguresPinFailsIfAlternateFunctionIsOutOfAllowedRangeInAFMode)
{
  pinConfig.mode              = GPIO::PinMode::AF;
  pinConfig.alternateFunction = static_cast<GPIO::AlternateFunction>(17u);
  expectNoRegisterToChange();

  const GPIO::ErrorCode errorCode = virtualGPIO.configurePin(RANDOM_GPIO_PIN, pinConfig);

  ASSERT_THAT(errorCode, Eq(GPIO::ErrorCode::PIN_CONFIG_PARAM_INVALID_VALUE));
}

TEST_F(AGPIO, InOutputModeSetsPinStateSetsPinStateToWantedLevel)
{
  const uint32_t expectedOdrValue =
    expectedRegVal(GPIO_PORT_ODR_RESET_VALUE, RANDOM_GPIO_PIN, 1u, static_cast<uint32_t>(GPIO::PinState::HIGH));
  pinConfig.mode = GPIO::PinMode::OUTPUT;
  virtualGPIO.configurePin(RANDOM_GPIO_PIN, pinConfig);
  expectRegisterSetOnlyOnce(&(virtualGPIOPort.ODR), expectedOdrValue);

  const GPIO::ErrorCode errorCode = virtualGPIO.setPinState(RANDOM_GPIO_PIN, GPIO::PinState::HIGH);

  ASSERT_THAT(errorCode, Eq(GPIO::ErrorCode::OK));
  ASSERT_THAT(virtualGPIOPort.ODR, Eq(expectedOdrValue));
}

TEST_F(AGPIO, SetPinStateFailesInAnyModeOtherThanOutput)
{
  pinConfig.mode = GPIO::PinMode::ANALOG;
  virtualGPIO.configurePin(RANDOM_GPIO_PIN, pinConfig);
  expectNoRegisterToChange();

  const GPIO::ErrorCode errorCode = virtualGPIO.setPinState(RANDOM_GPIO_PIN, GPIO::PinState::HIGH);

  ASSERT_THAT(errorCode, Eq(GPIO::ErrorCode::PIN_MODE_IS_NOT_APPROPRIATE));
}

TEST_F(AGPIO, SetsPinStateFailsIfPinIsOutOfAllowedRange)
{
  const GPIO::Pin invalidPin = static_cast<GPIO::Pin>(16u);
  expectNoRegisterToChange();

  const GPIO::ErrorCode errorCode = virtualGPIO.setPinState(invalidPin, GPIO::PinState::HIGH);

  ASSERT_THAT(errorCode, Eq(GPIO::ErrorCode::INVALID_PIN_VALUE));
}

TEST_F(AGPIO, SetsPinStateFailsIfPinStateIsOutOfAllowedRange)
{
  const GPIO::PinState invalidPinState = static_cast<GPIO::PinState>(2u);
  expectNoRegisterToChange();

  const GPIO::ErrorCode errorCode = virtualGPIO.setPinState(RANDOM_GPIO_PIN, invalidPinState);

  ASSERT_THAT(errorCode, Eq(GPIO::ErrorCode::INVALID_PIN_STATE_VALUE));
}

TEST_F(AGPIO, GetsPinMode)
{
  constexpr GPIO::PinMode EXPECTED_PIN_MODE = GPIO::PinMode::OUTPUT;
  pinConfig.mode = EXPECTED_PIN_MODE;
  virtualGPIO.configurePin(RANDOM_GPIO_PIN, pinConfig);
  expectNoRegisterToChange();

  GPIO::PinMode pinMode;
  const GPIO::ErrorCode errorCode = virtualGPIO.getPinMode(RANDOM_GPIO_PIN, pinMode);

  ASSERT_THAT(pinMode, Eq(EXPECTED_PIN_MODE));
  ASSERT_THAT(errorCode, Eq(GPIO::ErrorCode::OK));
}

TEST_F(AGPIO, GetsPinModeFailsIfPinIsOutOfAllowedRange)
{
  const GPIO::Pin invalidPin = static_cast<GPIO::Pin>(16u);
  expectNoRegisterToChange();

  GPIO::PinMode pinMode;
  const GPIO::ErrorCode errorCode = virtualGPIO.getPinMode(invalidPin, pinMode);

  ASSERT_THAT(errorCode, Eq(GPIO::ErrorCode::INVALID_PIN_VALUE));
}

TEST_F(AGPIO, GetsPinStateGetsValueFromODRWhenInOutputMode)
{
  virtualGPIOPort.ODR =
    static_cast<uint32_t>(GPIO::PinState::HIGH) << static_cast<uint32_t>(RANDOM_GPIO_PIN);
  pinConfig.mode = GPIO::PinMode::OUTPUT;
  virtualGPIO.configurePin(RANDOM_GPIO_PIN, pinConfig);
  expectNoRegisterToChange();

  GPIO::PinState state;
  const GPIO::ErrorCode errorCode = virtualGPIO.getPinState(RANDOM_GPIO_PIN, state);

  ASSERT_THAT(errorCode, Eq(GPIO::ErrorCode::OK));
  ASSERT_THAT(state, Eq(GPIO::PinState::HIGH));
}

TEST_F(AGPIO, GetsPinStateGetsValueFromIDRWhenInInputMode)
{
  virtualGPIOPort.IDR =
    static_cast<uint32_t>(GPIO::PinState::HIGH) << static_cast<uint32_t>(RANDOM_GPIO_PIN);
  pinConfig.mode = GPIO::PinMode::INPUT;
  virtualGPIO.configurePin(RANDOM_GPIO_PIN, pinConfig);
  expectNoRegisterToChange();

  GPIO::PinState state;
  const GPIO::ErrorCode errorCode = virtualGPIO.getPinState(RANDOM_GPIO_PIN, state);

  ASSERT_THAT(errorCode, Eq(GPIO::ErrorCode::OK));
  ASSERT_THAT(state, Eq(GPIO::PinState::HIGH));
}

TEST_F(AGPIO, GetsPinStateFailsForAnyModeOtherThanInputOrOutput)
{
  pinConfig.mode = GPIO::PinMode::ANALOG;
  virtualGPIO.configurePin(RANDOM_GPIO_PIN, pinConfig);
  expectNoRegisterToChange();

  GPIO::PinState state;
  const GPIO::ErrorCode errorCode = virtualGPIO.getPinState(RANDOM_GPIO_PIN, state);

  ASSERT_THAT(errorCode, Eq(GPIO::ErrorCode::PIN_MODE_IS_NOT_APPROPRIATE));
}

TEST_F(AGPIO, GetsPinStateFailsIfPinIsOutOfAllowedRange)
{
  const GPIO::Pin invalidPin = static_cast<GPIO::Pin>(16u);
  expectNoRegisterToChange();

  GPIO::PinState state;
  const GPIO::ErrorCode errorCode = virtualGPIO.getPinState(invalidPin, state);

  ASSERT_THAT(errorCode, Eq(GPIO::ErrorCode::INVALID_PIN_VALUE));
}

TEST_F(AGPIO, IsPinInUsageReturnsTrueIfPinIsConfiguredToWorkInAnyOfModesAkaIsInUsage)
{
  virtualGPIO.configurePin(RANDOM_GPIO_PIN, pinConfig);
  expectNoRegisterToChange();

  ASSERT_THAT(virtualGPIO.isPinInUsage(RANDOM_GPIO_PIN), Eq(true));
}

TEST_F(AGPIO, IsPinInUsageReturnsFalseIfPinIsNotConfiguredToWorkInAnyOfModesAkaIsNotInUsage)
{
  expectNoRegisterToChange();

  ASSERT_THAT(virtualGPIO.isPinInUsage(RANDOM_GPIO_PIN), Eq(false));
}