/******************************************************************************

    @file    StateOS: oscore.h
    @author  Rajmund Szymanski
    @date    16.07.2018
    @brief   StateOS port file for ARM Cotrex-M uC.

 ******************************************************************************

   Copyright (c) 2018 Rajmund Szymanski. All rights reserved.

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

#ifndef __STATEOSCORE_H
#define __STATEOSCORE_H

#include "osbase.h"

#ifdef __cplusplus
extern "C" {
#endif

/* -------------------------------------------------------------------------- */

#ifndef OS_HEAP_SIZE
#define OS_HEAP_SIZE          0 /* default system heap: all free memory       */
#endif

/* -------------------------------------------------------------------------- */

#ifndef OS_STACK_SIZE
#define OS_STACK_SIZE       256 /* default task stack size in bytes           */
#endif

#ifndef OS_IDLE_STACK
#define OS_IDLE_STACK       128 /* idle task stack size in bytes              */
#endif

/* -------------------------------------------------------------------------- */

#ifndef OS_LOCK_LEVEL
#define OS_LOCK_LEVEL         0 /* critical section blocks all interrupts     */
#endif

#if     OS_LOCK_LEVEL >= (1<<__NVIC_PRIO_BITS)
#error  osconfig.h: Incorrect OS_LOCK_LEVEL value! Must be less then (1<<__NVIC_PRIO_BITS).
#endif

/* -------------------------------------------------------------------------- */

#ifndef OS_MAIN_PRIO
#define OS_MAIN_PRIO          0 /* priority of main process                   */
#endif

/* -------------------------------------------------------------------------- */

#ifdef  __cplusplus

#ifndef OS_FUNCTIONAL

#if   defined(__CC_ARM) || defined(__CSMC__) || defined(__ICCARM__)
#define OS_FUNCTIONAL         0 /* c++ functional library header not included */
#else
#define OS_FUNCTIONAL         1 /* include c++ functional library header      */
#endif

#elif   OS_FUNCTIONAL

#if   defined(__CC_ARM) || defined(__CSMC__) || defined(__ICCARM__)
#error  c++ functional library not allowed for this compiler.
#endif

#endif//OS_FUNCTIONAL

#endif

/* -------------------------------------------------------------------------- */

typedef uint32_t              lck_t;
typedef uint64_t              stk_t;

/* -------------------------------------------------------------------------- */

extern  stk_t               __initial_sp[];
#define MAIN_TOP            __initial_sp

/* -------------------------------------------------------------------------- */
// task context

typedef struct __ctx ctx_t;

struct __ctx
{
	// context saved by the software
	unsigned r4, r5, r6, r7, r8, r9, r10, r11;
	unsigned lr;  // EXC_RETURN
	// context saved by the hardware
	unsigned r0, r1, r2, r3;
	unsigned r12; // ip
	unsigned r14; // lr
	fun_t  * pc;
	unsigned psr;
};

#define _CTX_INIT( pc ) { 0, 0, 0, 0, 0, 0, 0, 0, 0xFFFFFFFD, 0, 0, 0, 0, 0, 0, pc, 0x01000000 }

/* -------------------------------------------------------------------------- */
// init task context

__STATIC_INLINE
void port_ctx_init( ctx_t *ctx, fun_t *pc )
{
	ctx->lr  = 0xFFFFFFFD; // EXC_RETURN: return from psp
	ctx->pc  = pc;
	ctx->psr = 0x01000000;
}

/* -------------------------------------------------------------------------- */

#if   defined(__CSMC__)

#define __get_BASEPRI()     __ASM("mrs r0,basepri")
#define __set_BASEPRI(val)  __ASM("msr basepri,r0", val)

#endif

/* -------------------------------------------------------------------------- */
// is procedure inside ISR?

__STATIC_INLINE
bool port_isr_inside( void )
{
	return (__get_IPSR() != 0U);
}

/* -------------------------------------------------------------------------- */
// are interrupts masked?

__STATIC_INLINE
bool port_isr_masked( void )
{
#if __CORTEX_M >= 3
	return (__get_PRIMASK() != 0U) || (__get_BASEPRI() != 0U);
#else
	return (__get_PRIMASK() != 0U);
#endif
}

/* -------------------------------------------------------------------------- */
// get current stack pointer

__STATIC_INLINE
void * port_get_sp( void )
{
	return (void *) __get_PSP();
}

/* -------------------------------------------------------------------------- */

#if   defined(__CSMC__)

#define __disable_irq()     __ASM("cpsid i")
#define __enable_irq()      __ASM("cpsie i")

#endif

/* -------------------------------------------------------------------------- */

#if OS_LOCK_LEVEL && (__CORTEX_M >= 3)

#define port_get_lock()     __get_BASEPRI()
#define port_put_lock(lck)  __set_BASEPRI(lck)

#define port_set_lock()     __set_BASEPRI((OS_LOCK_LEVEL)<<(8-__NVIC_PRIO_BITS))
#define port_clr_lock()     __set_BASEPRI(0)

#else

#define port_get_lock()     __get_PRIMASK()
#define port_put_lock(lck)  __set_PRIMASK(lck)

#define port_set_lock()     __disable_irq()
#define port_clr_lock()     __enable_irq()

#endif

#define port_set_barrier()  __ISB()

/* -------------------------------------------------------------------------- */

#if __CORTEX_M > 0

#ifdef  OS_MULTICORE

#error  OS_MULTICORE is an internal port definition!

#else

#define OS_MULTICORE

__STATIC_INLINE
void port_spn_lock( volatile unsigned *lock )
{
    while (__LDREXW((volatile uint32_t *)lock) || __STREXW(1, (volatile uint32_t *)lock));
}

#endif//OS_MULTICORE

#endif//__CORTEX_M

/* -------------------------------------------------------------------------- */

#ifdef __cplusplus
}
#endif

#endif//__STATEOSCORE_H
