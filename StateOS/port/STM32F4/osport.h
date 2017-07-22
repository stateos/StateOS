/******************************************************************************

    @file    StateOS: osport.h
    @author  Rajmund Szymanski
    @date    22.07.2017
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

#ifdef __cplusplus
extern "C" {
#endif

/* -------------------------------------------------------------------------- */

#define GLUE( a, b, c )            a##b##c
#define  CAT( a, b, c )       GLUE(a, b, c)

/* -------------------------------------------------------------------------- */

#ifndef  OS_TIMER
#define  OS_TIMER             0 /* os uses SysTick as system timer            */
#endif

/* -------------------------------------------------------------------------- */

#if      OS_TIMER

#define  OS_TIM            CAT(TIM,OS_TIMER,)
#define  OS_TIM_CLK_ENABLE CAT(RCC_APB1ENR_TIM,OS_TIMER,EN)
#define  OS_TIM_IRQn       CAT(TIM,OS_TIMER,_IRQn)
#define  OS_TIM_IRQHandler CAT(TIM,OS_TIMER,_IRQHandler)

#define  Counter           OS_TIM->CNT

#endif

/* -------------------------------------------------------------------------- */

#ifndef CPU_FREQUENCY
#error   osconfig.h: Undefined CPU_FREQUENCY value!
#endif

/* -------------------------------------------------------------------------- */

#ifndef  OS_FREQUENCY

#if      OS_TIMER
#define  OS_FREQUENCY   1000000 /* Hz */
#else
#define  OS_FREQUENCY      1000 /* Hz */
#endif

#endif //OS_FREQUENCY

#if     (OS_TIMER == 0) && (OS_FREQUENCY > 1000)
#error   osconfig.h: Incorrect OS_FREQUENCY value!
#endif

/* -------------------------------------------------------------------------- */

#define  ST_FREQUENCY (CPU_FREQUENCY/8) /* alternate clock source for SysTick */

/* -------------------------------------------------------------------------- */

#ifndef  OS_ROBIN
#define  OS_ROBIN             0 /* system works in cooperative mode           */
#endif

#if     (OS_ROBIN > OS_FREQUENCY)
#error   osconfig.h: Incorrect OS_ROBIN value!
#endif

/* -------------------------------------------------------------------------- */

#ifndef  OS_HEAP_SIZE
#define  OS_HEAP_SIZE         0 /* default system heap: all free memory       */
#endif

/* -------------------------------------------------------------------------- */

#ifndef  OS_STACK_SIZE
#define  OS_STACK_SIZE      256 /* default task stack size in bytes           */
#endif

#ifndef  OS_IDLE_STACK
#define  OS_IDLE_STACK      128 /* idle task stack size in bytes              */
#endif

/* -------------------------------------------------------------------------- */

#ifndef  OS_LOCK_LEVEL
#define  OS_LOCK_LEVEL        0 /* critical section blocks all interrupts */
#endif

#if      OS_LOCK_LEVEL >= (1<<__NVIC_PRIO_BITS)
#error   osconfig.h: Incorrect OS_LOCK_LEVEL value! Must be less then (1<<__NVIC_PRIO_BITS).
#endif

/* -------------------------------------------------------------------------- */

#ifndef  OS_MAIN_PRIO
#define  OS_MAIN_PRIO         0 /* priority of main process                   */
#endif

/* -------------------------------------------------------------------------- */

#ifndef  OS_ASSERT
#define  OS_ASSERT            0 /* do not include standard assertions         */
#endif

#if     (OS_ASSERT == 0)
#ifndef  NDEBUG
#define  NDEBUG
#endif
#endif

#ifndef  NDEBUG
#define  __ASSERT_MSG
#endif

#include <assert.h>

/* -------------------------------------------------------------------------- */

#ifndef  OS_FUNCTIONAL

#if      defined(__CC_ARM) || defined(__CSMC__)
#define  OS_FUNCTIONAL        0 /* c++ functional library header not included */
#else
#define  OS_FUNCTIONAL        1 /* include c++ functional library header      */
#endif

#elif    OS_FUNCTIONAL

#if      defined(__cplusplus) && defined(__CC_ARM)
#error   c++ functional library not allowed for this compiler.
#endif

#endif //OS_FUNCTIONAL

/* -------------------------------------------------------------------------- */

#if      defined(__CSMC__)

#ifndef  __CONSTRUCTOR
#define  __CONSTRUCTOR
#warning No compiler specific solution for __CONSTRUCTOR. __CONSTRUCTOR is ignored.
#endif

#else

#ifndef  __CONSTRUCTOR
#define  __CONSTRUCTOR      __attribute__((constructor))
#endif

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
#if OS_TIMER == 0
	SysTick->CTRL;
#endif
#if OS_ROBIN && OS_TIMER
	SysTick->VAL = 0;
#endif
}

/* -------------------------------------------------------------------------- */

// clear time breakpoint
__STATIC_INLINE
void port_tmr_stop( void )
{
#if OS_ROBIN && OS_TIMER
	OS_TIM->DIER = 0;
#endif
}
	
/* -------------------------------------------------------------------------- */

// set time breakpoint
__STATIC_INLINE
void port_tmr_start( uint32_t timeout )
{
#if OS_ROBIN && OS_TIMER
	OS_TIM->CCR1 = timeout;
	OS_TIM->DIER = TIM_DIER_CC1IE;
#else
	(void) timeout;
#endif
}

/* -------------------------------------------------------------------------- */

// force timer interrupt
__STATIC_INLINE
void port_tmr_force( void )
{
#if OS_ROBIN && OS_TIMER
	NVIC_SetPendingIRQ(OS_TIM_IRQn);
#endif
}

/* -------------------------------------------------------------------------- */

#ifdef __cplusplus
}
#endif

/* -------------------------------------------------------------------------- */

#endif//__STATEOSPORT_H
