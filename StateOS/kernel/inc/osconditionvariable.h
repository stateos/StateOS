/******************************************************************************

    @file    StateOS: osconditionvariable.h
    @author  Rajmund Szymanski
    @date    15.07.2018
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

#ifndef __STATEOS_CND_H
#define __STATEOS_CND_H

#include "oskernel.h"
#include "osmutex.h"

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************
 *
 * Name              : condition variable
 *                     like a POSIX pthread_cond_t
 *
 ******************************************************************************/

typedef struct __cnd cnd_t, * const cnd_id;

struct __cnd
{
	tsk_t  * queue; // next process in the DELAYED queue
	void   * res;   // allocated condition variable object's resource
};

/* -------------------------------------------------------------------------- */

#define cndOne       ( false ) // notify one task
#define cndAll       ( true  ) // notify all tasks

/******************************************************************************
 *
 * Name              : _CND_INIT
 *
 * Description       : create and initialize a condition variable object
 *
 * Parameters        : none
 *
 * Return            : condition variable object
 *
 * Note              : for internal use
 *
 ******************************************************************************/

#define               _CND_INIT() { 0, 0 }

/******************************************************************************
 *
 * Name              : OS_CND
 *
 * Description       : define and initialize a condition variable object
 *
 * Parameters
 *   cnd             : name of a pointer to condition variable object
 *
 ******************************************************************************/

#define             OS_CND( cnd )                     \
                       cnd_t cnd##__cnd = _CND_INIT(); \
                       cnd_id cnd = & cnd##__cnd

/******************************************************************************
 *
 * Name              : static_CND
 *
 * Description       : define and initialize a static condition variable object
 *
 * Parameters
 *   cnd             : name of a pointer to condition variable object
 *
 ******************************************************************************/

#define         static_CND( cnd )                     \
                static cnd_t cnd##__cnd = _CND_INIT(); \
                static cnd_id cnd = & cnd##__cnd

/******************************************************************************
 *
 * Name              : CND_INIT
 *
 * Description       : create and initialize a condition variable object
 *
 * Parameters        : none
 *
 * Return            : condition variable object
 *
 * Note              : use only in 'C' code
 *
 ******************************************************************************/

#ifndef __cplusplus
#define                CND_INIT() \
                      _CND_INIT()
#endif

/******************************************************************************
 *
 * Name              : CND_CREATE
 * Alias             : CND_NEW
 *
 * Description       : create and initialize a condition variable object
 *
 * Parameters        : none
 *
 * Return            : pointer to condition variable object
 *
 * Note              : use only in 'C' code
 *
 ******************************************************************************/

#ifndef __cplusplus
#define                CND_CREATE() \
           (cnd_t[]) { CND_INIT  () }
#define                CND_NEW \
                       CND_CREATE
#endif

/******************************************************************************
 *
 * Name              : cnd_init
 *
 * Description       : initialize a condition variable object
 *
 * Parameters
 *   cnd             : pointer to condition variable object
 *
 * Return            : none
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

void cnd_init( cnd_t *cnd );

/******************************************************************************
 *
 * Name              : cnd_create
 * Alias             : cnd_new
 *
 * Description       : create and initialize a new condition variable object
 *
 * Parameters        : none
 *
 * Return            : pointer to condition variable object (condition variable successfully created)
 *   0               : condition variable not created (not enough free memory)
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

cnd_t *cnd_create( void );

__STATIC_INLINE
cnd_t *cnd_new( void ) { return cnd_create(); }

/******************************************************************************
 *
 * Name              : cnd_kill
 *
 * Description       : reset the condition variable object and wake up all waiting tasks with 'E_STOPPED' event value
 *
 * Parameters
 *   cnd             : pointer to condition variable object
 *
 * Return            : none
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

void cnd_kill( cnd_t *cnd );

/******************************************************************************
 *
 * Name              : cnd_delete
 *
 * Description       : reset the condition variable object and free allocated resource
 *
 * Parameters
 *   cnd             : pointer to condition variable object
 *
 * Return            : none
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

void cnd_delete( cnd_t *cnd );

/******************************************************************************
 *
 * Name              : cnd_waitUntil
 *
 * Description       : wait until given timepoint on the condition variable releasing the currently owned mutex,
 *                     and finally lock the mutex again
 *
 * Parameters
 *   cnd             : pointer to condition variable object
 *   mtx             : currently owned mutex
 *   time            : timepoint value
 *
 * Return
 *   E_SUCCESS       : condition variable object was successfully signalled and owned mutex locked again
 *   E_STOPPED       : condition variable object was killed before the specified timeout expired
 *   E_TIMEOUT       : condition variable object was not signalled before the specified timeout expired
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

unsigned cnd_waitUntil( cnd_t *cnd, mtx_t *mtx, cnt_t time );

/******************************************************************************
 *
 * Name              : cnd_waitFor
 *
 * Description       : wait for given duration of time on the condition variable releasing the currently owned mutex,
 *                     and finally lock the mutex again
 *
 * Parameters
 *   cnd             : pointer to condition variable object
 *   mtx             : currently owned mutex
 *   delay           : duration of time (maximum number of ticks to wait on the condition variable object)
 *                     IMMEDIATE: don't wait on the condition variable object
 *                     INFINITE:  wait indefinitely on the condition variable object
 *
 * Return
 *   E_SUCCESS       : condition variable object was successfully signalled and owned mutex locked again
 *   E_STOPPED       : condition variable object was killed before the specified timeout expired
 *   E_TIMEOUT       : condition variable object was not signalled before the specified timeout expired
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

unsigned cnd_waitFor( cnd_t *cnd, mtx_t *mtx, cnt_t delay );

/******************************************************************************
 *
 * Name              : cnd_wait
 *
 * Description       : wait indefinitely on the condition variable releasing the currently owned mutex,
 *                     and finally lock the mutex again
 *
 * Parameters
 *   cnd             : pointer to condition variable object
 *   mtx             : currently owned mutex
 *
 * Return
 *   E_SUCCESS       : condition variable object was successfully signalled and owned mutex locked again
 *   E_STOPPED       : condition variable object was killed
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

__STATIC_INLINE
unsigned cnd_wait( cnd_t *cnd, mtx_t *mtx ) { return cnd_waitFor(cnd, mtx, INFINITE); }

/******************************************************************************
 *
 * Name              : cnd_give
 * ISR alias         : cnd_giveISR
 *
 * Description       : signal one or all tasks that are waiting on the condition variable
 *
 * Parameters
 *   cnd             : pointer to condition variable object
 *   all             : signal receiver
 *                     cndOne: notify one task that is waiting on the condition variable
 *                     cndAll: notify all tasks that are waiting on the condition variable
 *
 * Return            : none
 *
 * Note              : may be used both in thread and handler mode
 *
 ******************************************************************************/

void cnd_give( cnd_t *cnd, bool all );

__STATIC_INLINE
void cnd_giveISR( cnd_t *cnd, bool all ) { cnd_give(cnd, all); }

#ifdef __cplusplus
}
#endif

/* -------------------------------------------------------------------------- */

#ifdef __cplusplus

/******************************************************************************
 *
 * Class             : ConditionVariable
 *
 * Description       : create and initialize a condition variable object
 *
 * Constructor parameters
 *                   : none
 *
 ******************************************************************************/

struct ConditionVariable : public __cnd
{
	 explicit
	 ConditionVariable( void ): __cnd _CND_INIT() {}
	~ConditionVariable( void ) { assert(queue == nullptr); }

	void     kill     ( void )                      {        cnd_kill     (this);               }
	unsigned waitUntil( mtx_t *_mtx, cnt_t _time  ) { return cnd_waitUntil(this, _mtx, _time);  }
	unsigned waitFor  ( mtx_t *_mtx, cnt_t _delay ) { return cnd_waitFor  (this, _mtx, _delay); }
	unsigned wait     ( mtx_t *_mtx )               { return cnd_wait     (this, _mtx);         }
	void     give     ( bool   _all = cndAll )      {        cnd_give     (this, _all);         }
	void     giveISR  ( bool   _all = cndAll )      {        cnd_giveISR  (this, _all);         }
};

#endif

/* -------------------------------------------------------------------------- */

#endif//__STATEOS_CND_H
