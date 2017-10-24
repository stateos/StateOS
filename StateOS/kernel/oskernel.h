/******************************************************************************

    @file    StateOS: oskernel.h
    @author  Rajmund Szymanski
    @date    24.10.2017
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

#include <string.h>
#include <stdlib.h>
#include <oscore.h>

/* -------------------------------------------------------------------------- */

#ifndef OS_ASSERT
#define OS_ASSERT             0 /* do not include standard assertions         */
#endif

#if     OS_ASSERT == 0
#ifndef NDEBUG
#define NDEBUG
#endif
#endif

#ifndef NDEBUG
#define __ASSERT_MSG
#endif

#include <assert.h>

/* -------------------------------------------------------------------------- */

#ifdef __cplusplus

#if OS_FUNCTIONAL
#include <functional>
typedef std::function<void( void )> FUN_t;
#else
typedef     void (* FUN_t)( void );
#endif

#endif

/* -------------------------------------------------------------------------- */

#define ASIZE( size ) \
   (((size_t)( size )+sizeof(stk_t)-1)/ (sizeof(stk_t)  ))

#define ABOVE( size ) \
   (((size_t)( size )+sizeof(stk_t)-1)&~(sizeof(stk_t)-1))

#define BELOW( size ) \
   (((size_t)( size )                )&~(sizeof(stk_t)-1))

/* -------------------------------------------------------------------------- */

#ifdef __cplusplus
extern "C" {
#endif

/* -------------------------------------------------------------------------- */

extern tsk_t MAIN;   // main task
extern tsk_t IDLE;   // idle task, tasks' queue
extern tmr_t WAIT;   // timers' queue
extern sys_t System; // system data

/* -------------------------------------------------------------------------- */

#ifndef Counter
#define Counter System.cnt
#endif
#define Current System.cur

/* -------------------------------------------------------------------------- */

#define core_stk_assert() \
        assert((Current == &MAIN) || (port_get_sp() >= Current->stack))

/* -------------------------------------------------------------------------- */

// initiating and running the system timer
// the port_sys_init procedure is normally called as a constructor
__CONSTRUCTOR
void port_sys_init( void );

/* -------------------------------------------------------------------------- */

// init task 'tsk' for context switch
void core_ctx_init( tsk_t *tsk );

// save status of current process and force yield system control to the next
void core_ctx_switch( void );

// system infinite loop for current process
__NO_RETURN
void core_tsk_loop( void );

// reset context switch indicator
__STATIC_INLINE
void core_ctx_reset( void )
{
	port_ctx_reset();
}

/* -------------------------------------------------------------------------- */

// system malloc procedure
void *core_sys_alloc( size_t size );

// system free procedure
void core_sys_free( void *ptr );

/* -------------------------------------------------------------------------- */

// add timer 'tmr' to timers READY queue with id 'id'
// start countdown
void core_tmr_insert( tmr_t *tmr, unsigned id );

// remove timer 'tmr' from timers READY queue
void core_tmr_remove( tmr_t *tmr );

// timers queue handler procedure
void core_tmr_handler( void );

/* -------------------------------------------------------------------------- */

// reset stack and restart current task
__NO_RETURN
void core_tsk_flip( void *sp );

// add task 'tsk' to tasks READY queue with id ID_READY
// force context switch if priority of task 'tsk' is greater then priority of current task and kernel works in preemptive mode
void core_tsk_insert( tsk_t *tsk );

// remove task 'tsk' from tasks READY queue
void core_tsk_remove( tsk_t *tsk );

// append task 'tsk' to object 'obj' delayed queue
void core_tsk_append( tsk_t *tsk, void *obj );

// remove task 'tsk' from object 'obj' delayed queue
// with 'event' event value
void core_tsk_unlink( tsk_t *tsk, unsigned event );

// delay execution of current task until given timepoint 'time'
// append current task to object 'obj' delayed queue
// remove current task from tasks READY queue
// add current task to timers READY queue
// force context switch
// return event value
unsigned core_tsk_waitUntil( void *obj, uint32_t time );

// delay execution of current task for given duration of time 'delay'
// append current task to object 'obj' delayed queue
// remove current task from tasks READY queue
// add current task to timers READY queue
// force context switch
// return event value
unsigned core_tsk_waitFor( void *obj, uint32_t delay );

// delay indefinitly execution of given task
// append given task to WAIT timer delayed queue
// remove given task from tasks READY queue
// add given task to timers READY queue
// force context switch if it is current task
void core_tsk_suspend( tsk_t *tsk );

// resume execution of delayed task 'tsk' with 'event' event value
// remove task 'tsk' from guard object delayed queue
// remove task 'tsk' from timers READY queue
// add task 'tsk' to tasks READY queue
// force context switch if priority of task 'tsk' is greater then priority of current task and kernel works in preemptive mode
// return 'tsk'
tsk_t *core_tsk_wakeup( tsk_t *tsk, unsigned event );

// resume execution of first task from object 'obj' delayed queue with 'event' event value
// remove first task from object 'obj' delayed queue
// remove resumed task from timers READY queue
// add resumed task to tasks READY queue
// force context switch if priority of resumed task is greater then priority of current task and kernel works in preemptive mode
// return pointer to resumed task
tsk_t *core_one_wakeup( void *obj, unsigned event );

// resume execution of all tasks from object 'obj' delayed queue with 'event' event value
// remove all tasks from object 'obj' delayed queue
// remove all resumed tasks from timers READY queue
// add all resumed tasks to tasks READY queue
// force context switch if priority of any resumed task is greater then priority of current task and kernel works in preemptive mode
void core_all_wakeup( void *obj, unsigned event );

// set task 'tsk' priority
// force context switch if new priority of task 'tsk' is greater then priority of current task and kernel works in preemptive mode
void core_tsk_prio( tsk_t *tsk, unsigned prio );

// set the current task priority
// force context switch if new priority of the current task is less then priority of next task in ready queue and kernel works in preemptive mode
void core_cur_prio( unsigned prio );

// tasks queue handler procedure
// save stack pointer 'sp' of the current task
// reset context switch timer counter
// return a pointer to the stack pointer of the next READY task the highest priority
void *core_tsk_handler( void *sp );

/* -------------------------------------------------------------------------- */

// internal handler of system timer
void core_sys_tick( void );

/* -------------------------------------------------------------------------- */

#ifdef __cplusplus
}
#endif

#endif//__STATEOSKERNEL_H
