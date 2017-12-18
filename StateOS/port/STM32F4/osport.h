/******************************************************************************

    @file    StateOS: osport.h
    @author  Rajmund Szymanski
    @date    18.12.2017
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
#include <osconfig.h>
#include <osdefs.h>

#ifdef __cplusplus
extern "C" {
#endif

/* -------------------------------------------------------------------------- */

#ifndef OS_TICKLESS
#define OS_TICKLESS           0 /* os does not work in tick-less mode         */
#endif

/* -------------------------------------------------------------------------- */

#ifndef CPU_FREQUENCY
#error  osconfig.h: Undefined CPU_FREQUENCY value!
#endif

/* -------------------------------------------------------------------------- */

#ifndef OS_FREQUENCY

#if     OS_TICKLESS
#define OS_FREQUENCY    1000000 /* Hz */
#else
#define OS_FREQUENCY       1000 /* Hz */
#endif

#endif//OS_FREQUENCY

#if    (OS_TICKLESS == 0) && (OS_FREQUENCY > 1000)
#error  osconfig.h: Incorrect OS_FREQUENCY value!
#endif

/* -------------------------------------------------------------------------- */
// alternate clock source for SysTick

#define ST_FREQUENCY        ((CPU_FREQUENCY)/8)

/* -------------------------------------------------------------------------- */

#ifndef OS_ROBIN
#define OS_ROBIN              0 /* system works in cooperative mode           */
#endif

#if     OS_ROBIN > OS_FREQUENCY
#error  osconfig.h: Incorrect OS_ROBIN value!
#endif

/* -------------------------------------------------------------------------- */
// return current system time

__STATIC_INLINE
uint32_t port_sys_time( void )
{
#if OS_TICKLESS
	return TIM2->CNT;
#else
	return 0;
#endif
}

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
#if OS_TICKLESS
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
#if OS_TICKLESS
	TIM2->DIER = 0;
#endif
}
	
/* -------------------------------------------------------------------------- */
// set time breakpoint

__STATIC_INLINE
void port_tmr_start( uint32_t timeout )
{
#if OS_TICKLESS
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
#if OS_TICKLESS
	NVIC_SetPendingIRQ(TIM2_IRQn);
#endif
}

/* -------------------------------------------------------------------------- */

#ifdef __cplusplus
}
#endif

/* -------------------------------------------------------------------------- */

#endif//__STATEOSPORT_H
