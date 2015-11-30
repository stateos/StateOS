/******************************************************************************

    @file    State Machine OS: os.h
    @author  Rajmund Szymanski
    @date    23.11.2015
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

#pragma once

#include <oskernel.h>
#include <os_evt.h> // event
#include <os_flg.h> // flag
#include <os_bar.h> // barrier
#include <os_sem.h> // semaphore
#include <os_mtx.h> // mutex
#include <os_cnd.h> // condition variable
#include <os_box.h> // mail box queue
#include <os_msg.h> // message queue
#include <os_tmr.h> // timer
#include <os_tsk.h> // task

#ifdef __cplusplus
extern "C" {
#endif

/* -------------------------------------------------------------------------- */

// zainicjowanie i uruchomienie zegara systemowego (system timer)
// funkcja port_sys_init jest standardowo wywoływana jako konstruktor
static inline
void   sys_init( void )                   { port_sys_init(); }

/* -------------------------------------------------------------------------- */

#ifdef __cplusplus
}
#endif

/* -------------------------------------------------------------------------- */

// disable interrupts / enter into critical section
#define sys_lock       port_sys_lock

#define sys_lockISR    port_sys_lock

// enable interrupts / exit from critical section
#define sys_unlock     port_sys_unlock

#define sys_unlockISR  port_sys_unlock

/* -------------------------------------------------------------------------- */

#ifdef __cplusplus

// critical section class

class CriticalSection
{
	unsigned state;

public:

	 CriticalSection( void ) { state = port_get_lock(); port_set_lock(); }

	~CriticalSection( void ) { port_put_lock(state); }
};

#endif

/* -------------------------------------------------------------------------- */
