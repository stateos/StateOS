/******************************************************************************

    @file    StateOS: osbase.h
    @author  Rajmund Szymanski
    @date    26.06.2018
    @brief   This file contains basic definitions for StateOS.

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

#ifndef __STATEOSBASE_H
#define __STATEOSBASE_H

#include <stdbool.h>
#include <osport.h>

#ifdef __cplusplus
extern "C" {
#endif

/* -------------------------------------------------------------------------- */

#ifndef OS_TIMER_SIZE
#define OS_TIMER_SIZE    32
#endif

/* -------------------------------------------------------------------------- */

#if     OS_TIMER_SIZE == 16
typedef uint16_t     cnt_t;
#define CNT_MAX          0xFFFFU
#elif   OS_TIMER_SIZE == 32
typedef uint32_t     cnt_t;
#define CNT_MAX          0xFFFFFFFFUL
#elif   OS_TIMER_SIZE == 64
typedef uint64_t     cnt_t;
#define CNT_MAX          0xFFFFFFFFFFFFFFFFULL
#else
#error  Invalid OS_TIMER_SIZE value!
#endif

/* -------------------------------------------------------------------------- */

#if     HW_TIMER_SIZE > OS_TIMER_SIZE
#error  HW_TIMER_SIZE > OS_TIMER_SIZE causes unexpected problems!
#endif

/* -------------------------------------------------------------------------- */

typedef struct __tmr tmr_t, * const tmr_id; // timer
typedef struct __tsk tsk_t, * const tsk_id; // task
typedef         void fun_t(); // timer/task procedure

typedef struct __spn spn_t, * const spn_id; // spin lock

/* -------------------------------------------------------------------------- */

// object (timer, task) header

typedef struct __obj obj_t;

struct __obj
{
	tsk_t  * queue; // next process in the DELAYED queue
	void   * res;   // allocated object's resource
	void   * prev;  // previous object (timer, task) in the READY queue
	void   * next;  // next object (timer, task) in the READY queue
};

#define               _OBJ_INIT() { 0, 0, 0, 0 }

/* -------------------------------------------------------------------------- */

// system data

typedef struct __sys sys_t;

struct __sys
{
	tsk_t  * cur;   // pointer to the current task control block
#if HW_TIMER_SIZE < OS_TIMER_SIZE
	volatile
	cnt_t    cnt;   // system timer counter
#endif
};

/* -------------------------------------------------------------------------- */

#if (OS_FREQUENCY)/1000000 > 0 && (OS_FREQUENCY)/1000000 < (CNT_MAX)
#define USEC       (cnt_t)((OS_FREQUENCY)/1000000)
#endif
#if (OS_FREQUENCY)/1000 > 0 && (OS_FREQUENCY)/1000 < (CNT_MAX)
#define MSEC       (cnt_t)((OS_FREQUENCY)/1000)
#endif
#if (OS_FREQUENCY) < (CNT_MAX)
#define  SEC       ((cnt_t)(OS_FREQUENCY))
#endif
#if (OS_FREQUENCY) < (CNT_MAX)/60
#define  MIN       ((cnt_t)(OS_FREQUENCY)*60)
#endif
#if (OS_FREQUENCY) < (CNT_MAX)/3600
#define HOUR       ((cnt_t)(OS_FREQUENCY)*3600)
#endif
#if (OS_FREQUENCY) < (CNT_MAX)/86400
#define  DAY       ((cnt_t)(OS_FREQUENCY)*86400)
#endif

/* -------------------------------------------------------------------------- */

#define ID_STOPPED ( 0U) // task or timer stopped
#define ID_READY   ( 1U) // task ready to run
#define ID_DELAYED ( 2U) // task waiting or suspended
#define ID_TIMER   ( 3U) // timer in the countdown state
#define ID_IDLE    ( 4U) // idle process

/* -------------------------------------------------------------------------- */

#define E_SUCCESS  ( 0U) // process was released by taking the supervising object
#define E_STOPPED  (~0U) // process was released by killing the supervising object
#define E_TIMEOUT  (~1U) // process was released by the end of the timer countdown

/* -------------------------------------------------------------------------- */

#ifndef IMMEDIATE
#define IMMEDIATE    0       // no waiting
#endif
#ifndef INFINITE
#define INFINITE     CNT_MAX // infinite waiting
#endif

/* -------------------------------------------------------------------------- */

#define JOINABLE   (tsk_t *)(0)           // task in joinable state
#define DETACHED   (tsk_t *)(UINTPTR_MAX) // task in detached state

/* -------------------------------------------------------------------------- */

#ifdef __cplusplus
}
#endif

#endif//__STATEOSBASE_H
