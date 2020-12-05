/******************************************************************************

    @file    StateOS: osport.h
    @author  Rajmund Szymanski
    @date    05.12.2020
    @brief   StateOS port definitions for STM8S uC.

 ******************************************************************************

   Copyright (c) 2018-2020 Rajmund Szymanski. All rights reserved.

   Permission is hereby granted, free of charge, to any person obtaining a copy
   of this software and associated documentation files (the "Software"), to
   deal in the Software without restriction, including without limitation the
   rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
   sell copies of the Software, and to permit persons to whom the Software is
   furnished to do so, subject to the following conditions:

   The above copyright notice and this permission notice shall be included
   in all copies or substantial portions of the Software.

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
   OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
   THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
   FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
   IN THE SOFTWARE.

 ******************************************************************************/

#ifndef __STATEOSPORT_H
#define __STATEOSPORT_H

#include <stm8s.h>
#ifndef   NOCONFIG
#include "osconfig.h"
#endif
#include "osdefs.h"

#ifdef __cplusplus
extern "C" {
#endif

/* -------------------------------------------------------------------------- */

#ifndef CPU_FREQUENCY
#define CPU_FREQUENCY  16000000 /* Hz */
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
#define HW_TIMER_SIZE        16 /* bit size of hardware timer                 */
#else
#define HW_TIMER_SIZE         0 /* os does not work in tick-less mode         */
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
uint16_t port_sys_time( void )
{
	return ((uint16_t)TIM3->CNTRH << 8) | TIM3->CNTRL;
}

#endif

/* -------------------------------------------------------------------------- */
// force yield system control to the next process

__STATIC_INLINE
void port_ctx_switch( void )
{
	TIM3->EGR = TIM3_EGR_CC1G;
}

/* -------------------------------------------------------------------------- */
// reset context switch indicator

__STATIC_INLINE
void port_ctx_reset( void )
{
#if HW_TIMER_SIZE
	#if OS_ROBIN
	uint16_t timeout = (((uint16_t)TIM3->CNTRH << 8) | TIM3->CNTRL) + (CPU_FREQUENCY)/(OS_FREQUENCY);
	TIM3->CCR1H = (uint8_t)(timeout >> 8);
	TIM3->CCR1L = (uint8_t)(timeout);
	#else
	TIM3->CCR1H = TIM3->CNTRH;
	TIM3->CCR1L = TIM3->CNTRL;
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
	TIM3->IER = TIM3_IER_CC1IE | TIM3_IER_UIE;
	#else
	TIM3->IER = TIM3_IER_CC1IE;
	#endif
#endif
}

/* -------------------------------------------------------------------------- */
// set time breakpoint

__STATIC_INLINE
void port_tmr_start( uint16_t timeout )
{
#if HW_TIMER_SIZE
	TIM3->CCR2H = (uint8_t)(timeout >> 8);
	TIM3->CCR2L = (uint8_t)(timeout);
	#if HW_TIMER_SIZE < OS_TIMER_SIZE
	TIM3->IER = TIM3_IER_CC2IE | TIM3_IER_CC1IE | TIM3_IER_UIE;
	#else
	TIM3->IER = TIM3_IER_CC2IE | TIM3_IER_CC1IE;
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
	TIM3->IER = TIM3_IER_CC2IE | TIM3_IER_CC1IE | TIM3_IER_UIE;
	#else
	TIM3->IER = TIM3_IER_CC2IE | TIM3_IER_CC1IE;
	#endif
	TIM3->EGR = TIM3_EGR_CC2G;
#endif
}

/* -------------------------------------------------------------------------- */

#if HW_TIMER_SIZE < OS_TIMER_SIZE
INTERRUPT_HANDLER(TIM3_UPD_OVF_BRK_IRQHandler, 15);
#endif
INTERRUPT_HANDLER(TIM3_CAP_COM_IRQHandler,     16);

/* -------------------------------------------------------------------------- */

#ifdef __cplusplus
}
#endif

/* -------------------------------------------------------------------------- */

#endif//__STATEOSPORT_H
