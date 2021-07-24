#include "sysclock.h"
#include "stm32l4r9xx.h"

void initSYSCLOCK(void)
{
	//wwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwww
	// System Clock Configuration
	// The system Clock is configured as follows :
	// System Clock source            = PLL (HSE)
	// SYSCLK(Hz)                     = 120000000
	// HCLK(Hz)                       = 120000000
	// AHB Prescaler                  = 1
	// APB1 Prescaler                 = 1
	// APB2 Prescaler                 = 1
	// HSE Frequency(Hz)              = 16000000
	// PLL_M                          = 1
	// PLL_N                          = 15
	// PLL_R                          = 2
	// PLL_P                          = 7
	// PLL_Q                          = 4
	// Flash Latency(WS)              = 4
	// clock accuracy				  = +/- 0.8%
	//wwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwww
	uint32_t PLLM = 1;
	uint32_t PLLN = 15;
	uint32_t PLLR = 2;
	uint32_t PLLP = 7;
	uint32_t PLLQ = 4;
	uint32_t PLLSRC = (RCC_PLLCFGR_PLLSRC_HSE);

	RCC->CR |= (RCC_CR_HSEON);	
	// wait until ready
	while((RCC->CR & (RCC_CR_HSERDY)) == 0);
	
	
										
    // set latency to 0WS
    FLASH->ACR &= ~(FLASH_ACR_LATENCY);
    FLASH->ACR |= (FLASH_ACR_LATENCY_0WS);
 
	// Power enable
	RCC->APB1ENR1 |= (RCC_APB1ENR1_PWREN);
    
    // Select the Voltage Range 1 (1.8 V)
    PWR->CR1 = (PWR_CR1_VOS_0);
    // Wait Until the Voltage Regulator is ready
    while(PWR->SR2 & (PWR_SR2_VOSF));

	// configure clock prescalers
	RCC->PLLCFGR  =  ((PLLM-1)<<4)|(PLLN<<8)|(PLLSRC)|(((PLLQ>>1)-1)<<21)|(((PLLR>>1)-1)<<25)|(PLLP<<27);
	
	// Enable the main PLL
	RCC->CR |= (RCC_CR_PLLON);
	
    // Enable PLL System Clock output
    RCC->PLLCFGR |= (RCC_PLLCFGR_PLLREN);
    
    // Wait till PLL is ready 
	while((RCC->CR & (RCC_CR_PLLRDY)) == 0);
  
    
    // 3.
	//// set latency to 4WS
    FLASH->ACR |= (FLASH_ACR_LATENCY_4WS);
    
	// HCLK Configuration
	RCC->CFGR &= ~(RCC_CFGR_HPRE);
	RCC->CFGR |= (RCC_CFGR_HPRE_DIV1);

	// SYSCLK Configuration -> PLLCLK
	RCC->CFGR &= ~(RCC_CFGR_SW);
	RCC->CFGR |= (RCC_CFGR_SW_PLL);

	// wait until PLL is ready
	while((RCC->CFGR & RCC_CFGR_SWS) != (RCC_CFGR_SWS_PLL));
  
	// PCLK1 Configuration
    RCC->CFGR &= ~(RCC_CFGR_PPRE1);
    RCC->CFGR |= (RCC_CFGR_PPRE1_DIV1);
    
	// PCLK2 Configuration
    RCC->CFGR &= ~(RCC_CFGR_PPRE2);
    RCC->CFGR |= (RCC_CFGR_PPRE2_DIV1);
}

