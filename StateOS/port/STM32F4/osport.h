/******************************************************************************

    @file    StateOS: osport.h
    @author  Rajmund Szymanski
    @date    05.01.2018
    @brief   StateOS port definitions for STM32F4 uC.

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

#ifndef __STATEOSPORT_H
#define __STATEOSPORT_H

#include <stm32f4xx.h>
#ifndef   NOCONFIG
#include <osconfig.h>
#endif
#include <osdefs.h>

#ifdef __cplusplus
extern "C" {
#endif

/* -------------------------------------------------------------------------- */

#ifndef CPU_FREQUENCY
#define CPU_FREQUENCY 168000000 /* Hz */
#endif

/* -------------------------------------------------------------------------- */

#ifndef OS_FREQUENCY
#define OS_FREQUENCY       1000 /* Hz */
#endif

/* -------------------------------------------------------------------------- */
// !! WARNING! OS_TIMER_SIZE < HW_TIMER_SIZE may cause unexpected problems !!

#ifndef OS_TIMER_SIZE
#define OS_TIMER_SIZE        32 /* bit size of system timer counter           */
#endif

/* -------------------------------------------------------------------------- */
// !! WARNING! OS_TIMER_SIZE < HW_TIMER_SIZE may cause unexpected problems !!

#ifdef  HW_TIMER_SIZE
#error  HW_TIMER_SIZE is an internal os definition!
#elif   OS_FREQUENCY > 1000 
#define HW_TIMER_SIZE        32 /* bit size of hardware timer                 */
#else
#define HW_TIMER_SIZE         0 /* os does not work in tick-less mode         */
#endif

/* -------------------------------------------------------------------------- */
// alternate clock source for SysTick

#ifdef  ST_FREQUENCY
#error  ST_FREQUENCY is an internal port definition!
#else
#define ST_FREQUENCY        ((CPU_FREQUENCY)/8)
#endif

/* -------------------------------------------------------------------------- */

#ifndef OS_ROBIN
#define OS_ROBIN              0 /* system works in cooperative mode           */
#endif

#if     OS_ROBIN > OS_FREQUENCY
#error  osconfig.h: Incorrect OS_ROBIN value!
#endif

/* -------------------------------------------------------------------------- */
// return current system time

#if HW_TIMER_SIZE >= OS_TIMER_SIZE

__STATIC_INLINE
uint32_t port_sys_time( void )
{
	return TIM2->CNT;
}

#endif

/* -------------------------------------------------------------------------- */
// force yield system control to the next process

__STATIC_INLINE
void port_ctx_switch( void )
{
	SCB->ICSR = SCB_ICSR_PENDSVSET_Msk;
}

/* -------------------------------------------------------------------------- */
// reset context switch indicator

__STATIC_INLINE
void port_ctx_reset( void )
{
#if HW_TIMER_SIZE
	#if OS_ROBIN
	SysTick->VAL = 0;
	#endif
#endif
}

/* -------------------------------------------------------------------------- */
// clear time breakpoint

__STATIC_INLINE
void port_tmr_stop( void )
{
#if HW_TIMER_SIZE
	#if HW_TIMER_SIZE < OS_TIMER_SIZE
	TIM2->DIER = TIM_DIER_UIE;
	#else
	TIM2->DIER = 0;
	#endif
#endif
}
	
/* -------------------------------------------------------------------------- */
// set time breakpoint

__STATIC_INLINE
void port_tmr_start( uint32_t timeout )
{
#if HW_TIMER_SIZE
	TIM2->CCR1 = timeout;
	#if HW_TIMER_SIZE < OS_TIMER_SIZE
	TIM2->DIER = TIM_DIER_CC1IE | TIM_DIER_UIE;
	#else
	TIM2->DIER = TIM_DIER_CC1IE;
	#endif
#else
	(void) timeout;
#endif
}

/* -------------------------------------------------------------------------- */
// force timer interrupt

__STATIC_INLINE
void port_tmr_force( void )
{
#if HW_TIMER_SIZE
	#if HW_TIMER_SIZE < OS_TIMER_SIZE
	TIM2->DIER = TIM_DIER_CC1IE | TIM_DIER_UIE;
	TIM2->EGR  = TIM_EGR_CC1G;
	#else
	NVIC_SetPendingIRQ(TIM2_IRQn);
	#endif
#endif
}

/* -------------------------------------------------------------------------- */

#ifdef __cplusplus
}
#endif

/* -------------------------------------------------------------------------- */

#endif//__STATEOSPORT_H
