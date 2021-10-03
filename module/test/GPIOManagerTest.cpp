#include "GPIOManager.h"
#include "GPIOMock.h"
#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include <cstdint>


using namespace ::testing;


class TheGPIOManager : public Test
{
public:

  GPIOMock mockGPIOA;
  GPIOMock mockGPIOB;

  GPIOManager::GPIOPinConfiguration gpioPinsConfiguration[2] =
  {
    [0] =
    {
      .gpio = mockGPIOA,
      .pin  = GPIO::Pin::PIN3,
      .pinConfiguration =
      {
        .mode       = GPIO::PinMode::INPUT,
        .pullConfig = GPIO::PullConfig::PULL_UP
      }
    },

    [1] =
    {
      .gpio = mockGPIOB,
      .pin  = GPIO::Pin::PIN10,
      .pinConfiguration =
      {
        .mode              = GPIO::PinMode::AF,
        .pullConfig        = GPIO::PullConfig::NO_PULL,
        .outputSpeed       = GPIO::OutputSpeed::LOW,
        .outputType        = GPIO::OutputType::PUSH_PULL,
        .alternateFunction = GPIO::AlternateFunction::AF7
      }
    }
  };
};

static void expectConfigurePinCall(GPIOMock &gpio, GPIOManager::GPIOPinConfiguration &gpioPinsConfiguration)
{
  EXPECT_CALL(gpio, configurePin(_, _))
    .WillOnce([&](GPIO::Pin pin, const GPIO::PinConfiguration &pinConfiguration) -> GPIO::ErrorCode
    {
      EXPECT_THAT(pin, Eq(gpioPinsConfiguration.pin));
      EXPECT_THAT(pinConfiguration, Eq(gpioPinsConfiguration.pinConfiguration));

      return GPIO::ErrorCode::OK;
    });
}

static bool operator==(const GPIO::PinConfiguration &gpioPinConfigLhs,
  const GPIO::PinConfiguration &gpioPinConfigRhs)
{
  bool areGPIOPinConfigsEqual = true;

  if (gpioPinConfigLhs.mode != gpioPinConfigRhs.mode)
  {
    areGPIOPinConfigsEqual = false;
  }

  switch (gpioPinConfigLhs.mode)
  {
    case GPIO::PinMode::AF:
    {
      if ((gpioPinConfigLhs.pullConfig != gpioPinConfigRhs.pullConfig)   ||
          (gpioPinConfigLhs.outputType != gpioPinConfigRhs.outputType)   ||
          (gpioPinConfigLhs.outputSpeed != gpioPinConfigRhs.outputSpeed) ||
          (gpioPinConfigLhs.alternateFunction != gpioPinConfigRhs.alternateFunction))
      {
        areGPIOPinConfigsEqual = false;
      }
    }
    break;

    case GPIO::PinMode::OUTPUT:
    {
      if ((gpioPinConfigLhs.pullConfig != gpioPinConfigRhs.pullConfig) ||
          (gpioPinConfigLhs.outputType != gpioPinConfigRhs.outputType) ||
          (gpioPinConfigLhs.outputSpeed != gpioPinConfigRhs.outputSpeed))
      {
        areGPIOPinConfigsEqual = false;
      }
    }

    case GPIO::PinMode::INPUT:
    {
      if (gpioPinConfigLhs.pullConfig != gpioPinConfigRhs.pullConfig)
      {
        areGPIOPinConfigsEqual = false;
      }
    }

    case GPIO::PinMode::ANALOG:
    {
      // do nothing
    }
    break;

    default:
    {
      areGPIOPinConfigsEqual = false;
    }
    break;
  }

  return areGPIOPinConfigsEqual;
}


TEST_F(TheGPIOManager, ConfiguresGPIOPinsSuccessfully)
{
  expectConfigurePinCall(mockGPIOA, gpioPinsConfiguration[0]);
  expectConfigurePinCall(mockGPIOB, gpioPinsConfiguration[1]);

  GPIOManager::ErrorCode errorCode = GPIOManager::configureGPIOPins(gpioPinsConfiguration);

  ASSERT_THAT(errorCode, Eq(GPIOManager::ErrorCode::OK));
}