#ifndef INTERRUPT_DISPATCHER_H
#define INTERRUPT_DISPATCHER_H


#ifdef __cplusplus
extern "C"
{
#endif // #ifdef __cplusplus

void SysTick_Handler(void);
void USART1_IRQHandler(void);
void USART2_IRQHandler(void);
void USART3_IRQHandler(void);
void UART4_IRQHandler(void);
void UART5_IRQHandler(void);
void LPUART1_IRQHandler(void);
void DMA2D_IRQHandler(void);
void I2C1_EV_IRQHandler(void);
void EXTI0_IRQHandler(void);
void EXTI1_IRQHandler(void);
void EXTI2_IRQHandler(void);
void EXTI3_IRQHandler(void);
void EXTI4_IRQHandler(void);
void EXTI9_5_IRQHandler(void);
void EXTI15_10_IRQHandler(void);

#ifdef __cplusplus
}
#endif // #ifdef __cplusplus

#endif // #ifndef INTERRUPT_DISPATCHER_H