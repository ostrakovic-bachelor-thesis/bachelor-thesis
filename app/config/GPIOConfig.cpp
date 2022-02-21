#include "GPIOConfig.h"
#include "DriverManager.h"
#include "GPIO.h"


GPIOManager::GPIOPinConfiguration g_gpioPinsConfig[7] =
{
  // LED LD2
  [0] =
  {
    .gpio = DriverManager::getInstance(DriverManager::GPIOInstance::GPIOH),
    .pin  = GPIO::Pin::PIN4,
    .pinConfiguration =
    {
      .mode        = GPIO::PinMode::OUTPUT,
      .pullConfig  = GPIO::PullConfig::PULL_DOWN,
      .outputSpeed = GPIO::OutputSpeed::HIGH,
      .outputType  = GPIO::OutputType::PUSH_PULL
    }
  },
  // USART2 TX
  [1] =
  {
    .gpio = DriverManager::getInstance(DriverManager::GPIOInstance::GPIOA),
    .pin  = GPIO::Pin::PIN2,
    .pinConfiguration =
    {
      .mode              = GPIO::PinMode::AF,
      .pullConfig        = GPIO::PullConfig::NO_PULL,
      .outputSpeed       = GPIO::OutputSpeed::HIGH,
      .outputType        = GPIO::OutputType::PUSH_PULL,
      .alternateFunction = GPIO::AlternateFunction::AF7
    }
  },
  // USART2 RX
  [2] =
  {
    .gpio = DriverManager::getInstance(DriverManager::GPIOInstance::GPIOA),
    .pin  = GPIO::Pin::PIN3,
    .pinConfiguration =
    {
      .mode              = GPIO::PinMode::AF,
      .pullConfig        = GPIO::PullConfig::NO_PULL,
      .outputSpeed       = GPIO::OutputSpeed::HIGH,
      .outputType        = GPIO::OutputType::PUSH_PULL,
      .alternateFunction = GPIO::AlternateFunction::AF7
    }
  },
  // MFX WAKEUP
  [3] =
  {
    .gpio = DriverManager::getInstance(DriverManager::GPIOInstance::GPIOB),
    .pin  = GPIO::Pin::PIN2,
    .pinConfiguration =
    {
      .mode        = GPIO::PinMode::OUTPUT,
      .pullConfig  = GPIO::PullConfig::NO_PULL,
      .outputSpeed = GPIO::OutputSpeed::HIGH,
      .outputType  = GPIO::OutputType::PUSH_PULL,
    }
  },
  // MFX IRQ OUT
  [4] =
  {
    .gpio = DriverManager::getInstance(DriverManager::GPIOInstance::GPIOI),
    .pin  = GPIO::Pin::PIN1,
    .pinConfiguration =
    {
      .mode       = GPIO::PinMode::INPUT,
      .pullConfig = GPIO::PullConfig::NO_PULL
    }
  },
  // I2C1 SCL
  [5] =
  {
    .gpio = DriverManager::getInstance(DriverManager::GPIOInstance::GPIOB),
    .pin  = GPIO::Pin::PIN6,
    .pinConfiguration =
    {
      .mode              = GPIO::PinMode::AF,
      .pullConfig        = GPIO::PullConfig::PULL_UP,
      .outputSpeed       = GPIO::OutputSpeed::VERY_HIGH,
      .outputType        = GPIO::OutputType::OPEN_DRAIN,
      .alternateFunction = GPIO::AlternateFunction::AF4
    }
  },
	// I2C1 SDA
  [6] =
  {
    .gpio = DriverManager::getInstance(DriverManager::GPIOInstance::GPIOG),
    .pin  = GPIO::Pin::PIN13,
    .pinConfiguration =
    {
      .mode              = GPIO::PinMode::AF,
      .pullConfig        = GPIO::PullConfig::PULL_UP,
      .outputSpeed       = GPIO::OutputSpeed::VERY_HIGH,
      .outputType        = GPIO::OutputType::OPEN_DRAIN,
      .alternateFunction = GPIO::AlternateFunction::AF4
    }
  }
};
