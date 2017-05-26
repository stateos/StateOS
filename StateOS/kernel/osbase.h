/******************************************************************************

    @file    StateOS: osbase.h
    @author  Rajmund Szymanski
    @date    29.04.2017
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

#define USEC       (unsigned)((OS_FREQUENCY)/1000000.0+0.5)
#define MSEC       (unsigned)((OS_FREQUENCY)/   1000.0+0.5)
#define  SEC       (unsigned)((OS_FREQUENCY)*      1.0+0.5)
#define  MIN       (unsigned)((OS_FREQUENCY)*     60.0+0.5)
#define HOUR       (unsigned)((OS_FREQUENCY)*   3600.0+0.5)

/* -------------------------------------------------------------------------- */

#define ID_STOPPED ( 0U) // task or timer stopped
#define ID_READY   ( 1U) // task ready to run
#define ID_DELAYED ( 2U) // task waiting or suspended
#define ID_TIMER   ( 3U) // timer in the countdown state
#define ID_IDLE    ( 4U) // idle process

/* -------------------------------------------------------------------------- */

#define E_SUCCESS  ( 0U) // process released by taking the supervising object
#define E_STOPPED  (~0U) // process released by killing the supervising object
#define E_TIMEOUT  (~1U) // process released by the end of the timer countdown

/* -------------------------------------------------------------------------- */

#ifndef IMMEDIATE
#define IMMEDIATE  ( 0U) // no waiting
#endif
#ifndef INFINITE
#define INFINITE   (~0U) // infinite waiting
#endif

/* -------------------------------------------------------------------------- */

#define JOINABLE   ((void*) 0) // task in joinable state
#define DETACHED   ((void*)-1) // task in detached state

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
	unsigned id;    // object id: ID_STOPPED, ID_READY, ID_DELAYED, ID_TIMER, ID_IDLE
	void   * prev;  // previous object (timer, task) in the READY queue
	void   * next;  // next object (timer, task) in the READY queue
};

/* -------------------------------------------------------------------------- */

// system data

typedef struct __sys sys_t;

struct __sys
{
	tsk_t  * cur;   // pointer to the current task control block
#if OS_TIMER == 0
	volatile
	unsigned cnt;   // system timer counter
#if OS_ROBIN
	unsigned dly;   // task switch counter
#endif
#endif
};

/* -------------------------------------------------------------------------- */

#ifdef __cplusplus
}
#endif

#endif//__STATEOSBASE_H
