/******************************************************************************

    @file    StateOS: oskernel.h
    @author  Rajmund Szymanski
    @date    18.11.2019
    @brief   This file defines set of kernel functions for StateOS.

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

#ifndef __STATEOSKERNEL_H
#define __STATEOSKERNEL_H

#include <string.h>
#include <stdlib.h>
#include "oscore.h"

/* -------------------------------------------------------------------------- */

#ifdef  __cplusplus

#if OS_FUNCTIONAL
#include <functional>
typedef std::function<void( void )>     FUN_t;
static_assert(sizeof(FUN_t) == sizeof(void*)*(OS_FUNCTIONAL), "incorrect value of OS_FUNCTIONAL constant!");
typedef std::function<void( unsigned )> ACT_t;
static_assert(sizeof(ACT_t) == sizeof(void*)*(OS_FUNCTIONAL), "incorrect value of OS_FUNCTIONAL constant!");
#else
typedef     void (* FUN_t)( void );
typedef     void (* ACT_t)( unsigned );
#endif

#else

#if OS_FUNCTIONAL
typedef     void  * FUN_t [ OS_FUNCTIONAL ];
typedef     void  * ACT_t [ OS_FUNCTIONAL ];
#endif

#endif

/* -------------------------------------------------------------------------- */

#define ALIGNED_SIZE( size, type ) \
          (((size_t)( size ) + sizeof(type) - 1) / sizeof(type))

#define LIMITED_SIZE( size, type ) \
          (((size_t)( size )) / sizeof(type))

#define      ALIGNED( size, type ) \
       (ALIGNED_SIZE( size, type ) * sizeof(type))

#define      LIMITED( size, type ) \
       (LIMITED_SIZE( size, type ) * sizeof(type))

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

#define assert_ctx_integrity(tsk) \
        assert(((tsk) == &MAIN) || ((uintptr_t)(tsk)->stack < (uintptr_t)(tsk)->sp))

#define assert_stk_integrity() \
        assert((System.cur == &MAIN) || ((uintptr_t)System.cur->stack < (uintptr_t)port_get_sp()))

/* -------------------------------------------------------------------------- */

#define assert_tsk_context() \
        assert(port_isr_context() == false)

/* -------------------------------------------------------------------------- */

// initiate and run the system timer
// the port_sys_init procedure is normally called as a constructor
__CONSTRUCTOR
void port_sys_init( void );

/* -------------------------------------------------------------------------- */

// initiate task 'tsk' for context switch
void core_ctx_init( tsk_t *tsk );

// save status of the current process and force yield system control to the next
void core_ctx_switch( void );

// save status of the current process and immediately yield system control to the next
__STATIC_INLINE
void core_ctx_switchNow( void )
{
	core_ctx_switch();
	port_clr_lock(); __ISB();
}

// system infinite loop procedure for the current process
__NO_RETURN
void core_tsk_loop( void );

// reset context switch indicator
__STATIC_INLINE
void core_ctx_reset( void )
{
	port_ctx_reset();
}

/* -------------------------------------------------------------------------- */

// insert task / timer 'tmr' into timers READY queue
void core_tmr_insert( tmr_t *tmr );

// remove task / timer 'tmr' from timers READY queue
void core_tmr_remove( tmr_t *tmr );

// timers queue handler procedure
void core_tmr_handler( void );

/* -------------------------------------------------------------------------- */

// reset stack and restart the current task
__NO_RETURN
void core_tsk_flip( void *sp );

// insert task 'tsk' into tasks READY queue with id ID_READY
// force context switch if priority of task 'tsk' is greater then priority of the current task and kernel works in preemptive mode
void core_tsk_insert( tsk_t *tsk );

// remove task 'tsk' from tasks READY queue
void core_tsk_remove( tsk_t *tsk );

// append task 'tsk' to the blocked queue 'que'
void core_tsk_append( tsk_t *tsk, tsk_t **obj );

// remove task 'tsk' from the blocked queue with event value 'event'
void core_tsk_unlink( tsk_t *tsk, unsigned event );

// transfer task 'tsk' to the blocked queue 'que'
void core_tsk_transfer( tsk_t *tsk, tsk_t **que );

// delay execution of current task for given duration of time 'delay'
// append the current task to the blocked queue 'que'
// remove the current task from tasks READY queue
// insert the current task into timers READY queue
// force context switch
// return event value
unsigned core_tsk_waitFor( tsk_t **que, cnt_t delay );

// delay execution of given task 'tsk'
// append the current task to the blocked queue 'que'
// remove the current task from tasks READY queue
// insert the current task into timers READY queue
// context switch depends on the 'yield' value
// return event value
unsigned core_tsk_wait( tsk_t *tsk, tsk_t **que, bool yield );

// delay execution of current task for given duration of time 'delay' from the end of the previous countdown
// append the current task to the blocked queue 'que'
// remove the current task from tasks READY queue
// insert the current task into timers READY queue
// force context switch
// return event value
unsigned core_tsk_waitNext( tsk_t **que, cnt_t delay );

// delay execution of the current task until given time point 'time'
// append the current task to the blocked queue 'que'
// remove the current task from tasks READY queue
// insert the current task into timers READY queue
// force context switch
// return event value
unsigned core_tsk_waitUntil( tsk_t **que, cnt_t time );

// delay indefinitely execution of given task
// append given task to System.dly queue
// remove given task from tasks READY queue
// insert given task into timers READY queue
// force context switch if it is the current task
void core_tsk_suspend( tsk_t *tsk );

// resume execution of blocked task 'tsk' with event value 'event'
// remove resumed task from guard object blocked queue
// remove resumed task from timers READY queue
// insert resumed task into tasks READY queue
// force context switch if priority of resumed task is greater then priority of the current task and kernel works in preemptive mode
// return 'tsk'
tsk_t *core_tsk_wakeup( tsk_t *tsk, unsigned event );

// resume execution of first task from blocked queue with event value 'event'; 'tsk' is the head (first task) of the queue
// remove resumed task from guard object blocked queue
// remove resumed task from timers READY queue
// insert resumed task into tasks READY queue
// force context switch if priority of resumed task is greater then priority of the current task and kernel works in preemptive mode
// return 'tsk'
__STATIC_INLINE
tsk_t *core_one_wakeup( tsk_t *tsk, unsigned event )
{
	return core_tsk_wakeup(tsk, event);
}

// resume execution of all tasks from blocked queue with event value 'event'; 'tsk' is the head (first task) of the queue
// remove all resumed tasks from guard object blocked queue
// remove all resumed tasks from timers READY queue
// insert all resumed tasks into tasks READY queue
// force context switch if priority of any resumed task is greater then priority of the current task and kernel works in preemptive mode
void core_all_wakeup( tsk_t *tsk, unsigned event );

// return count of tasks blocked on the queue; 'tsk' is the head (first task) of the queue
unsigned core_tsk_count( tsk_t *tsk );

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

// set the task 'tsk' as the owner of the mutex 'mtx'
void core_mtx_link( mtx_t *mtx, tsk_t *tsk );

// remove owner of the mutex 'mtx'
void core_mtx_unlink( mtx_t *mtx );

// transfer lock to the next task in the blocked queue of mutex 'mtx'
// the task is waked with event 'event'
// return pointer to the waked task or 0 if the blocked queue of 'mtx' is empty
tsk_t *core_mtx_transferLock( mtx_t *mtx, unsigned event );

// reset mutex 'mtx' and release all blocked tasks with event 'event'
void core_mtx_reset( mtx_t *mtx, unsigned event );

/* -------------------------------------------------------------------------- */

// return current system time in tick-less mode
#if HW_TIMER_SIZE < OS_TIMER_SIZE // because of CSMCC
cnt_t port_sys_time( void );
#endif

// return current system time
__STATIC_INLINE
cnt_t core_sys_time( void )
{
#if HW_TIMER_SIZE == 0
	return System.cnt;
#else
	return port_sys_time();
#endif
}

// internal handler of system timer
#if HW_TIMER_SIZE == 0
void core_sys_tick( void );
#else
__STATIC_INLINE
void core_sys_tick( void )
{
#if HW_TIMER_SIZE < OS_TIMER_SIZE
	System.cnt += (cnt_t)(1) << (HW_TIMER_SIZE);
#endif
}
#endif

// default handler of idle process
void idle_tsk_default( void );

// destructor handler of idle process
void idle_tsk_destructor( void );

/* -------------------------------------------------------------------------- */

#ifdef __cplusplus
}
#endif

#endif//__STATEOSKERNEL_H
