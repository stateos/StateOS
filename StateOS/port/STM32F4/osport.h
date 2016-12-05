/******************************************************************************

    @file    StateOS: osport.h
    @author  Rajmund Szymanski
    @date    05.12.2016
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

#include <stdint.h>
#include <osconfig.h>

#ifdef __cplusplus
extern "C" {
#endif

/* -------------------------------------------------------------------------- */

#define GLUE( a, b, c )            a##b##c
#define  CAT( a, b, c )       GLUE(a, b, c)

/* -------------------------------------------------------------------------- */

#ifndef  __CORTEX_M
#error   osconfig.h: Include CMSIS device peripheral access layer header file!
#endif

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

#define  Counter OS_TIM->CNT

#else

#define  Counter System.cnt

#endif

#define  Current System.cur

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

#else

#if     (OS_TIMER == 0) && (OS_FREQUENCY > 1000)
#error   osconfig.h: Incorrect OS_FREQUENCY value!
#endif

#endif //OS_FREQUENCY

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

/* -------------------------------------------------------------------------- */

#ifndef  OS_LOCK_LEVEL
#define  OS_LOCK_LEVEL        0 /* critical section blocks all interrupts */
#endif

#if      OS_LOCK_LEVEL >= (1<<__NVIC_PRIO_BITS)
#error   osconfig.h: Incorrect OS_LOCK_LEVEL value! Must be less then (1<<__NVIC_PRIO_BITS).
#endif

/* -------------------------------------------------------------------------- */

#ifndef  OS_MAIN_PRIO
#define  OS_MAIN_PRIO         0 /* priority of main process */
#endif

/* -------------------------------------------------------------------------- */

typedef  uint64_t             stk_t;

/* -------------------------------------------------------------------------- */

#if      defined(__CSMC__)
#define  __initial_sp        _stack
#endif

extern   stk_t              __initial_sp[];
#define  MAIN_TOP           __initial_sp

/* -------------------------------------------------------------------------- */

static inline
void port_ctx_reset( void )
{
#if OS_ROBIN && OS_TIMER
	SysTick->VAL = 0;
#endif
}

/* -------------------------------------------------------------------------- */

static inline
void port_ctx_switch( void )
{
	SCB->ICSR = SCB_ICSR_PENDSVSET_Msk;
}

/* -------------------------------------------------------------------------- */

// clear time breakpoint
static inline
void port_tmr_stop( void )
{
#if OS_ROBIN && OS_TIMER
	OS_TIM->DIER = 0;
#endif
}
	
/* -------------------------------------------------------------------------- */

// set time breakpoint
static inline
void port_tmr_start( unsigned timeout )
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
static inline
void port_tmr_force( void )
{
#if OS_ROBIN && OS_TIMER
	NVIC_SetPendingIRQ(OS_TIM_IRQn);
#endif
}

/* -------------------------------------------------------------------------- */

#if      defined(__ARMCC_VERSION)

#ifndef  __ALWAYS
#define  __ALWAYS                 __attribute__((always_inline))
#endif
#ifndef  __CONSTRUCTOR
#define  __CONSTRUCTOR            __attribute__((constructor))
#endif
#ifndef  __NORETURN
#define  __NORETURN               __attribute__((noreturn))
#endif
#ifndef  __WEAK
#define  __WEAK                   __attribute__((weak))
#endif

#elif    defined(__GNUC__)

#ifndef  __ALWAYS
#define  __ALWAYS                 __attribute__((always_inline))
#endif
#ifndef  __CONSTRUCTOR
#define  __CONSTRUCTOR            __attribute__((constructor))
#endif
#ifndef  __NORETURN
#define  __NORETURN               __attribute__((noreturn, naked))
#endif
#ifndef  __WEAK
#define  __WEAK                   __attribute__((weak))
#endif

#elif    defined(__CSMC__)

#ifndef  __ALWAYS
#define  __ALWAYS
#endif
#ifndef  __CONSTRUCTOR
#define  __CONSTRUCTOR
#warning No compiler specific solution for __CONSTRUCTOR. __CONSTRUCTOR is ignored.
#endif
#ifndef  __NORETURN
#define  __NORETURN
#endif
#ifndef  __WEAK
#define  __WEAK                   __weak
#endif

#define  __disable_irq()          __ASM("cpsid i")
#define  __enable_irq()           __ASM("cpsie i")

#else

#error   Unknown compiler!

#endif

/* -------------------------------------------------------------------------- */

#if      defined(__GNUC__)
#define  port_set_sp(sp)          __ASM("mov sp, %0" :: "r" (sp) : "memory")
#elif    defined(__CSMC__)
#define  port_set_sp(sp)          __ASM("mov sp, r0", sp)
#else
#define  port_set_sp(sp)          __set_PSP((unsigned)(sp))
#endif

#if OS_LOCK_LEVEL && (__CORTEX_M >= 3)

#define  port_get_lock()          __get_BASEPRI()
#define  port_put_lock(state)     __set_BASEPRI(state)

#define  port_set_lock()          __set_BASEPRI((OS_LOCK_LEVEL)<<(8-__NVIC_PRIO_BITS))
#define  port_clr_lock()          __set_BASEPRI(0)

#else

#define  port_get_lock()          __get_PRIMASK()
#define  port_put_lock(state)     __set_PRIMASK(state)

#define  port_set_lock()          __disable_irq()
#define  port_clr_lock()          __enable_irq()

#endif

#define  port_sys_lock()       do { unsigned __LOCK = port_get_lock(); port_set_lock()
#define  port_sys_unlock()          port_put_lock(__LOCK); } while(0)

#define  port_sys_enable()     do { unsigned __LOCK = port_get_lock(); port_clr_lock()
#define  port_sys_disable()         port_put_lock(__LOCK); } while(0)

#define  port_isr_lock()       do { port_set_lock()
#define  port_isr_unlock()          port_clr_lock(); } while(0)

#define  port_isr_enable()     do { port_clr_lock()
#define  port_isr_disable()         port_set_lock(); } while(0)

/* -------------------------------------------------------------------------- */

#ifdef __cplusplus
}
#endif

/* -------------------------------------------------------------------------- */

#endif//__STATEOSPORT_H
