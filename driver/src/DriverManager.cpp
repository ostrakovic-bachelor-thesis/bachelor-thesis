#include "DriverManager.h"


ClockControl DriverManager::s_clockControlDriverInstance = 
  ClockControl(reinterpret_cast<RCC_TypeDef*>(Peripheral::RCC));

ResetControl DriverManager::s_resetControlDriverInstance =
  ResetControl(reinterpret_cast<RCC_TypeDef*>(Peripheral::RCC));

InterruptController DriverManager::s_interruptControllerDriverInstance =
  InterruptController(reinterpret_cast<NVIC_Type*>(CoreHardware::NVIC), reinterpret_cast<SCB_Type*>(CoreHardware::SCB));

SysTick DriverManager::s_sysTickDriverInstance =
  SysTick(reinterpret_cast<SysTick_Type*>(CoreHardware::SYSTICK), &s_clockControlDriverInstance);

GPIO DriverManager::s_gpioDriverInstance[] =
{
  [static_cast<uint8_t>(GPIOInstance::GPIOA)] = GPIO(reinterpret_cast<GPIO_TypeDef*>(Peripheral::GPIOA)),
  [static_cast<uint8_t>(GPIOInstance::GPIOB)] = GPIO(reinterpret_cast<GPIO_TypeDef*>(Peripheral::GPIOB)),
  [static_cast<uint8_t>(GPIOInstance::GPIOC)] = GPIO(reinterpret_cast<GPIO_TypeDef*>(Peripheral::GPIOC)),
  [static_cast<uint8_t>(GPIOInstance::GPIOD)] = GPIO(reinterpret_cast<GPIO_TypeDef*>(Peripheral::GPIOD)),
  [static_cast<uint8_t>(GPIOInstance::GPIOE)] = GPIO(reinterpret_cast<GPIO_TypeDef*>(Peripheral::GPIOE)),
  [static_cast<uint8_t>(GPIOInstance::GPIOF)] = GPIO(reinterpret_cast<GPIO_TypeDef*>(Peripheral::GPIOF)),
  [static_cast<uint8_t>(GPIOInstance::GPIOG)] = GPIO(reinterpret_cast<GPIO_TypeDef*>(Peripheral::GPIOG)),
  [static_cast<uint8_t>(GPIOInstance::GPIOH)] = GPIO(reinterpret_cast<GPIO_TypeDef*>(Peripheral::GPIOH)),
  [static_cast<uint8_t>(GPIOInstance::GPIOI)] = GPIO(reinterpret_cast<GPIO_TypeDef*>(Peripheral::GPIOI))
};

USART DriverManager::s_usartDriverInstance[] =
{
  [static_cast<uint8_t>(USARTInstance::USART1)]  = 
    USART(reinterpret_cast<USART_TypeDef*>(Peripheral::USART1), &s_clockControlDriverInstance),
  [static_cast<uint8_t>(USARTInstance::USART2)]  = 
    USART(reinterpret_cast<USART_TypeDef*>(Peripheral::USART2), &s_clockControlDriverInstance),
  [static_cast<uint8_t>(USARTInstance::USART3)]  = 
    USART(reinterpret_cast<USART_TypeDef*>(Peripheral::USART3), &s_clockControlDriverInstance),
  [static_cast<uint8_t>(USARTInstance::UART4)]   = 
    USART(reinterpret_cast<USART_TypeDef*>(Peripheral::UART4), &s_clockControlDriverInstance),
  [static_cast<uint8_t>(USARTInstance::UART5)]   = 
    USART(reinterpret_cast<USART_TypeDef*>(Peripheral::UART4), &s_clockControlDriverInstance),
  [static_cast<uint8_t>(USARTInstance::LPUART1)] = 
    USART(reinterpret_cast<USART_TypeDef*>(Peripheral::LPUART1), &s_clockControlDriverInstance)
};