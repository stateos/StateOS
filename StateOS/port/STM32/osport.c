/******************************************************************************

    @file    State Machine OS: osport.c
    @author  Rajmund Szymanski
    @date    20.11.2015
    @brief   StateOS port file for STM32 uC.

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

__attribute__ (( constructor ))
void port_sys_init( void )
{
#if OS_TIMER

#if	CPU_FREQUENCY/OS_FREQUENCY/2-1 > UINT16_MAX
#error Incorrect Timer frequency!
#endif

#if OS_ROBIN

	NVIC_SetPriority(OS_TIM_IRQn, 0xFF);
	NVIC_EnableIRQ  (OS_TIM_IRQn);

#endif

	RCC->APB1ENR = OS_TIM_CLK_ENABLE;

	OS_TIM->PSC  = CPU_FREQUENCY/OS_FREQUENCY/2-1;
	OS_TIM->ARR  = INFINITE;
#if OS_ROBIN
	OS_TIM->DIER = TIM_DIER_CC1IE;
#endif
	OS_TIM->EGR  = TIM_EGR_UG;
	OS_TIM->CR1  = TIM_CR1_CEN;

#else

#if	CPU_FREQUENCY/OS_FREQUENCY-1 > SysTick_LOAD_RELOAD_Msk
#error Incorrect SysTick frequency!
#endif

	SysTick_Config(CPU_FREQUENCY/OS_FREQUENCY);

#endif

#if OS_ROBIN

	NVIC_SetPriority(PendSV_IRQn, 0xFF);

#endif

	core_ctx_reset();
}

/* -------------------------------------------------------------------------- */

#if  OS_TIMER == 0

void SysTick_Handler( void )
{
	    ++System.cnt;
#if OS_ROBIN
	core_tmr_handler();
	if (++System.dly >= OS_FREQUENCY/OS_ROBIN)
	core_ctx_switch();
#endif
}

#endif

/* -------------------------------------------------------------------------- */

#if OS_TIMER != 0
#if OS_ROBIN

void OS_TIM_IRQHandler( void )
{
	unsigned state = OS_TIM->SR;
	OS_TIM->SR = ~state;
	if (state & TIM_SR_CC2IF) core_tmr_handler();
	if (state & TIM_SR_CC1IF) core_ctx_switch();
}

#endif
#endif

/* -------------------------------------------------------------------------- */

__attribute__ (( weak ))
void port_idle_hook( void )
{
#if OS_ROBIN || OS_TIMER == 0
	__WFI();
#endif
}

/* -------------------------------------------------------------------------- */
