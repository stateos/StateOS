/******************************************************************************

    @file    StateOS: oscriticalsection.h
    @author  Rajmund Szymanski
    @date    10.07.2018
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

#ifndef __STATEOS_CRI_H
#define __STATEOS_CRI_H

#include "oskernel.h"

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************
 *
 * Name              : sys_lock
 * ISR alias         : sys_lockISR
 *
 * Description       : disable interrupts / enter into critical section
 *
 * Parameters        : none
 *
 * Return            : none
 *
 * Note              : may be used both in thread and handler mode
 *
 ******************************************************************************/

#define                sys_lock() \
                       port_sys_lock()

#define                sys_lockISR() \
                       port_sys_lock()

/******************************************************************************
 *
 * Name              : sys_unlock
 * ISR alias         : sys_unlockISR
 *
 * Description       : enable interrupts / exit from critical section
 *
 * Parameters        : none
 *
 * Return            : none
 *
 * Note              : may be used both in thread and handler mode
 *
 ******************************************************************************/

#define                sys_unlock() \
                       port_sys_unlock()

#define                sys_unlockISR() \
                       port_sys_unlock()

#ifdef __cplusplus
}
#endif

/* -------------------------------------------------------------------------- */

#ifdef __cplusplus

/******************************************************************************
 *
 * Class             : CriticalSection
 *
 * Description       : create and initialize a critical section guard object
 *
 * Constructor parameters
 *                   : none
 *
 ******************************************************************************/

struct CriticalSection
{
	 CriticalSection( void ) { state = port_get_lock(); port_set_lock(); }
	~CriticalSection( void ) { port_put_lock(state); }

	private:
	lck_t state;
};

#endif

/* -------------------------------------------------------------------------- */

#endif//__STATEOS_CRI_H
