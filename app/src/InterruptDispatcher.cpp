#include "InterruptDispatcher.h"
#include "DriverManager.h"


void SysTick_Handler(void)
{
  static SysTick &sysTick = DriverManager::getInstance(DriverManager::SysTickInstance::GENERIC);

  sysTick.IRQHandler();
}

void USART1_IRQHandler(void)
{
  static USART &usart1 = DriverManager::getInstance(DriverManager::USARTInstance::USART1);

  usart1.IRQHandler();
}

void USART2_IRQHandler(void)
{
  static USART &usart2 = DriverManager::getInstance(DriverManager::USARTInstance::USART2);

  usart2.IRQHandler();
}

void USART3_IRQHandler(void)
{
  static USART &usart3 = DriverManager::getInstance(DriverManager::USARTInstance::USART3);

  usart3.IRQHandler();
}

void UART4_IRQHandler(void)
{
  static USART &uart4 = DriverManager::getInstance(DriverManager::USARTInstance::UART4);

  uart4.IRQHandler();
}

void UART5_IRQHandler(void)
{
  static USART &uart5 = DriverManager::getInstance(DriverManager::USARTInstance::UART5);

  uart5.IRQHandler();
}

void LPUART1_IRQHandler(void)
{
  static USART &lpUart1 = DriverManager::getInstance(DriverManager::USARTInstance::LPUART1);

  lpUart1.IRQHandler();
}

void DMA2D_IRQHandler(void)
{
  static DMA2D &dma2D = DriverManager::getInstance(DriverManager::DMA2DInstance::GENERIC);

  dma2D.IRQHandler();
}
