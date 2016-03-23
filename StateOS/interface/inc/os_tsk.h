/******************************************************************************

    @file    StateOS: os_tsk.h
    @author  Rajmund Szymanski
    @date    23.03.2016
    @brief   This file contains definitions for StateOS.

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

#include <oskernel.h>

#ifdef __cplusplus
extern "C" {
#endif

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : task (thread)                                                                                  *
 *                                                                                                                    *
 **********************************************************************************************************************/

#ifdef __CC_ARM
#pragma push
#pragma anon_unions
#endif

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

#ifdef __CC_ARM
#pragma pop
#endif

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : _TSK_INIT                                                                                      *
 *                                                                                                                    *
 * Description       : create and initilize a task object                                                             *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   prio            : initial task priority (any unsigned int value)                                                 *
 *   state           : task state (initial task function) doesn't have to be noreturn-type                            *
 *                     it will be executed into an infinite system-implemented loop                                   *
 *   top             : top of task's private stack storage                                                            *
 *                                                                                                                    *
 * Return            : task object                                                                                    *
 *                                                                                                                    *
 * Note              : for internal use                                                                               *
 *                                                                                                                    *
 **********************************************************************************************************************/

#define               _TSK_INIT( _prio, _state, _top ) { 0, 0, 0, 0, _state, 0, 0, _prio, 0, _top, 0, _prio, 0, 0, { 0 }, { 0 } }

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : _TSK_STACK                                                                                     *
 *                                                                                                                    *
 * Description       : create task's private stack storage                                                            *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   size            : size of task's private stack storage (in bytes)                                                *
 *                                                                                                                    *
 * Return            : top of task's private stack storage                                                            *
 *                                                                                                                    *
 * Note              : for internal use                                                                               *
 *                                                                                                                    *
 **********************************************************************************************************************/

#define               _TSK_STACK( _size ) (__osalign char[ASIZE(_size)]){ 0 } + ASIZE(_size)

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : OS_WRK                                                                                         *
 *                                                                                                                    *
 * Description       : define and initilize complete work area for task object                                        *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   tsk             : name of a pointer to task object                                                               *
 *   prio            : initial task priority (any unsigned int value)                                                 *
 *   state           : task state (initial task function) doesn't have to be noreturn-type                            *
 *                     it will be executed into an infinite system-implemented loop                                   *
 *   size            : size of task private stack (in bytes)                                                          *
 *                                                                                                                    *
 **********************************************************************************************************************/

#define             OS_WRK( tsk, prio, state, size )                                        \
                       char tsk##__stk[ASIZE( size )] __osalign;                             \
                       tsk_t tsk##__tsk = _TSK_INIT( prio, state, tsk##__stk+ASIZE( size ) ); \
                       tsk_id tsk = & tsk##__tsk

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : OS_TSK                                                                                         *
 *                                                                                                                    *
 * Description       : define and initilize complete work area for task obj. with stack size defined by OS_STACK_SIZE *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   tsk             : name of a pointer to task object                                                               *
 *   prio            : initial task priority (any unsigned int value)                                                 *
 *   state           : task state (initial task function) doesn't have to be noreturn-type                            *
 *                     it will be executed into an infinite system-implemented loop                                   *
 *                                                                                                                    *
 **********************************************************************************************************************/

#define             OS_TSK( tsk, prio, state ) \
                    OS_WRK( tsk, prio, state, OS_STACK_SIZE )

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : OS_DEF                                                                                         *
 *                                                                                                                    *
 * Description       : define and initilize complete work area for task obj. with stack size defined by OS_STACK_SIZE *
 *                     task state (function body) must be defined immediately below                                   *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   tsk             : name of a pointer to task object                                                               *
 *   prio            : initial task priority (any unsigned int value)                                                 *
 *                                                                                                                    *
 **********************************************************************************************************************/

#define             OS_DEF( tsk, prio )            \
                       void tsk##__fun( void );     \
                    OS_TSK( tsk, prio, tsk##__fun ); \
                       void tsk##__fun( void )

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : static_WRK                                                                                     *
 *                                                                                                                    *
 * Description       : define and initilize static work area for task object                                          *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   tsk             : name of a pointer to task object                                                               *
 *   prio            : initial task priority (any unsigned int value)                                                 *
 *   state           : task state (initial task function) doesn't have to be noreturn-type                            *
 *                     it will be executed into an infinite system-implemented loop                                   *
 *   size            : size of task private stack (in bytes)                                                          *
 *                                                                                                                    *
 **********************************************************************************************************************/

#define         static_WRK( tsk, prio, state, size )                                        \
                static char tsk##__stk[ASIZE( size )] __osalign;                             \
                static tsk_t tsk##__tsk = _TSK_INIT( prio, state, tsk##__stk+ASIZE( size ) ); \
                static tsk_id tsk = & tsk##__tsk

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : static_TSK                                                                                     *
 *                                                                                                                    *
 * Description       : define and initilize static work area for task object with stack size defined by OS_STACK_SIZE *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   tsk             : name of a pointer to task object                                                               *
 *   prio            : initial task priority (any unsigned int value)                                                 *
 *   state           : task state (initial task function) doesn't have to be noreturn-type                            *
 *                     it will be executed into an infinite system-implemented loop                                   *
 *                                                                                                                    *
 **********************************************************************************************************************/

#define         static_TSK( tsk, prio, state ) \
                static_WRK( tsk, prio, state, OS_STACK_SIZE )

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : static_DEF                                                                                     *
 *                                                                                                                    *
 * Description       : define and initilize static work area for task object with stack size defined by OS_STACK_SIZE *
 *                     task state (function body) must be defined immediately below                                   *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   tsk             : name of a pointer to task object                                                               *
 *   prio            : initial task priority (any unsigned int value)                                                 *
 *                                                                                                                    *
 **********************************************************************************************************************/

#define         static_DEF( tsk, prio )            \
                static void tsk##__fun( void );     \
                static_TSK( tsk, prio, tsk##__fun ); \
                static void tsk##__fun( void )

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : WRK_INIT                                                                                       *
 *                                                                                                                    *
 * Description       : create and initilize complete work area for task object                                        *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   prio            : initial task priority (any unsigned int value)                                                 *
 *   state           : task state (initial task function) doesn't have to be noreturn-type                            *
 *                     it will be executed into an infinite system-implemented loop                                   *
 *   size            : size of task private stack (in bytes)                                                          *
 *                                                                                                                    *
 * Return            : task object                                                                                    *
 *                                                                                                                    *
 * Note              : use only in 'C' code                                                                           *
 *                                                                                                                    *
 **********************************************************************************************************************/

#define                WRK_INIT( prio, state, size ) \
                      _TSK_INIT( prio, state, _TSK_STACK( size ) )

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : WRK_CREATE                                                                                     *
 *                                                                                                                    *
 * Description       : create and initilize complete work area for task object                                        *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   prio            : initial task priority (any unsigned int value)                                                 *
 *   state           : task state (initial task function) doesn't have to be noreturn-type                            *
 *                     it will be executed into an infinite system-implemented loop                                   *
 *   size            : size of task private stack (in bytes)                                                          *
 *                                                                                                                    *
 * Return            : pointer to task object                                                                         *
 *                                                                                                                    *
 * Note              : use only in 'C' code                                                                           *
 *                                                                                                                    *
 **********************************************************************************************************************/

#ifndef __cplusplus
#define                WRK_CREATE( prio, state, size ) \
               &(tsk_t)WRK_INIT( prio, state, size )
#endif

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : TSK_INIT                                                                                       *
 *                                                                                                                    *
 * Description       : create and initilize complete work area for task obj. with stack size defined by OS_STACK_SIZE *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   prio            : initial task priority (any unsigned int value)                                                 *
 *   state           : task state (initial task function) doesn't have to be noreturn-type                            *
 *                     it will be executed into an infinite system-implemented loop                                   *
 *                                                                                                                    *
 * Return            : task object                                                                                    *
 *                                                                                                                    *
 * Note              : use only in 'C' code                                                                           *
 *                                                                                                                    *
 **********************************************************************************************************************/

#define                TSK_INIT( prio, state ) \
                       WRK_INIT( prio, state, OS_STACK_SIZE )

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : TSK_CREATE                                                                                     *
 *                                                                                                                    *
 * Description       : create and initilize complete work area for task obj. with stack size defined by OS_STACK_SIZE *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   prio            : initial task priority (any unsigned int value)                                                 *
 *   state           : task state (initial task function) doesn't have to be noreturn-type                            *
 *                     it will be executed into an infinite system-implemented loop                                   *
 *                                                                                                                    *
 * Return            : pointer to task object                                                                         *
 *                                                                                                                    *
 * Note              : use only in 'C' code                                                                           *
 *                                                                                                                    *
 **********************************************************************************************************************/

#ifndef __cplusplus
#define                TSK_CREATE( prio, state ) \
                       WRK_CREATE( prio, state, OS_STACK_SIZE )
#endif

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : tsk_create                                                                                     *
 *                                                                                                                    *
 * Description       : create and initilize complete work area for task object                                        *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   prio            : initial task priority (any unsigned int value)                                                 *
 *   state           : task state (initial task function) doesn't have to be noreturn-type                            *
 *                     it will be executed into an infinite system-implemented loop                                   *
 *   size            : size of task private stack (in bytes)                                                          *
 *                                                                                                                    *
 * Return            : pointer to task object (task successfully created)                                             *
 *   0               : task not created (not enough free memory)                                                      *
 *                                                                                                                    *
 * Note              : use only in thread mode                                                                        *
 *                                                                                                                    *
 **********************************************************************************************************************/

              tsk_id   tsk_create( unsigned prio, fun_id state, unsigned size );

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : tsk_new                                                                                        *
 *                                                                                                                    *
 * Description       : create and initilize complete work area for task obj. with stack size defined by OS_STACK_SIZE *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   prio            : initial task priority (any unsigned int value)                                                 *
 *   state           : task state (initial task function) doesn't have to be noreturn-type                            *
 *                     it will be executed into an infinite system-implemented loop                                   *
 *                                                                                                                    *
 * Return            : pointer to task object (task successfully created)                                             *
 *   0               : task not created (not enough free memory)                                                      *
 *                                                                                                                    *
 * Note              : use only in thread mode                                                                        *
 *                                                                                                                    *
 **********************************************************************************************************************/

static inline tsk_id   tsk_new( unsigned prio, fun_id state ) { return tsk_create(prio, state, OS_STACK_SIZE); }

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : tsk_start                                                                                      *
 *                                                                                                                    *
 * Description       : start previously defined/created/stopped task object                                           *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   tsk             : pointer to task object                                                                         *
 *                                                                                                                    *
 * Return            : none                                                                                           *
 *                                                                                                                    *
 * Note              : use only in thread mode                                                                        *
 *                                                                                                                    *
 **********************************************************************************************************************/

              void     tsk_start( tsk_id tsk );

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : tsk_startFrom                                                                                  *
 *                                                                                                                    *
 * Description       : start previously defined/created/stopped task object                                           *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   tsk             : pointer to task object                                                                         *
 *   state           : task state (initial task function) doesn't have to be noreturn-type                            *
 *                     it will be executed into an infinite system-implemented loop                                   *
 *                                                                                                                    *
 * Return            : none                                                                                           *
 *                                                                                                                    *
 * Note              : use only in thread mode                                                                        *
 *                                                                                                                    *
 **********************************************************************************************************************/

              void     tsk_startFrom( tsk_id tsk, fun_id state );

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : tsk_stop                                                                                       *
 *                                                                                                                    *
 * Description       : stop current task and remove it from READY queue                                               *
 *                                                                                                                    *
 * Parameters        : none                                                                                           *
 *                                                                                                                    *
 * Return            : none                                                                                           *
 *                                                                                                                    *
 * Note              : use only in thread mode                                                                        *
 *                                                                                                                    *
 **********************************************************************************************************************/

              void     tsk_stop( void ) __noreturn;

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : tsk_kill                                                                                       *
 *                                                                                                                    *
 * Description       : reset the task object (not current task) and remove it from READY/DELAYED queue                *
 *                     for current task use tsk_stop instead                                                          *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   tsk             : pointer to task object                                                                         *
 *                                                                                                                    *
 * Return            : none                                                                                           *
 *                                                                                                                    *
 * Note              : use only in thread mode                                                                        *
 *                                                                                                                    *
 **********************************************************************************************************************/

              void     tsk_kill( tsk_id tsk );

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : tsk_pass                                                                                       *
 *                                                                                                                    *
 * Description       : yield system control to the next task with the same priority in READY queue                    *
 *                                                                                                                    *
 * Parameters        : none                                                                                           *
 *                                                                                                                    *
 * Return            : none                                                                                           *
 *                                                                                                                    *
 * Note              : use only in thread mode                                                                        *
 *                                                                                                                    *
 **********************************************************************************************************************/

static inline void     tsk_pass ( void ) { core_ctx_switch(); }

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : tsk_yield                                                                                      *
 *                                                                                                                    *
 * Description       : the same as tsk_pass (force context switch)                                                    *
 *                                                                                                                    *
 * Parameters        : none                                                                                           *
 *                                                                                                                    *
 * Return            : none                                                                                           *
 *                                                                                                                    *
 * Note              : use only in thread mode                                                                        *
 *                                                                                                                    *
 **********************************************************************************************************************/

static inline void     tsk_yield( void ) { core_ctx_switch(); }

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : tsk_flip                                                                                       *
 *                                                                                                                    *
 * Description       : restart current task with the new state (task function)                                        *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   proc            : new task state (task function)                                                                 *
 *                                                                                                                    *
 * Return            : none                                                                                           *
 *                                                                                                                    *
 * Note              : use only in thread mode                                                                        *
 *                                                                                                                    *
 **********************************************************************************************************************/

              void     tsk_flip( fun_id state ) __noreturn;

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : tsk_prio                                                                                       *
 *                                                                                                                    *
 * Description       : set current task priority                                                                      *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   prio            : new task priority value                                                                        *
 *                                                                                                                    *
 * Return            : none                                                                                           *
 *                                                                                                                    *
 * Note              : use only in thread mode                                                                        *
 *                                                                                                                    *
 **********************************************************************************************************************/

              void     tsk_prio( unsigned prio );

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : tsk_sleepUntil                                                                                 *
 *                                                                                                                    *
 * Description       : delay execution of current task until given timepoint                                          *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   time            : timepoint value                                                                                *
 *                                                                                                                    *
 * Return                                                                                                             *
 *   E_TIMEOUT       : task object successfully finished countdown                                                    *
 *   'another'       : task was resumed with 'another' event value                                                    *
 *                                                                                                                    *
 * Note              : use only in thread mode                                                                        *
 *                                                                                                                    *
 **********************************************************************************************************************/

              unsigned tsk_sleepUntil( unsigned time );

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : tsk_sleepFor                                                                                   *
 *                                                                                                                    *
 * Description       : delay execution of current task for given duration of time                                     *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   delay           : duration of time (maximum number of ticks to delay execution of current task)                  *
 *                     IMMEDIATE: don't delay execution of current task                                               *
 *                     INFINITE:  delay indefinitly execution of current task                                         *
 *                                                                                                                    *
 * Return                                                                                                             *
 *   E_TIMEOUT       : task object successfully finished countdown                                                    *
 *   'another'       : task was resumed with 'another' event value                                                    *
 *                                                                                                                    *
 * Note              : use only in thread mode                                                                        *
 *                                                                                                                    *
 **********************************************************************************************************************/

              unsigned tsk_sleepFor( unsigned delay );

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : tsk_sleep                                                                                      *
 *                                                                                                                    *
 * Description       : delay indefinitly execution of current task                                                    *
 *                                                                                                                    *
 * Parameters        : none                                                                                           *
 *                                                                                                                    *
 * Return                                                                                                             *
 *   'another'       : task was resumed with 'another' event value                                                    *
 *                                                                                                                    *
 * Note              : use only in thread mode                                                                        *
 *                                                                                                                    *
 **********************************************************************************************************************/

static inline unsigned tsk_sleep( void ) { return tsk_sleepFor(INFINITE); }

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : tsk_delay                                                                                      *
 *                                                                                                                    *
 * Description       : the same as tsk_sleepFor, delay execution of current task for given duration of time           *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   delay           : duration of time (maximum number of ticks to delay execution of current task)                  *
 *                     IMMEDIATE: don't delay execution of current task                                               *
 *                     INFINITE:  delay indefinitly execution of current task                                         *
 *                                                                                                                    *
 * Return                                                                                                             *
 *   E_TIMEOUT       : task object successfully finished countdown                                                    *
 *   'another'       : task was resumed with 'another' event value                                                    *
 *                                                                                                                    *
 * Note              : use only in thread mode                                                                        *
 *                                                                                                                    *
 **********************************************************************************************************************/

static inline unsigned tsk_delay( unsigned delay ) { return tsk_sleepFor(delay); }

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : tsk_suspend                                                                                    *
 *                                                                                                                    *
 * Description       : the same as tsk_sleep, delay indefinitly execution of current task                             *
 *                                                                                                                    *
 * Parameters        : none                                                                                           *
 *                                                                                                                    *
 * Return                                                                                                             *
 *   'another'       : task was resumed with 'another' event value                                                    *
 *                                                                                                                    *
 * Note              : use only in thread mode                                                                        *
 *                                                                                                                    *
 **********************************************************************************************************************/

static inline unsigned tsk_suspend( void ) { return tsk_sleep(); }

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : tsk_resume                                                                                     *
 *                                                                                                                    *
 * Description       : resume execution of given delayed task                                                         *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   tsk             : pointer to delayed task object                                                                 *
 *   event           : the value at which the given task is woken up                                                  *
 *                                                                                                                    *
 * Return            : none                                                                                           *
 *                                                                                                                    *
 * Note              : use only in thread mode                                                                        *
 *                                                                                                                    *
 **********************************************************************************************************************/

              void     tsk_resume( tsk_id tsk, unsigned event );

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : tsk_resumeISR                                                                                  *
 *                                                                                                                    *
 * Description       : resume execution of given delayed task                                                         *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   tsk             : pointer to delayed task object                                                                 *
 *   event           : the value at which the given task is woken up                                                  *
 *                                                                                                                    *
 * Return            : none                                                                                           *
 *                                                                                                                    *
 * Note              : use only in handler mode                                                                       *
 *                                                                                                                    *
 **********************************************************************************************************************/

static inline void     tsk_resumeISR( tsk_id tsk, unsigned event ) { tsk_resume(tsk, event); }

#ifdef __cplusplus
}
#endif

/* -------------------------------------------------------------------------- */

#ifdef __cplusplus

/**********************************************************************************************************************
 *                                                                                                                    *
 * Class             : TaskBase                                                                                       *
 *                                                                                                                    *
 * Description       : create and initilize a task object, internal                                                   *
 *                                                                                                                    *
 * Constructor parameters                                                                                             *
 *   prio            : initial task priority (any unsigned int value)                                                 *
 *   state           : task state (initial task function) doesn't have to be noreturn-type                            *
 *                     it will be executed into an infinite system-implemented loop                                   *
 *   top             : initial value of task's stack pointer                                                          *
 *                                                                                                                    *
 **********************************************************************************************************************/

class TaskBase : public __tsk, private ObjectGuard<__tsk>
{
public:

	explicit
	TaskBase( const unsigned _prio, const fun_id _state, const os_id _top ): __tsk(_TSK_INIT(_prio, _state, _top)) {}

	void     kill      ( void )                          {        tsk_kill      (this);                }
	void     start     ( void )                          {        tsk_start     (this);                }
	void     startFrom ( fun_id   _state )               {        tsk_startFrom (this, _state);        }
	void     resume    ( unsigned _event )               {        tsk_resume    (this, _event);        }
	void     resumeISR ( unsigned _event )               {        tsk_resumeISR (this, _event);        }
};

/**********************************************************************************************************************
 *                                                                                                                    *
 * Namespace         : ThisTask                                                                                       *
 *                                                                                                                    *
 * Description       : provide set of functions for Current Task                                                      *
 *                                                                                                                    *
 **********************************************************************************************************************/

namespace ThisTask
{
	void     pass      ( void )                          {        tsk_pass      ();                    }
	void     yield     ( void )                          {        tsk_yield     ();                    }
	void     flip      ( fun_id   _state )               {        tsk_flip      (_state);              }
	void     stop      ( void )                          {        tsk_stop      ();                    }
	void     prio      ( unsigned _prio )                {        tsk_prio      (_prio);               }

	unsigned sleepUntil( unsigned _time )                { return tsk_sleepUntil(_time);               }
	unsigned sleepFor  ( unsigned _delay )               { return tsk_sleepFor  (_delay);              }
	unsigned sleep     ( void )                          { return tsk_sleep     ();                    }
	unsigned delay     ( unsigned _delay )               { return tsk_delay     (_delay);              }
	unsigned suspend   ( void )                          { return tsk_suspend   ();                    }
}

/**********************************************************************************************************************
 *                                                                                                                    *
 * Class             : TaskT                                                                                          *
 *                                                                                                                    *
 * Description       : create and initilize complete work area for task object                                        *
 *                                                                                                                    *
 * Constructor parameters                                                                                             *
 *   size            : size of task private stack (in bytes)                                                          *
 *   prio            : initial task priority (any unsigned int value)                                                 *
 *   state           : task state (initial task function) doesn't have to be noreturn-type                            *
 *                     it will be executed into an infinite system-implemented loop                                   *
 *                                                                                                                    *
 **********************************************************************************************************************/

template<unsigned _size>
class TaskT : public TaskBase
{
	char _stack[ASIZE(_size)] __osalign;

public:

	explicit
	TaskT( const unsigned _prio, const fun_id _state ): TaskBase(_prio, _state, _stack+ASIZE(_size)) {}
};

/**********************************************************************************************************************
 *                                                                                                                    *
 * Class             : Task                                                                                           *
 *                                                                                                                    *
 * Description       : create and initilize complete work area for task object with default stack size                *
 *                                                                                                                    *
 * Constructor parameters                                                                                             *
 *   prio            : initial task priority (any unsigned int value)                                                 *
 *   state           : task state (initial task function) doesn't have to be noreturn-type                            *
 *                     it will be executed into an infinite system-implemented loop                                   *
 *                                                                                                                    *
 **********************************************************************************************************************/

class Task: public TaskT<OS_STACK_SIZE>
{
public:

	explicit
	Task( const unsigned _prio, const fun_id _state ): TaskT<OS_STACK_SIZE>(_prio, _state) {}
};

/**********************************************************************************************************************
 *                                                                                                                    *
 * Class             : startTaskT                                                                                     *
 *                                                                                                                    *
 * Description       : create and initilize complete work area for task object                                        *
 *                     and start task object                                                                          *
 *                                                                                                                    *
 * Constructor parameters                                                                                             *
 *   size            : size of task private stack (in bytes)                                                          *
 *   prio            : initial task priority (any unsigned int value)                                                 *
 *   state           : task state (initial task function) doesn't have to be noreturn-type                            *
 *                     it will be executed into an infinite system-implemented loop                                   *
 *                                                                                                                    *
 **********************************************************************************************************************/

template<unsigned _size>
class startTaskT : public TaskT<_size>
{
public:

	explicit
	startTaskT( const unsigned _prio, const fun_id _state ): TaskT<_size>(_prio, _state) { tsk_start(this); }
};

/**********************************************************************************************************************
 *                                                                                                                    *
 * Class             : startTask                                                                                      *
 *                                                                                                                    *
 * Description       : create and initilize complete work area for task object with default stack size                *
 *                     and start task object                                                                          *
 *                                                                                                                    *
 * Constructor parameters                                                                                             *
 *   prio            : initial task priority (any unsigned int value)                                                 *
 *   state           : task state (initial task function) doesn't have to be noreturn-type                            *
 *                     it will be executed into an infinite system-implemented loop                                   *
 *                                                                                                                    *
 **********************************************************************************************************************/

class startTask : public Task
{
public:

	explicit
	startTask( const unsigned _prio, const fun_id _state ): Task(_prio, _state) { tsk_start(this); }
};

#endif//__cplusplus

/* -------------------------------------------------------------------------- */
