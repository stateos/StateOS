/******************************************************************************

    @file    StateOS: osbase.h
    @author  Rajmund Szymanski
    @date    31.12.2017
    @brief   This file contains basic definitions for StateOS.

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

#ifndef __STATEOSBASE_H
#define __STATEOSBASE_H

#include <stdbool.h>
#include <osport.h>

#ifdef __cplusplus
extern "C" {
#endif

/* -------------------------------------------------------------------------- */

typedef struct __tmr tmr_t, * const tmr_id; // timer
typedef struct __tsk tsk_t, * const tsk_id; // task
typedef         void fun_t(); // timer/task procedure

/* -------------------------------------------------------------------------- */

// queue

typedef struct __que que_t;

struct __que
{
	que_t  * next; // next object in the queue
};

/* -------------------------------------------------------------------------- */

// object (timer, task) header

typedef struct __obj obj_t;

struct __obj
{
	tsk_t  * queue; // next process in the DELAYED queue
	void   * res;   // allocated object's resource
	unsigned id;    // object's id: ID_STOPPED, ID_READY, ID_DELAYED, ID_TIMER, ID_IDLE
	void   * prev;  // previous object (timer, task) in the READY queue
	void   * next;  // next object (timer, task) in the READY queue
};

#define               _OBJ_INIT() { 0, 0, 0, 0, 0 }

/* -------------------------------------------------------------------------- */

// system data

typedef struct __sys sys_t;

struct __sys
{
	tsk_t  * cur;   // pointer to the current task control block
	volatile
	uint32_t cnt;   // system timer counter
};

/* -------------------------------------------------------------------------- */

#if (OS_FREQUENCY)/1000000 > 0 && (OS_FREQUENCY)/1000000 < (UINT32_MAX)
#define USEC       (uint32_t)((OS_FREQUENCY)/1000000)
#endif
#if (OS_FREQUENCY)/1000 > 0 && (OS_FREQUENCY)/1000 < (UINT32_MAX)
#define MSEC       (uint32_t)((OS_FREQUENCY)/1000)
#endif
#if (OS_FREQUENCY) < (UINT32_MAX)
#define  SEC       (uint32_t)((OS_FREQUENCY))
#endif
#if (OS_FREQUENCY) < (UINT32_MAX)/60
#define  MIN       (uint32_t)((OS_FREQUENCY)*60)
#endif
#if (OS_FREQUENCY) < (UINT32_MAX)/3600
#define HOUR       (uint32_t)((OS_FREQUENCY)*3600)
#endif
#if (OS_FREQUENCY) < (UINT32_MAX)/86400
#define  DAY       (uint32_t)((OS_FREQUENCY)*86400)
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
#define IMMEDIATE    0UL        // no waiting
#endif
#ifndef INFINITE
#define INFINITE     UINT32_MAX // infinite waiting
#endif

/* -------------------------------------------------------------------------- */

#define JOINABLE   (tsk_t *)(0)           // task in joinable state
#define DETACHED   (tsk_t *)(UINTPTR_MAX) // task in detached state

/* -------------------------------------------------------------------------- */

#ifdef __cplusplus
}
#endif

#endif//__STATEOSBASE_H
