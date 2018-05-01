/******************************************************************************

    @file    StateOS: os.h
    @author  Rajmund Szymanski
    @date    01.05.2018
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

#ifndef __STATEOS_H
#define __STATEOS_H

#include "oskernel.h"
#include "inc/os_sig.h" // signal
#include "inc/os_evt.h" // event
#include "inc/os_flg.h" // flag
#include "inc/os_bar.h" // barrier
#include "inc/os_sem.h" // semaphore
#include "inc/os_mtx.h" // mutex
#include "inc/os_mut.h" // fast mutex
#include "inc/os_cnd.h" // condition variable
#include "inc/os_lst.h" // list
#include "inc/os_mem.h" // memory pool
#include "inc/os_stm.h" // stream buffer
#include "inc/os_msg.h" // message buffer
#include "inc/os_box.h" // mailbox queue
#include "inc/os_job.h" // job queue
#include "inc/os_evq.h" // event queue
#include "inc/os_tmr.h" // timer
#include "inc/os_tsk.h" // task

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************
 *
 * Name              : sys_init
 *
 * Description       : initialize system timer and enable services
 *
 * Parameters        : none
 *
 * Return            : none
 *
 * Note              : function port_sys_init should be invoked as a constructor
 *                   : otherwise, call sys_init as the first instruction in function main
 *
 ******************************************************************************/

__STATIC_INLINE
void sys_init( void ) { port_sys_init(); }

/******************************************************************************
 *
 * Name              : sys_alloc
 *
 * Description       : system malloc procedure with clearing the allocated memory
 *
 * Parameters
 *   size            : required size of the memory segment (in bytes)
 *
 * Return            : pointer to the beginning of allocated and cleared memory segment
 *   0               : memory segment not allocated (not enough free memory)
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

__STATIC_INLINE
void *sys_alloc( size_t size ) { return core_sys_alloc(size); }

/******************************************************************************
 *
 * Name              : sys_free
 *
 * Description       : system free procedure
 *
 * Parameters
 *   ptr             : pointer to a memory segment previously allocated with sys_alloc, xxx_create or xxx_new functions
 *
 * Return            : none
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

__STATIC_INLINE
void sys_free( void *ptr ) { core_sys_free(ptr); }

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

/******************************************************************************
 *
 * Name              : sys_time
 * ISR alias         : sys_timeISR
 *
 * Description       : return current value of system counter
 *
 * Parameters        : none
 *
 * Return            : current value of system counter
 *
 * Note              : may be used both in thread and handler mode
 *
 ******************************************************************************/

cnt_t sys_time( void );

__STATIC_INLINE
cnt_t sys_timeISR( void ) { return sys_time(); }

/******************************************************************************
 *
 * Name              : stk_assert
 *
 * Description       : check stack integrity of the current task
 *
 * Parameters        : none
 *
 * Return            : none
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

#define                stk_assert() \
                       core_stk_assert()

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

#endif//__STATEOS_H
