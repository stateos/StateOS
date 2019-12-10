/******************************************************************************
 * @file    system_stm32f4xx.c
 * @author  Rajmund Szymanski
 * @date    28.11.2019
 * @brief   This file provides set of configuration functions for STM32F4 uC.
 ******************************************************************************/

#include <stm32f4xx.h>

/* -------------------------------------------------------------------------- */

#ifdef USE_HAL_DRIVER
 const uint8_t AHBPrescTable[16] = {0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 3, 4, 6, 7, 8, 9};
 const uint8_t APBPrescTable[8]  = {0, 0, 0, 0, 1, 2, 3, 4};
#endif

/* -------------------------------------------------------------------------- */

//#define PLL_SOURCE_HSI        // PLL source: HSI (16MHz)
  #define PLL_SOURCE_HSE        // PLL source: HSE ( 8MHz)
//#define PLL_SOURCE_HSE_BYPASS // PLL source: HSE ( 8MHz)

/* -------------------------------------------------------------------------- */

#define MHz    1000000
#define HSI_FREQ    16 /* MHz */
#define HSE_FREQ     8 /* MHz */
#define USB_FREQ    48 /* MHz */
#define CPU_FREQ   168 /* MHz */
#define VDD       3000 /* mV  */

#define LATENCY  ((100*CPU_FREQ+VDD/2)/VDD)

#ifdef  PLL_SOURCE_HSI
#define PLL_M (HSI_FREQ)
#else
#define PLL_M (HSE_FREQ)
#endif
#define PLL_P (2)
#define PLL_N (PLL_P*CPU_FREQ)
#define PLL_Q (PLL_N/USB_FREQ)

/* -------------------------------------------------------------------------- */

#ifndef __NO_SYSTEM_INIT
__WEAK
void SystemInit( void )
{
	FLASH->ACR = LATENCY | FLASH_ACR_PRFTEN | FLASH_ACR_ICEN | FLASH_ACR_DCEN;
#ifdef  PLL_SOURCE_HSI
	RCC->PLLCFGR = (PLL_M<<0) | (PLL_N<<6) | (((PLL_P>>1)-1)<<16) | RCC_PLLCFGR_PLLSRC_HSI | (PLL_Q<<24);
#else
#ifdef  PLL_SOURCE_HSE_BYPASS
	RCC->CR |= RCC_CR_HSEON | RCC_CR_HSEBYP;
#else
	RCC->CR |= RCC_CR_HSEON;
#endif//PLL_SOURCE_HSE_BYPASS
	while ((RCC->CR & RCC_CR_HSERDY) != RCC_CR_HSERDY);

	RCC->PLLCFGR = (PLL_M<<0) | (PLL_N<<6) | (((PLL_P>>1)-1)<<16) | RCC_PLLCFGR_PLLSRC_HSE | (PLL_Q<<24);
#endif//PLL_SOURCE_HSI
	RCC->CR |= RCC_CR_PLLON;
	while ((RCC->CR & RCC_CR_PLLRDY) != RCC_CR_PLLRDY);

	RCC->CFGR = RCC_CFGR_HPRE_DIV1 | RCC_CFGR_PPRE1_DIV4 | RCC_CFGR_PPRE2_DIV2 | RCC_CFGR_SW_PLL;
	while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL);
}
#endif//__NO_SYSTEM_INIT

/* -------------------------------------------------------------------------- */

#ifndef __NO_SYSTEM_INIT
__WEAK
void SystemCoreClockUpdate( void )
{
}
#endif//__NO_SYSTEM_INIT

/* -------------------------------------------------------------------------- */

#ifndef __NO_SYSTEM_INIT
__WEAK
uint32_t SystemCoreClock = CPU_FREQ * MHz;
#else
__WEAK
uint32_t SystemCoreClock = HSI_FREQ * MHz;
#endif//__NO_SYSTEM_INIT

/* -------------------------------------------------------------------------- */
