/******************************************************************************

    @file    StateOS: osport.c
    @author  Rajmund Szymanski
    @date    18.12.2017
    @brief   StateOS port file for STM32F4 uC.

 ******************************************************************************

    StateOS - Copyright (C) 2013 Rajmund Szymanski.

    This file is part of StateOS distribution.

    StateOS is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published
    by the Free Software Foundation; either version 3 of the License,
    or (at your option) any later version.

    StateOS is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program. If not, see <http://www.gnu.org/licenses/>.

 ******************************************************************************/

#include <oskernel.h>

/* -------------------------------------------------------------------------- */

void port_sys_init( void )
{
/******************************************************************************
 Make sure that the system timer has not yet been initialized
 This is only needed for compilers supporting the "constructor" function attribute or its equivalent
*******************************************************************************/

	if (NVIC_GetPriority(PendSV_IRQn)) return;

/******************************************************************************
 End of check
*******************************************************************************/

#if OS_TICKLESS == 0

/******************************************************************************
 Non-tick-less mode: configuration of system timer
 It must generate interrupts with frequency OS_FREQUENCY
*******************************************************************************/

	#if (CPU_FREQUENCY)/(OS_FREQUENCY)-1 <= SysTick_LOAD_RELOAD_Msk

	SysTick_Config((CPU_FREQUENCY)/(OS_FREQUENCY));

	#elif defined(ST_FREQUENCY) && \
	    (ST_FREQUENCY)/(OS_FREQUENCY)-1 <= SysTick_LOAD_RELOAD_Msk

	NVIC_SetPriority(SysTick_IRQn, 0xFF);

	SysTick->LOAD = (ST_FREQUENCY)/(OS_FREQUENCY)-1;
	SysTick->VAL  = 0U;
	SysTick->CTRL = SysTick_CTRL_ENABLE_Msk|SysTick_CTRL_TICKINT_Msk;

	#else
	#error Incorrect SysTick frequency!
	#endif

/******************************************************************************
 End of configuration
*******************************************************************************/

#else //OS_TICKLESS

/******************************************************************************
 Tick-less mode: configuration of system timer
 It must be rescaled to frequency OS_FREQUENCY
*******************************************************************************/

	#if (CPU_FREQUENCY)/(OS_FREQUENCY)/2-1 > UINT16_MAX
	#error Incorrect Timer frequency!
	#endif

	RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
	NVIC_SetPriority(TIM2_IRQn, 0xFF);
	NVIC_EnableIRQ(TIM2_IRQn);

	TIM2->PSC  = (CPU_FREQUENCY)/(OS_FREQUENCY)/2-1;
	TIM2->EGR  = TIM_EGR_UG;
	TIM2->CR1  = TIM_CR1_CEN;
	#if OS_ROBIN
	TIM2->CCR1 = (OS_FREQUENCY)/(OS_ROBIN);
	TIM2->DIER = TIM_DIER_UIE | TIM_DIER_CC1IE;
	#else
	TIM2->DIER = TIM_DIER_UIE;
	#endif

/******************************************************************************
 End of configuration
*******************************************************************************/

#endif//OS_TICKLESS

/******************************************************************************
 Configuration of interrupt for context switch
*******************************************************************************/

	NVIC_SetPriority(PendSV_IRQn, 0xFF);

/******************************************************************************
 End of configuration
*******************************************************************************/
}

/* -------------------------------------------------------------------------- */

#if OS_TICKLESS == 0

/******************************************************************************
 Non-tick-less mode: interrupt handler of system timer
*******************************************************************************/

void SysTick_Handler( void )
{
	SysTick->CTRL;
	core_sys_tick();
}

/******************************************************************************
 End of the handler
*******************************************************************************/

#else //OS_TICKLESS

/******************************************************************************
 Tick-less mode: interrupt handler of system timer
*******************************************************************************/

void TIM2_IRQHandler( void )
{
	uint32_t sr = TIM2->SR;
	TIM2->SR = ~sr;

	if (sizeof(System.cnt) > 4)     // TIM2->CNT is 32-bit (4 bytes)
		if (sr & TIM_SR_UIF)
			System.cnt += 1ULL<<32; // TIM2->CNT is 32-bit (4 bytes)
	if (sr & TIM_SR_CC2IF)
		core_tmr_handler();
	#if OS_ROBIN
	if (sr & TIM_SR_CC1IF)
		core_ctx_switch();
	#endif
}

/******************************************************************************
 End of the handler
*******************************************************************************/

#endif//OS_TICKLESS

/******************************************************************************
 Interrupt handler for context switch
*******************************************************************************/

void PendSV_Handler( void );

/******************************************************************************
 End of the handler
*******************************************************************************/

/* -------------------------------------------------------------------------- */
