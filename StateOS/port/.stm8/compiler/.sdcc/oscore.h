/******************************************************************************

    @file    StateOS: oscore.h
    @author  Rajmund Szymanski
    @date    29.03.2020
    @brief   StateOS port file for STM8 uC.

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
#define OS_STACK_SIZE       128 /* default task stack size in bytes           */
#endif

#ifndef OS_IDLE_STACK
#define OS_IDLE_STACK       128 /* idle task stack size in bytes              */
#endif

/* -------------------------------------------------------------------------- */

#ifndef OS_LOCK_LEVEL
#define OS_LOCK_LEVEL         0 /* critical section blocks all interrupts     */
#endif

#if     OS_LOCK_LEVEL > 0
#error  osconfig.h: Incorrect OS_LOCK_LEVEL value! Must be 0.
#endif

/* -------------------------------------------------------------------------- */

#ifndef OS_MAIN_PRIO
#define OS_MAIN_PRIO          0 /* priority of main process                   */
#endif

/* -------------------------------------------------------------------------- */

#ifndef OS_FUNCTIONAL
#define OS_FUNCTIONAL         0 /* c++ functional library header not included */
#elif   OS_FUNCTIONAL
#error  OS_FUNCTIONAL is an internal port definition!
#endif//OS_FUNCTIONAL

/* -------------------------------------------------------------------------- */

typedef uint8_t               lck_t;
typedef uint8_t               stk_t;

/* -------------------------------------------------------------------------- */
// task context

typedef struct __ctx ctx_t;

struct __ctx
{
	char     dummy; // sp is a pointer to the first free byte on the stack
	// context saved by the software
	// context saved by the hardware
	char     cc;
	char     a;
	unsigned x;
	unsigned y;
#if !defined(__SDCC_MODEL_LARGE)
	char     far;   // hardware saves pc as a far pointer (3 bytes)
#endif
	fun_t  * pc;
};

#if !defined(__SDCC_MODEL_LARGE)
#define _CTX_INIT( pc ) { 0, 0x20, 0, 0, 0, 0, (fun_t*)pc }
#else
#define _CTX_INIT( pc ) { 0, 0x20, 0, 0, 0,    (fun_t*)pc }
#endif

/* -------------------------------------------------------------------------- */
// init task context

__STATIC_INLINE
void port_ctx_init( ctx_t *ctx, fun_t *pc )
{
	ctx->cc = 0x20;
#if !defined(__SDCC_MODEL_LARGE)
	ctx->far = 0;
#endif
	ctx->pc = pc;
}

/* -------------------------------------------------------------------------- */
// is procedure inside ISR?

__STATIC_INLINE
bool port_isr_context( void )
{
	return false;
}

/* -------------------------------------------------------------------------- */

void *_get_SP( void );
void  _set_SP( void *sp );

lck_t _get_CC( void );
void  _set_CC( lck_t lck );

/* -------------------------------------------------------------------------- */
// get current stack pointer

__STATIC_INLINE
void * port_get_sp( void )
{
	return _get_SP();
}

/* -------------------------------------------------------------------------- */

__STATIC_INLINE
lck_t port_get_lock( void )
{
	return _get_CC();
}

__STATIC_INLINE
void port_put_lock( lck_t lck )
{
	_set_CC(lck);
}

__STATIC_INLINE
void port_set_lock( void )
{
	disableInterrupts();
}

__STATIC_INLINE
void port_clr_lock( void )
{
	enableInterrupts();
}

/* -------------------------------------------------------------------------- */

#ifdef __cplusplus
}
#endif

#endif//__STATEOSCORE_H
