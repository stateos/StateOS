/******************************************************************************

    @file    StateOS: ostask.h
    @author  Rajmund Szymanski
    @date    12.05.2020
    @brief   This file contains definitions for StateOS.

 ******************************************************************************

   Copyright (c) 2018-2020 Rajmund Szymanski. All rights reserved.

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

/* -------------------------------------------------------------------------- */

#define STK_SIZE( size ) \
    ALIGNED_SIZE( size, stk_t )

#define STK_OVER( size ) \
         ALIGNED( size, stk_t )

#define STK_CROP( base, size ) \
         LIMITED( (intptr_t)base + (intptr_t)size, stk_t )

/* -------------------------------------------------------------------------- */

#define JOINABLE     ((tsk_t *)((uintptr_t)0))     // task in joinable state
#define DETACHED     ((tsk_t *)((uintptr_t)0 - 1)) // task in detached state

/******************************************************************************
 *
 * Name              : task (thread)
 *
 ******************************************************************************/

struct __tsk
{
	hdr_t    hdr;   // timer / task header

	fun_t  * state; // task state (initial task function, doesn't have to be noreturn-type)
	cnt_t    start; // inherited from timer
	cnt_t    delay; // inherited from timer
	cnt_t    slice;	// time slice

	tsk_t ** back;  // previous object in the BLOCKED queue
	stk_t  * stack; // base of stack
	size_t   size;  // size of stack (in bytes)
	void   * sp;    // current stack pointer

	unsigned basic; // basic priority
	unsigned prio;  // current priority

	tsk_t  * join;  // joinable state
	tsk_t ** guard; // BLOCKED queue for the pending process

	unsigned event; // wakeup event

	struct {
	mtx_t  * list;  // list of mutexes held
	mtx_t  * tree;  // tree of tasks waiting for mutexes
	}        mtx;

	struct {
	unsigned sigset;// pending signals
	act_t  * action;// signal handler
	struct {
	void   * sp;
	tsk_t ** guard;
	}        backup;
	}        sig;

	union  {

	struct {
	unsigned*data;
	}        evt;   // temporary data used by event object

	struct {
	unsigned sigset;
	}        sig;   // temporary data used by signal object

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
	union  {
	unsigned out;
	unsigned*in;
	}        data;
	}        evq;   // temporary data used by event queue object

	struct {
	union  {
	fun_t  * out;
	fun_t ** in;
	}        data;
	}        job;   // temporary data used by job queue object

	}        tmp;
#if defined(__ARMCC_VERSION) && !defined(__MICROLIB)
	char     libspace[96];
	#define _TSK_EXTRA { 0 }
#else
	#define _TSK_EXTRA
#endif
};

#ifdef __cplusplus
extern "C" {
#endif

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

#define               _TSK_INIT( _prio, _state, _stack, _size )                                                   \
                       { _HDR_INIT(), _state, 0, 0, 0, NULL, _stack, _size, NULL, _prio, _prio, JOINABLE, NULL, 0, \
                       { NULL, NULL }, { 0, NULL, { NULL, NULL } }, { { NULL } }, _TSK_EXTRA }

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
#define               _TSK_STACK( _size ) (stk_t[STK_SIZE(_size)]){ 0 }
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

#define               _VA_STK( _size ) ((_size + 0) ? (_size + 0) : (OS_STACK_SIZE))

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

#define             OS_WRK( tsk, prio, state, size )                                  \
                       struct { tsk_t tsk; stk_t buf[STK_SIZE( size )]; } tsk##__wrk = \
                       { _TSK_INIT( prio, state, tsk##__wrk.buf, size ), { 0 } };       \
                       tsk_id tsk = & tsk##__wrk.tsk

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

#define         static_WRK( tsk, prio, state, size )                                  \
                static struct { tsk_t tsk; stk_t buf[STK_SIZE( size )]; } tsk##__wrk = \
                       { _TSK_INIT( prio, state, tsk##__wrk.buf, size ), { 0 } };       \
                static tsk_id tsk = & tsk##__wrk.tsk

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
 * Alias             : cur_task
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

__STATIC_INLINE
tsk_t *cur_task( void ) { return System.cur; }

/******************************************************************************
 *
 * Name              : wrk_init
 *
 * Description       : initialize complete work area for task object
 *
 * Parameters
 *   tsk             : pointer to task object
 *   prio            : initial task priority (any unsigned int value)
 *   state           : task state (initial task function) doesn't have to be noreturn-type
 *                     it will be executed into an infinite system-implemented loop
 *   stack           : base of task's private stack storage
 *   size            : size of task private stack (in bytes)
 *
 * Return            : none
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

void wrk_init( tsk_t *tsk, unsigned prio, fun_t *state, stk_t *stack, size_t size );

/******************************************************************************
 *
 * Name              : tsk_init
 *
 * Description       : initialize complete work area for task object
 *                     and start the task
 *
 * Parameters
 *   tsk             : pointer to task object
 *   prio            : initial task priority (any unsigned int value)
 *   state           : task state (initial task function) doesn't have to be noreturn-type
 *                     it will be executed into an infinite system-implemented loop
 *   stack           : base of task's private stack storage
 *   size            : size of task private stack (in bytes)
 *
 * Return            : none
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

void tsk_init( tsk_t *tsk, unsigned prio, fun_t *state, stk_t *stack, size_t size );

/******************************************************************************
 *
 * Name              : wrk_create
 * Alias             : wrk_new
 *
 * Description       : create and initialize complete work area for task object
 *                     and start the task
 *
 * Parameters
 *   prio            : initial task priority (any unsigned int value)
 *   state           : task state (initial task function) doesn't have to be noreturn-type
 *                     it will be executed into an infinite system-implemented loop
 *   size            : size of task private stack (in bytes)
 *
 * Return            : pointer to task object
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

tsk_t *wrk_create( unsigned prio, fun_t *state, size_t size );

__STATIC_INLINE
tsk_t *wrk_new( unsigned prio, fun_t *state, size_t size ) { return wrk_create(prio, state, size); }

/******************************************************************************
 *
 * Name              : wrk_detached
 *
 * Description       : create and initialize complete work area for detached task object
 *                     and start the task
 *
 * Parameters
 *   prio            : initial task priority (any unsigned int value)
 *   state           : task state (initial task function) doesn't have to be noreturn-type
 *                     it will be executed into an infinite system-implemented loop
 *   size            : size of task private stack (in bytes)
 *
 * Return            : pointer to task object
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

tsk_t *wrk_detached( unsigned prio, fun_t *state, size_t size );

/******************************************************************************
 *
 * Name              : tsk_create
 * Alias             : tsk_new
 *
 * Description       : create and initialize a new task object with default stack size
 *                     and start the task
 *
 * Parameters
 *   prio            : initial task priority (any unsigned int value)
 *   state           : task state (initial task function) doesn't have to be noreturn-type
 *                     it will be executed into an infinite system-implemented loop
 *
 * Return            : pointer to task object
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
 * Name              : tsk_detached
 *
 * Description       : create and initialize a new detached task object with default stack size
 *                     and start the task
 *
 * Parameters
 *   prio            : initial task priority (any unsigned int value)
 *   state           : task state (initial task function) doesn't have to be noreturn-type
 *                     it will be executed into an infinite system-implemented loop
 *
 * Return            : pointer to task object
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

__STATIC_INLINE
tsk_t *tsk_detached( unsigned prio, fun_t *state ) { return wrk_detached(prio, state, OS_STACK_SIZE); }

/******************************************************************************
 *
 * Name              : thd_create
 *
 * Description       : create and initialize complete work area for task object
 *                     with defined state: JOINABLE / DETACHED (don't start the task)
 *
 * Parameters
 *   prio            : initial task priority (any unsigned int value)
 *   state           : task state (initial task function) doesn't have to be noreturn-type
 *                     it will be executed into an infinite system-implemented loop
 *   size            : size of task private stack (in bytes)
 *   joinable        : JOINABLE / DETACHED
 *
 * Return            : pointer to task object
 *
 * Note              : for internal use
 *
 ******************************************************************************/

tsk_t *thd_create( unsigned prio, fun_t *state, size_t size, tsk_t *joinable );

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
 * Name              : tsk_detach
 *
 * Description       : detach given task
 *
 * Parameters
 *   tsk             : pointer to task object
 *
 * Return
 *   E_SUCCESS       : given task was successfully detached
 *   E_FAILURE       : given task cannot be detached
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

unsigned tsk_detach( tsk_t *tsk );

/******************************************************************************
 *
 * Name              : cur_detach
 *
 * Description       : detach the current task
 *
 * Parameters        : none
 *
 * Return
 *   E_SUCCESS       : current task was successfully detached
 *   E_FAILURE       : current task cannot be detached
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

__STATIC_INLINE
unsigned cur_detach( void ) { return tsk_detach(System.cur); }

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
 *   E_SUCCESS       : given task has stopped (stop / exit)
 *   E_STOPPED       : given task was reseted (reset / kill)
 *   E_DELETED       : given task was deleted (delete / destroy)
 *   E_FAILURE       : given task cannot be joined
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

unsigned tsk_join( tsk_t *tsk );

/******************************************************************************
 *
 * Name              : tsk_stop
 * Alias             : tsk_exit
 *
 * Description       : stop the current task and remove it from READY queue
 *                     detached task is destroyed, otherwise
 *                     function doesn't destroy the stack storage,
 *                     and all allocated resources will remain intact until
 *                     joining, destroying or restarting the task
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

__STATIC_INLINE
void tsk_exit( void ) { tsk_stop(); }

/******************************************************************************
 *
 * Name              : tsk_reset
 * Alias             : tsk_kill
 *
 * Description       : reset the task object and remove it from READY/BLOCKED queue
 *
 * Parameters
 *   tsk             : pointer to task object
 *
 * Return
 *   E_SUCCESS       : given task was already inactive or has been reseted
 *   E_FAILURE       : given task cannot be reseted
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

unsigned tsk_reset( tsk_t *tsk );

__STATIC_INLINE
unsigned tsk_kill( tsk_t *tsk ) { return tsk_reset(tsk); }

/******************************************************************************
 *
 * Name              : cur_reset
 * Alias             : cur_kill
 *
 * Description       : reset the current task and remove it from READY/BLOCKED queue
 *
 * Parameters        : none
 *
 * Return
 *   E_SUCCESS       : current task has been reseted
 *   E_FAILURE       : current task cannot be reseted
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

__STATIC_INLINE
unsigned cur_reset( void ) { return tsk_reset(System.cur); }

__STATIC_INLINE
unsigned cur_kill( void ) { return cur_reset(); }

/******************************************************************************
 *
 * Name              : tsk_destroy
 * Alias             : tsk_delete
 *
 * Description       : reset the task object and free allocated resources
 *
 * Parameters
 *   tsk             : pointer to task object
 *
 * Return
 *   E_SUCCESS       : given task has been destroyed
 *   E_FAILURE       : given task cannot be destroyed
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

unsigned tsk_destroy( tsk_t *tsk );

__STATIC_INLINE
unsigned tsk_delete( tsk_t *tsk ) { return tsk_destroy(tsk); }

/******************************************************************************
 *
 * Name              : cur_destroy
 * Alias             : cur_delete
 *
 * Description       : reset the current task and free allocated resources
 *
 * Parameters        : none
 *
 * Return
 *   E_SUCCESS       : current task has been destroyed
 *   E_FAILURE       : current task cannot be destroyed
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

__STATIC_INLINE
unsigned cur_destroy( void ) { return tsk_destroy(System.cur); }

__STATIC_INLINE
unsigned cur_delete( void ) { return cur_destroy(); }

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
 * Name              : tsk_setPrio
 * Alias             : tsk_prio
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

void tsk_setPrio( unsigned prio );

__STATIC_INLINE
void tsk_prio( unsigned prio ) { tsk_setPrio(prio); }

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

unsigned tsk_getPrio( void );

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
 * Return            : none
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

void tsk_sleepFor( cnt_t delay );

__STATIC_INLINE
void tsk_delay( cnt_t delay ) { tsk_sleepFor(delay); }

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
 * Return            : none
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

void tsk_sleepNext( cnt_t delay );

/******************************************************************************
 *
 * Name              : tsk_sleepUntil
 *
 * Description       : delay execution of current task until given timepoint
 *
 * Parameters
 *   time            : timepoint value
 *
 * Return            : none
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

void tsk_sleepUntil( cnt_t time );

/******************************************************************************
 *
 * Name              : tsk_sleep
 *
 * Description       : delay indefinitely execution of current task
 *                     execution of the task can be resumed
 *
 * Parameters        : none
 *
 * Return            : none
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

__STATIC_INLINE
void tsk_sleep( void ) { tsk_sleepFor(INFINITE); }

/******************************************************************************
 *
 * Name              : tsk_suspend
 *
 * Description       : delay indefinitely execution of given task
 *                     execution of the task can be resumed
 *
 * Parameters
 *   tsk             : pointer to task object
 *
 * Return
 *   E_SUCCESS       : task was successfully suspended
 *   E_FAILURE       : task cannot be suspended
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

unsigned tsk_suspend( tsk_t *tsk );

/******************************************************************************
 *
 * Name              : cur_suspend
 *
 * Description       : delay indefinitely execution of current task
 *                     execution of the task can be resumed
 *
 * Parameters        : none
 *
 * Return            : none
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

__STATIC_INLINE
void cur_suspend( void ) { tsk_suspend(System.cur); }

/******************************************************************************
 *
 * Name              : tsk_resume
 * ISR alias         : tsk_resumeISR
 *
 * Description       : resume execution of given suspended task
 *                     only suspended or indefinitely blocked tasks can be resumed
 *
 * Parameters
 *   tsk             : pointer to suspended task object
 *
 * Return
 *   E_SUCCESS       : task was successfully resumed
 *   E_FAILURE       : task cannot be resumed
 *
 * Note              : may be used both in thread and handler mode
 *
 ******************************************************************************/

unsigned tsk_resume( tsk_t *tsk );

__STATIC_INLINE
unsigned tsk_resumeISR( tsk_t *tsk ) { return tsk_resume(tsk); }

/******************************************************************************
 *
 * Name              : tsk_give
 * Alias             : tsk_signal
 *
 * Description       : send given signal to the task
 *
 * Parameters
 *   tsk             : pointer to the task object
 *   signo           : signal number
 *
 * Return            : none
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

void tsk_give( tsk_t *tsk, unsigned signo );

__STATIC_INLINE
void tsk_signal( tsk_t *tsk, unsigned signo ) { tsk_give(tsk, signo); }

/******************************************************************************
 *
 * Name              : cur_give
 * Alias             : cur_signal
 *
 * Description       : send given signal to the current task
 *
 * Parameters
 *   signo           : signal number
 *
 * Return            : none
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

__STATIC_INLINE
void cur_give( unsigned signo ) { tsk_give(System.cur, signo); }

__STATIC_INLINE
void cur_signal( unsigned signo ) { cur_give(signo); }

/******************************************************************************
 *
 * Name              : tsk_action
 *
 * Description       : set given function as a signal handler
 *
 * Parameters
 *   tsk             : pointer to the task object
 *   action          : signal handler
 *
 * Return            : none
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

void tsk_action( tsk_t *tsk, act_t *action );

/******************************************************************************
 *
 * Name              : cur_action
 *
 * Description       : set given function as a signal handler for current task
 *
 * Parameters
 *   signo           : signal number
 *   action          : signal handler
 *
 * Return            : none
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

__STATIC_INLINE
void cur_action( act_t *action ) { tsk_action(System.cur, action); }

#ifdef __cplusplus
}
#endif

/* -------------------------------------------------------------------------- */

#ifdef __cplusplus

/******************************************************************************
 *
 * Class             : baseStack
 *
 * Description       : create base class for stack storage object
 *
 * Constructor parameters
 *   size            : size of stack (in bytes)
 *
 * Note              : for internal use
 *
 ******************************************************************************/

template<size_t size_ = OS_STACK_SIZE>
struct baseStack
{
	stk_t stack_[ STK_SIZE(size_) ];
};

/******************************************************************************
 *
 * Class             : baseTask
 *
 * Description       : create and initialize base class for task objects
 *
 * Constructor parameters
 *   prio            : initial task priority (any unsigned int value)
 *   state           : task state (initial task function) doesn't have to be noreturn-type
 *                     it will be executed into an infinite system-implemented loop
 *   stack           : base of task's private stack storage
 *   size            : size of task private stack (in bytes)
 *
 * Note              : for internal use
 *
 ******************************************************************************/

struct baseTask : public __tsk
{
#if OS_FUNCTIONAL
	template<class T>
	baseTask( const unsigned _prio, const T _state, stk_t * const _stack, const size_t _size ) : __tsk _TSK_INIT(_prio, fun_, _stack, _size), fun{_state} {}
#else
	baseTask( const unsigned _prio, fun_t * _state, stk_t * const _stack, const size_t _size ) : __tsk _TSK_INIT(_prio, _state, _stack, _size) {}
#endif

	void     start    ( void )             {        tsk_start    (this);          }
#if OS_FUNCTIONAL
	template<class T>
	void     startFrom( const T  _state )  {        new (&fun) Fun_t(_state);
	                                                tsk_startFrom(this, fun_);    }
#else
	void     startFrom( fun_t *  _state )  {        tsk_startFrom(this, _state);  }
#endif
	unsigned detach   ( void )             { return tsk_detach   (this);          }
	unsigned join     ( void )             { return tsk_join     (this);          }
	unsigned reset    ( void )             { return tsk_reset    (this);          }
	unsigned kill     ( void )             { return tsk_kill     (this);          }
	unsigned destroy  ( void )             { return tsk_destroy  (this);          }
	unsigned prio     ( void )             { return __tsk::basic;                 }
	unsigned getPrio  ( void )             { return __tsk::basic;                 }
	unsigned suspend  ( void )             { return tsk_suspend  (this);          }
	unsigned resume   ( void )             { return tsk_resume   (this);          }
	unsigned resumeISR( void )             { return tsk_resumeISR(this);          }
	void     give     ( unsigned _signo )  {        tsk_give     (this, _signo);  }
	void     signal   ( unsigned _signo )  {        tsk_signal   (this, _signo);  }
#if OS_FUNCTIONAL
	template<class T>
	void     action   ( const T  _action ) {        new (&act) Act_t(_action);
	                                                tsk_action   (this, act_);    }
#else
	void     action   ( act_t *  _action ) {        tsk_action   (this, _action); }
#endif
	bool     operator!( void )             { return __tsk::hdr.id == ID_STOPPED;  }

#if OS_FUNCTIONAL
	static
	void     fun_     ( void )             {        static_cast<baseTask *>(tsk_this())->fun(); }
	Fun_t    fun;
	static
	void     act_     ( unsigned _signo )  {        static_cast<baseTask *>(tsk_this())->act(_signo); }
	Act_t    act;
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

template<size_t size_ = OS_STACK_SIZE>
struct TaskT : public baseTask, public baseStack<size_>
{
	template<class T>
	TaskT( const unsigned _prio, const T _state ):           baseTask{_prio, _state, baseStack<size_>::stack_, size_} {}
#if OS_FUNCTIONAL
	template<typename F, typename... A>
	TaskT( const unsigned _prio, F&& _state, A&&... _args ): baseTask{_prio, std::bind(std::forward<F>(_state), std::forward<A>(_args)...), baseStack<size_>::stack_, size_} {}
#endif

	TaskT( TaskT<size_>&& ) = default;
	TaskT( const TaskT<size_>& ) = delete;
	TaskT<size_>& operator=( TaskT<size_>&& ) = delete;
	TaskT<size_>& operator=( const TaskT<size_>& ) = delete;

	~TaskT( void ) { assert(__tsk::hdr.id == ID_STOPPED); }

/******************************************************************************
 *
 * Name              : TaskT<>::Make
 *
 * Description       : create and initialize static undetachable task
 *
 * Parameters
 *   size            : size of task private stack (in bytes)
 *   prio            : initial task priority (any unsigned int value)
 *   state           : task state (initial task function) doesn't have to be noreturn-type
 *                     it will be executed into an infinite system-implemented loop
 *   args            : arguments for state function
 *
 * Return            : TaskT<> object
 *
 ******************************************************************************/

	template<class T>
	static
	TaskT<size_> Make( const unsigned _prio, const T _state )
	{
		return { _prio, _state };
	}

#if OS_FUNCTIONAL
	template<typename F, typename... A>
	static
	TaskT<size_> Make( const unsigned _prio, F&& _state, A&&... _args )
	{
		return { _prio, std::bind(std::forward<F>(_state), std::forward<A>(_args)...) };
	}
#endif

/******************************************************************************
 *
 * Name              : TaskT<>::Start
 *
 * Description       : create, initialize and start static undetachable task
 *
 * Parameters
 *   size            : size of task private stack (in bytes)
 *   prio            : initial task priority (any unsigned int value)
 *   state           : task state (initial task function) doesn't have to be noreturn-type
 *                     it will be executed into an infinite system-implemented loop
 *   args            : arguments for state function
 *
 * Return            : TaskT<> object
 *
 ******************************************************************************/

	template<class T>
	static
	TaskT<size_> Start( const unsigned _prio, const T _state )
	{
		TaskT<size_> tsk { _prio, _state };
		tsk.start();
		return tsk;
	}

#if OS_FUNCTIONAL
	template<typename F, typename... A>
	static
	TaskT<size_> Start( const unsigned _prio, F&& _state, A&&... _args )
	{
		TaskT<size_> tsk { _prio, std::bind(std::forward<F>(_state), std::forward<A>(_args)...) };
		tsk.start();
		return tsk;
	}
#endif

/******************************************************************************
 *
 * Name              : TaskT<>::Create
 *
 * Description       : create, initialize and start dynamic detachable task
 *                     with manageable resources
 *
 * Parameters
 *   size            : size of task private stack (in bytes)
 *   prio            : initial task priority (any unsigned int value)
 *   state           : task state (initial task function) doesn't have to be noreturn-type
 *                     it will be executed into an infinite system-implemented loop
 *   args            : arguments for state function
 *
 * Return            : pointer to TaskT<> object
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

	template<class T>
	static
	TaskT<size_> *Create( const unsigned _prio, const T _state )
	{
#if OS_FUNCTIONAL
		auto tsk = reinterpret_cast<TaskT<size_> *>(sys_alloc(sizeof(TaskT<size_>)));
		new (tsk) TaskT<size_>(_prio, _state);
		tsk->__tsk::hdr.obj.res = tsk;
		tsk->start();
		return tsk;
#else
		return reinterpret_cast<TaskT<size_> *>(wrk_create(_prio, _state, size_));
#endif
	}

#if OS_FUNCTIONAL
	template<typename F, typename... A>
	static
	TaskT<size_> *Create( const unsigned _prio, F&& _state, A&&... _args )
	{
		auto tsk = reinterpret_cast<TaskT<size_> *>(sys_alloc(sizeof(TaskT<size_>)));
		new (tsk) TaskT<size_>(_prio, std::bind(std::forward<F>(_state), std::forward<A>(_args)...));
		tsk->__tsk::hdr.obj.res = tsk;
		tsk->start();
		return tsk;
	}
#endif

/******************************************************************************
 *
 * Name              : TaskT<>::Detached
 *
 * Description       : create, initialize and start dynamic detached task
 *                     with manageable resources
 *
 * Parameters
 *   size            : size of task private stack (in bytes)
 *   prio            : initial task priority (any unsigned int value)
 *   state           : task state (initial task function) doesn't have to be noreturn-type
 *                     it will be executed into an infinite system-implemented loop
 *   args            : arguments for state function
 *
 * Return            : pointer to TaskT<> object
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

	template<class T>
	static
	TaskT<size_> *Detached( const unsigned _prio, const T _state )
	{
#if OS_FUNCTIONAL
		auto tsk = reinterpret_cast<TaskT<size_> *>(sys_alloc(sizeof(TaskT<size_>)));
		new (tsk) TaskT<size_>(_prio, _state);
		tsk->__tsk::hdr.obj.res = tsk;
		tsk->__tsk::join = DETACHED;
		tsk->start();
		return tsk;
#else
		return reinterpret_cast<TaskT<size_> *>(wrk_detached(_prio, _state, size_));
#endif
	}

#if OS_FUNCTIONAL
	template<typename F, typename... A>
	static
	TaskT<size_> *Detached( const unsigned _prio, F&& _state, A&&... _args )
	{
		auto tsk = reinterpret_cast<TaskT<size_> *>(sys_alloc(sizeof(TaskT<size_>)));
		new (tsk) TaskT<size_>(_prio, std::bind(std::forward<F>(_state), std::forward<A>(_args)...));
		tsk->__tsk::hdr.obj.res = tsk;
		tsk->__tsk::join = DETACHED;
		tsk->start();
		return tsk;
	}
#endif

};

/* -------------------------------------------------------------------------- */

using Task = TaskT<OS_STACK_SIZE>;

/******************************************************************************
 *
 * Namespace         : ThisTask
 *
 * Description       : provide set of functions for current task
 *
 ******************************************************************************/

namespace ThisTask
{
	template<class T = baseTask>
	static inline T  *     current   ( void )             { return static_cast<T *>(tsk_this()); }
	static inline unsigned detach    ( void )             { return cur_detach    ();        }
	static inline void     stop      ( void )             {        tsk_stop      ();        }
	static inline void     exit      ( void )             {        tsk_exit      ();        }
	static inline unsigned reset     ( void )             { return cur_reset     ();        }
	static inline unsigned kill      ( void )             { return cur_kill      ();        }
	static inline unsigned destroy   ( void )             { return cur_destroy   ();        }
	static inline void     yield     ( void )             {        tsk_yield     ();        }
	static inline void     pass      ( void )             {        tsk_pass      ();        }
#if OS_FUNCTIONAL
	template<class T>
	static inline void     flip      ( const T  _state )  {        new (&ThisTask::current()->fun) Fun_t(_state);
	                                                               tsk_flip      (baseTask::fun_); }
#else
	static inline void     flip      ( fun_t *  _state )  {        tsk_flip      (_state);  }
#endif
	static inline void     setPrio   ( unsigned _prio )   {        tsk_setPrio   (_prio);   }
	static inline void     prio      ( unsigned _prio )   {        tsk_prio      (_prio);   }
	static inline unsigned getPrio   ( void )             { return tsk_getPrio   ();        }
	static inline unsigned prio      ( void )             { return tsk_getPrio   ();        }
	static inline void     sleepFor  ( cnt_t    _delay )  {        tsk_sleepFor  (_delay);  }
	static inline void     sleepNext ( cnt_t    _delay )  {        tsk_sleepNext (_delay);  }
	static inline void     sleepUntil( cnt_t    _time )   {        tsk_sleepUntil(_time);   }
	static inline void     sleep     ( void )             {        tsk_sleep     ();        }
	static inline void     delay     ( cnt_t    _delay )  {        tsk_delay     (_delay);  }
	static inline void     suspend   ( void )             {        cur_suspend   ();        }
	static inline void     give      ( unsigned _signo )  {        cur_give      (_signo);  }
	static inline void     signal    ( unsigned _signo )  {        cur_signal    (_signo);  }
#if OS_FUNCTIONAL
	template<class T>
	static inline void     action    ( const T  _action ) {        new (&ThisTask::current()->act) Act_t(_action);
	                                                               cur_action    (baseTask::act_); }
#else
	static inline void     action    ( act_t *  _action ) {        cur_action    (_action); }
#endif
}

#endif//__cplusplus

/* -------------------------------------------------------------------------- */

#endif//__STATEOS_TSK_H
