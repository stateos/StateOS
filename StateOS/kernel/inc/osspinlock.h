/******************************************************************************

    @file    StateOS: osspinlock.h
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

#ifndef __STATEOS_SPN_H
#define __STATEOS_SPN_H

#include "oskernel.h"
#include "oscriticalsection.h"

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************
 *
 * Name              : spin lock
 *
 ******************************************************************************/

typedef volatile unsigned spn_t, * const spn_id;

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

#define               _SPN_INIT()   0

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
           (spn_t[]) { SPN_INIT  () }
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
void spn_init( spn_t *spn ) { *spn = 0; }

/******************************************************************************
 *
 * Name              : spn_lock
 *
 * Description       : lock the spin lock object,
 *                     wait indefinitely if the spin lock object can't be locked immediately
 *
 * Parameters
 *   spn             : pointer to spin lock object
 *
 * Return            : none
 *
 * Note              : do not use waiting functions inside spn_lock / spn_unlock
 *                     use only in thread mode
 *
 ******************************************************************************/

#ifdef  OS_MULTICORE
#define                spn_lock(spn) \
                       core_sys_lock(); port_spn_lock(spn)
#else
#define                spn_lock(spn) \
                       core_sys_lock()
#endif

/******************************************************************************
 *
 * Name              : spn_unlock
 *
 * Description       : unlock the spin lock object
 *
 * Parameters
 *   spn             : pointer to spin lock object
 *
 * Return            : none
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

#ifdef  OS_MULTICORE
#define                spn_unlock(spn) \
                       *(spn) = 0; core_sys_unlock()
#else
#define                spn_unlock(spn) \
                                   core_sys_unlock()
#endif

#ifdef __cplusplus
}
#endif

/* -------------------------------------------------------------------------- */

#ifdef __cplusplus

/******************************************************************************
 *
 * Class             : SpinLock
 *
 * Description       : use a spin lock object
 *
 * Constructor parameters
 *   spn             : pointer to spin lock object
 *
 ******************************************************************************/

struct SpinLock : private CriticalSection
{
#ifdef  OS_MULTICORE
	explicit
	SpinLock( spn_id _spn ): spin(_spn) {}

	void lock  ( void ) { port_spn_lock(spin); }
	void unlock( void ) { *spin = 0;           }

	private:
	spn_id spin;
#else
	explicit
	SpinLock( spn_id _spn ) { (void)_spn; }

	void lock  ( void ) {}
	void unlock( void ) {}
#endif
};

#endif

/* -------------------------------------------------------------------------- */

#endif//__STATEOS_SPN_H
