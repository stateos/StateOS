/******************************************************************************

    @file    StateOS: osbase.h
    @author  Rajmund Szymanski
    @date    26.12.2020
    @brief   This file contains basic definitions for StateOS.

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

#ifndef __STATEOSBASE_H
#define __STATEOSBASE_H

#ifndef   DEBUG
#ifndef   NDEBUG
#define   NDEBUG
#endif // NDEBUG
#endif // DEBUG

#include <stdint.h>
#include <limits.h>
#include <stdbool.h>
#include <assert.h>
#include "osport.h"

/* -------------------------------------------------------------------------- */

#ifndef __cplusplus
#define __STD
#else
#define __STD std::
#endif

/* -------------------------------------------------------------------------- */

#ifdef __cplusplus
extern "C" {
#endif

/* -------------------------------------------------------------------------- *
 *                              FOR INTERNAL USE                              *
 * -------------------------------------------------------------------------- */

#ifndef OS_ATOMICS
#define OS_ATOMICS        0
#endif

/* -------------------------------------------------------------------------- */

#ifndef OS_TASK_EXIT
#define OS_TASK_EXIT      0
#endif

/* -------------------------------------------------------------------------- */

#ifndef OS_GUARD_SIZE
#define OS_GUARD_SIZE     0
#endif

#ifndef __MPU_USED
#define __MPU_USED        0
#endif

#if     __MPU_USED
#define __STKALIGN      __ALIGNED(OS_GUARD_SIZE)
#else
#define __STKALIGN
#endif

#ifndef __ALIGNED
#define __ALIGNED(x)
#endif

/* -------------------------------------------------------------------------- */

#ifndef OS_TIMER_SIZE
#define OS_TIMER_SIZE    32
#endif

/* -------------------------------------------------------------------------- */

#if     OS_TIMER_SIZE == 16
typedef uint16_t     cnt_t;
#define CNT_MAX           0xFFFFU
#elif   OS_TIMER_SIZE == 32
typedef uint32_t     cnt_t;
#define CNT_MAX           0xFFFFFFFFUL
#elif   OS_TIMER_SIZE == 64
typedef uint64_t     cnt_t;
#define CNT_MAX           0xFFFFFFFFFFFFFFFFULL
#else
#error  Invalid OS_TIMER_SIZE value!
#endif

#define CNT_LIMIT       ((CNT_MAX)-((CNT_MAX)>>((OS_TIMER_SIZE)/4)))

/* -------------------------------------------------------------------------- */

#if     HW_TIMER_SIZE > OS_TIMER_SIZE
#error  HW_TIMER_SIZE > OS_TIMER_SIZE causes unexpected problems!
#endif

/* -------------------------------------------------------------------------- */

typedef struct __mtx mtx_t, * const mtx_id; // mutex
typedef struct __tmr tmr_t, * const tmr_id; // timer
typedef struct __tsk tsk_t, * const tsk_id; // task
typedef         void fun_t();               // timer/task procedure
typedef         void act_t(unsigned);       // signal action

/* -------------------------------------------------------------------------- */

// error codes

enum
{
	E_SUCCESS = 0,  // process was released as a result of taking the supervising object
	E_FAILURE,      // process was released as a result of any failure
	E_STOPPED,      // process was released as a result of reset the supervising object
	E_DELETED,      // process was released as a result of deleting the supervising object
	E_TIMEOUT,      // process was released as a result of the end of the timer countdown
	OWNERDEAD,      // previous owner has been killed
};

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

#define               _OBJ_INIT() { NULL, NULL }

/* -------------------------------------------------------------------------- */

__STATIC_INLINE
void core_obj_init( obj_t *obj, void *res )
{
	obj->res = res;
}

/* -------------------------------------------------------------------------- */

// timer / task header

typedef struct __hdr
{
	void   * prev;  // previous object (timer / task) in the READY queue
	void   * next;  // next object (timer / task) in the READY queue
	tid_t    id;    // timer / task id

}	hdr_t;

#define               _HDR_INIT() { NULL, NULL, ID_STOPPED }

/* -------------------------------------------------------------------------- */

__STATIC_INLINE
void core_hdr_init( hdr_t *hdr )
{
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
