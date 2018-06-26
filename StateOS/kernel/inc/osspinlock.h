/******************************************************************************

    @file    StateOS: osspinlock.h
    @author  Rajmund Szymanski
    @date    26.06.2018
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

struct __spn
{
	volatile
	unsigned lock;
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

#define               _SPN_INIT() { 0 }

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

__STATIC_INLINE
void spn_init( spn_t *spn ) { spn->lock = 0; }

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

__STATIC_INLINE
void spn_kill( spn_t *spn ) { spn->lock = 0; }

/******************************************************************************
 *
 * Name              : spn_lock
 * Alias             : spn_wait
 *
 * Description       : lock the spin lock object,
 *                     wait indefinitely if the spin lock object can't be locked immediately
 *
 * Parameters
 *   spn             : pointer to spin lock object
 *
 * Return            : none
 *
 * Note              : use only in thread mode, do not use on single-core systems
 *                     be careful using this function, it can block the system
 *                     do not use blocking functions inside spn_lock / spn_unlock
 *
 ******************************************************************************/

#ifndef OS_SPN_ARCH

__STATIC_INLINE
void spn_lock( spn_t *spn )
{
	unsigned lock;
	do
	{
		port_sys_lock();
		lock = spn->lock;
		spn->lock = 1;
		port_sys_unlock();
	}
	while (lock);
}

#endif

__STATIC_INLINE
void spn_wait( spn_t *spn ) { spn_lock(spn); }

/******************************************************************************
 *
 * Name              : spn_unlock
 * Alias             : spn_give
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

__STATIC_INLINE
void spn_unlock( spn_t *spn ) { spn->lock = 0; }

__STATIC_INLINE
void spn_give( spn_t *spn ) { spn_unlock(spn); }

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

	void kill  ( void ) { spn_kill  (this); }
	void lock  ( void ) { spn_lock  (this); }
	void wait  ( void ) { spn_wait  (this); }
	void unlock( void ) { spn_unlock(this); }
	void give  ( void ) { spn_give  (this); }
};

#endif

/* -------------------------------------------------------------------------- */

#endif//__STATEOS_SPN_H
