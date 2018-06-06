/******************************************************************************

    @file    StateOS: osspinlock.h
    @author  Rajmund Szymanski
    @date    05.06.2018
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

#ifndef __STATEOS_SPN_H
#define __STATEOS_SPN_H

#include "oskernel.h"

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************
 *
 * Name              : spin lock
 *
 ******************************************************************************/

typedef struct __spn spn_t, * const spn_id;

struct __spn
{
	tsk_t  * queue; // next process in the DELAYED queue
	void   * res;   // allocated spin lock object's resource
	unsigned flag;  // spin lock's current value
};

/******************************************************************************
 *
 * Name              : _SPN_INIT
 *
 * Description       : create and initialize a spin lock object
 *
 * Parameters        : none
 *
 * Return            : spin lock object
 *
 * Note              : for internal use
 *
 ******************************************************************************/

#define               _SPN_INIT() { 0, 0, 0 }

/******************************************************************************
 *
 * Name              : OS_SPN
 *
 * Description       : define and initialize a spin lock object
 *
 * Parameters
 *   spn             : name of a pointer to spin lock object
 *
 ******************************************************************************/

#define             OS_SPN( spn )                     \
                       spn_t spn##__spn = _SPN_INIT(); \
                       spn_id spn = & spn##__spn

/******************************************************************************
 *
 * Name              : static_SPN
 *
 * Description       : define and initialize a static spin lock object
 *
 * Parameters
 *   spn             : name of a pointer to spin lock object
 *
 ******************************************************************************/

#define         static_SPN( spn )                     \
                static spn_t spn##__spn = _SPN_INIT(); \
                static spn_id spn = & spn##__spn

/******************************************************************************
 *
 * Name              : SPN_INIT
 *
 * Description       : create and initialize a spin lock object
 *
 * Parameters        : none
 *
 * Return            : spin lock object
 *
 * Note              : use only in 'C' code
 *
 ******************************************************************************/

#ifndef __cplusplus
#define                SPN_INIT() \
                      _SPN_INIT()
#endif

/******************************************************************************
 *
 * Name              : SPN_CREATE
 * Alias             : SPN_NEW
 *
 * Description       : create and initialize a spin lock object
 *
 * Parameters        : none
 *
 * Return            : pointer to spin lock object
 *
 * Note              : use only in 'C' code
 *
 ******************************************************************************/

#ifndef __cplusplus
#define                SPN_CREATE() \
             & (spn_t) SPN_INIT  ()
#define                SPN_NEW \
                       SPN_CREATE
#endif

/******************************************************************************
 *
 * Name              : spn_init
 *
 * Description       : initialize a spin lock object
 *
 * Parameters
 *   spn             : pointer to spin lock object
 *
 * Return            : none
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

void spn_init( spn_t *spn );

/******************************************************************************
 *
 * Name              : spn_create
 * Alias             : spn_new
 *
 * Description       : create and initialize a new spin lock object
 *
 * Parameters        : none
 *
 * Return            : pointer to spin lock object (spin lock successfully created)
 *   0               : spin lock not created (not enough free memory)
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

spn_t *spn_create( void );

__STATIC_INLINE
spn_t *spn_new( void ) { return spn_create(); }

/******************************************************************************
 *
 * Name              : spn_kill
 *
 * Description       : reset the spin lock object and wake up all waiting tasks with 'E_STOPPED' event value
 *
 * Parameters
 *   spn             : pointer to spin lock object
 *
 * Return            : none
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

void spn_kill( spn_t *spn );

/******************************************************************************
 *
 * Name              : spn_delete
 *
 * Description       : reset the spin lock object and free allocated resource
 *
 * Parameters
 *   spn             : pointer to spin lock object
 *
 * Return            : none
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

void spn_delete( spn_t *spn );

/******************************************************************************
 *
 * Name              : spn_waitUntil
 *
 * Description       : try to lock the spin lock object,
 *                     wait until given timepoint if the spin lock object can't be locked immediately
 *
 * Parameters
 *   spn             : pointer to spin lock object
 *   time            : timepoint value
 *
 * Return
 *   E_SUCCESS       : spin lock object was successfully locked
 *   E_STOPPED       : spin lock object was killed before the specified timeout expired
 *   E_TIMEOUT       : spin lock object was not locked before the specified timeout expired
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

unsigned spn_waitUntil( spn_t *spn, cnt_t time );

/******************************************************************************
 *
 * Name              : spn_waitFor
 *
 * Description       : try to lock the spin lock object,
 *                     wait for given duration of time if the spin lock object can't be locked immediately
 *
 * Parameters
 *   spn             : pointer to spin lock object
 *   delay           : duration of time (maximum number of ticks to wait for lock the spin lock object)
 *                     IMMEDIATE: don't wait if the spin lock object can't be locked immediately
 *                     INFINITE:  wait indefinitely until the spin lock object has been locked
 *
 * Return
 *   E_SUCCESS       : spin lock object was successfully locked
 *   E_STOPPED       : spin lock object was killed before the specified timeout expired
 *   E_TIMEOUT       : spin lock object was not locked before the specified timeout expired
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

unsigned spn_waitFor( spn_t *spn, cnt_t delay );

/******************************************************************************
 *
 * Name              : spn_wait
 *
 * Description       : try to lock the spin lock object,
 *                     wait indefinitely if the spin lock object can't be locked immediately
 *
 * Parameters
 *   spn             : pointer to spin lock object
 *
 * Return
 *   E_SUCCESS       : spin lock object was successfully locked
 *   E_STOPPED       : spin lock object was killed
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

__STATIC_INLINE
unsigned spn_wait( spn_t *spn ) { return spn_waitFor(spn, INFINITE); }

/******************************************************************************
 *
 * Name              : spn_take
 * ISR alias         : spn_takeISR
 *
 * Description       : try to lock the spin lock object,
 *                     don't wait if the spin lock object can't be locked immediately
 *
 * Parameters
 *   spn             : pointer to spin lock object
 *
 * Return
 *   E_SUCCESS       : spin lock object was successfully locked
 *   E_TIMEOUT       : spin lock object can't be locked immediately
 *
 * Note              : may be used both in thread and handler mode
 *
 ******************************************************************************/

unsigned spn_take( spn_t *spn );

__STATIC_INLINE
unsigned spn_takeISR( spn_t *spn ) { return spn_take(spn); }

/******************************************************************************
 *
 * Name              : spn_give
 * ISR alias         : spn_giveISR
 *
 * Description       : unlock the spin lock object
 *
 * Parameters
 *   spn             : pointer to spin lock object
 *
 * Return            : none
 *
 * Note              : may be used both in thread and handler mode
 *
 ******************************************************************************/

void spn_give( spn_t *spn );

__STATIC_INLINE
void spn_giveISR( spn_t *spn ) { spn_give(spn); }

#ifdef __cplusplus
}
#endif

/* -------------------------------------------------------------------------- */

#ifdef __cplusplus

/******************************************************************************
 *
 * Class             : SpinLock
 *
 * Description       : create and initialize a spin lock object
 *
 * Constructor parameters
 *                   : none
 *
 ******************************************************************************/

struct SpinLock : public __spn
{
	 explicit
	 SpinLock( void ): __spn _SPN_INIT() {}
	~SpinLock( void ) { assert(queue == nullptr); }

	void     kill     ( void )         {        spn_kill     (this);         }
	unsigned waitUntil( cnt_t _time  ) { return spn_waitUntil(this, _time);  }
	unsigned waitFor  ( cnt_t _delay ) { return spn_waitFor  (this, _delay); }
	unsigned wait     ( void )         { return spn_wait     (this);         }
	unsigned take     ( void )         { return spn_take     (this);         }
	unsigned takeISR  ( void )         { return spn_takeISR  (this);         }
	void     give     ( void )         {        spn_give     (this);         }
	void     giveISR  ( void )         {        spn_giveISR  (this);         }
};

#endif

/* -------------------------------------------------------------------------- */

#endif//__STATEOS_SPN_H
