/******************************************************************************

    @file    StateOS: osport.c
    @author  Rajmund Szymanski
    @date    03.03.2016
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
#if OS_TIMER

/******************************************************************************
 Put here configuration of system timer for tick-less mode
*******************************************************************************/

	#if	CPU_FREQUENCY/OS_FREQUENCY/2-1 > UINT16_MAX
	#error Incorrect Timer frequency!
	#endif

	#if OS_ROBIN
	NVIC_SetPriority(OS_TIM_IRQn, 0xFF);
	NVIC_EnableIRQ(OS_TIM_IRQn);
	#endif
	BB(RCC->APB1ENR, OS_TIM_CLK_ENABLE) = 1;

	OS_TIM->PSC  = CPU_FREQUENCY/OS_FREQUENCY/2-1;
	OS_TIM->EGR  = TIM_EGR_UG;
	OS_TIM->CR1  = TIM_CR1_CEN;
	#if OS_ROBIN
	OS_TIM->CCR1 = OS_FREQUENCY/OS_ROBIN;
	OS_TIM->DIER = TIM_DIER_CC1IE;
	#endif

/******************************************************************************
 End of configuration
*******************************************************************************/

#else //OS_TIMER == 0

/******************************************************************************
 Put here configuration of system timer for non-tick-less mode

 Example for TIM1:

	NVIC_SetPriority(TIM1_UP_TIM10_IRQn, 0xFF);
	NVIC_EnableIRQ  (TIM1_UP_TIM10_IRQn);
	BB(RCC->APB2ENR, RCC_APB2ENR_TIM1EN) = 1;

	TIM1->PSC  = CPU_FREQUENCY / 10000 - 1;
	TIM1->ARR  = 10000 /  OS_FREQUENCY - 1;
	TIM1->EGR  = TIM_EGR_UG;
	TIM1->CR1  = TIM_CR1_CEN;
	TIM1->DIER = TIM_DIER_UIE;

*******************************************************************************/

	#if	CPU_FREQUENCY/OS_FREQUENCY-1 > SysTick_LOAD_RELOAD_Msk
	#error Incorrect SysTick frequency!
	#endif

	SysTick_Config(CPU_FREQUENCY/OS_FREQUENCY);

/******************************************************************************
 End of configuration
*******************************************************************************/

#endif//OS_TIMER

/******************************************************************************
 Put here configuration of interrupt for context switch
*******************************************************************************/

	NVIC_SetPriority(PendSV_IRQn, 0xFF);

/******************************************************************************
 End of configuration
*******************************************************************************/
}

/* -------------------------------------------------------------------------- */

#if  OS_TIMER == 0

/******************************************************************************
 Put here the procedure of interrupt handler of system timer for non-tick-less mode

 Example for TIM1:

void TIM1_UP_TIM10_IRQHandler( void )
{
	TIM1->SR = 0; // clear timer's status register
	System.cnt++;
#if OS_ROBIN
	core_tmr_handler();
	System.dly++;
	if (System.dly >= OS_FREQUENCY/OS_ROBIN)
	port_ctx_switch();
#endif
}

*******************************************************************************/

void SysTick_Handler( void )
{
	System.cnt++;
#if OS_ROBIN
	core_tmr_handler();
	System.dly++;
	if (System.dly >= OS_FREQUENCY/OS_ROBIN)
	port_ctx_switch();
#endif
}

/******************************************************************************
 End of the procedure of interrupt handler
*******************************************************************************/

#endif

/* -------------------------------------------------------------------------- */

#if OS_TIMER != 0
#if OS_ROBIN

/******************************************************************************
 Put here the procedure of interrupt handler of system timer for tick-less mode witch preemption
*******************************************************************************/

void OS_TIM_IRQHandler( void )
{
	unsigned state = OS_TIM->SR;
	OS_TIM->SR = 0;
	if (state & TIM_SR_CC2IF) core_tmr_handler();
	if (state & TIM_SR_CC1IF) port_ctx_switch();
}

/******************************************************************************
 End of the procedure of interrupt handler
*******************************************************************************/

#endif
#endif

/* -------------------------------------------------------------------------- */

/******************************************************************************
 Put here the default procedure of idle process
*******************************************************************************/

__attribute__((weak))
void port_idle_hook( void )
{
#if OS_ROBIN || OS_TIMER == 0
	__WFI();
#endif
}

/******************************************************************************
 End of the default procedure of idle process
*******************************************************************************/

/* -------------------------------------------------------------------------- */
