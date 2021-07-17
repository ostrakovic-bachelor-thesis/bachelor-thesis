#include "DriverManager.h"
#include "GPIO.h"
#include "USART.h"
#include "ClockControl.h"
#include "ResetControl.h"
#include "gtest/gtest.h"
#include "gmock/gmock.h"


using namespace ::testing;


TEST(TheDriverManager, GetsGPIOInstance)
{ 
  const GPIO &gpioD = DriverManager::getInstance(DriverManager::GPIOInstance::GPIOD);

  ASSERT_THAT(reinterpret_cast<uintptr_t>(gpioD.getRawPointer()), Eq(static_cast<uintptr_t>(Peripheral::GPIOD)));
}

TEST(TheDriverManager, GetsUSARTInstance)
{ 
  const USART &usart3 = DriverManager::getInstance(DriverManager::USARTInstance::USART3);

  ASSERT_THAT(reinterpret_cast<uintptr_t>(usart3.getRawPointer()), Eq(static_cast<uintptr_t>(Peripheral::USART3)));
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