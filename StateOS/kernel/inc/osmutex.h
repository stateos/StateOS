/******************************************************************************

    @file    StateOS: osmutex.h
    @author  Rajmund Szymanski
    @date    27.05.2020
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

#ifndef __STATEOS_MTX_H
#define __STATEOS_MTX_H

#include "oskernel.h"
#include "osalloc.h"
#include "osclock.h"

/* -------------------------------------------------------------------------- */

/////// mutex type
#define mtxNormal        0U // normal mutex
#define mtxErrorCheck    1U // error checking mutex
#define mtxRecursive     2U // recursive mutex
#define mtxDefault      mtxNormal
#define mtxTypeMASK   ( mtxNormal | mtxErrorCheck | mtxRecursive )

/////// mutex protocol
#define mtxPrioNone      0U // none
#define mtxPrioInherit   4U // priority inheritance mutex
#define mtxPrioProtect   8U // priority protected mutex (OCPP)
#define mtxPrioMASK   ( mtxPrioNone | mtxPrioInherit | mtxPrioProtect )

/////// mutex robustness
#define mtxStalled       0U // stalled mutex
#define mtxRobust       16U // robust mutex

/////// inconsistency of robust mutex
#define mtxInconsistent 32U // inconsistent mutex

#define mtxMASK       ( mtxTypeMASK | mtxPrioMASK | mtxRobust | mtxInconsistent )

/* -------------------------------------------------------------------------- */

#define MTX_LIMIT    ( 0U-1 )

/******************************************************************************
 *
 * Name              : mutex
 *                     like a POSIX pthread_mutex_t
 *
 ******************************************************************************/

struct __mtx
{
	obj_t    obj;   // object header

	tsk_t  * owner; // mutex owner
	unsigned mode;  // mutex mode: mutex type + mutex protocol + mutex robustness
	unsigned count; // current value of the mutex counter
	unsigned prio;  // mutex priority; unused if mtxPrioProtect protocol is not set
	mtx_t  * list;  // list of mutexes held by owner
};

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************
 *
 * Name              : _MTX_INIT
 *
 * Description       : create and initialize a mutex object
 *
 * Parameters
 *   mode            : mutex mode (mutex type + mutex protocol + mutex robustness)
 *                           type: mtxNormal or mtxErrorCheck or mtxRecursive
 *                       protocol: mtxPrioNone or mtxPrioInherit or mtxPrioProtect
 *                     robustness: mtxStalled or mtxRobust
 *   prio            : mutex priority; unused if mtxPrioProtect protocol is not set
 *
 * Return            : mutex object
 *
 * Note              : for internal use
 *
 ******************************************************************************/

#define               _MTX_INIT( _mode, _prio ) { _OBJ_INIT(), NULL, _mode, 0, _prio, NULL }

/******************************************************************************
 *
 * Name              : _VA_MTX
 *
 * Description       : calculate mutex priority from optional parameter
 *                     default: 0
 *
 * Note              : for internal use
 *
 ******************************************************************************/

#define               _VA_MTX( _prio ) ( _prio + 0 )

/******************************************************************************
 *
 * Name              : OS_MTX
 *
 * Description       : define and initialize a mutex object
 *
 * Parameters
 *   mtx             : name of a pointer to mutex object
 *   mode            : mutex mode (mutex type + mutex protocol + mutex robustness)
 *                           type: mtxNormal or mtxErrorCheck or mtxRecursive
 *                       protocol: mtxPrioNone or mtxPrioInherit or mtxPrioProtect
 *                     robustness: mtxStalled or mtxRobust
 *   prio            : mutex priority; unused if mtxPrioProtect protocol is not set
 *
 ******************************************************************************/

#define             OS_MTX( mtx, mode, ... )                                    \
                       mtx_t mtx##__mtx = _MTX_INIT(mode, _VA_MTX(__VA_ARGS__)); \
                       mtx_id mtx = & mtx##__mtx

/******************************************************************************
 *
 * Name              : static_MTX
 *
 * Description       : define and initialize a static mutex object
 *
 * Parameters
 *   mtx             : name of a pointer to mutex object
 *   mode            : mutex mode (mutex type + mutex protocol + mutex robustness)
 *                           type: mtxNormal or mtxErrorCheck or mtxRecursive
 *                       protocol: mtxPrioNone or mtxPrioInherit or mtxPrioProtect
 *                     robustness: mtxStalled or mtxRobust
 *   prio            : mutex priority; unused if mtxPrioProtect protocol is not set
 *
 ******************************************************************************/

#define         static_MTX( mtx, mode, ... )                                    \
                static mtx_t mtx##__mtx = _MTX_INIT(mode, _VA_MTX(__VA_ARGS__)); \
                static mtx_id mtx = & mtx##__mtx

/******************************************************************************
 *
 * Name              : MTX_INIT
 *
 * Description       : create and initialize a mutex object
 *
 * Parameters
 *   mode            : mutex mode (mutex type + mutex protocol + mutex robustness)
 *                           type: mtxNormal or mtxErrorCheck or mtxRecursive
 *                       protocol: mtxPrioNone or mtxPrioInherit or mtxPrioProtect
 *                     robustness: mtxStalled or mtxRobust
 *   prio            : mutex priority; unused if mtxPrioProtect protocol is not set
 *
 * Return            : mutex object
 *
 * Note              : use only in 'C' code
 *
 ******************************************************************************/

#ifndef __cplusplus
#define                MTX_INIT( mode, ... ) \
                      _MTX_INIT( mode, _VA_MTX(__VA_ARGS__) )
#endif

/******************************************************************************
 *
 * Name              : MTX_CREATE
 * Alias             : MTX_NEW
 *
 * Description       : create and initialize a mutex object
 *
 * Parameters
 *   mode            : mutex mode (mutex type + mutex protocol + mutex robustness)
 *                           type: mtxNormal or mtxErrorCheck or mtxRecursive
 *                       protocol: mtxPrioNone or mtxPrioInherit or mtxPrioProtect
 *                     robustness: mtxStalled or mtxRobust
 *   prio            : mutex priority; unused if mtxPrioProtect protocol is not set
 *
 * Return            : pointer to mutex object
 *
 * Note              : use only in 'C' code
 *
 ******************************************************************************/

#ifndef __cplusplus
#define                MTX_CREATE( mode, ... ) \
           (mtx_t[]) { MTX_INIT  ( mode, _VA_MTX(__VA_ARGS__) ) }
#define                MTX_NEW \
                       MTX_CREATE
#endif

/******************************************************************************
 *
 * Name              : mtx_init
 *
 * Description       : initialize a mutex object
 *
 * Parameters
 *   mtx             : pointer to mutex object
 *   mode            : mutex mode (mutex type + mutex protocol + mutex robustness)
 *                           type: mtxNormal or mtxErrorCheck or mtxRecursive
 *                       protocol: mtxPrioNone or mtxPrioInherit or mtxPrioProtect
 *                     robustness: mtxStalled or mtxRobust
 *   prio            : mutex priority; unused if mtxPrioProtect protocol is not set
 *
 * Return            : none
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

void mtx_init( mtx_t *mtx, unsigned mode, unsigned prio );

/******************************************************************************
 *
 * Name              : mtx_create
 * Alias             : mtx_new
 *
 * Description       : create and initialize a new mutex object
 *
 * Parameters
 *   mode            : mutex mode (mutex type + mutex protocol + mutex robustness)
 *                           type: mtxNormal or mtxErrorCheck or mtxRecursive
 *                       protocol: mtxPrioNone or mtxPrioInherit or mtxPrioProtect
 *                     robustness: mtxStalled or mtxRobust
 *   prio            : mutex priority; unused if mtxPrioProtect protocol is not set
 *
 * Return            : pointer to mutex object
 *   NULL            : object not created (not enough free memory)
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

mtx_t *mtx_create( unsigned mode, unsigned prio );

__STATIC_INLINE
mtx_t *mtx_new( unsigned mode, unsigned prio ) { return mtx_create(mode, prio); }

/******************************************************************************
 *
 * Name              : mtx_reset
 * Alias             : mtx_kill
 *
 * Description       : reset the mutex object and wake up all waiting tasks with 'E_STOPPED' event value
 *
 * Parameters
 *   mtx             : pointer to mutex object
 *
 * Return            : none
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

void mtx_reset( mtx_t *mtx );

__STATIC_INLINE
void mtx_kill( mtx_t *mtx ) { mtx_reset(mtx); }

/******************************************************************************
 *
 * Name              : mtx_destroy
 * Alias             : mtx_delete
 *
 * Description       : reset the mutex object, wake up all waiting tasks with 'E_DELETED' event value and free allocated resource
 *
 * Parameters
 *   mtx             : pointer to mutex object
 *
 * Return            : none
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

void mtx_destroy( mtx_t *mtx );

__STATIC_INLINE
void mtx_delete( mtx_t *mtx ) { mtx_destroy(mtx); }

/******************************************************************************
 *
 * Name              : mtx_setPrio
 * Alias             : mtx_prio
 *
 * Description       : set priority of given mutex
 *
 * Parameters
 *   mtx             : pointer to mutex object
 *   prio            : new mutex priority value
 *
 * Return            : none
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

void mtx_setPrio( mtx_t *mtx, unsigned prio );

__STATIC_INLINE
void mtx_prio( mtx_t *mtx, unsigned prio ) { mtx_setPrio(mtx, prio); }

/******************************************************************************
 *
 * Name              : mtx_getPrio
 *
 * Description       : get priority of given mutex
 *
 * Parameters
 *   mtx             : pointer to mutex object
 *
 * Return            : mutex priority
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

unsigned mtx_getPrio( mtx_t *mtx );

/******************************************************************************
 *
 * Name              : mtx_take
 * Alias             : mtx_tryLock
 *
 * Description       : try to lock the mutex object,
 *                     don't wait if the mutex object can't be locked immediately
 *
 * Parameters
 *   mtx             : pointer to mutex object
 *
 * Return
 *   E_SUCCESS       : mutex object was successfully locked
 *   OWNERDEAD       : mutex object was successfully locked, previous owner was reseted
 *   E_FAILURE       : mutex object can't be locked
 *   E_TIMEOUT       : mutex object can't be locked immediately, try again
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

unsigned mtx_take( mtx_t *mtx );

__STATIC_INLINE
unsigned mtx_tryLock( mtx_t *mtx ) { return mtx_take(mtx); }

/******************************************************************************
 *
 * Name              : mtx_waitFor
 *
 * Description       : try to lock the mutex object,
 *                     wait for given duration of time if the mutex object can't be locked immediately
 *
 * Parameters
 *   mtx             : pointer to mutex object
 *   delay           : duration of time (maximum number of ticks to wait for lock the mutex object)
 *                     IMMEDIATE: don't wait if the mutex object can't be locked immediately
 *                     INFINITE:  wait indefinitely until the mutex object has been locked
 *
 * Return
 *   E_SUCCESS       : mutex object was successfully locked
 *   OWNERDEAD       : mutex object was successfully locked, previous owner was reseted
 *   E_FAILURE       : mutex object can't be locked
 *   E_STOPPED       : mutex object was reseted before the specified timeout expired
 *   E_DELETED       : mutex object was deleted before the specified timeout expired
 *   E_TIMEOUT       : mutex object was not locked before the specified timeout expired
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

unsigned mtx_waitFor( mtx_t *mtx, cnt_t delay );

/******************************************************************************
 *
 * Name              : mtx_waitUntil
 *
 * Description       : try to lock the mutex object,
 *                     wait until given timepoint if the mutex object can't be locked immediately
 *
 * Parameters
 *   mtx             : pointer to mutex object
 *   time            : timepoint value
 *
 * Return
 *   E_SUCCESS       : mutex object was successfully locked
 *   OWNERDEAD       : mutex object was successfully locked, previous owner was reseted
 *   E_FAILURE       : mutex object can't be locked
 *   E_STOPPED       : mutex object was reseted before the specified timeout expired
 *   E_DELETED       : mutex object was deleted before the specified timeout expired
 *   E_TIMEOUT       : mutex object was not locked before the specified timeout expired
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

unsigned mtx_waitUntil( mtx_t *mtx, cnt_t time );

/******************************************************************************
 *
 * Name              : mtx_wait
 * Alias             : mtx_lock
 *
 * Description       : try to lock the mutex object,
 *                     wait indefinitely if the mutex object can't be locked immediately
 *
 * Parameters
 *   mtx             : pointer to mutex object
 *
 * Return
 *   E_SUCCESS       : mutex object was successfully locked
 *   OWNERDEAD       : mutex object was successfully locked, previous owner was reseted
 *   E_FAILURE       : mutex object can't be locked
 *   E_STOPPED       : mutex object was reseted
 *   E_DELETED       : mutex object was deleted
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

__STATIC_INLINE
unsigned mtx_wait( mtx_t *mtx ) { return mtx_waitFor(mtx, INFINITE); }

__STATIC_INLINE
unsigned mtx_lock( mtx_t *mtx ) { return mtx_wait(mtx); }

/******************************************************************************
 *
 * Name              : mtx_give
 * Alias             : mtx_unlock
 *
 * Description       : try to unlock the mutex object (only owner task can unlock mutex object),
 *                     don't wait if the mutex object can't be unlocked
 *
 * Parameters
 *   mtx             : pointer to mutex object
 *
 * Return
 *   E_SUCCESS       : mutex object was successfully unlocked
 *   E_FAILURE       : mutex object can't be unlocked
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

unsigned mtx_give( mtx_t *mtx );

__STATIC_INLINE
unsigned mtx_unlock( mtx_t *mtx ) { return mtx_give(mtx); }

#ifdef __cplusplus
}
#endif

/* -------------------------------------------------------------------------- */

#ifdef __cplusplus

/******************************************************************************
 *
 * Class             : Mutex
 *
 * Description       : create and initialize a mutex object
 *
 * Constructor parameters
 *   mode            : mutex mode (mutex type + mutex protocol + mutex robustness)
 *                           type: mtxNormal or mtxErrorCheck or mtxRecursive
 *                       protocol: mtxPrioNone or mtxPrioInherit or mtxPrioProtect
 *                     robustness: mtxStalled or mtxRobust
 *   prio            : mutex priority; unused if mtxPrioProtect protocol is not set
 *
 ******************************************************************************/

struct Mutex : public __mtx
{
	constexpr
	Mutex( const unsigned _mode, const unsigned _prio = 0 ): __mtx _MTX_INIT(_mode, _prio) {}

	Mutex( Mutex&& ) = default;
	Mutex( const Mutex& ) = delete;
	Mutex& operator=( Mutex&& ) = delete;
	Mutex& operator=( const Mutex& ) = delete;

	~Mutex( void ) { assert(__mtx::owner == nullptr); }

/******************************************************************************
 *
 * Name              : Mutex::Create
 *
 * Description       : create dynamic object with manageable resources
 *
 * Parameters
 *   mode            : mutex mode (mutex type + mutex protocol + mutex robustness)
 *                           type: mtxNormal or mtxErrorCheck or mtxRecursive
 *                       protocol: mtxPrioNone or mtxPrioInherit or mtxPrioProtect
 *                     robustness: mtxStalled or mtxRobust
 *   prio            : mutex priority; unused if mtxPrioProtect protocol is not set
 *
 * Return            : pointer to Mutex object
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

	static
	Mutex *Create( const unsigned _mode, const unsigned _prio = 0 )
	{
#if __cplusplus >= 201402
		auto mtx = reinterpret_cast<Mutex *>(sys_alloc(sizeof(Mutex)));
		if (mtx != nullptr)
		{
			new (mtx) Mutex(_mode, _prio);
			mtx->__mtx::obj.res = mtx;
		}
		return mtx;
#else
		return static_cast<Mutex *>(mtx_create(_mode, _prio));
#endif
	}

	void reset    ( void )           {        mtx_reset    (this); }
	void kill     ( void )           {        mtx_kill     (this); }
	void destroy  ( void )           {        mtx_destroy  (this); }
	void setPrio  ( unsigned _prio ) {        mtx_setPrio  (this, _prio); }
	void prio     ( unsigned _prio ) {        mtx_prio     (this, _prio); }
	uint getPrio  ( void )           { return mtx_getPrio  (this); }
	uint prio     ( void )           { return mtx_getPrio  (this); }
	uint take     ( void )           { return mtx_take     (this); }
	uint tryLock  ( void )           { return mtx_tryLock  (this); }
	template<typename T>
	uint waitFor  ( const T _delay ) { return mtx_waitFor  (this, Clock::count(_delay)); }
	template<typename T>
	uint waitUntil( const T _time )  { return mtx_waitUntil(this, Clock::until(_time)); }
	uint wait     ( void )           { return mtx_wait     (this); }
	uint lock     ( void )           { return mtx_lock     (this); }
	uint give     ( void )           { return mtx_give     (this); }
	uint unlock   ( void )           { return mtx_unlock   (this); }
};

#endif//__cplusplus

/* -------------------------------------------------------------------------- */

#endif//__STATEOS_MTX_H
