/******************************************************************************

    @file    StateOS: os_mtx.h
    @author  Rajmund Szymanski
    @date    24.01.2018
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

/******************************************************************************
 *
 * Name              : mutex
 *                     like a POSIX pthread_mutex_t
 *
 ******************************************************************************/

typedef struct __mtx mtx_t, * const mtx_id;

struct __mtx
{
	tsk_t  * queue; // next process in the DELAYED queue
	void   * res;   // allocated mutex object's resource
	tsk_t  * owner; // owner task
	unsigned count; // mutex's curent value
	mtx_t  * list;  // list of mutexes held by owner
};

/******************************************************************************
 *
 * Name              : _MTX_INIT
 *
 * Description       : create and initialize a mutex object
 *
 * Parameters        : none
 *
 * Return            : mutex object
 *
 * Note              : for internal use
 *
 ******************************************************************************/

#define               _MTX_INIT() { 0, 0, 0, 0, 0 }

/******************************************************************************
 *
 * Name              : OS_MTX
 *
 * Description       : define and initialize a mutex object
 *
 * Parameters
 *   mtx             : name of a pointer to mutex object
 *
 ******************************************************************************/

#define             OS_MTX( mtx )                     \
                       mtx_t mtx##__mtx = _MTX_INIT(); \
                       mtx_id mtx = & mtx##__mtx

/******************************************************************************
 *
 * Name              : static_MTX
 *
 * Description       : define and initialize a static mutex object
 *
 * Parameters
 *   mtx             : name of a pointer to mutex object
 *
 ******************************************************************************/

#define         static_MTX( mtx )                     \
                static mtx_t mtx##__mtx = _MTX_INIT(); \
                static mtx_id mtx = & mtx##__mtx

/******************************************************************************
 *
 * Name              : MTX_INIT
 *
 * Description       : create and initialize a mutex object
 *
 * Parameters        : none
 *
 * Return            : mutex object
 *
 * Note              : use only in 'C' code
 *
 ******************************************************************************/

#ifndef __cplusplus
#define                MTX_INIT() \
                      _MTX_INIT()
#endif

/******************************************************************************
 *
 * Name              : MTX_CREATE
 * Alias             : MTX_NEW
 *
 * Description       : create and initialize a mutex object
 *
 * Parameters        : none
 *
 * Return            : pointer to mutex object
 *
 * Note              : use only in 'C' code
 *
 ******************************************************************************/

#ifndef __cplusplus
#define                MTX_CREATE() \
             & (mtx_t) MTX_INIT  ()
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
 *
 * Return            : none
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

void mtx_init( mtx_t *mtx );

/******************************************************************************
 *
 * Name              : mtx_create
 * Alias             : mtx_new
 *
 * Description       : create and initialize a new mutex object
 *
 * Parameters        : none
 *
 * Return            : pointer to mutex object (mutex successfully created)
 *   0               : mutex not created (not enough free memory)
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

mtx_t *mtx_create( void );

__STATIC_INLINE
mtx_t *mtx_new( void ) { return mtx_create(); }

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
 *   E_TIMEOUT       : mutex object was not locked before the specified timeout expired
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

unsigned mtx_waitUntil( mtx_t *mtx, cnt_t time );

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
 *   E_TIMEOUT       : mutex object was not locked before the specified timeout expired
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

unsigned mtx_waitFor( mtx_t *mtx, cnt_t delay );

/******************************************************************************
 *
 * Name              : mtx_wait
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
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

__STATIC_INLINE
unsigned mtx_wait( mtx_t *mtx ) { return mtx_waitFor(mtx, INFINITE); }

/******************************************************************************
 *
 * Name              : mtx_take
 *
 * Description       : try to lock the mutex object,
 *                     don't wait if the mutex object can't be locked immediately
 *
 * Parameters
 *   mtx             : pointer to mutex object
 *
 * Return
 *   E_SUCCESS       : mutex object was successfully locked
 *   E_TIMEOUT       : mutex object can't be locked immediately
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

__STATIC_INLINE
unsigned mtx_take( mtx_t *mtx ) { return mtx_waitFor(mtx, IMMEDIATE); }

/******************************************************************************
 *
 * Name              : mtx_give
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
 *                   : none
 *
 ******************************************************************************/

struct Mutex : public __mtx
{
	 explicit
	 Mutex( void ): __mtx _MTX_INIT() {}
	~Mutex( void ) { assert(owner == nullptr); }

	void     kill     ( void )         {        mtx_kill     (this);         }
	unsigned waitUntil( cnt_t _time  ) { return mtx_waitUntil(this, _time);  }
	unsigned waitFor  ( cnt_t _delay ) { return mtx_waitFor  (this, _delay); }
	unsigned wait     ( void )         { return mtx_wait     (this);         }
	unsigned take     ( void )         { return mtx_take     (this);         }
	unsigned give     ( void )         { return mtx_give     (this);         }
};

#endif

/* -------------------------------------------------------------------------- */

#endif//__STATEOS_MTX_H
