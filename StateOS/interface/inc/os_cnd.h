/******************************************************************************

    @file    StateOS: os_cnd.h
    @author  Rajmund Szymanski
    @date    04.11.2016
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

#ifndef __STATEOS_CND_H
#define __STATEOS_CND_H

#include <oskernel.h>

#ifdef __cplusplus
extern "C" {
#endif

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : condition variable                                                                             *
 *                     like a POSIX pthread_cond_t                                                                    *
 *                                                                                                                    *
 **********************************************************************************************************************/

typedef struct __cnd
{
	tsk_id   queue; // next process in the DELAYED queue

}	cnd_t, *cnd_id;

/* -------------------------------------------------------------------------- */

#define cndOne       ( false ) // notify one task
#define cndAll       ( true  ) // notify all tasks

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : _CND_INIT                                                                                      *
 *                                                                                                                    *
 * Description       : create and initilize a condition variable object                                               *
 *                                                                                                                    *
 * Parameters        : none                                                                                           *
 *                                                                                                                    *
 * Return            : condition variable object                                                                      *
 *                                                                                                                    *
 * Note              : for internal use                                                                               *
 *                                                                                                                    *
 **********************************************************************************************************************/

#define               _CND_INIT() { 0 }

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : OS_CND                                                                                         *
 *                                                                                                                    *
 * Description       : define and initilize a condition variable object                                               *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   cnd             : name of a pointer to condition variable object                                                 *
 *                                                                                                                    *
 **********************************************************************************************************************/

#define             OS_CND( cnd )                     \
                       cnd_t cnd##__cnd = _CND_INIT(); \
                       cnd_id cnd = & cnd##__cnd

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : static_CND                                                                                     *
 *                                                                                                                    *
 * Description       : define and initilize a static condition variable object                                        *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   cnd             : name of a pointer to condition variable object                                                 *
 *                                                                                                                    *
 **********************************************************************************************************************/

#define         static_CND( cnd )                     \
                static cnd_t cnd##__cnd = _CND_INIT(); \
                static cnd_id cnd = & cnd##__cnd

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : CND_INIT                                                                                       *
 *                                                                                                                    *
 * Description       : create and initilize a condition variable object                                               *
 *                                                                                                                    *
 * Parameters        : none                                                                                           *
 *                                                                                                                    *
 * Return            : condition variable object                                                                      *
 *                                                                                                                    *
 * Note              : use only in 'C' code                                                                           *
 *                                                                                                                    *
 **********************************************************************************************************************/

#define                CND_INIT() \
                      _CND_INIT()

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : CND_CREATE                                                                                     *
 *                                                                                                                    *
 * Description       : create and initilize a condition variable object                                               *
 *                                                                                                                    *
 * Parameters        : none                                                                                           *
 *                                                                                                                    *
 * Return            : pointer to condition variable object                                                           *
 *                                                                                                                    *
 * Note              : use only in 'C' code                                                                           *
 *                                                                                                                    *
 **********************************************************************************************************************/

#ifndef __cplusplus
#define                CND_CREATE() \
               &(cnd_t)CND_INIT()
#endif

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : cnd_create                                                                                     *
 *                                                                                                                    *
 * Description       : create and initilize a new condition variable object                                           *
 *                                                                                                                    *
 * Parameters        : none                                                                                           *
 *                                                                                                                    *
 * Return            : pointer to condition variable object (condition variable successfully created)                 *
 *   0               : condition variable not created (not enough free memory)                                        *
 *                                                                                                                    *
 * Note              : use only in thread mode                                                                        *
 *                                                                                                                    *
 **********************************************************************************************************************/

              cnd_id   cnd_create( void );

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : cnd_kill                                                                                       *
 *                                                                                                                    *
 * Description       : reset the condition variable object and wake up all waiting tasks with 'E_STOPPED' event value *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   cnd             : pointer to condition variable object                                                           *
 *                                                                                                                    *
 * Return            : none                                                                                           *
 *                                                                                                                    *
 * Note              : use only in thread mode                                                                        *
 *                                                                                                                    *
 **********************************************************************************************************************/

              void     cnd_kill( cnd_id cnd );

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : cnd_waitUntil                                                                                  *
 *                                                                                                                    *
 * Description       : wait until given timepoint on the condition variable releasing the currently owned mutex,      *
 *                     and finally lock the mutex again                                                               *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   cnd             : pointer to condition variable object                                                           *
 *   mtx             : currently owned mutex                                                                          *
 *   time            : timepoint value                                                                                *
 *                                                                                                                    *
 * Return                                                                                                             *
 *   E_SUCCESS       : condition variable object was successfully signalled and owned mutex locked again              *
 *   E_STOPPED       : condition variable object was killed before the specified timeout expired                      *
 *   E_TIMEOUT       : condition variable object was not signalled before the specified timeout expired               *
 *   'another'       : task was resumed with 'another' event value                                                    *
 *                                                                                                                    *
 * Note              : use only in thread mode                                                                        *
 *                                                                                                                    *
 **********************************************************************************************************************/

              unsigned cnd_waitUntil( cnd_id cnd, mtx_id mtx, unsigned time );

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : cnd_waitFor                                                                                    *
 *                                                                                                                    *
 * Description       : wait for given duration of time on the condition variable releasing the currently owned mutex, *
 *                     and finally lock the mutex again                                                               *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   cnd             : pointer to condition variable object                                                           *
 *   mtx             : currently owned mutex                                                                          *
 *   delay           : duration of time (maximum number of ticks to wait on the condition variable object)            *
 *                     IMMEDIATE: don't wait on the condition variable object                                         *
 *                     INFINITE:  wait indefinitly on the condition variable object                                   *
 *                                                                                                                    *
 * Return                                                                                                             *
 *   E_SUCCESS       : condition variable object was successfully signalled and owned mutex locked again              *
 *   E_STOPPED       : condition variable object was killed before the specified timeout expired                      *
 *   E_TIMEOUT       : condition variable object was not signalled before the specified timeout expired               *
 *   'another'       : task was resumed with 'another' event value                                                    *
 *                                                                                                                    *
 * Note              : use only in thread mode                                                                        *
 *                                                                                                                    *
 **********************************************************************************************************************/

              unsigned cnd_waitFor( cnd_id cnd, mtx_id mtx, unsigned delay );

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : cnd_wait                                                                                       *
 *                                                                                                                    *
 * Description       : wait indefinitly on the condition variable releasing the currently owned mutex,                *
 *                     and finally lock the mutex again                                                               *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   cnd             : pointer to condition variable object                                                           *
 *   mtx             : currently owned mutex                                                                          *
 *                                                                                                                    *
 * Return                                                                                                             *
 *   E_SUCCESS       : condition variable object was successfully signalled and owned mutex locked again              *
 *   E_STOPPED       : condition variable object was killed                                                           *
 *   'another'       : task was resumed with 'another' event value                                                    *
 *                                                                                                                    *
 * Note              : use only in thread mode                                                                        *
 *                                                                                                                    *
 **********************************************************************************************************************/

static inline unsigned cnd_wait( cnd_id cnd, mtx_id mtx ) { return cnd_waitFor(cnd, mtx, INFINITE); }

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : cnd_give                                                                                       *
 *                                                                                                                    *
 * Description       : signal one or all tasks that are waiting on the condition variable                             *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   cnd             : pointer to condition variable object                                                           *
 *   all             : signal receiver                                                                                *
 *                     cndOne: notify one task that is waiting on the condition variable                              *
 *                     cndAll: notify all tasks that are waiting on the condition variable                            *
 *                                                                                                                    *
 * Return            : none                                                                                           *
 *                                                                                                                    *
 * Note              : use only in thread mode                                                                        *
 *                                                                                                                    *
 **********************************************************************************************************************/

              void     cnd_give( cnd_id cnd, bool all );

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : cnd_giveISR                                                                                    *
 *                                                                                                                    *
 * Description       : signal one or all tasks that are waiting on the condition variable                             *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   cnd             : pointer to condition variable object                                                           *
 *   all             : signal receiver                                                                                *
 *                     cndOne: notify one task that is waiting on the condition variable                              *
 *                     cndAll: notify all tasks that are waiting on the condition variable                            *
 *                                                                                                                    *
 * Return            : none                                                                                           *
 *                                                                                                                    *
 * Note              : use only in handler mode                                                                       *
 *                                                                                                                    *
 **********************************************************************************************************************/

static inline void     cnd_giveISR( cnd_id cnd, bool all ) { cnd_give(cnd, all); }

#ifdef __cplusplus
}
#endif

/* -------------------------------------------------------------------------- */

#ifdef __cplusplus

/**********************************************************************************************************************
 *                                                                                                                    *
 * Class             : ConditionVariable                                                                              *
 *                                                                                                                    *
 * Description       : create and initilize a condition variable object                                               *
 *                                                                                                                    *
 * Constructor parameters                                                                                             *
 *                   : none                                                                                           *
 *                                                                                                                    *
 **********************************************************************************************************************/

class ConditionVariable : public __cnd, private EventGuard<__cnd>
{
public:

	explicit
	ConditionVariable( void ): __cnd _CND_INIT() {}

	void     kill     ( void )                         {        cnd_kill     (this);               }
	unsigned waitUntil( mtx_id _mtx, unsigned _time  ) { return cnd_waitUntil(this, _mtx, _time);  }
	unsigned waitFor  ( mtx_id _mtx, unsigned _delay ) { return cnd_waitFor  (this, _mtx, _delay); }
	unsigned wait     ( mtx_id _mtx )                  { return cnd_wait     (this, _mtx);         }
	void     give     ( bool   _all = cndAll )         {        cnd_give     (this, _all);         }
	void     giveISR  ( bool   _all = cndAll )         {        cnd_giveISR  (this, _all);         }
};

#endif

/* -------------------------------------------------------------------------- */

#endif//__STATEOS_CND_H
