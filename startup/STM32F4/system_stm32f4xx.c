/******************************************************************************
 * @file    system_stm32f4xx.c
 * @author  Rajmund Szymanski
 * @date    20.11.2015
 * @brief   This file provides set of configuration functions for STM32F4 uC.
 ******************************************************************************/

#include <stm32f4xx.h>

/* -------------------------------------------------------------------------- */

#define MHz    1000000
#define HSI_FREQ    16 /* MHz */
#define HSE_FREQ     8 /* MHz */
#define USB_FREQ    48 /* MHz */
#define CPU_FREQ   168 /* MHz */
#define VDD       3300 /* mV  */

#define LATENCY  ((100*CPU_FREQ+VDD/2)/VDD)

#define PLL_M (HSE_FREQ)
#define PLL_P (2)
#define PLL_N (PLL_P*CPU_FREQ)
#define PLL_Q (PLL_N/USB_FREQ)

/* -------------------------------------------------------------------------- */

__attribute__ (( weak ))
void SystemInit( void )
{
	FLASH->ACR = LATENCY | FLASH_ACR_PRFTEN | FLASH_ACR_ICEN | FLASH_ACR_DCEN;

	RCC->CR |= RCC_CR_HSEON;
	while ((RCC->CR & RCC_CR_HSERDY) != RCC_CR_HSERDY);

	RCC->PLLCFGR = (PLL_M<<0) | (PLL_N<<6) | (((PLL_P>>1)-1)<<16) | RCC_PLLCFGR_PLLSRC_HSE | (PLL_Q<<24);
	RCC->CR |= RCC_CR_PLLON;
	while ((RCC->CR & RCC_CR_PLLRDY) != RCC_CR_PLLRDY);

	RCC->CFGR = RCC_CFGR_HPRE_DIV1 | RCC_CFGR_PPRE1_DIV4 | RCC_CFGR_PPRE2_DIV2 | RCC_CFGR_SW_PLL;
	while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL);
}

/* -------------------------------------------------------------------------- */

__attribute__ (( weak ))
uint32_t SystemCoreClock = CPU_FREQ * MHz;

/* -------------------------------------------------------------------------- */
