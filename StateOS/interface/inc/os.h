/******************************************************************************

    @file    StateOS: os.h
    @author  Rajmund Szymanski
    @date    24.02.2017
    @brief   This file contains definitions for StateOS.

 ******************************************************************************

    StateOS - Copyright (C) 2013 Rajmund Szymanski.

    This file is part of StateOS distribution.

    StateOS is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published
    by the Free Software Foundation; either version 3 of the License,
    or (at your option) any later version.

    StateOS is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program. If not, see <http://www.gnu.org/licenses/>.

 ******************************************************************************/

#ifndef __STATEOS_H
#define __STATEOS_H

#include <string.h>
#include <oskernel.h>
#include <os_sig.h> // signal
#include <os_evt.h> // event
#include <os_flg.h> // flag
#include <os_bar.h> // barrier
#include <os_sem.h> // semaphore
#include <os_mtx.h> // mutex
#include <os_mut.h> // fast mutex
#include <os_cnd.h> // condition variable
#include <os_lst.h> // list
#include <os_mem.h> // memory pool
#include <os_box.h> // mailbox queue
#include <os_msg.h> // message queue
#include <os_tmr.h> // timer
#include <os_tsk.h> // task

#ifdef __cplusplus
extern "C" {
#endif

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : sys_init                                                                                       *
 *                                                                                                                    *
 * Description       : initialize system timer and enabled services                                                   *
 *                                                                                                                    *
 * Parameters        : none                                                                                           *
 *                                                                                                                    *
 * Return            : none                                                                                           *
 *                                                                                                                    *
 * Note              : function port_sys_init should be invoked as a constructor                                      *
 *                   : otherwise, call sys_init as the first instruction in function main                             *
 *                                                                                                                    *
 **********************************************************************************************************************/

__STATIC_INLINE
void sys_init( void ) { port_sys_init(); }

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : sys_alloc                                                                                      *
 *                                                                                                                    *
 * Description       : system malloc procedure                                                                        *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   size            : size of the memory block (in bytes)                                                            *
 *                                                                                                                    *
 * Return            : pointer to the beginning of allocated block of memory                                          *
 *   0               : memory block not allocated (not enough free memory)                                            *
 *                                                                                                                    *
 * Note              : use only in thread mode                                                                        *
 *                                                                                                                    *
 **********************************************************************************************************************/

__STATIC_INLINE
void *sys_alloc( size_t size ) { return core_sys_alloc(size); }

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : sys_free                                                                                       *
 *                                                                                                                    *
 * Description       : system free procedure                                                                          *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   ptr             : pointer to a memory block previously allocated with sys_alloc or xxx_create                    *
 *                                                                                                                    *
 * Return            : none                                                                                           *
 *                                                                                                                    *
 * Note              : use only in thread mode                                                                        *
 *                                                                                                                    *
 **********************************************************************************************************************/

__STATIC_INLINE
void sys_free( void *ptr ) { core_sys_free(ptr); }

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : sys_lock                                                                                       *
 *                                                                                                                    *
 * Description       : disable interrupts / enter into critical section (in thread mode)                              *
 *                                                                                                                    *
 * Parameters        : none                                                                                           *
 *                                                                                                                    *
 * Return            : none                                                                                           *
 *                                                                                                                    *
 * Note              : use only in thread mode                                                                        *
 *                                                                                                                    *
 **********************************************************************************************************************/

#define                sys_lock() \
                       port_sys_lock()

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : sys_lockISR                                                                                    *
 *                                                                                                                    *
 * Description       : disable interrupts / enter into critical section (in handler mode)                             *
 *                                                                                                                    *
 * Parameters        : none                                                                                           *
 *                                                                                                                    *
 * Return            : none                                                                                           *
 *                                                                                                                    *
 * Note              : use only in handler mode                                                                       *
 *                                                                                                                    *
 **********************************************************************************************************************/

#define                sys_lockISR() \
                       port_sys_lock()

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : sys_unlock                                                                                     *
 *                                                                                                                    *
 * Description       : enable interrupts / exit from critical section (in thread mode)                                *
 *                                                                                                                    *
 * Parameters        : none                                                                                           *
 *                                                                                                                    *
 * Return            : none                                                                                           *
 *                                                                                                                    *
 * Note              : use only in thread mode                                                                        *
 *                                                                                                                    *
 **********************************************************************************************************************/

#define                sys_unlock() \
                       port_sys_unlock()

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : sys_unlockISR                                                                                  *
 *                                                                                                                    *
 * Description       : enable interrupts / exit from critical section (in handler mode)                               *
 *                                                                                                                    *
 * Parameters        : none                                                                                           *
 *                                                                                                                    *
 * Return            : none                                                                                           *
 *                                                                                                                    *
 * Note              : use only in handler mode                                                                       *
 *                                                                                                                    *
 **********************************************************************************************************************/

#define                sys_unlockISR() \
                       port_sys_unlock()

#ifdef __cplusplus
}
#endif

/* -------------------------------------------------------------------------- */

#ifdef __cplusplus

struct CriticalSection
{
	 CriticalSection( void ) { state = port_get_lock(); port_set_lock(); }
	~CriticalSection( void ) { port_put_lock(state); }

	private:
	unsigned state;
};

#endif

/* -------------------------------------------------------------------------- */

#endif//__STATEOS_H
