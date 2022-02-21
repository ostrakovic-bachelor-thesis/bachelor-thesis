#include "MFXSTM32L152Config.h"
#include "DriverManager.h"
#include "GPIO.h"


MFXSTM32L152::MFXSTM32L152Config g_mfxConfig =
{
  .peripheralAddress    = 0x42,
  .wakeUpPinGPIOPortPtr = &DriverManager::getInstance(DriverManager::GPIOInstance::GPIOB),
  .wakeUpPin            = GPIO::Pin::PIN2,
};

MFXSTM32L152::IRQPinConfiguration g_mfxIrqPinConfiguration =
{
  .outputType = MFXSTM32L152::GPIOOutputType::PUSH_PULL,
  .polarity   = MFXSTM32L152::IRQPinPolarity::HIGH
};

MFXSTM32L152::GPIOPinConfiguration g_mfxGPIOPin0Config =
{
  .mode       = MFXSTM32L152::GPIOPinMode::OUTPUT,
  .outputType = MFXSTM32L152::GPIOOutputType::PUSH_PULL,
  .pullConfig = MFXSTM32L152::GPIOPullConfig::NO_PULL
};

MFXSTM32L152::GPIOPinConfiguration g_mfxGPIOPin1Config =
{
  .mode             = MFXSTM32L152::GPIOPinMode::INTERRUPT,
  .outputType       = MFXSTM32L152::GPIOOutputType::PUSH_PULL,
  .pullConfig       = MFXSTM32L152::GPIOPullConfig::PULL_DOWN,
  .interruptTrigger = MFXSTM32L152::GPIOInterruptTrigger::HIGH_LEVEL
};

MFXSTM32L152::GPIOPinConfiguration g_mfxGPIOPin2Config =
{
  .mode             = MFXSTM32L152::GPIOPinMode::INTERRUPT,
  .outputType       = MFXSTM32L152::GPIOOutputType::PUSH_PULL,
  .pullConfig       = MFXSTM32L152::GPIOPullConfig::PULL_DOWN,
  .interruptTrigger = MFXSTM32L152::GPIOInterruptTrigger::HIGH_LEVEL
};

MFXSTM32L152::GPIOPinConfiguration g_mfxGPIOPin3Config =
{
  .mode             = MFXSTM32L152::GPIOPinMode::INTERRUPT,
  .outputType       = MFXSTM32L152::GPIOOutputType::PUSH_PULL,
  .pullConfig       = MFXSTM32L152::GPIOPullConfig::PULL_DOWN,
  .interruptTrigger = MFXSTM32L152::GPIOInterruptTrigger::HIGH_LEVEL
};

MFXSTM32L152::GPIOPinConfiguration g_mfxGPIOPin4Config =
{
  .mode             = MFXSTM32L152::GPIOPinMode::INTERRUPT,
  .outputType       = MFXSTM32L152::GPIOOutputType::PUSH_PULL,
  .pullConfig       = MFXSTM32L152::GPIOPullConfig::PULL_DOWN,
  .interruptTrigger = MFXSTM32L152::GPIOInterruptTrigger::HIGH_LEVEL
};

MFXSTM32L152::GPIOPinConfiguration g_mfxGPIOPin8Config =
{
  .mode       = MFXSTM32L152::GPIOPinMode::OUTPUT,
  .outputType = MFXSTM32L152::GPIOOutputType::PUSH_PULL,
  .pullConfig = MFXSTM32L152::GPIOPullConfig::NO_PULL
};

MFXSTM32L152::GPIOPinConfiguration g_mfxGPIOPin9Config =
{
  .mode             = MFXSTM32L152::GPIOPinMode::INTERRUPT,
  .outputType       = MFXSTM32L152::GPIOOutputType::PUSH_PULL,
  .pullConfig       = MFXSTM32L152::GPIOPullConfig::PULL_UP,
  .interruptTrigger = MFXSTM32L152::GPIOInterruptTrigger::FALLING_EDGE
};

MFXSTM32L152::GPIOPinConfiguration g_mfxGPIOPin10Config =
{
  .mode       = MFXSTM32L152::GPIOPinMode::OUTPUT,
  .outputType = MFXSTM32L152::GPIOOutputType::PUSH_PULL,
  .pullConfig = MFXSTM32L152::GPIOPullConfig::NO_PULL
};

MFXSTM32L152::GPIOPinConfiguration g_mfxGPIOPin18Config =
{
  .mode       = MFXSTM32L152::GPIOPinMode::OUTPUT,
  .outputType = MFXSTM32L152::GPIOOutputType::PUSH_PULL,
  .pullConfig = MFXSTM32L152::GPIOPullConfig::NO_PULL
};
