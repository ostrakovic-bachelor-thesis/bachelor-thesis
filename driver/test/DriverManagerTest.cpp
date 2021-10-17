#include "DriverManager.h"
#include "Peripheral.h"
#include "CoreHardware.h"
#include "SysTick.h"
#include "InterruptController.h"
#include "GPIO.h"
#include "I2C.h"
#include "USART.h"
#include "ClockControl.h"
#include "ResetControl.h"
#include "gtest/gtest.h"
#include "gmock/gmock.h"


using namespace ::testing;


TEST(TheDriverManager, GetsGPIOInstance)
{
  const GPIO &gpioD = DriverManager::getInstance(DriverManager::GPIOInstance::GPIOD);

  ASSERT_THAT(gpioD.getPeripheralTag(), Eq(Peripheral::GPIOD));
}

TEST(TheDriverManager, GetsUSARTInstance)
{
  const USART &usart3 = DriverManager::getInstance(DriverManager::USARTInstance::USART3);

  ASSERT_THAT(usart3.getPeripheralTag(), Eq(Peripheral::USART3));
}

TEST(TheDriverManager, GetsClockControlInstance)
{
  const ClockControl &clockControl = DriverManager::getInstance(DriverManager::ClockControlInstance::GENERIC);

  ASSERT_THAT(reinterpret_cast<uintptr_t>(clockControl.getRawPointer()), Eq(static_cast<uintptr_t>(Peripheral::RCC)));
}

TEST(TheDriverManager, GetsResetControlInstance)
{
  const ResetControl &resetControl = DriverManager::getInstance(DriverManager::ResetControlInstance::GENERIC);

  ASSERT_THAT(reinterpret_cast<uintptr_t>(resetControl.getRawPointer()), Eq(static_cast<uintptr_t>(Peripheral::RCC)));
}

TEST(TheDriverManager, GetsSysTickInstance)
{
  const SysTick &sysTick = DriverManager::getInstance(DriverManager::SysTickInstance::GENERIC);

  ASSERT_THAT(reinterpret_cast<uintptr_t>(sysTick.getRawPointer()), Eq(static_cast<uintptr_t>(CoreHardware::SYSTICK)));
}

TEST(TheDriverManager, GetsDMA2DInstance)
{
  const DMA2D &dma2D = DriverManager::getInstance(DriverManager::DMA2DInstance::GENERIC);

  ASSERT_THAT(dma2D.getPeripheralTag(), Eq(Peripheral::DMA2D));
}

TEST(TheDriverManager, GetsI2CInstance)
{
  const I2C &i2c2 = DriverManager::getInstance(DriverManager::I2CInstance::I2C2);

  ASSERT_THAT(i2c2.getPeripheralTag(), Eq(Peripheral::I2C2));
}

TEST(TheDriverManager, GetsInterruptControllerInstance)
{
  const InterruptController &interruptController =
    DriverManager::getInstance(DriverManager::InterruptControllerInstance::GENERIC);

  ASSERT_THAT(reinterpret_cast<uintptr_t>(interruptController.getRawPointerNVIC()),
    Eq(static_cast<uintptr_t>(CoreHardware::NVIC)));
  ASSERT_THAT(reinterpret_cast<uintptr_t>(interruptController.getRawPointerSCB()),
    Eq(static_cast<uintptr_t>(CoreHardware::SCB)));
}

TEST(TheDriverManager, GetsPowerControlInstance)
{
  const PowerControl &powerControl =
    DriverManager::getInstance(DriverManager::PowerControlInstance::GENERIC);

  ASSERT_THAT(powerControl.getPeripheralTag(), Eq(Peripheral::PWR));
}

TEST(TheDriverManager, GetsEXTIInstance)
{
  const EXTI &exti = DriverManager::getInstance(DriverManager::EXTIInstance::GENERIC);

  ASSERT_THAT(exti.getPeripheralTag(), Eq(Peripheral::EXTI));
}

TEST(TheDriverManager, GetsSystemConfigInstance)
{
  const SystemConfig &systemConfig = DriverManager::getInstance(DriverManager::SystemConfigInstance::GENERIC);

  ASSERT_THAT(systemConfig.getPeripheralTag(), Eq(Peripheral::SYSCFG));
}