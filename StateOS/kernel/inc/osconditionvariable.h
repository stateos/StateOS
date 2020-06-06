/******************************************************************************

    @file    StateOS: osconditionvariable.h
    @author  Rajmund Szymanski
    @date    06.06.2020
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

#ifndef __STATEOS_CND_H
#define __STATEOS_CND_H

#include "oskernel.h"
#include "osclock.h"
#include "osmutex.h"

/* -------------------------------------------------------------------------- */

#define cndOne       ( false ) // notify one task
#define cndAll       ( true  ) // notify all tasks (broadcast)

/******************************************************************************
 *
 * Name              : condition variable
 *                     like a POSIX pthread_cond_t
 *
 ******************************************************************************/

typedef struct __cnd cnd_t, * const cnd_id;

struct __cnd
{
	obj_t    obj;   // object header
};

#ifdef __cplusplus
extern "C" {
#endif

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

#define               _CND_INIT() { _OBJ_INIT() }

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
 * Return            : pointer to condition variable object
 *   NULL            : object not created (not enough free memory)
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

cnd_t *cnd_create( void );

__STATIC_INLINE
cnd_t *cnd_new( void ) { return cnd_create(); }

/******************************************************************************
 *
 * Name              : cnd_reset
 * Alias             : cnd_kill
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

void cnd_reset( cnd_t *cnd );

__STATIC_INLINE
void cnd_kill( cnd_t *cnd ) { cnd_reset(cnd); }

/******************************************************************************
 *
 * Name              : cnd_destroy
 * Alias             : cnd_delete
 *
 * Description       : reset the condition variable object, wake up all waiting tasks with 'E_DELETED' event value and free allocated resource
 *
 * Parameters
 *   cnd             : pointer to condition variable object
 *
 * Return            : none
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

void cnd_destroy( cnd_t *cnd );

__STATIC_INLINE
void cnd_delete( cnd_t *cnd ) { cnd_destroy(cnd); }

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
 *   E_SUCCESS       : condition variable object was successfully signalled; owned mutex was locked again
 *   OWNERDEAD       : owned mutex was locked again but previous owner of the mutex was reseted
 *   E_FAILURE       : mutex object can't be unlocked
 *   E_STOPPED       : condition variable or mutex object was reseted
 *   E_DELETED       : condition variable or mutex object was deleted
 *   E_TIMEOUT       : condition variable object was not signalled before the specified timeout expired; owned mutex was locked again
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

unsigned cnd_waitFor( cnd_t *cnd, mtx_t *mtx, cnt_t delay );

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
 *   E_SUCCESS       : condition variable object was successfully signalled; owned mutex was locked again
 *   OWNERDEAD       : owned mutex was locked again but previous owner of the mutex was reseted
 *   E_FAILURE       : mutex object can't be unlocked
 *   E_STOPPED       : condition variable or mutex object was reseted
 *   E_DELETED       : condition variable or mutex object was deleted
 *   E_TIMEOUT       : condition variable object was not signalled before the specified timeout expired; owned mutex was locked again
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

unsigned cnd_waitUntil( cnd_t *cnd, mtx_t *mtx, cnt_t time );

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
 *   E_SUCCESS       : condition variable object was successfully signalled; owned mutex was locked again
 *   OWNERDEAD       : owned mutex was locked again but previous owner of the mutex was reseted
 *   E_FAILURE       : mutex object can't be unlocked
 *   E_STOPPED       : condition variable or mutex object was reseted
 *   E_DELETED       : condition variable or mutex object was deleted
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

/******************************************************************************
 *
 * Name              : cnd_notifyOne
 *
 * Description       : signal one task waiting on the condition variable
 *
 * Parameters
 *   cnd             : pointer to condition variable object
 *
 * Return            : none
 *
 * Note              : may be used both in thread and handler mode
 *
 ******************************************************************************/

__STATIC_INLINE
void cnd_notifyOne( cnd_t *cnd ) { cnd_give(cnd, false); }

/******************************************************************************
 *
 * Name              : cnd_notifyAll
 *
 * Description       : signal all tasks waiting on the condition variable
 *
 * Parameters
 *   cnd             : pointer to condition variable object
 *
 * Return            : none
 *
 * Note              : may be used both in thread and handler mode
 *
 ******************************************************************************/

__STATIC_INLINE
void cnd_notifyAll( cnd_t *cnd ) { cnd_give(cnd, true); }

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
	constexpr
	ConditionVariable( void ): __cnd _CND_INIT() {}

	ConditionVariable( ConditionVariable&& ) = default;
	ConditionVariable( const ConditionVariable& ) = delete;
	ConditionVariable& operator=( ConditionVariable&& ) = delete;
	ConditionVariable& operator=( const ConditionVariable& ) = delete;

	~ConditionVariable( void ) { assert(__cnd::obj.queue == nullptr); }

#if __cplusplus >= 201402
	using Ptr = std::unique_ptr<ConditionVariable>;
#else
	using Ptr = ConditionVariable *;
#endif

/******************************************************************************
 *
 * Name              : ConditionVariable::Create
 *
 * Description       : create and initialize dynamic object with manageable resources
 *
 * Parameters        : none
 *
 * Return            : std::unique_pointer / pointer to ConditionVariable object
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

#if __cplusplus >= 201402
	static
	std::unique_ptr<ConditionVariable> Create( void )
	{
		auto cnd = reinterpret_cast<ConditionVariable *>(malloc(sizeof(ConditionVariable)));
		if (cnd != nullptr)
		{
			new (cnd) ConditionVariable();
			cnd->__cnd::obj.res = cnd;
		}
		return std::unique_ptr<ConditionVariable>(cnd);
	}
#else
	static
	ConditionVariable *Create( void )
	{
		return static_cast<ConditionVariable *>(cnd_create());
	}
#endif

	void reset    ( void )                        {        cnd_reset    (this); }
	void kill     ( void )                        {        cnd_kill     (this); }
	void destroy  ( void )                        {        cnd_destroy  (this); }
	template<typename T>
	uint waitFor  ( mtx_t *_mtx, const T _delay ) { return cnd_waitFor  (this,  _mtx, Clock::count(_delay)); }
	template<typename T>
	uint waitFor  ( mtx_t &_mtx, const T _delay ) { return cnd_waitFor  (this, &_mtx, Clock::count(_delay)); }
	template<typename T>
	uint waitUntil( mtx_t *_mtx, const T _time )  { return cnd_waitUntil(this,  _mtx, Clock::until(_time)); }
	template<typename T>
	uint waitUntil( mtx_t &_mtx, const T _time )  { return cnd_waitUntil(this, &_mtx, Clock::until(_time)); }
	uint wait     ( mtx_t *_mtx )                 { return cnd_wait     (this,  _mtx); }
	uint wait     ( mtx_t &_mtx )                 { return cnd_wait     (this, &_mtx); }
	void give     ( bool   _all = cndAll )        {        cnd_give     (this,  _all); }
	void giveISR  ( bool   _all = cndAll )        {        cnd_giveISR  (this,  _all); }
	void notifyOne( void )                        {        cnd_notifyOne(this); }
	void notifyAll( void )                        {        cnd_notifyAll(this); }
};

#endif//__cplusplus

/* -------------------------------------------------------------------------- */

#endif//__STATEOS_CND_H
