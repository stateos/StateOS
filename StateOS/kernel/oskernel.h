/******************************************************************************

    @file    StateOS: oskernel.h
    @author  Rajmund Szymanski
    @date    20.11.2016
    @brief   This file defines set of kernel functions for StateOS.

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

#ifndef __STATEOSKERNEL_H
#define __STATEOSKERNEL_H

#include <stdlib.h>
#include <osbase.h>

#ifdef __cplusplus
extern "C" {
#endif

/* -------------------------------------------------------------------------- */

extern tsk_t IDLE;   // idle task, tasks' queue
extern tsk_t MAIN;   // main task
extern tmr_t WAIT;   // timers' queue
extern sys_t System; // system data

/* -------------------------------------------------------------------------- */

// initiating and running the system timer
// the port_sys_init procedure is normally called as a constructor
void port_sys_init( void ) __CONSTRUCTOR;

/* -------------------------------------------------------------------------- */

// save status of current process and force yield system control to the next
void core_ctx_switch( void );

// reset context switch indicator
static inline
void core_ctx_reset( void )
{
#if OS_ROBIN && OS_TIMER == 0
	System.dly = 0;
#endif
	port_ctx_reset();
}

/* -------------------------------------------------------------------------- */

// system malloc procedure
void *core_sys_alloc( size_t size );

/* -------------------------------------------------------------------------- */

// add timer 'tmr' to timers READY queue with id 'id'
// start countdown
void core_tmr_insert( tmr_id tmr, unsigned id );

// remove timer 'tmr' from timers READY queue
void core_tmr_remove( tmr_id tmr );

// timers queue handler procedure
void core_tmr_handler( void );

/* -------------------------------------------------------------------------- */

// abort and reset current process and force yield system control to the next
void     core_tsk_break( void ) __NORETURN;

// add task 'tsk' to tasks READY queue with id ID_READY
// force context switch if priority of task 'tsk' is greater then priority of current task and kernel works in preemptive mode
void     core_tsk_insert( tsk_id tsk );

// remove task 'tsk' from tasks READY queue
void     core_tsk_remove( tsk_id tsk );

// append task 'tsk' to object 'obj' delayed queue
void     core_tsk_append( tsk_id tsk, void *obj );

// remove task 'tsk' from object 'obj' delayed queue
// with 'event' event value
void     core_tsk_unlink( tsk_id tsk, unsigned event );

// delay execution of current task until given timepoint 'time'
// append current task to object 'obj' delayed queue
// remove current task from tasks READY queue
// add current task to timers READY queue
// force context switch
// return event value
unsigned core_tsk_waitUntil( void *obj, unsigned time );

// delay execution of current task for given duration of time 'delay'
// append current task to object 'obj' delayed queue
// remove current task from tasks READY queue
// add current task to timers READY queue
// force context switch
// return event value
unsigned core_tsk_waitFor( void *obj, unsigned delay );

// resume execution of delayed task 'tsk' with 'event' event value
// remove task 'tsk' from guard object delayed queue
// remove task 'tsk' from timers READY queue
// add task 'tsk' to tasks READY queue
// force context switch if priority of task 'tsk' is greater then priority of current task and kernel works in preemptive mode
// return 'tsk'
tsk_id   core_tsk_wakeup( tsk_id tsk, unsigned event );

// resume execution of first task from object 'obj' delayed queue with 'event' event value
// remove first task from object 'obj' delayed queue
// remove resumed task from timers READY queue
// add resumed task to tasks READY queue
// force context switch if priority of resumed task is greater then priority of current task and kernel works in preemptive mode
// return pointer to resumed task
tsk_id   core_one_wakeup( void *obj, unsigned event );

// resume execution of all tasks from object 'obj' delayed queue with 'event' event value
// remove all tasks from object 'obj' delayed queue
// remove all resumed tasks from timers READY queue
// add all resumed tasks to tasks READY queue
// force context switch if priority of any resumed task is greater then priority of current task and kernel works in preemptive mode
void     core_all_wakeup( void *obj, unsigned event );

// set task 'tsk' priority
// force context switch if new priority of task 'tsk' is greater then priority of current task and kernel works in preemptive mode
void     core_tsk_prio( tsk_id tsk, unsigned prio );

// tasks queue handler procedure
// save stack pointer 'sp' of the current task
// reset context switch timer counter
// return a pointer to the stack pointer of the next READY task the highest priority
void *   core_tsk_handler( void *sp );

/* -------------------------------------------------------------------------- */

// procedure inside ISR?
static inline
unsigned port_isr_inside( void )
{
	return __get_IPSR();
}

/* -------------------------------------------------------------------------- */

#ifdef __cplusplus
}
#endif

/* -------------------------------------------------------------------------- */

#ifdef __cplusplus

template <class T>
class EventGuard
{
public:
	// an event can be safely destroyed if there are no tasks in the DELAYED queue
	~EventGuard( void )
	{
		auto obj = reinterpret_cast<volatile T *>(this);
		while (obj->queue != nullptr);
	}
};

#endif

/* -------------------------------------------------------------------------- */

#ifdef __cplusplus

template <class T>
class MutexGuard
{
public:
	// a mutex (Mutex, FastMutex) can be safely destroyed if it has no owner
	~MutexGuard( void )
	{
		auto obj = reinterpret_cast<volatile T *>(this);
		while (obj->owner != nullptr);
	}
};

#endif

/* -------------------------------------------------------------------------- */

#ifdef __cplusplus

template <class T>
class ObjectGuard
{
public:
	// an object (Timer, Task) can be safely destroyed if it is stopped
	~ObjectGuard( void )
	{
		auto obj = reinterpret_cast<volatile T *>(this);
		while (obj->id != ID_STOPPED);
	}
};

#endif

/* -------------------------------------------------------------------------- */

#endif//__STATEOSKERNEL_H
