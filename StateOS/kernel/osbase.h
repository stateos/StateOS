/******************************************************************************

    @file    StateOS: osbase.h
    @author  Rajmund Szymanski
    @date    28.10.2016
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

#define ASIZE( size ) \
 (((unsigned)( size )+(sizeof(stk_t)-1))/sizeof(stk_t))

/* -------------------------------------------------------------------------- */

#define UMIN( a, b ) \
 ((unsigned)( a )<(unsigned)( b )?(unsigned)( a ):(unsigned)( b ))

/* -------------------------------------------------------------------------- */

typedef void   *os_id;    // os pointer
typedef void (*fun_id)(); // task/timer procedure

/* -------------------------------------------------------------------------- */

typedef struct __mtx mtx_t, *mtx_id; // mutex
typedef struct __tmr tmr_t, *tmr_id; // timer
typedef struct __tsk tsk_t, *tsk_id; // task

/* -------------------------------------------------------------------------- */

// object (timer, task) header

typedef struct __obj
{
	tsk_id   queue; // next process in the DELAYED queue
	unsigned id;    // object id: ID_STOPPED, ID_READY, ID_DELAYED, ID_TIMER, ID_IDLE
	void    *prev;  // previous object (timer, task) in the READY queue
	void    *next;  // next object (timer, task) in the READY queue

}	obj_t, *obj_id;


/* -------------------------------------------------------------------------- */

// system data

typedef struct __sys
{
	tsk_id   cur;   // pointer to the current task control block
#if OS_TIMER == 0
	volatile
	unsigned cnt;   // system timer counter
#if OS_ROBIN
	unsigned dly;   // task switch counter
#endif
#endif
}	sys_t, *sys_id;

/* -------------------------------------------------------------------------- */

// task context

typedef struct __sft // context saved by the software
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

}	sft_t, *sft_id;

typedef struct __ctx // context saved by the hardware
{
	unsigned r0;
	unsigned r1;
	unsigned r2;
	unsigned r3;
	void    *ip;
	void    *lr;
	void    *pc;
	unsigned psr;

}	ctx_t, *ctx_id;

/* -------------------------------------------------------------------------- */

#ifdef __cplusplus
}
#endif

/* -------------------------------------------------------------------------- */

#endif//__STATEOSBASE_H
