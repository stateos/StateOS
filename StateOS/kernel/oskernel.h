/******************************************************************************

    @file    StateOS: oskernel.h
    @author  Rajmund Szymanski
    @date    16.07.2018
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

#ifndef DEBUG
#ifndef NDEBUG
#define NDEBUG
#endif//NDEBUG
#endif//DEBUG

#include <assert.h>

/* -------------------------------------------------------------------------- */

#ifdef  __cplusplus

#if OS_FUNCTIONAL
#include <functional>
typedef std::function<void( void )> FUN_t;
#else
typedef     void (* FUN_t)( void );
#endif

#endif

/* -------------------------------------------------------------------------- */

#define ALIGNED( size, type ) \
     (((size_t)( size )+sizeof(type)-1)&~(sizeof(type)-1))

#define LIMITED( size, type ) \
     (((size_t)( size ))&~(sizeof(type)-1))

#define ALIGNED_SIZE( size, type ) \
          (((size_t)( size )+sizeof(type)-1)/sizeof(type))

#define LIMITED_SIZE( size, type ) \
          (((size_t)( size ))/sizeof(type))

#define ABOVE( size ) \
      ALIGNED( size, stk_t )

#define SSIZE( size ) \
 ALIGNED_SIZE( size, stk_t )

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

#define core_stk_assert() \
        assert((System.cur == &MAIN) || (System.cur->stack <= port_get_sp()))

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

// system malloc procedure with clearing the allocated memory
void *core_sys_alloc( size_t size );

// system free procedure
void core_sys_free( void *ptr );

/* -------------------------------------------------------------------------- */

// insert timer 'tmr' into timers READY queue with id 'id' and start it
void core_tmr_insert( tmr_t *tmr, unsigned id );

// remove timer 'tmr' from timers READY queue
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

// append task 'tsk' to the delayed queue of object 'obj'
void core_tsk_append( tsk_t *tsk, void *obj );

// remove task 'tsk' from the delayed queue of object 'obj' with event value 'event'
void core_tsk_unlink( tsk_t *tsk, unsigned event );

// transfer task 'tsk' to the delayed queue of object 'obj'
void core_tsk_transfer( tsk_t *tsk, void *obj );

// delay execution of the current task until given time point 'time'
// append the current task to the delayed queue of object 'obj'
// remove the current task from tasks READY queue
// insert the current task into timers READY queue
// force context switch
// return event value
unsigned core_tsk_waitUntil( void *obj, cnt_t time );

// delay execution of current task for given duration of time 'delay'
// append the current task to the delayed queue of object 'obj'
// remove the current task from tasks READY queue
// insert the current task into timers READY queue
// force context switch
// return event value
unsigned core_tsk_waitFor( void *obj, cnt_t delay );

// delay indefinitely execution of given task
// append given task to WAIT timer delayed queue
// remove given task from tasks READY queue
// insert given task into timers READY queue
// force context switch if it is the current task
void core_tsk_suspend( tsk_t *tsk );

// resume execution of delayed task 'tsk' with event value 'event'
// remove task 'tsk' from guard object delayed queue
// remove task 'tsk' from timers READY queue
// insert task 'tsk' into tasks READY queue
// force context switch if priority of task 'tsk' is greater then priority of the current task and kernel works in preemptive mode
// return 'tsk'
tsk_t *core_tsk_wakeup( tsk_t *tsk, unsigned event );

// resume execution of first task from object 'obj' delayed queue with event value 'event'
// remove first task from object 'obj' delayed queue
// remove resumed task from timers READY queue
// insert resumed task into tasks READY queue
// force context switch if priority of resumed task is greater then priority of the current task and kernel works in preemptive mode
// return pointer to resumed task
tsk_t *core_one_wakeup( void *obj, unsigned event );

// resume execution of all tasks from object 'obj' delayed queue with event value 'event'
// remove all tasks from object 'obj' delayed queue
// remove all resumed tasks from timers READY queue
// insert all resumed tasks into tasks READY queue
// force context switch if priority of any resumed task is greater then priority of the current task and kernel works in preemptive mode
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

/* -------------------------------------------------------------------------- */

#ifdef __cplusplus
}
#endif

#endif//__STATEOSKERNEL_H
