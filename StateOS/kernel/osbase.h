/******************************************************************************

    @file    State Machine OS: osbase.h
    @author  Rajmund Szymanski
    @date    14.12.2015
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

#pragma once

#include <stdbool.h>
#include <osport.h>

#ifdef __cplusplus
extern "C" {
#endif

/* -------------------------------------------------------------------------- */

#define USEC       (unsigned)(OS_FREQUENCY)/1000000U
#define MSEC       (unsigned)(OS_FREQUENCY)/1000U
#define  SEC       (unsigned)(OS_FREQUENCY)
#define  MIN       (unsigned)(OS_FREQUENCY)*60U
#define HOUR       (unsigned)(OS_FREQUENCY)*3600U

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

#define IMMEDIATE  ( 0U) // no waiting
#define INFINITE   (~0U) // infinite waiting

/* -------------------------------------------------------------------------- */

#define __osalign \
        __attribute__(( aligned( OS_ALIGN ) ))

/* -------------------------------------------------------------------------- */

#define ASIZE( size ) \
        ((size + OS_ALIGN - 1) & ~(OS_ALIGN - 1))

/* -------------------------------------------------------------------------- */

typedef void                *  os_id;
typedef void               (* fun_id)( void );
typedef struct __evt evt_t, * evt_id; // event
typedef struct __flg flg_t, * flg_id; // flag
typedef struct __bar bar_t, * bar_id; // barrier
typedef struct __sem sem_t, * sem_id; // semaphore
typedef struct __mtx mtx_t, * mtx_id; // mutex
typedef struct __cnd cnd_t, * cnd_id; // condition variable
typedef struct __box box_t, * box_id; // mailbox queue
typedef struct __msg msg_t, * msg_id; // message queue
typedef struct __obj obj_t, * obj_id;
typedef struct __tmr tmr_t, * tmr_id; // timer
typedef struct __tsk tsk_t, * tsk_id; // task
typedef struct __sys sys_t, * sys_id; // system data
typedef struct __ctx ctx_t, * ctx_id; // task context

/* -------------------------------------------------------------------------- */

// event

struct __evt
{
	tsk_id   queue; // next process in the DELAYED queue
};

#define _EVT_INIT( ) { 0 }

/* -------------------------------------------------------------------------- */

// flag

struct __flg
{
	tsk_id   queue; // next process in the DELAYED queue
	unsigned flags; // flag's current value
	unsigned mask;  // mask for blocked flags
};

#define _FLG_INIT( mask ) { 0, 0, mask }

/* -------------------------------------------------------------------------- */

// barrier

struct __bar
{
	tsk_id   queue; // next process in the DELAYED queue
	unsigned count; // barrier's current value
	unsigned limit; // barrier's value limit
};

#define _BAR_INIT( limit ) { 0, limit, limit }

/* -------------------------------------------------------------------------- */

// semaphore

struct __sem
{
	tsk_id   queue; // next process in the DELAYED queue
	unsigned count; // semaphore's current value
	unsigned limit; // semaphore's value limit
};

#define _SEM_INIT( count, limit ) { 0, count, limit }

/* -------------------------------------------------------------------------- */

// mutex

struct __mtx
{
	tsk_id   queue; // next process in the DELAYED queue
	unsigned count;
	unsigned type;  // mutex type: mtxNormal, mtxRecursive, mtxPriorityProtect, mtxPriorityInheritance
	tsk_id   owner; // owner task
	mtx_id   mlist; // list of mutexes held by owner
};

#define _MTX_INIT( type ) { 0, 0, type }

/* -------------------------------------------------------------------------- */

// condition variable

struct __cnd
{
	tsk_id   queue;
};

#define _CND_INIT( ) { 0 }

/* -------------------------------------------------------------------------- */

// mailbox queue

struct __box
{
	tsk_id   queue; // inherited from semaphore
	unsigned count; // inherited from semaphore
	unsigned limit; // inherited from semaphore

	unsigned size;
	char   * data;
	unsigned first;
	unsigned next;
};

#define _BOX_INIT( limit, size, data ) { 0, 0, limit, size, data }

/* -------------------------------------------------------------------------- */

// message queue

struct __msg
{
	tsk_id   queue; // inherited from semaphore
	unsigned count; // inherited from semaphore
	unsigned limit; // inherited from semaphore

	unsigned * data;
	unsigned first;
	unsigned next;
};

#define _MSG_INIT( limit, data ) { 0, 0, limit, data }

/* -------------------------------------------------------------------------- */

// object header (timer, task)

struct __obj
{
	tsk_id   queue; // next process in the DELAYED queue
	unsigned id;    // ID_STOPPED, ID_READY, ID_DELAYED, ID_TIMER
	obj_id   next;  // next object in the READY queue
	obj_id   prev;  // previous object in the READY queue
};

/* -------------------------------------------------------------------------- */

// timer

struct __tmr
{
	tsk_id   queue; // inherited from object
	unsigned id;    // inherited from object
	tmr_id   next;  // inherited from object
	tmr_id   prev;  // inherited from object

	fun_id   state;
	unsigned start;
	unsigned delay;
	unsigned period;
};

#define _TMR_INIT( ) { 0 }

/* -------------------------------------------------------------------------- */

#ifdef __CC_ARM
#pragma push
#pragma anon_unions
#endif

// task

struct __tsk
{
	tsk_id   queue; // inherited from timer
	unsigned id;    // inherited from timer
	tsk_id   next;  // inherited from timer
	tsk_id   prev;  // inherited from timer

	fun_id   state; // inherited from timer
	unsigned start; // inherited from timer
	unsigned delay; // inherited from timer
	unsigned prio;

	 os_id   sp;    // stack pointer
	 os_id   top;   // top of stack
	tsk_id   back;  // previous process in the DELAYED queue
	obj_id   guard; // object that controls the pending process
	mtx_id   mlist; // list of mutexes held

	unsigned bprio; // basic priority
	union
	{
	unsigned flags;
	unsigned msg;
	unsigned*data;
	};
	union
	{
	unsigned event; // wakeup event
	unsigned mode;
	};
#if defined(__CC_ARM) && !defined(__MICROLIB)
	char     libspace[96];
#endif
};

#define _TSK_INIT( prio, state, top ) { 0, 0, 0, 0, state, 0, 0, prio, 0, top }

#ifdef __CC_ARM
#pragma pop
#endif

/* -------------------------------------------------------------------------- */

// system data

struct __sys
{
	tsk_id   cur; // pointer to the current task control block
	tmr_id   tmr; // pointer to the current timer control block
#if OS_TIMER == 0
	volatile
	unsigned cnt; // system timer counter
#if OS_ROBIN
	unsigned dly; // task switch counter
#endif
#endif
};

/* -------------------------------------------------------------------------- */

// task context

struct __ctx
{
// context saved by the software
	unsigned r4;
	unsigned r5;
	unsigned r6;
	unsigned r7;
	unsigned r8;
	unsigned r9;
	unsigned r10;
	unsigned r11;
	unsigned exc_return;
// context saved by the hardware
	unsigned r0;
	unsigned r1;
	unsigned r2;
	unsigned r3;
	os_id    ip;
	os_id    lr;
	os_id    pc;
	unsigned psr;
};

/* -------------------------------------------------------------------------- */

#ifdef __cplusplus
}
#endif
