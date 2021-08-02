#include "DriverManager.h"
#include "Peripheral.h"
#include "CoreHardware.h"
#include "SysTick.h"
#include "InterruptController.h"
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

TEST(TheDriverManager, GetsSysTickInstance)
{
  const SysTick &sysTick = DriverManager::getInstance(DriverManager::SysTickInstance::GENERIC);

  ASSERT_THAT(reinterpret_cast<uintptr_t>(sysTick.getRawPointer()), Eq(static_cast<uintptr_t>(CoreHardware::SYSTICK)));
}

TEST(TheDriverManager, GetsDMA2DInstance)
{
  const DMA2D &dma2D = DriverManager::getInstance(DriverManager::DMA2DInstance::GENERIC);

  ASSERT_THAT(reinterpret_cast<uintptr_t>(dma2D.getRawPointer()), Eq(static_cast<uintptr_t>(Peripheral::DMA2D)));
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