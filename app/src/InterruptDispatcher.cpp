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

void I2C1_EV_IRQHandler(void)
{
  static I2C &i2c1 = DriverManager::getInstance(DriverManager::I2CInstance::I2C1);

  i2c1.IRQHandler();
}

void EXTI0_IRQHandler(void)
{
  static EXTI &exti = DriverManager::getInstance(DriverManager::EXTIInstance::GENERIC);

  exti.IRQHandler(EXTI::EXTILine::LINE0, EXTI::EXTILine::LINE0);
}

void EXTI1_IRQHandler(void)
{
  static EXTI &exti = DriverManager::getInstance(DriverManager::EXTIInstance::GENERIC);

  exti.IRQHandler(EXTI::EXTILine::LINE1, EXTI::EXTILine::LINE1);
}

void EXTI2_IRQHandler(void)
{
  static EXTI &exti = DriverManager::getInstance(DriverManager::EXTIInstance::GENERIC);

  exti.IRQHandler(EXTI::EXTILine::LINE2, EXTI::EXTILine::LINE2);
}

void EXTI3_IRQHandler(void)
{
  static EXTI &exti = DriverManager::getInstance(DriverManager::EXTIInstance::GENERIC);

  exti.IRQHandler(EXTI::EXTILine::LINE3, EXTI::EXTILine::LINE3);
}

void EXTI4_IRQHandler(void)
{
  static EXTI &exti = DriverManager::getInstance(DriverManager::EXTIInstance::GENERIC);

  exti.IRQHandler(EXTI::EXTILine::LINE4, EXTI::EXTILine::LINE4);
}

void EXTI9_5_IRQHandler(void)
{
  static EXTI &exti = DriverManager::getInstance(DriverManager::EXTIInstance::GENERIC);

  exti.IRQHandler(EXTI::EXTILine::LINE5, EXTI::EXTILine::LINE9);
}

void EXTI15_10_IRQHandler(void)
{
  static EXTI &exti = DriverManager::getInstance(DriverManager::EXTIInstance::GENERIC);

  exti.IRQHandler(EXTI::EXTILine::LINE10, EXTI::EXTILine::LINE15);
}
