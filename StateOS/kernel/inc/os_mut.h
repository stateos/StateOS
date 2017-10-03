/******************************************************************************

    @file    StateOS: os_mut.h
    @author  Rajmund Szymanski
    @date    03.10.2017
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

#ifndef __STATEOS_MUT_H
#define __STATEOS_MUT_H

#include "oskernel.h"

#ifdef __cplusplus
extern "C" {
#endif

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : fast mutex (non-recursive and non-priority-inheritance)                                        *
 *                                                                                                                    *
 * Note              : use only to synchronize tasks with the same priority                                           *
 *                                                                                                                    *
 **********************************************************************************************************************/

typedef struct __mut mut_t, * const mut_id;

struct __mut
{
	tsk_t  * queue; // next process in the DELAYED queue
	tsk_t  * owner; // owner task
};

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : _MUT_INIT                                                                                      *
 *                                                                                                                    *
 * Description       : create and initilize a fast mutex object                                                       *
 *                                                                                                                    *
 * Parameters        : none                                                                                           *
 *                                                                                                                    *
 * Return            : fast mutex object                                                                              *
 *                                                                                                                    *
 * Note              : for internal use                                                                               *
 *                                                                                                                    *
 **********************************************************************************************************************/

#define               _MUT_INIT() { 0, 0 }

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : OS_MUT                                                                                         *
 *                                                                                                                    *
 * Description       : define and initilize a fast mutex object                                                       *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   mut             : name of a pointer to fast mutex object                                                         *
 *                                                                                                                    *
 **********************************************************************************************************************/

#define             OS_MUT( mut )                     \
                       mut_t mut##__mut = _MUT_INIT(); \
                       mut_id mut = & mut##__mut

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : static_MUT                                                                                     *
 *                                                                                                                    *
 * Description       : define and initilize a static fast mutex object                                                *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   mut             : name of a pointer to fast mutex object                                                         *
 *                                                                                                                    *
 **********************************************************************************************************************/

#define         static_MUT( mut )                     \
                static mut_t mut##__mut = _MUT_INIT(); \
                static mut_id mut = & mut##__mut

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : MUT_INIT                                                                                       *
 *                                                                                                                    *
 * Description       : create and initilize a fast mutex object                                                       *
 *                                                                                                                    *
 * Parameters        : none                                                                                           *
 *                                                                                                                    *
 * Return            : fast mutex object                                                                              *
 *                                                                                                                    *
 * Note              : use only in 'C' code                                                                           *
 *                                                                                                                    *
 **********************************************************************************************************************/

#ifndef __cplusplus
#define                MUT_INIT() \
                      _MUT_INIT()
#endif

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : MUT_CREATE                                                                                     *
 * Alias             : MUT_NEW                                                                                        *
 *                                                                                                                    *
 * Description       : create and initilize a fast mutex object                                                       *
 *                                                                                                                    *
 * Parameters        : none                                                                                           *
 *                                                                                                                    *
 * Return            : pointer to fast mutex object                                                                   *
 *                                                                                                                    *
 * Note              : use only in 'C' code                                                                           *
 *                                                                                                                    *
 **********************************************************************************************************************/

#ifndef __cplusplus
#define                MUT_CREATE() \
             & (mut_t) MUT_INIT  ()
#define                MUT_NEW \
                       MUT_CREATE
#endif

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : mut_init                                                                                       *
 *                                                                                                                    *
 * Description       : initilize a fast mutex object                                                                  *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   mut             : pointer to fast mutex object                                                                   *
 *                                                                                                                    *
 * Return            : none                                                                                           *
 *                                                                                                                    *
 * Note              : use only in thread mode                                                                        *
 *                                                                                                                    *
 **********************************************************************************************************************/

void mut_init( mut_t *mut );

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : mut_create                                                                                     *
 * Alias             : mut_new                                                                                        *
 *                                                                                                                    *
 * Description       : create and initilize a new fast mutex object                                                   *
 *                                                                                                                    *
 * Parameters        : none                                                                                           *
 *                                                                                                                    *
 * Return            : pointer to fast mutex object (fast mutex successfully created)                                 *
 *   0               : fast mutex not created (not enough free memory)                                                *
 *                                                                                                                    *
 * Note              : use only in thread mode                                                                        *
 *                                                                                                                    *
 **********************************************************************************************************************/

mut_t *mut_create( void );
__STATIC_INLINE
mut_t *mut_new   ( void ) { return mut_create(); }

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : mut_kill                                                                                       *
 *                                                                                                                    *
 * Description       : reset the fast mutex object and wake up all waiting tasks with 'E_STOPPED' event value         *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   mut             : pointer to fast mutex object                                                                   *
 *                                                                                                                    *
 * Return            : none                                                                                           *
 *                                                                                                                    *
 * Note              : use only in thread mode                                                                        *
 *                                                                                                                    *
 **********************************************************************************************************************/

void mut_kill( mut_t *mut );

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : mut_waitUntil                                                                                  *
 *                                                                                                                    *
 * Description       : try to lock the fast mutex object,                                                             *
 *                     wait until given timepoint if the fast mutex object can't be locked immediately                *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   mut             : pointer to fast mutex object                                                                   *
 *   time            : timepoint value                                                                                *
 *                                                                                                                    *
 * Return                                                                                                             *
 *   E_SUCCESS       : fast mutex object was successfully locked                                                      *
 *   E_STOPPED       : fast mutex object was killed before the specified timeout expired                              *
 *   E_TIMEOUT       : fast mutex object was not locked before the specified timeout expired                          *
 *                                                                                                                    *
 * Note              : use only in thread mode                                                                        *
 *                                                                                                                    *
 **********************************************************************************************************************/

unsigned mut_waitUntil( mut_t *mut, uint32_t time );

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : mut_waitFor                                                                                    *
 *                                                                                                                    *
 * Description       : try to lock the fast mutex object,                                                             *
 *                     wait for given duration of time if the fast mutex object can't be locked immediately           *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   mut             : pointer to fast mutex object                                                                   *
 *   delay           : duration of time (maximum number of ticks to wait for lock the fast mutex object)              *
 *                     IMMEDIATE: don't wait if the fast mutex object can't be locked immediately                     *
 *                     INFINITE:  wait indefinitly until the fast mutex object has been locked                        *
 *                                                                                                                    *
 * Return                                                                                                             *
 *   E_SUCCESS       : fast mutex object was successfully locked                                                      *
 *   E_STOPPED       : fast mutex object was killed before the specified timeout expired                              *
 *   E_TIMEOUT       : fast mutex object was not locked before the specified timeout expired                          *
 *                                                                                                                    *
 * Note              : use only in thread mode                                                                        *
 *                                                                                                                    *
 **********************************************************************************************************************/

unsigned mut_waitFor( mut_t *mut, uint32_t delay );

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : mut_wait                                                                                       *
 *                                                                                                                    *
 * Description       : try to lock the fast mutex object,                                                             *
 *                     wait indefinitly if the fast mutex object can't be locked immediately                          *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   mut             : pointer to fast mutex object                                                                   *
 *                                                                                                                    *
 * Return                                                                                                             *
 *   E_SUCCESS       : fast mutex object was successfully locked                                                      *
 *   E_STOPPED       : fast mutex object was killed                                                                   *
 *                                                                                                                    *
 * Note              : use only in thread mode                                                                        *
 *                                                                                                                    *
 **********************************************************************************************************************/

__STATIC_INLINE
unsigned mut_wait( mut_t *mut ) { return mut_waitFor(mut, INFINITE); }

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : mut_take                                                                                       *
 *                                                                                                                    *
 * Description       : try to lock the fast mutex object,                                                             *
 *                     don't wait if the fast mutex object can't be locked immediately                                *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   mut             : pointer to fast mutex object                                                                   *
 *                                                                                                                    *
 * Return                                                                                                             *
 *   E_SUCCESS       : fast mutex object was successfully locked                                                      *
 *   E_TIMEOUT       : fast mutex object can't be locked immediately                                                  *
 *                                                                                                                    *
 * Note              : use only in thread mode                                                                        *
 *                                                                                                                    *
 **********************************************************************************************************************/

__STATIC_INLINE
unsigned mut_take( mut_t *mut ) { return mut_waitFor(mut, IMMEDIATE); }

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : mut_give                                                                                       *
 *                                                                                                                    *
 * Description       : try to unlock the fast mutex object (only owner task can unlock fast mutex object),            *
 *                     don't wait if the fast mutex object can't be unlocked                                          *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   mut             : pointer to fast mutex object                                                                   *
 *                                                                                                                    *
 * Return                                                                                                             *
 *   E_SUCCESS       : fast mutex object was successfully unlocked                                                    *
 *   E_TIMEOUT       : fast mutex object can't be unlocked                                                            *
 *                                                                                                                    *
 * Note              : use only in thread mode                                                                        *
 *                                                                                                                    *
 **********************************************************************************************************************/

unsigned mut_give( mut_t *mut );

#ifdef __cplusplus
}
#endif

/* -------------------------------------------------------------------------- */

#ifdef __cplusplus

/**********************************************************************************************************************
 *                                                                                                                    *
 * Class             : FastMutex                                                                                      *
 *                                                                                                                    *
 * Description       : create and initilize a fast mutex object                                                       *
 *                                                                                                                    *
 * Constructor parameters                                                                                             *
 *                   : none                                                                                           *
 *                                                                                                                    *
 **********************************************************************************************************************/

struct FastMutex : public __mut
{
	 explicit
	 FastMutex( void ): __mut _MUT_INIT() {}
	~FastMutex( void ) { assert(owner == nullptr); }

	void     kill     ( void )            {        mut_kill     (this);         }
	unsigned waitUntil( uint32_t _time  ) { return mut_waitUntil(this, _time);  }
	unsigned waitFor  ( uint32_t _delay ) { return mut_waitFor  (this, _delay); }
	unsigned wait     ( void )            { return mut_wait     (this);         }
	unsigned take     ( void )            { return mut_take     (this);         }
	unsigned give     ( void )            { return mut_give     (this);         }
};

#endif

/* -------------------------------------------------------------------------- */

#endif//__STATEOS_MUT_H
