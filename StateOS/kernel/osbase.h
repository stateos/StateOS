/******************************************************************************

    @file    StateOS: osbase.h
    @author  Rajmund Szymanski
    @date    25.10.2018
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

#include <stdint.h>
#include <limits.h>
#include <stdbool.h>
#include "osport.h"

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

typedef struct __mtx mtx_t, * const mtx_id;
typedef struct __tmr tmr_t, * const tmr_id; // timer
typedef struct __tsk tsk_t, * const tsk_id; // task
typedef         void fun_t(); // timer/task procedure

/* -------------------------------------------------------------------------- */

#define E_SUCCESS  ( 0U )     // process was released as a result of taking the supervising object
#define E_FAILURE  ( 0U-1 )   // process was released as a result of any failure
#define E_STOPPED  ( 0U-2 )   // process was released as a result of reset the supervising object
#define E_DELETED  ( 0U-3 )   // process was released as a result of deleting the supervising object
#define E_TIMEOUT  ( 0U-4 )   // process was released as a result of the end of the timer countdown
#define OWNERDEAD  ( 1U )     // previous owner has been killed

/* -------------------------------------------------------------------------- */

// object id

typedef enum __tid
{
	ID_STOPPED = 0, // inactive object
	ID_READY,       // active task
	ID_TIMER,       // active timer

}	tid_t;

/* -------------------------------------------------------------------------- */

// object header

typedef struct __obj
{
	tsk_t  * queue; // next process in the BLOCKED queue
	void   * res;   // allocated object's resource

}	obj_t;

#define               _OBJ_INIT() { 0, 0 }

/* -------------------------------------------------------------------------- */

__STATIC_INLINE
void core_obj_init( obj_t *obj )
{
	(void) obj;
}

/* -------------------------------------------------------------------------- */

// timer / task header

typedef struct __hdr
{
	obj_t    obj;   // object header
	void   * prev;  // previous object (timer / task) in the READY queue
	void   * next;  // next object (timer / task) in the READY queue
	tid_t    id;    // timer / task id

}	hdr_t;

#define               _HDR_INIT() { _OBJ_INIT(), 0, 0, ID_STOPPED }

/* -------------------------------------------------------------------------- */

__STATIC_INLINE
void core_hdr_init( hdr_t *hdr )
{
	core_obj_init(&hdr->obj);

	hdr->id = ID_STOPPED;
}

/* -------------------------------------------------------------------------- */

// system data

typedef struct __sys
{
	tsk_t  * cur;   // pointer to the current task control block
#if HW_TIMER_SIZE < OS_TIMER_SIZE
	volatile
	cnt_t    cnt;   // system timer counter
#endif
	tsk_t  * sig;   // queue of tasks waiting for a signal
	tsk_t  * dly;   // queue of sleeping and suspended tasks
	tsk_t  * des;   // queue of tasks waiting for destruction

}	sys_t;

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

#ifndef IMMEDIATE
#define IMMEDIATE    0       // no waiting
#endif
#ifndef INFINITE
#define INFINITE     CNT_MAX // infinite waiting
#endif

/* -------------------------------------------------------------------------- */

#define RELEASED   ((void *)((uintptr_t)0 - 1)) // released resouces

/* -------------------------------------------------------------------------- */

#ifdef __cplusplus
}
#endif

#endif//__STATEOSBASE_H
