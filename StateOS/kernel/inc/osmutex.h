/******************************************************************************

    @file    StateOS: osmutex.h
    @author  Rajmund Szymanski
    @date    19.09.2018
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

#ifndef __STATEOS_MTX_H
#define __STATEOS_MTX_H

#include "oskernel.h"

#ifdef __cplusplus
extern "C" {
#endif

/* -------------------------------------------------------------------------- */

/////// mutex type
#define mtxNormal       0 // normal mutex
#define mtxErrorCheck   1 // error checking mutex
#define mtxRecursive    2 // recursive mutex
#define mtxTypeMASK   ( mtxNormal | mtxErrorCheck | mtxRecursive )

/////// mutex protocol
#define mtxPrioNone     0 // none
#define mtxPrioInherit  4 // priority inheritance mutex
#define mtxPrioProtect  8 // priority protected mutex (OCPP)
#define mtxPrioMASK   ( mtxPrioNone | mtxPrioInherit | mtxPrioProtect )

/////// mutex robustness
#define mtxStalled      0 // stalled mutex
#define mtxRobust      16 // robust mutex
#define mtxRobustMASK ( mtxStalled | mtxRobust )

#define mtxMASK       ( mtxTypeMASK + mtxPrioMASK + mtxRobustMASK )

#define mtxDefault      mtxNormal

/******************************************************************************
 *
 * Name              : mutex
 *                     like a POSIX pthread_mutex_t
 *
 ******************************************************************************/

typedef struct __mtx mtx_t, * const mtx_id;

struct __mtx
{
	obj_t    obj;   // object header

	tsk_t  * owner; // mutex owner
	unsigned mode;  // mutex mode: mutex type + mutex protocol + mutex robustness
	unsigned count; // current value of the mutex counter
	unsigned prio;  // mutex priority; unused if mtxPrioProtect protocol is not set
	mtx_t  * list;  // list of mutexes held by owner
};

/******************************************************************************
 *
 * Name              : _MTX_INIT
 *
 * Description       : create and initialize a mutex object
 *
 * Parameters
 *   mode            : mutex mode (mutex type + mutex protocol + mutex robustness)
 *                           type: mtxErrorCheck or mtxRecursive
 *                       protocol: mtxPrioNone or mtxPrioInherit or mtxPrioProtect
 *                     robustness: mtxStalled or mtxRobust
 *   prio            : mutex priority; unused if mtxPrioProtect protocol is not set
 *
 * Return            : mutex object
 *
 * Note              : for internal use
 *
 ******************************************************************************/

#define               _MTX_INIT( _mode, _prio ) { _OBJ_INIT(), 0, _mode, 0, _prio, 0 }

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
 *                           type: mtxErrorCheck or mtxRecursive
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
 *                           type: mtxErrorCheck or mtxRecursive
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
 *                           type: mtxErrorCheck or mtxRecursive
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
 *                           type: mtxErrorCheck or mtxRecursive
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
 *                           type: mtxErrorCheck or mtxRecursive
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
 *                           type: mtxErrorCheck or mtxRecursive
 *                       protocol: mtxPrioNone or mtxPrioInherit or mtxPrioProtect
 *                     robustness: mtxStalled or mtxRobust
 *   prio            : mutex priority; unused if mtxPrioProtect protocol is not set
 *
 * Return            : pointer to mutex object (mutex successfully created)
 *   0               : mutex not created (not enough free memory)
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

mtx_t *mtx_create( unsigned mode, unsigned prio );

__STATIC_INLINE
mtx_t *mtx_new( unsigned mode, unsigned prio ) { return mtx_create(mode, prio); }

/******************************************************************************
 *
 * Name              : mtx_kill
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

void mtx_kill( mtx_t *mtx );

/******************************************************************************
 *
 * Name              : mtx_delete
 *
 * Description       : reset the mutex object and free allocated resource
 *
 * Parameters
 *   mtx             : pointer to mutex object
 *
 * Return            : none
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

void mtx_delete( mtx_t *mtx );

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
 *   E_TIMEOUT       : mutex object can't be locked immediately or
 *                     locking is not possible due to priority protection
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
 *   E_STOPPED       : mutex object was killed before the specified timeout expired
 *   E_TIMEOUT       : mutex object was not locked before the specified timeout expired or
 *                     locking is not possible due to priority protection
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
 *   E_STOPPED       : mutex object was killed before the specified timeout expired
 *   E_TIMEOUT       : mutex object was not locked before the specified timeout expired or
 *                     locking is not possible due to priority protection
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
 *   E_STOPPED       : mutex object was killed
 *   E_TIMEOUT       : locking is not possible due to priority protection
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
 *   E_TIMEOUT       : mutex object can't be unlocked
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
 *                           type: mtxErrorCheck or mtxRecursive
 *                       protocol: mtxPrioNone or mtxPrioInherit or mtxPrioProtect
 *                     robustness: mtxStalled or mtxRobust
 *   prio            : mutex priority; unused if mtxPrioProtect protocol is not set
 *
 ******************************************************************************/

struct Mutex : public __mtx
{
	 Mutex( const unsigned _mode, const unsigned _prio = 0 ): __mtx _MTX_INIT(_mode, _prio) {}
	~Mutex( void ) { assert(__mtx::owner == nullptr); }

	void     kill     ( void )            {        mtx_kill     (this);         }
	void     setPrio  ( unsigned _prio )  {        mtx_setPrio  (this, _prio);  }
	void     prio     ( unsigned _prio )  {        mtx_prio     (this, _prio);  }
	unsigned getPrio  ( void )            { return mtx_getPrio  (this);         }
	unsigned prio     ( void )            { return mtx_getPrio  (this);         }
	unsigned take     ( void )            { return mtx_take     (this);         }
	unsigned tryLock  ( void )            { return mtx_tryLock  (this);         }
	unsigned waitFor  ( cnt_t    _delay ) { return mtx_waitFor  (this, _delay); }
	unsigned waitUntil( cnt_t    _time )  { return mtx_waitUntil(this, _time);  }
	unsigned wait     ( void )            { return mtx_wait     (this);         }
	unsigned lock     ( void )            { return mtx_lock     (this);         }
	unsigned give     ( void )            { return mtx_give     (this);         }
	unsigned unlock   ( void )            { return mtx_unlock   (this);         }
};

#endif//__cplusplus

/* -------------------------------------------------------------------------- */

#endif//__STATEOS_MTX_H
