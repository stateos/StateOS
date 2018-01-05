/******************************************************************************

    @file    StateOS: os_tsk.h
    @author  Rajmund Szymanski
    @date    01.01.2018
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

#ifndef __STATEOS_TSK_H
#define __STATEOS_TSK_H

#include "oskernel.h"
#include "os_mtx.h"
#include "os_tmr.h"

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************
 *
 * Name              : task (thread)
 *
 ******************************************************************************/

struct __tsk
{
	obj_t    obj;   // inherited from timer

	fun_t  * state; // inherited from timer
	cnt_t    start; // inherited from timer
	cnt_t    delay; // inherited from timer
	cnt_t    period;// inherited from timer

	tsk_t  * back;  // previous process in the DELAYED queue
	void   * sp;    // current stack pointer
	stk_t  * top;   // top of stack
	void   * stack; // base of stack

	unsigned basic; // basic priority
	unsigned prio;  // current priority

	tsk_t  * join;  // joinable state
	void   * guard; // object that controls the pending process
	tsk_t  * mtree; // tree of tasks waiting for mutexes
	mtx_t  * mlist; // list of mutexes held

	cnt_t    slice;	// time slice
	union  {
	unsigned mode;  // used by flag object
	void   * data;  // used by queue objects
	unsigned msg;   // used by message queue object
	fun_t  * fun;   // used by job queue object
	}        tmp;
	union  {
	unsigned flags; // used by flag object: all flags to wait
	unsigned event; // wakeup event
	}        evt;
#if defined(__ARMCC_VERSION) && !defined(__MICROLIB)
	char     libspace[96];
#endif
};

/******************************************************************************
 *
 * Name              : _TSK_INIT
 *
 * Description       : create and initialize a task object
 *
 * Parameters
 *   prio            : initial task priority (any unsigned int value)
 *   state           : task state (initial task function) doesn't have to be noreturn-type
 *                     it will be executed into an infinite system-implemented loop
 *   stack           : base of task's private stack storage
 *   size            : size of task private stack (in bytes)
 *
 * Return            : task object
 *
 * Note              : for internal use
 *
 ******************************************************************************/

#if defined(__ARMCC_VERSION) && !defined(__MICROLIB)
#define               _TSK_INIT( _prio, _state, _stack, _size ) \
                       { _OBJ_INIT(), _state, 0, 0, 0, 0, 0, _stack+SSIZE(_size), _stack, _prio, _prio, 0, 0, 0, 0, 0, { 0 }, { 0 }, { 0 } }
#else
#define               _TSK_INIT( _prio, _state, _stack, _size ) \
                       { _OBJ_INIT(), _state, 0, 0, 0, 0, 0, _stack+SSIZE(_size), _stack, _prio, _prio, 0, 0, 0, 0, 0, { 0 }, { 0 } }
#endif

/******************************************************************************
 *
 * Name              : _TSK_CREATE
 *
 * Description       : create and initialize a task object
 *
 * Parameters
 *   prio            : initial task priority (any unsigned int value)
 *   state           : task state (initial task function) doesn't have to be noreturn-type
 *                     it will be executed into an infinite system-implemented loop
 *   stack           : base of task's private stack storage
 *   size            : size of task private stack (in bytes)
 *
 * Return            : pointer to task object
 *
 * Note              : for internal use
 *
 ******************************************************************************/

#ifndef __cplusplus
#define               _TSK_CREATE( _prio, _state, _stack, _size ) \
            & (tsk_t) _TSK_INIT  ( _prio, _state, _stack, _size )
#endif

/******************************************************************************
 *
 * Name              : _TSK_STACK
 *
 * Description       : create task's private stack storage
 *
 * Parameters
 *   size            : size of task's private stack storage (in bytes)
 *
 * Return            : base of task's private stack storage
 *
 * Note              : for internal use
 *
 ******************************************************************************/

#ifndef __cplusplus
#define               _TSK_STACK( _size ) \
                       ( stk_t[SSIZE( _size )] ){ 0 }
#endif

/******************************************************************************
 *
 * Name              : OS_WRK
 *
 * Description       : define and initialize complete work area for task object
 *
 * Parameters
 *   tsk             : name of a pointer to task object
 *   prio            : initial task priority (any unsigned int value)
 *   state           : task state (initial task function) doesn't have to be noreturn-type
 *                     it will be executed into an infinite system-implemented loop
 *   size            : size of task private stack (in bytes)
 *
 ******************************************************************************/

#define             OS_WRK( tsk, prio, state, size )                                \
                       stk_t tsk##__stk[SSIZE( size )];                              \
                       tsk_t tsk##__tsk = _TSK_INIT( prio, state, tsk##__stk, size ); \
                       tsk_id tsk = & tsk##__tsk

/******************************************************************************
 *
 * Name              : OS_TSK
 *
 * Description       : define and initialize complete work area for task object with default stack size
 *
 * Parameters
 *   tsk             : name of a pointer to task object
 *   prio            : initial task priority (any unsigned int value)
 *   state           : task state (initial task function) doesn't have to be noreturn-type
 *                     it will be executed into an infinite system-implemented loop
 *
 ******************************************************************************/

#define             OS_TSK( tsk, prio, state ) \
                    OS_WRK( tsk, prio, state, OS_STACK_SIZE )

/******************************************************************************
 *
 * Name              : OS_WRK_DEF
 *
 * Description       : define and initialize complete work area for task object
 *                     task state (function body) must be defined immediately below
 *
 * Parameters
 *   tsk             : name of a pointer to task object
 *   prio            : initial task priority (any unsigned int value)
 *   size            : size of task private stack (in bytes)
 *
 ******************************************************************************/

#define             OS_WRK_DEF( tsk, prio, size )        \
                       void tsk##__fun( void );           \
                    OS_WRK( tsk, prio, tsk##__fun, size ); \
                       void tsk##__fun( void )

/******************************************************************************
 *
 * Name              : OS_TSK_DEF
 *
 * Description       : define and initialize complete work area for task object with default stack size
 *                     task state (function body) must be defined immediately below
 *
 * Parameters
 *   tsk             : name of a pointer to task object
 *   prio            : initial task priority (any unsigned int value)
 *
 ******************************************************************************/

#define             OS_TSK_DEF( tsk, prio ) \
                    OS_WRK_DEF( tsk, prio, OS_STACK_SIZE )

/******************************************************************************
 *
 * Name              : OS_WRK_START
 *
 * Description       : define, initialize and start complete work area for task object
 *                     task state (function body) must be defined immediately below
 *
 * Parameters
 *   tsk             : name of a pointer to task object
 *   prio            : initial task priority (any unsigned int value)
 *   size            : size of task private stack (in bytes)
 *
 * Note              : only available for compilers supporting the "constructor" function attribute or its equivalent
 *
 ******************************************************************************/

#define             OS_WRK_START( tsk, prio, size )                                \
                       void tsk##__fun( void );                                     \
                    OS_WRK( tsk, prio, tsk##__fun, size );                           \
         __CONSTRUCTOR void tsk##__start( void ) { port_sys_init(); tsk_start(tsk); } \
                       void tsk##__fun( void )

/******************************************************************************
 *
 * Name              : OS_TSK_START
 *
 * Description       : define, initialize and start complete work area for task object with default stack size
 *                     task state (function body) must be defined immediately below
 *
 * Parameters
 *   tsk             : name of a pointer to task object
 *   prio            : initial task priority (any unsigned int value)
 *
 * Note              : only available for compilers supporting the "constructor" function attribute or its equivalent
 *
 ******************************************************************************/

#define             OS_TSK_START( tsk, prio ) \
                    OS_WRK_START( tsk, prio, OS_STACK_SIZE )

/******************************************************************************
 *
 * Name              : static_WRK
 *
 * Description       : define and initialize static work area for task object
 *
 * Parameters
 *   tsk             : name of a pointer to task object
 *   prio            : initial task priority (any unsigned int value)
 *   state           : task state (initial task function) doesn't have to be noreturn-type
 *                     it will be executed into an infinite system-implemented loop
 *   size            : size of task private stack (in bytes)
 *
 ******************************************************************************/

#define         static_WRK( tsk, prio, state, size )                                \
                static stk_t tsk##__stk[SSIZE( size )];                              \
                static tsk_t tsk##__tsk = _TSK_INIT( prio, state, tsk##__stk, size ); \
                static tsk_id tsk = & tsk##__tsk

/******************************************************************************
 *
 * Name              : static_TSK
 *
 * Description       : define and initialize static work area for task object with default stack size
 *
 * Parameters
 *   tsk             : name of a pointer to task object
 *   prio            : initial task priority (any unsigned int value)
 *   state           : task state (initial task function) doesn't have to be noreturn-type
 *                     it will be executed into an infinite system-implemented loop
 *
 ******************************************************************************/

#define         static_TSK( tsk, prio, state ) \
                static_WRK( tsk, prio, state, OS_STACK_SIZE )

/******************************************************************************
 *
 * Name              : static_WRK_DEF
 *
 * Description       : define and initialize static work area for task object
 *                     task state (function body) must be defined immediately below
 *
 * Parameters
 *   tsk             : name of a pointer to task object
 *   prio            : initial task priority (any unsigned int value)
 *   size            : size of task private stack (in bytes)
 *
 ******************************************************************************/

#define         static_WRK_DEF( tsk, prio, size )        \
                static void tsk##__fun( void );           \
                static_WRK( tsk, prio, tsk##__fun, size ); \
                static void tsk##__fun( void )

/******************************************************************************
 *
 * Name              : static_TSK_DEF
 *
 * Description       : define and initialize static work area for task object with default stack size
 *                     task state (function body) must be defined immediately below
 *
 * Parameters
 *   tsk             : name of a pointer to task object
 *   prio            : initial task priority (any unsigned int value)
 *
 ******************************************************************************/

#define         static_TSK_DEF( tsk, prio ) \
                static_WRK_DEF( tsk, prio, OS_STACK_SIZE )

/******************************************************************************
 *
 * Name              : static_WRK_START
 *
 * Description       : define, initialize and start static work area for task object
 *                     task state (function body) must be defined immediately below
 *
 * Parameters
 *   tsk             : name of a pointer to task object
 *   prio            : initial task priority (any unsigned int value)
 *   size            : size of task private stack (in bytes)
 *
 * Note              : only available for compilers supporting the "constructor" function attribute or its equivalent
 *
 ******************************************************************************/

#define         static_WRK_START( tsk, prio, size )                                \
                static void tsk##__fun( void );                                     \
                static_WRK( tsk, prio, tsk##__fun, size );                           \
  __CONSTRUCTOR static void tsk##__start( void ) { port_sys_init(); tsk_start(tsk); } \
                static void tsk##__fun( void )

/******************************************************************************
 *
 * Name              : static_TSK_START
 *
 * Description       : define, initialize and start static work area for task object with default stack size
 *                     task state (function body) must be defined immediately below
 *
 * Parameters
 *   tsk             : name of a pointer to task object
 *   prio            : initial task priority (any unsigned int value)
 *
 * Note              : only available for compilers supporting the "constructor" function attribute or its equivalent
 *
 ******************************************************************************/

#define         static_TSK_START( tsk, prio ) \
                static_WRK_START( tsk, prio, OS_STACK_SIZE )

/******************************************************************************
 *
 * Name              : WRK_INIT
 *
 * Description       : create and initialize complete work area for task object
 *
 * Parameters
 *   prio            : initial task priority (any unsigned int value)
 *   state           : task state (initial task function) doesn't have to be noreturn-type
 *                     it will be executed into an infinite system-implemented loop
 *   size            : size of task private stack (in bytes)
 *
 * Return            : task object
 *
 * Note              : use only in 'C' code
 *
 ******************************************************************************/

#ifndef __cplusplus
#define                WRK_INIT( prio, state, size ) \
                      _TSK_INIT( prio, state, _TSK_STACK( size ), size )
#endif

/******************************************************************************
 *
 * Name              : WRK_CREATE
 * Alias             : WRK_NEW
 *
 * Description       : create and initialize complete work area for task object
 *
 * Parameters
 *   prio            : initial task priority (any unsigned int value)
 *   state           : task state (initial task function) doesn't have to be noreturn-type
 *                     it will be executed into an infinite system-implemented loop
 *   size            : size of task private stack (in bytes)
 *
 * Return            : pointer to task object
 *
 * Note              : use only in 'C' code
 *
 ******************************************************************************/

#ifndef __cplusplus
#define                WRK_CREATE( prio, state, size ) \
             & (tsk_t) WRK_INIT  ( prio, state, size )
#define                WRK_NEW \
                       WRK_CREATE
#endif

/******************************************************************************
 *
 * Name              : TSK_INIT
 *
 * Description       : create and initialize complete work area for task object with default stack size
 *
 * Parameters
 *   prio            : initial task priority (any unsigned int value)
 *   state           : task state (initial task function) doesn't have to be noreturn-type
 *                     it will be executed into an infinite system-implemented loop
 *
 * Return            : task object
 *
 * Note              : use only in 'C' code
 *
 ******************************************************************************/

#ifndef __cplusplus
#define                TSK_INIT( prio, state ) \
                       WRK_INIT( prio, state, OS_STACK_SIZE )
#endif

/******************************************************************************
 *
 * Name              : TSK_CREATE
 * Alias             : TSK_NEW
 *
 * Description       : create and initialize complete work area for task object with default stack size
 *
 * Parameters
 *   prio            : initial task priority (any unsigned int value)
 *   state           : task state (initial task function) doesn't have to be noreturn-type
 *                     it will be executed into an infinite system-implemented loop
 *
 * Return            : pointer to task object
 *
 * Note              : use only in 'C' code
 *
 ******************************************************************************/

#ifndef __cplusplus
#define                TSK_CREATE( prio, state ) \
                       WRK_CREATE( prio, state, OS_STACK_SIZE )
#define                TSK_NEW \
                       TSK_CREATE
#endif

/******************************************************************************
 *
 * Name              : tsk_this
 *
 * Description       : return current task object
 *
 * Parameters        : none
 *
 * Return            : current task object
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

__STATIC_INLINE
tsk_t *tsk_this( void ) { return System.cur; }

/******************************************************************************
 *
 * Name              : tsk_init
 *
 * Description       : initialize complete work area for task object and start the task
 *
 * Parameters
 *   tsk             : pointer to task object
 *   prio            : initial task priority (any unsigned int value)
 *   state           : task state (initial task function) doesn't have to be noreturn-type
 *                     it will be executed into an infinite system-implemented loop
 *   stack           : base of task's private stack storage
 *   size            : size of task private stack (in bytes)
 *
 * Return            : task object
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

void tsk_init( tsk_t *tsk, unsigned prio, fun_t *state, void *stack, unsigned size );

/******************************************************************************
 *
 * Name              : wrk_create
 * Alias             : wrk_new
 *
 * Description       : create and initialize complete work area for task object and start the task
 *
 * Parameters
 *   prio            : initial task priority (any unsigned int value)
 *   state           : task state (initial task function) doesn't have to be noreturn-type
 *                     it will be executed into an infinite system-implemented loop
 *   size            : size of task private stack (in bytes)
 *
 * Return            : pointer to task object (task successfully created)
 *   0               : task not created (not enough free memory)
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

tsk_t *wrk_create( unsigned prio, fun_t *state, unsigned size );

__STATIC_INLINE
tsk_t *wrk_new( unsigned prio, fun_t *state, unsigned size ) { return wrk_create(prio, state, size); }

/******************************************************************************
 *
 * Name              : tsk_create
 * Alias             : tsk_new
 *
 * Description       : create and initialize complete work area for task object with default stack size
 *
 * Parameters
 *   prio            : initial task priority (any unsigned int value)
 *   state           : task state (initial task function) doesn't have to be noreturn-type
 *                     it will be executed into an infinite system-implemented loop
 *
 * Return            : pointer to task object (task successfully created)
 *   0               : task not created (not enough free memory)
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

__STATIC_INLINE
tsk_t *tsk_create( unsigned prio, fun_t *state ) { return wrk_create(prio, state, OS_STACK_SIZE); }
__STATIC_INLINE
tsk_t *tsk_new   ( unsigned prio, fun_t *state ) { return wrk_create(prio, state, OS_STACK_SIZE); }

/******************************************************************************
 *
 * Name              : tsk_start
 *
 * Description       : start previously defined/created/stopped task object
 *
 * Parameters
 *   tsk             : pointer to task object
 *
 * Return            : none
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

void tsk_start( tsk_t *tsk );

/******************************************************************************
 *
 * Name              : tsk_startFrom
 *
 * Description       : start previously defined/created/stopped task object
 *
 * Parameters
 *   tsk             : pointer to task object
 *   state           : task state (initial task function) doesn't have to be noreturn-type
 *                     it will be executed into an infinite system-implemented loop
 *
 * Return            : none
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

void tsk_startFrom( tsk_t *tsk, fun_t *state );

/******************************************************************************
 *
 * Name              : tsk_stop
 *
 * Description       : stop current task and remove it from READY queue
 *                     function doesn't destroy the stack storage
 *                     all allocated resources remain intact until restarting the task
 *
 * Parameters        : none
 *
 * Return            : none
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

__NO_RETURN
void tsk_stop( void );

/******************************************************************************
 *
 * Name              : tsk_kill
 *
 * Description       : reset the task object and remove it from READY/DELAYED queue
 *
 * Parameters
 *   tsk             : pointer to task object
 *
 * Return            : none
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

void tsk_kill( tsk_t *tsk );

/******************************************************************************
 *
 * Name              : tsk_delete
 *
 * Description       : reset the task object and free allocated resource
 *
 * Parameters
 *   tsk             : pointer to task object
 *
 * Return            : none
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

void tsk_delete( tsk_t *tsk );

/******************************************************************************
 *
 * Name              : tsk_detach
 *
 * Description       : detach given task
 *
 * Parameters
 *   tsk             : pointer to task object
 *
 * Return
 *   E_SUCCESS       : given task was successfully detached
 *   E_TIMEOUT       : given task can not be detached
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

unsigned tsk_detach( tsk_t *tsk );

/******************************************************************************
 *
 * Name              : tsk_join
 *
 * Description       : delay execution of current task until termination of given task
 *
 * Parameters
 *   tsk             : pointer to task object
 *
 * Return
 *   E_SUCCESS       : joined task was stopped its execution
 *   E_STOPPED       : joined task was killed
 *   E_TIMEOUT       : joined task was detached
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

unsigned tsk_join( tsk_t *tsk );

/******************************************************************************
 *
 * Name              : tsk_yield
 * Alias             : tsk_pass
 *
 * Description       : yield system control to the next task with the same priority in READY queue
 *
 * Parameters        : none
 *
 * Return            : none
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

void tsk_yield( void );
__STATIC_INLINE
void tsk_pass ( void ) { tsk_yield(); }

/******************************************************************************
 *
 * Name              : tsk_flip
 *
 * Description       : restart current task with the new state (task function)
 *
 * Parameters
 *   proc            : new task state (task function)
 *
 * Return            : none
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

__NO_RETURN
void tsk_flip( fun_t *state );

/******************************************************************************
 *
 * Name              : tsk_prio
 * Alias             : tsk_setPrio
 *
 * Description       : set current task priority
 *
 * Parameters
 *   prio            : new task priority value
 *
 * Return            : none
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

void tsk_prio   ( unsigned prio );
__STATIC_INLINE
void tsk_setPrio( unsigned prio ) { tsk_prio(prio); }

/******************************************************************************
 *
 * Name              : tsk_getPrio
 *
 * Description       : get current task priority
 *
 * Parameters        : none
 *
 * Return            : current task priority value
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

__STATIC_INLINE
unsigned tsk_getPrio( void ) { return System.cur->basic; }

/******************************************************************************
 *
 * Name              : tsk_waitUntil
 *
 * Description       : delay execution of current task until given timepoint and wait for flags or message
 *
 * Parameters
 *   flags           : all flags to wait
 *                     0: wait for any flags or message
 *   time            : timepoint value
 *
 * Return
 *   E_TIMEOUT       : task object was not released before the specified timeout expired
 *   'another'       : task object resumed by the direct transfer of 'another' flags or message (tsk_give)
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

unsigned tsk_waitUntil( unsigned flags, cnt_t time );

/******************************************************************************
 *
 * Name              : tsk_waitFor
 *
 * Description       : delay execution of current task for given duration of time and wait for flags or message
 *
 * Parameters
 *   flags           : all flags to wait
 *                     0: wait for any flag or message
 *   delay           : duration of time (maximum number of ticks to delay execution of current task)
 *                     IMMEDIATE: don't delay execution of current task
 *                     INFINITE:  delay indefinitely execution of current task
 *
 * Return
 *   E_TIMEOUT       : task object was not released before the specified timeout expired
 *   'another'       : task object resumed by the direct transfer of 'another' flags or message (tsk_give)
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

unsigned tsk_waitFor( unsigned flags, cnt_t delay );

/******************************************************************************
 *
 * Name              : tsk_wait
 *
 * Description       : delay indefinitely execution of current task and wait for flags or message
 *
 * Parameters
 *   flags           : all flags to wait
 *                     0: wait for any flag or message
 *
 * Return
 *   'another'       : task object resumed by the direct transfer of 'another' flags or message (tsk_give)
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

__STATIC_INLINE
unsigned tsk_wait( unsigned flags ) { return tsk_waitFor(flags, INFINITE); }

/******************************************************************************
 *
 * Name              : tsk_give
 * ISR alias         : tsk_giveISR
 *
 * Description       : set given flags in waiting task (tsk_wait)
 *
 * Parameters
 *   tsk             : pointer to delayed task object
 *   flags           : flags or message transfered to the task
 *
 * Return            : none
 *
 * Note              : may be used both in thread and handler mode
 *
 ******************************************************************************/

void tsk_give( tsk_t *tsk, unsigned flags );

__STATIC_INLINE
void tsk_giveISR( tsk_t *tsk, unsigned flags ) { tsk_give(tsk, flags); }

/******************************************************************************
 *
 * Name              : tsk_sleepUntil
 *
 * Description       : delay execution of current task until given timepoint
 *
 * Parameters
 *   time            : timepoint value
 *
 * Return
 *   E_TIMEOUT       : task object successfully finished countdown
 *   E_STOPPED       : task object was resumed (tsk_resume)
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

__STATIC_INLINE
unsigned tsk_sleepUntil( cnt_t time ) { return tmr_waitUntil(&WAIT, time); }

/******************************************************************************
 *
 * Name              : tsk_sleepFor
 *
 * Description       : delay execution of current task for given duration of time
 *
 * Parameters
 *   delay           : duration of time (maximum number of ticks to delay execution of current task)
 *                     IMMEDIATE: don't delay execution of current task
 *                     INFINITE:  delay indefinitely execution of current task
 *
 * Return
 *   E_TIMEOUT       : task object successfully finished countdown
 *   E_STOPPED       : task object was resumed (tsk_resume)
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

__STATIC_INLINE
unsigned tsk_sleepFor( cnt_t delay ) { return tmr_waitFor(&WAIT, delay); }

/******************************************************************************
 *
 * Name              : tsk_sleep
 *
 * Description       : delay indefinitely execution of current task
 *
 * Parameters        : none
 *
 * Return
 *   E_STOPPED       : task object was resumed (tsk_resume)
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

__STATIC_INLINE
unsigned tsk_sleep( void ) { return tmr_wait(&WAIT); }

/******************************************************************************
 *
 * Name              : tsk_delay
 *
 * Description       : the same as tsk_sleepFor, delay execution of current task for given duration of time
 *
 * Parameters
 *   delay           : duration of time (maximum number of ticks to delay execution of current task)
 *                     IMMEDIATE: don't delay execution of current task
 *                     INFINITE:  delay indefinitely execution of current task
 *
 * Return
 *   E_TIMEOUT       : task object successfully finished countdown
 *   E_STOPPED       : task object was resumed (tsk_resume)
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

__STATIC_INLINE
unsigned tsk_delay( cnt_t delay ) { return tsk_sleepFor(delay); }

/******************************************************************************
 *
 * Name              : tsk_suspend
 *
 * Description       : delay indefinitely execution of given task
 *                     tasks from delayed queue can not be suspended
 *
 * Parameters
 *   tsk             : pointer to task object
 *
 * Return
 *   E_SUCCESS       : task was successfully suspended
 *   E_STOPPED       : task can not be suspended
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

unsigned tsk_suspend( tsk_t *tsk );

/******************************************************************************
 *
 * Name              : tsk_resume
 * ISR alias         : tsk_resumeISR
 *
 * Description       : resume execution of given delayed task
 *                     only suspended and sleeping tasks can be resumed
 *
 * Parameters
 *   tsk             : pointer to delayed task object
 *
 * Return
 *   E_SUCCESS       : task was successfully resumed
 *   E_STOPPED       : task can not be resumed
 *
 * Note              : may be used both in thread and handler mode
 *
 ******************************************************************************/

unsigned tsk_resume( tsk_t *tsk );

__STATIC_INLINE
unsigned tsk_resumeISR( tsk_t *tsk ) { return tsk_resume(tsk); }

#ifdef __cplusplus
}
#endif

/* -------------------------------------------------------------------------- */

#ifdef __cplusplus

/******************************************************************************
 *
 * Class             : baseTask
 *
 * Description       : create task object
 *
 * Constructor parameters
 *   prio            : initial task priority (any unsigned int value)
 *   state           : task state (initial task function) doesn't have to be noreturn-type
 *                     it will be executed into an infinite system-implemented loop
 *   stack           : base of task's private stack storage
 *   size            : size of task private stack (in bytes)
 *
 ******************************************************************************/

struct baseTask : public __tsk
{
#if OS_FUNCTIONAL
	 explicit
	 baseTask( const unsigned _prio, FUN_t _state, stk_t * const _stack, const unsigned _size ): __tsk _TSK_INIT(_prio, run_, _stack, _size), fun_(_state) {}
	~baseTask( void ) { assert(__tsk::obj.id == ID_STOPPED); }
#else
	 explicit
	 baseTask( const unsigned _prio, FUN_t _state, stk_t * const _stack, const unsigned _size ): __tsk _TSK_INIT(_prio, _state, _stack, _size) {}
	~baseTask( void ) { assert(__tsk::obj.id == ID_STOPPED); }
#endif

	void     kill     ( void )            {        tsk_kill      (this);         }
	unsigned detach   ( void )            { return tsk_detach    (this);         }
	unsigned join     ( void )            { return tsk_join      (this);         }
	void     start    ( void )            {        tsk_start     (this);         }
#if OS_FUNCTIONAL
	void     startFrom( FUN_t    _state ) {        fun_ = _state;
	                                               tsk_startFrom (this, run_);   }
#else
	void     startFrom( FUN_t    _state ) {        tsk_startFrom (this, _state); }
#endif
	void     give     ( unsigned _flags ) {        tsk_give      (this, _flags); }
	void     giveISR  ( unsigned _flags ) {        tsk_giveISR   (this, _flags); }
	unsigned suspend  ( void )            { return tsk_suspend   (this);         }
	unsigned resume   ( void )            { return tsk_resume    (this);         }
	unsigned resumeISR( void )            { return tsk_resumeISR (this);         }

	unsigned prio     ( void )            { return __tsk::basic;                 }
	unsigned getPrio  ( void )            { return __tsk::basic;                 }
	bool     operator!( void )            { return __tsk::obj.id == ID_STOPPED;  }
#if OS_FUNCTIONAL
	static
	void     run_( void ) { ((baseTask *) System.cur)->fun_(); }
	FUN_t    fun_;
#endif
};

/******************************************************************************
 *
 * Class             : TaskT<>
 *
 * Description       : create and initialize complete work area for task object
 *
 * Constructor parameters
 *   size            : size of task private stack (in bytes)
 *   prio            : initial task priority (any unsigned int value)
 *   state           : task state (initial task function) doesn't have to be noreturn-type
 *                     it will be executed into an infinite system-implemented loop
 *
 ******************************************************************************/

template<unsigned _size>
struct TaskT : public baseTask
{
	explicit
	TaskT( const unsigned _prio, FUN_t _state ): baseTask(_prio, _state, stack_, _size) {}

	private:
	stk_t stack_[SSIZE(_size)];
};

/******************************************************************************
 *
 * Class             : Task
 *
 * Description       : create and initialize complete work area for task object with default stack size
 *
 * Constructor parameters
 *   prio            : initial task priority (any unsigned int value)
 *   state           : task state (initial task function) doesn't have to be noreturn-type
 *                     it will be executed into an infinite system-implemented loop
 *
 ******************************************************************************/

struct Task: public TaskT<OS_STACK_SIZE>
{
	explicit
	Task( const unsigned _prio, FUN_t _state ): TaskT<OS_STACK_SIZE>(_prio, _state) {}
};

/******************************************************************************
 *
 * Class             : startTaskT<>
 *
 * Description       : create and initialize complete work area for task object
 *                     and start task object
 *
 * Constructor parameters
 *   size            : size of task private stack (in bytes)
 *   prio            : initial task priority (any unsigned int value)
 *   state           : task state (initial task function) doesn't have to be noreturn-type
 *                     it will be executed into an infinite system-implemented loop
 *
 ******************************************************************************/

template<unsigned _size>
struct startTaskT : public TaskT<_size>
{
	explicit
	startTaskT( const unsigned _prio, FUN_t _state ): TaskT<_size>(_prio, _state) { port_sys_init(); tsk_start(this); }
};

/******************************************************************************
 *
 * Class             : startTask
 *
 * Description       : create and initialize complete work area for task object with default stack size
 *                     and start task object
 *
 * Constructor parameters
 *   prio            : initial task priority (any unsigned int value)
 *   state           : task state (initial task function) doesn't have to be noreturn-type
 *                     it will be executed into an infinite system-implemented loop
 *
 ******************************************************************************/

struct startTask : public startTaskT<OS_STACK_SIZE>
{
	explicit
	startTask( const unsigned _prio, FUN_t _state ): startTaskT<OS_STACK_SIZE>(_prio, _state) {}
};

/******************************************************************************
 *
 * Namespace         : ThisTask
 *
 * Description       : provide set of functions for Current Task
 *
 ******************************************************************************/

namespace ThisTask
{
	static inline void     pass      ( void )                          {        tsk_pass      ();                         }
	static inline void     yield     ( void )                          {        tsk_yield     ();                         }
#if OS_FUNCTIONAL
	static inline void     flip      ( FUN_t    _state )               {        ((baseTask *) System.cur)->fun_ = _state;
	                                                                            tsk_flip      (baseTask::run_);           }
#else
	static inline void     flip      ( FUN_t    _state )               {        tsk_flip      (_state);                   }
#endif
	static inline void     stop      ( void )                          {        tsk_stop      ();                         }
	static inline void     prio      ( unsigned _prio )                {        tsk_prio      (_prio);                    }
	static inline void     setPrio   ( unsigned _prio )                {        tsk_setPrio   (_prio);                    }
	static inline unsigned getPrio   ( void )                          { return tsk_getPrio   ();                         }
	static inline unsigned prio      ( void )                          { return tsk_getPrio   ();                         }

	static inline void     kill      ( void )                          {        tsk_kill      (System.cur);               }
	static inline unsigned detach    ( void )                          { return tsk_detach    (System.cur);               }
	static inline void     suspend   ( void )                          {        tsk_suspend   (System.cur);               }

	static inline unsigned waitUntil ( unsigned _flags, cnt_t _time )  { return tsk_waitUntil (_flags, _time);            }
	static inline unsigned waitFor   ( unsigned _flags, cnt_t _delay ) { return tsk_waitFor   (_flags, _delay);           }
	static inline unsigned wait      ( unsigned _flags )               { return tsk_wait      (_flags);                   }
	static inline unsigned sleepUntil( cnt_t    _time )                { return tsk_sleepUntil(_time);                    }
	static inline unsigned sleepFor  ( cnt_t    _delay )               { return tsk_sleepFor  (_delay);                   }
	static inline unsigned sleep     ( void )                          { return tsk_sleep     ();                         }
	static inline unsigned delay     ( cnt_t    _delay )               { return tsk_delay     (_delay);                   }
}

#endif//__cplusplus

/* -------------------------------------------------------------------------- */

#endif//__STATEOS_TSK_H
