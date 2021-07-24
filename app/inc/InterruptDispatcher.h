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

#ifdef __cplusplus
}
#endif // #ifdef __cplusplus

#endif // #ifndef INTERRUPT_DISPATCHER_H