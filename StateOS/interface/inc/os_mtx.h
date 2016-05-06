/******************************************************************************

    @file    StateOS: os_mtx.h
    @author  Rajmund Szymanski
    @date    06.05.2016
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

#ifdef __cplusplus
extern "C" {
#endif

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : mutex                                                                                          *
 *                     like a POSIX pthread_mutex_t                                                                   *
 *                                                                                                                    *
 **********************************************************************************************************************/

struct __mtx
{
	tsk_id   queue; // next process in the DELAYED queue
	tsk_id   owner; // owner task
	unsigned count; // mutex's curent value
	mtx_id   list;  // list of mutexes held by owner
};

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : _MTX_INIT                                                                                      *
 *                                                                                                                    *
 * Description       : create and initilize a mutex object                                                            *
 *                                                                                                                    *
 * Parameters        : none                                                                                           *
 *                                                                                                                    *
 * Return            : mutex object                                                                                   *
 *                                                                                                                    *
 * Note              : for internal use                                                                               *
 *                                                                                                                    *
 **********************************************************************************************************************/

#define               _MTX_INIT() { 0, 0, 0, 0 }

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : OS_MTX                                                                                         *
 *                                                                                                                    *
 * Description       : define and initilize a mutex object                                                            *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   mtx             : name of a pointer to mutex object                                                              *
 *                                                                                                                    *
 **********************************************************************************************************************/

#define             OS_MTX( mtx )                     \
                       mtx_t mtx##__mtx = _MTX_INIT(); \
                       mtx_id mtx = & mtx##__mtx

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : static_MTX                                                                                     *
 *                                                                                                                    *
 * Description       : define and initilize a static mutex object                                                     *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   mtx             : name of a pointer to mutex object                                                              *
 *                                                                                                                    *
 **********************************************************************************************************************/

#define         static_MTX( mtx )                     \
                static mtx_t mtx##__mtx = _MTX_INIT(); \
                static mtx_id mtx = & mtx##__mtx

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : MTX_INIT                                                                                       *
 *                                                                                                                    *
 * Description       : create and initilize a mutex object                                                            *
 *                                                                                                                    *
 * Parameters        : none                                                                                           *
 *                                                                                                                    *
 * Return            : mutex object                                                                                   *
 *                                                                                                                    *
 * Note              : use only in 'C' code                                                                           *
 *                                                                                                                    *
 **********************************************************************************************************************/

#define                MTX_INIT() \
                      _MTX_INIT()

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : MTX_CREATE                                                                                     *
 *                                                                                                                    *
 * Description       : create and initilize a mutex object                                                            *
 *                                                                                                                    *
 * Parameters        : none                                                                                           *
 *                                                                                                                    *
 * Return            : pointer to mutex object                                                                        *
 *                                                                                                                    *
 * Note              : use only in 'C' code                                                                           *
 *                                                                                                                    *
 **********************************************************************************************************************/

#ifndef __cplusplus
#define                MTX_CREATE() \
               &(mtx_t)MTX_INIT()
#endif

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : mtx_create                                                                                     *
 *                                                                                                                    *
 * Description       : create and initilize a new mutex object                                                        *
 *                                                                                                                    *
 * Parameters        : none                                                                                           *
 *                                                                                                                    *
 * Return            : pointer to mutex object (mutex successfully created)                                           *
 *   0               : mutex not created (not enough free memory)                                                     *
 *                                                                                                                    *
 * Note              : use only in thread mode                                                                        *
 *                                                                                                                    *
 **********************************************************************************************************************/

              mtx_id   mtx_create( void );

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : mtx_kill                                                                                       *
 *                                                                                                                    *
 * Description       : reset the mutex object and wake up all waiting tasks with 'E_STOPPED' event value              *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   mtx             : pointer to mutex object                                                                        *
 *                                                                                                                    *
 * Return            : none                                                                                           *
 *                                                                                                                    *
 * Note              : use only in thread mode                                                                        *
 *                                                                                                                    *
 **********************************************************************************************************************/

              void     mtx_kill( mtx_id mtx );

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : mtx_waitUntil                                                                                  *
 *                                                                                                                    *
 * Description       : try to lock the mutex object,                                                                  *
 *                     wait until given timepoint if the mutex object can't be locked immediately                     *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   mtx             : pointer to mutex object                                                                        *
 *   time            : timepoint value                                                                                *
 *                                                                                                                    *
 * Return                                                                                                             *
 *   E_SUCCESS       : mutex object was successfully locked                                                           *
 *   E_STOPPED       : mutex object was killed before the specified timeout expired                                   *
 *   E_TIMEOUT       : mutex object was not locked before the specified timeout expired                               *
 *   'another'       : task was resumed with 'another' event value                                                    *
 *                                                                                                                    *
 * Note              : use only in thread mode                                                                        *
 *                                                                                                                    *
 **********************************************************************************************************************/

              unsigned mtx_waitUntil( mtx_id mtx, unsigned time );

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : mtx_waitFor                                                                                    *
 *                                                                                                                    *
 * Description       : try to lock the mutex object,                                                                  *
 *                     wait for given duration of time if the mutex object can't be locked immediately                *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   mtx             : pointer to mutex object                                                                        *
 *   delay           : duration of time (maximum number of ticks to wait for lock the mutex object)                   *
 *                     IMMEDIATE: don't wait if the mutex object can't be locked immediately                          *
 *                     INFINITE:  wait indefinitly until the mutex object has been locked                             *
 *                                                                                                                    *
 * Return                                                                                                             *
 *   E_SUCCESS       : mutex object was successfully locked                                                           *
 *   E_STOPPED       : mutex object was killed before the specified timeout expired                                   *
 *   E_TIMEOUT       : mutex object was not locked before the specified timeout expired                               *
 *   'another'       : task was resumed with 'another' event value                                                    *
 *                                                                                                                    *
 * Note              : use only in thread mode                                                                        *
 *                                                                                                                    *
 **********************************************************************************************************************/

              unsigned mtx_waitFor( mtx_id mtx, unsigned delay );

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : mtx_wait                                                                                       *
 *                                                                                                                    *
 * Description       : try to lock the mutex object,                                                                  *
 *                     wait indefinitly if the mutex object can't be locked immediately                               *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   mtx             : pointer to mutex object                                                                        *
 *                                                                                                                    *
 * Return                                                                                                             *
 *   E_SUCCESS       : mutex object was successfully locked                                                           *
 *   E_STOPPED       : mutex object was killed                                                                        *
 *   'another'       : task was resumed with 'another' event value                                                    *
 *                                                                                                                    *
 * Note              : use only in thread mode                                                                        *
 *                                                                                                                    *
 **********************************************************************************************************************/

static inline unsigned mtx_wait( mtx_id mtx ) { return mtx_waitFor(mtx, INFINITE); }

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : mtx_take                                                                                       *
 *                                                                                                                    *
 * Description       : try to lock the mutex object,                                                                  *
 *                     don't wait if the mutex object can't be locked immediately                                     *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   mtx             : pointer to mutex object                                                                        *
 *                                                                                                                    *
 * Return                                                                                                             *
 *   E_SUCCESS       : mutex object was successfully locked                                                           *
 *   E_TIMEOUT       : mutex object can't be locked immediately                                                       *
 *                                                                                                                    *
 * Note              : use only in thread mode                                                                        *
 *                                                                                                                    *
 **********************************************************************************************************************/

static inline unsigned mtx_take( mtx_id mtx ) { return mtx_waitFor(mtx, IMMEDIATE); }

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : mtx_give                                                                                       *
 *                                                                                                                    *
 * Description       : try to unlock the mutex object (only owner task can unlock mutex object),                      *
 *                     don't wait if the mutex object can't be unlocked                                               *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   mtx             : pointer to mutex object                                                                        *
 *                                                                                                                    *
 * Return                                                                                                             *
 *   E_SUCCESS       : mutex object was successfully unlocked                                                         *
 *   E_TIMEOUT       : mutex object can't be unlocked                                                                 *
 *                                                                                                                    *
 * Note              : use only in thread mode                                                                        *
 *                                                                                                                    *
 **********************************************************************************************************************/

              unsigned mtx_give( mtx_id mtx );

#ifdef __cplusplus
}
#endif

/* -------------------------------------------------------------------------- */

#ifdef __cplusplus

/**********************************************************************************************************************
 *                                                                                                                    *
 * Class             : Mutex                                                                                          *
 *                                                                                                                    *
 * Description       : create and initilize a mutex object                                                            *
 *                                                                                                                    *
 * Constructor parameters                                                                                             *
 *                   : none                                                                                           *
 *                                                                                                                    *
 **********************************************************************************************************************/

class Mutex : public __mtx, private MutexGuard<__mtx>
{
public:

	explicit
	Mutex( void ): __mtx(_MTX_INIT()) {}

	void     kill     ( void )            {        mtx_kill     (this);         }
	unsigned waitUntil( unsigned _time  ) { return mtx_waitUntil(this, _time);  }
	unsigned waitFor  ( unsigned _delay ) { return mtx_waitFor  (this, _delay); }
	unsigned wait     ( void )            { return mtx_wait     (this);         }
	unsigned take     ( void )            { return mtx_take     (this);         }
	unsigned give     ( void )            { return mtx_give     (this);         }
};

#endif

/* -------------------------------------------------------------------------- */
