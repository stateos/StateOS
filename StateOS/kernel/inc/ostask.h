/******************************************************************************

    @file    StateOS: ostask.h
    @author  Rajmund Szymanski
    @date    14.08.2018
    @brief   This file contains definitions for StateOS.

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

#ifndef __STATEOS_TSK_H
#define __STATEOS_TSK_H

#include "oskernel.h"
#include "osmutex.h"
#include "ostimer.h"

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
	tid_t    id;    // task's id: ID_STOPPED, ID_READY, ID_DELAYED, ID_IDLE

	fun_t  * state; // task state (initial task function, doesn't have to be noreturn-type)
	cnt_t    start; // inherited from timer
	cnt_t    delay; // inherited from timer
	cnt_t    slice;	// time slice

	tsk_t  * back;  // previous process in the DELAYED queue
	void   * stack; // base of stack
	stk_t  * top;   // top of stack
	void   * sp;    // current stack pointer

	unsigned basic; // basic priority
	unsigned prio;  // current priority

	tsk_t  * join;  // joinable state
	void   * guard; // object that controls the pending process

	unsigned event; // wakeup event

	struct {
	mtx_t  * list;  // list of mutexes held
	tsk_t  * tree;  // tree of tasks waiting for mutexes
	}        mtx;

	union  {

	struct {
	unsigned flags;
	unsigned mode;
	}        flg;   // temporary data used by flag object

	struct {
	union  {
	const
	void  ** out;
	void  ** in;
	}        data;
	}        lst;   // temporary data used by list / memory pool object

	struct {
	union  {
	const
	char   * out;
	char   * in;
	}        data;
	unsigned size;
	}        stm;   // temporary data used by stream buffer object

	struct {
	union  {
	const
	char   * out;
	char   * in;
	}        data;
	unsigned size;
	}        msg;   // temporary data used by message buffer object

	struct {
	union  {
	const
	void   * out;
	void   * in;
	}        data;
	}        box;   // temporary data used by mailbox queue object

	struct {
	fun_t  * fun;
	}        job;   // temporary data used by job queue object

	struct {
	unsigned event;
	}        evq;   // temporary data used by event queue object

	}        tmp;
#if defined(__ARMCC_VERSION) && !defined(__MICROLIB)
	char     libspace[96];
	#define _TSK_EXTRA { 0 }
#else
	#define _TSK_EXTRA
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

#define               _TSK_INIT( _prio, _state, _stack, _size ) \
                       { _OBJ_INIT(), ID_STOPPED, _state, 0, 0, 0, 0, _stack, _stack+SSIZE(_size), 0, _prio, _prio, 0, 0, 0, { 0, 0 }, { { 0, 0 } }, _TSK_EXTRA }

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
          (tsk_t[]) { _TSK_INIT  ( _prio, _state, _stack, _size ) }
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
 * Name              : _VA_STK
 *
 * Description       : calculate stack size from optional parameter
 *
 * Note              : for internal use
 *
 ******************************************************************************/

#define               _VA_STK( _size ) \
                       ( (_size + 0) ? (_size + 0) : (OS_STACK_SIZE) )

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
 *   size            : (optional) size of task private stack (in bytes); default: OS_STACK_SIZE
 *
 ******************************************************************************/

#define             OS_TSK( tsk, prio, state, ... ) \
                    OS_WRK( tsk, prio, state, _VA_STK(__VA_ARGS__) )

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
 *   size            : (optional) size of task private stack (in bytes); default: OS_STACK_SIZE
 *
 ******************************************************************************/

#define             OS_TSK_DEF( tsk, prio, ... ) \
                    OS_WRK_DEF( tsk, prio, _VA_STK(__VA_ARGS__) )

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
 *   size            : (optional) size of task private stack (in bytes); default: OS_STACK_SIZE
 *
 * Note              : only available for compilers supporting the "constructor" function attribute or its equivalent
 *
 ******************************************************************************/

#define             OS_TSK_START( tsk, prio, ... ) \
                    OS_WRK_START( tsk, prio, _VA_STK(__VA_ARGS__) )

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
 *   size            : (optional) size of task private stack (in bytes); default: OS_STACK_SIZE
 *
 ******************************************************************************/

#define         static_TSK( tsk, prio, state, ... ) \
                static_WRK( tsk, prio, state, _VA_STK(__VA_ARGS__) )

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
 *   size            : (optional) size of task private stack (in bytes); default: OS_STACK_SIZE
 *
 ******************************************************************************/

#define         static_TSK_DEF( tsk, prio, ... ) \
                static_WRK_DEF( tsk, prio, _VA_STK(__VA_ARGS__) )

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
 *   size            : (optional) size of task private stack (in bytes); default: OS_STACK_SIZE
 *
 * Note              : only available for compilers supporting the "constructor" function attribute or its equivalent
 *
 ******************************************************************************/

#define         static_TSK_START( tsk, prio, ... ) \
                static_WRK_START( tsk, prio, _VA_STK(__VA_ARGS__) )

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
           (tsk_t[]) { WRK_INIT  ( prio, state, size ) }
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
 *   size            : (optional) size of task private stack (in bytes); default: OS_STACK_SIZE
 *
 * Return            : task object
 *
 * Note              : use only in 'C' code
 *
 ******************************************************************************/

#ifndef __cplusplus
#define                TSK_INIT( prio, state, ... ) \
                       WRK_INIT( prio, state, _VA_STK(__VA_ARGS__) )
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
 *   size            : (optional) size of task private stack (in bytes); default: OS_STACK_SIZE
 *
 * Return            : pointer to task object
 *
 * Note              : use only in 'C' code
 *
 ******************************************************************************/

#ifndef __cplusplus
#define                TSK_CREATE( prio, state, ... ) \
                       WRK_CREATE( prio, state, _VA_STK(__VA_ARGS__) )
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
 * Name              : tsk_sleepFor
 * Alias             : tsk_delay
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

__STATIC_INLINE
unsigned tsk_delay( cnt_t delay ) { return tsk_sleepFor(delay); }

/******************************************************************************
 *
 * Name              : tsk_sleepNext
 *
 * Description       : delay execution of current task for given duration of time
 *                     from the end of the previous countdown
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
unsigned tsk_sleepNext( cnt_t delay ) { return tmr_waitNext(&WAIT, delay); }

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
 * Class             : staticTaskT<>
 *
 * Description       : create and initialize complete work area for static task object
 *
 * Constructor parameters
 *   size            : size of task private stack (in bytes)
 *   prio            : initial task priority (any unsigned int value)
 *   state           : task state (initial task function) doesn't have to be noreturn-type
 *                     it will be executed into an infinite system-implemented loop
 *
 ******************************************************************************/

template<unsigned size_ = OS_STACK_SIZE>
struct staticTaskT : public __tsk
{
	 staticTaskT( const unsigned _prio, fun_t *_state ): __tsk _TSK_INIT(_prio, _state, stack_, size_) {}
	~staticTaskT( void ) { assert(__tsk::id == ID_STOPPED); }

	void     kill     ( void )            {        tsk_kill      (this);         }
	unsigned detach   ( void )            { return tsk_detach    (this);         }
	unsigned join     ( void )            { return tsk_join      (this);         }
	void     start    ( void )            {        tsk_start     (this);         }
	void     startFrom( fun_t  * _state ) {        tsk_startFrom (this, _state); }
	void     give     ( unsigned _flags ) {        tsk_give      (this, _flags); }
	void     giveISR  ( unsigned _flags ) {        tsk_giveISR   (this, _flags); }
	unsigned suspend  ( void )            { return tsk_suspend   (this);         }
	unsigned resume   ( void )            { return tsk_resume    (this);         }
	unsigned resumeISR( void )            { return tsk_resumeISR (this);         }

	unsigned prio     ( void )            { return __tsk::basic;                 }
	unsigned getPrio  ( void )            { return __tsk::basic;                 }
	bool     operator!( void )            { return __tsk::id == ID_STOPPED;      }

	private:
	stk_t stack_[SSIZE(size_)];
};

/* -------------------------------------------------------------------------- */

typedef staticTaskT<OS_STACK_SIZE> staticTask;

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

template<unsigned size_ = OS_STACK_SIZE>
struct TaskT : public staticTaskT<size_>
{
#if OS_FUNCTIONAL
	TaskT( const unsigned _prio, FUN_t _state ): staticTaskT<size_>(_prio, run_), fun_(_state) {}

	void  startFrom( FUN_t _state ) { fun_ = _state; tsk_startFrom(this, run_); }

	static
	void  run_( void ) { ((TaskT *)System.cur)->fun_(); }
	FUN_t fun_;
#else
	TaskT( const unsigned _prio, FUN_t _state ): staticTaskT<size_>(_prio, _state) {}
#endif
};

/* -------------------------------------------------------------------------- */

typedef TaskT<OS_STACK_SIZE> Task;

/******************************************************************************
 *
 * Class             : startTaskT<>
 *
 * Description       : create and initialize complete work area for autorun task object
 *
 * Constructor parameters
 *   size            : size of task private stack (in bytes)
 *   prio            : initial task priority (any unsigned int value)
 *   state           : task state (initial task function) doesn't have to be noreturn-type
 *                     it will be executed into an infinite system-implemented loop
 *
 ******************************************************************************/

template<unsigned size_ = OS_STACK_SIZE>
struct startTaskT : public TaskT<size_>
{
	startTaskT( const unsigned _prio, FUN_t _state ): TaskT<size_>(_prio, _state) { port_sys_init(); tsk_start(this); }
};

/* -------------------------------------------------------------------------- */

typedef startTaskT<OS_STACK_SIZE> startTask;

/******************************************************************************
 *
 * Namespace         : ThisTask
 *
 * Description       : provide set of functions for current task
 *
 ******************************************************************************/

namespace ThisTask
{
	static inline void     pass      ( void )                          {        tsk_pass      ();                      }
	static inline void     yield     ( void )                          {        tsk_yield     ();                      }
#if OS_FUNCTIONAL
	static inline void     flip      ( FUN_t    _state )               {        ((TaskT<>*)System.cur)->fun_ = _state;
	                                                                            tsk_flip      (TaskT<>::run_);         }
#else
	static inline void     flip      ( FUN_t    _state )               {        tsk_flip      (_state);                }
#endif
	static inline void     stop      ( void )                          {        tsk_stop      ();                      }
	static inline void     prio      ( unsigned _prio )                {        tsk_prio      (_prio);                 }
	static inline void     setPrio   ( unsigned _prio )                {        tsk_setPrio   (_prio);                 }
	static inline unsigned getPrio   ( void )                          { return tsk_getPrio   ();                      }
	static inline unsigned prio      ( void )                          { return tsk_getPrio   ();                      }

	static inline void     kill      ( void )                          {        tsk_kill      (System.cur);            }
	static inline unsigned detach    ( void )                          { return tsk_detach    (System.cur);            }
	static inline void     suspend   ( void )                          {        tsk_suspend   (System.cur);            }

	static inline unsigned waitFor   ( unsigned _flags, cnt_t _delay ) { return tsk_waitFor   (_flags, _delay);        }
	static inline unsigned waitUntil ( unsigned _flags, cnt_t _time )  { return tsk_waitUntil (_flags, _time);         }
	static inline unsigned wait      ( unsigned _flags )               { return tsk_wait      (_flags);                }
	static inline unsigned sleepFor  ( cnt_t    _delay )               { return tsk_sleepFor  (_delay);                }
	static inline unsigned sleepNext ( cnt_t    _delay )               { return tsk_sleepNext (_delay);                }
	static inline unsigned sleepUntil( cnt_t    _time )                { return tsk_sleepUntil(_time);                 }
	static inline unsigned sleep     ( void )                          { return tsk_sleep     ();                      }
	static inline unsigned delay     ( cnt_t    _delay )               { return tsk_delay     (_delay);                }
}

#endif//__cplusplus

/* -------------------------------------------------------------------------- */

#endif//__STATEOS_TSK_H
