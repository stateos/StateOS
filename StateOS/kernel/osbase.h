/******************************************************************************

    @file    StateOS: osbase.h
    @author  Rajmund Szymanski
    @date    17.03.2016
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

#ifdef __CC_ARM
#pragma push
#pragma anon_unions
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
        __attribute__((aligned(8)))

/* -------------------------------------------------------------------------- */

#define ASIZE( size ) \
 (((unsigned)( size )+7U)&~7U)

/* -------------------------------------------------------------------------- */

#define UMIN( a, b ) \
 ((unsigned)( a )<(unsigned)( b )?(unsigned)( a ):(unsigned)( b ))

/* -------------------------------------------------------------------------- */

typedef void                 *os_id;
typedef void               (*fun_id)();
typedef struct __sig sig_t, *sig_id; // signal
typedef struct __evt evt_t, *evt_id; // event
typedef struct __flg flg_t, *flg_id; // flag
typedef struct __bar bar_t, *bar_id; // barrier
typedef struct __sem sem_t, *sem_id; // semaphore
typedef struct __mtx mtx_t, *mtx_id; // mutex
typedef struct __mut mut_t, *mut_id; // fast mutex
typedef struct __cnd cnd_t, *cnd_id; // condition variable
typedef struct __box box_t, *box_id; // mailbox queue
typedef struct __msg msg_t, *msg_id; // message queue
typedef struct __obj obj_t, *obj_id;
typedef struct __tmr tmr_t, *tmr_id; // timer
typedef struct __tsk tsk_t, *tsk_id; // task
typedef struct __sys sys_t, *sys_id; // system data
typedef struct __sft sft_t, *sft_id; // task context saved by the software
typedef struct __ctx ctx_t, *ctx_id; // task context saved by the hardware

/* -------------------------------------------------------------------------- */

// signal

struct __sig
{
	tsk_id   queue; // next process in the DELAYED queue
	unsigned flag;  // signal's current value
	unsigned type;  // signal type: sigClear, sigProtect
};

#define _SIG_INIT( _type ) { /*queue*/0, /*flag*/0, /*type*/_type }

/* -------------------------------------------------------------------------- */

// event

struct __evt
{
	tsk_id   queue; // next process in the DELAYED queue
};

#define _EVT_INIT() { 0 }

/* -------------------------------------------------------------------------- */

// flag

struct __flg
{
	tsk_id   queue; // next process in the DELAYED queue
	unsigned flags; // flag's current value
};

#define _FLG_INIT() { 0 }

/* -------------------------------------------------------------------------- */

// barrier

struct __bar
{
	tsk_id   queue; // next process in the DELAYED queue
	unsigned count; // barrier's current value
	unsigned limit; // barrier's value limit
};

#define _BAR_INIT( _limit ) { /*queue*/0, /*count*/_limit, /*limit*/_limit }

/* -------------------------------------------------------------------------- */

// semaphore

struct __sem
{
	tsk_id   queue; // next process in the DELAYED queue
	unsigned count; // semaphore's current value
	unsigned limit; // semaphore's value limit
};

#define _SEM_INIT( _count, _limit ) { /*queue*/0, /*count*/UMIN(_count,_limit), /*limit*/_limit }

/* -------------------------------------------------------------------------- */

// mutex

struct __mtx
{
	tsk_id   queue; // next process in the DELAYED queue
	tsk_id   owner; // owner task
	unsigned count; // mutex's curent value
	mtx_id   list;  // list of mutexes held by owner
};

#define _MTX_INIT() { 0 }

/* -------------------------------------------------------------------------- */

// fast mutex

struct __mut
{
	tsk_id   queue; // next process in the DELAYED queue
	tsk_id   owner; // owner task
};

#define _MUT_INIT() { 0 }

/* -------------------------------------------------------------------------- */

// condition variable

struct __cnd
{
	tsk_id   queue; // next process in the DELAYED queue
};

#define _CND_INIT() { 0 }

/* -------------------------------------------------------------------------- */

// mailbox queue

struct __box
{
	tsk_id   queue; // inherited from semaphore
	unsigned count; // inherited from semaphore
	unsigned limit; // inherited from semaphore

	unsigned first; // first element to read from queue
	unsigned next;  // next element to write into queue
	char    *data;  // queue data
	unsigned size;  // size of a single mail (in bytes)
};

#define _BOX_INIT( _limit, _size, _data ) { /*queue*/0, /*count*/0, /*limit*/_limit, /*first*/0, /*next*/0, /*data*/_data, /*size*/_size }

#define _BOX_DATA( _limit, _size ) (char[_limit*_size]){ 0 }

/* -------------------------------------------------------------------------- */

// message queue

struct __msg
{
	tsk_id   queue; // inherited from semaphore
	unsigned count; // inherited from semaphore
	unsigned limit; // inherited from semaphore

	unsigned first; // first element to read from queue
	unsigned next;  // next element to write into queue
	unsigned*data;  // queue data
};

#define _MSG_INIT( _limit, _data ) { /*queue*/0, /*count*/0, /*limit*/_limit, /*first*/0, /*next*/0, /*data*/_data }

#define _MSG_DATA( _limit ) (unsigned[_limit]){ 0 }

/* -------------------------------------------------------------------------- */

// object header (timer, task)

struct __obj
{
	tsk_id   queue; // next process in the DELAYED queue
	unsigned id;    // object id: ID_STOPPED, ID_READY, ID_DELAYED, ID_TIMER, ID_IDLE
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

	fun_id   state; // callback procedure
	unsigned start;
	unsigned delay;
	unsigned period;
};

#define _TMR_INIT( _state ) { /*queue*/0, /*id*/0, /*next*/0, /*prev*/0, /*state*/_state }

/* -------------------------------------------------------------------------- */

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
	unsigned prio;  // current priority

	void    *sp;    // stack pointer
	void    *top;   // top of stack
	tsk_id   back;  // previous process in the DELAYED queue
	unsigned basic; // basic priority

	void    *guard; // object that controls the pending process
	mtx_id   list;  // list of mutexes held
	union  {
	unsigned all;   // used by flag object: wait for all flags to be set
	void    *data;  // used by mailbox queue object
	unsigned msg;   // used by message queue object
	};
	union  {
	unsigned flags; // used by flag object: all flags to wait
	unsigned event; // wakeup event
	};
#if defined(__CC_ARM) && !defined(__MICROLIB)
	char     libspace[96];
#endif
};

#define _TSK_INIT( _prio, _state, _top ) { /*queue*/0,      /*id*/   0,    /*next*/ 0, /*prev*/ 0,   \
                                           /*state*/_state, /*start*/0,    /*delay*/0, /*prio*/ _prio,\
                                           /*sp*/   0,      /*top*/  _top, /*back*/ 0, /*basic*/_prio }

#define _TSK_STACK( _size ) (__osalign char[ASIZE(_size)]){ 0 } + ASIZE(_size)

/* -------------------------------------------------------------------------- */

// system data

struct __sys
{
	tsk_id   cur;   // pointer to the current task control block
	tmr_id   tmr;   // pointer to the current timer control block
#if OS_TIMER == 0
	volatile
	unsigned cnt;   // system timer counter
#if OS_ROBIN
	unsigned dly;   // task switch counter
#endif
#endif
};

/* -------------------------------------------------------------------------- */

// task context

struct __sft // context saved by the software
{
	unsigned r4;
	unsigned r5;
	unsigned r6;
	unsigned r7;
	unsigned r8;
	unsigned r9;
	unsigned r10;
	unsigned r11;
	unsigned lr; // EXC_RETURN
};

struct __ctx // context saved by the hardware
{
	unsigned r0;
	unsigned r1;
	unsigned r2;
	unsigned r3;
	void    *ip;
	void    *lr;
	void    *pc;
	unsigned psr;
};

/* -------------------------------------------------------------------------- */

#ifdef __CC_ARM
#pragma pop
#endif

#ifdef __cplusplus
}
#endif
