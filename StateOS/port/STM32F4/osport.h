/******************************************************************************

    @file    StateOS: osport.h
    @author  Rajmund Szymanski
    @date    28.12.2017
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

#ifdef  HW_TIMER_SIZE
#error  HW_TIMER_SIZE is an internal definition!
#elif   OS_FREQUENCY > 1000 
#define HW_TIMER_SIZE        32
#else
#define HW_TIMER_SIZE         0
#endif

/* -------------------------------------------------------------------------- */
// alternate clock source for SysTick

#ifdef  ST_FREQUENCY
#error  ST_FREQUENCY is an internal definition!
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

#if HW_TIMER_SIZE >= 32

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
	TIM2->DIER = 0;
#endif
}
	
/* -------------------------------------------------------------------------- */
// set time breakpoint

__STATIC_INLINE
void port_tmr_start( uint32_t timeout )
{
#if HW_TIMER_SIZE
	TIM2->CCR1 = timeout;
	TIM2->DIER = TIM_DIER_CC1IE;
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
	NVIC_SetPendingIRQ(TIM2_IRQn);
#endif
}

/* -------------------------------------------------------------------------- */

#ifdef __cplusplus
}
#endif

/* -------------------------------------------------------------------------- */

#endif//__STATEOSPORT_H
