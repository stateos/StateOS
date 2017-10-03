/******************************************************************************

    @file    StateOS: os_bar.h
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

#ifndef __STATEOS_BAR_H
#define __STATEOS_BAR_H

#include "oskernel.h"

#ifdef __cplusplus
extern "C" {
#endif

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : barrier                                                                                        *
 *                     like a POSIX pthread_barrier_t                                                                 *
 *                                                                                                                    *
 **********************************************************************************************************************/

typedef struct __bar bar_t, * const bar_id;

struct __bar
{
	tsk_t  * queue; // next process in the DELAYED queue
	unsigned count; // barrier's current value
	unsigned limit; // barrier's value limit
};

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : _BAR_INIT                                                                                      *
 *                                                                                                                    *
 * Description       : create and initilize a barrier object                                                          *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   limit           : number of tasks that must call bar_wait[Until|For] function to release the barrier object      *
 *                                                                                                                    *
 * Return            : barrier object                                                                                 *
 *                                                                                                                    *
 * Note              : for internal use                                                                               *
 *                                                                                                                    *
 **********************************************************************************************************************/

#define               _BAR_INIT( _limit ) { 0, _limit, _limit }

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : OS_BAR                                                                                         *
 *                                                                                                                    *
 * Description       : define and initilize a barrier object                                                          *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   bar             : name of a pointer to barrier object                                                            *
 *   limit           : number of tasks that must call bar_wait[Until|For] function to release the barrier object      *
 *                                                                                                                    *
 **********************************************************************************************************************/

#define             OS_BAR( bar, limit )                     \
                       bar_t bar##__bar = _BAR_INIT( limit ); \
                       bar_id bar = & bar##__bar

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : static_BAR                                                                                     *
 *                                                                                                                    *
 * Description       : define and initilize a static barrier object                                                   *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   bar             : name of a pointer to barrier object                                                            *
 *   limit           : number of tasks that must call bar_wait[Until|For] function to release the barrier object      *
 *                                                                                                                    *
 **********************************************************************************************************************/

#define         static_BAR( bar, limit )                     \
                static bar_t bar##__bar = _BAR_INIT( limit ); \
                static bar_id bar = & bar##__bar

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : BAR_INIT                                                                                       *
 *                                                                                                                    *
 * Description       : create and initilize a barrier object                                                          *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   limit           : number of tasks that must call bar_wait[Until|For] function to release the barrier object      *
 *                                                                                                                    *
 * Return            : barrier object                                                                                 *
 *                                                                                                                    *
 * Note              : use only in 'C' code                                                                           *
 *                                                                                                                    *
 **********************************************************************************************************************/

#ifndef __cplusplus
#define                BAR_INIT( limit ) \
                      _BAR_INIT( limit )
#endif

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : BAR_CREATE                                                                                     *
 * Alias             : BAR_NEW                                                                                        *
 *                                                                                                                    *
 * Description       : create and initilize a barrier object                                                          *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   limit           : number of tasks that must call bar_wait[Until|For] function to release the barrier object      *
 *                                                                                                                    *
 * Return            : pointer to barrier object                                                                      *
 *                                                                                                                    *
 * Note              : use only in 'C' code                                                                           *
 *                                                                                                                    *
 **********************************************************************************************************************/

#ifndef __cplusplus
#define                BAR_CREATE( limit ) \
             & (bar_t) BAR_INIT  ( limit )
#define                BAR_NEW \
                       BAR_CREATE
#endif

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : bar_init                                                                                       *
 *                                                                                                                    *
 * Description       : initilize a barrier object                                                                     *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   bar             : pointer to barrier object                                                                      *
 *   limit           : number of tasks that must call bar_wait[Until|For] function to release the barrier object      *
 *                                                                                                                    *
 * Return            : none                                                                                           *
 *                                                                                                                    *
 * Note              : use only in thread mode                                                                        *
 *                                                                                                                    *
 **********************************************************************************************************************/

void bar_init( bar_t *bar, unsigned limit );

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : bar_create                                                                                     *
 * Alias             : bar_new                                                                                        *
 *                                                                                                                    *
 * Description       : create and initilize a new barrier object                                                      *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   limit           : number of tasks that must call bar_wait[Until|For] function to release the barrier object      *
 *                                                                                                                    *
 * Return            : pointer to barrier object (barrier successfully created)                                       *
 *   0               : barrier not created (not enough free memory)                                                   *
 *                                                                                                                    *
 * Note              : use only in thread mode                                                                        *
 *                                                                                                                    *
 **********************************************************************************************************************/

bar_t *bar_create( unsigned limit );
__STATIC_INLINE
bar_t *bar_new   ( unsigned limit ) { return bar_create(limit); }

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : bar_kill                                                                                       *
 *                                                                                                                    *
 * Description       : reset the barrier object and wake up all waiting tasks with 'E_STOPPED' event value            *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   bar             : pointer to barrier object                                                                      *
 *                                                                                                                    *
 * Return            : none                                                                                           *
 *                                                                                                                    *
 * Note              : use only in thread mode                                                                        *
 *                                                                                                                    *
 **********************************************************************************************************************/

void bar_kill( bar_t *bar );

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : bar_waitUntil                                                                                  *
 *                                                                                                                    *
 * Description       : wait for release the barrier object until given timepoint                                      *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   bar             : pointer to barrier object                                                                      *
 *   time            : timepoint value                                                                                *
 *                                                                                                                    *
 * Return                                                                                                             *
 *   E_SUCCESS       : barrier object was successfully released                                                       *
 *   E_STOPPED       : barrier object was killed before the specified timeout expired                                 *
 *   E_TIMEOUT       : barrier object was not released before the specified timeout expired                           *
 *                                                                                                                    *
 * Note              : use only in thread mode                                                                        *
 *                                                                                                                    *
 **********************************************************************************************************************/

unsigned bar_waitUntil( bar_t *bar, uint32_t time );

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : bar_waitFor                                                                                    *
 *                                                                                                                    *
 * Description       : wait for release the barrier object for given duration of time                                 *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   bar             : pointer to barrier object                                                                      *
 *   delay           : duration of time (maximum number of ticks to wait for release the barrier object)              *
 *                     IMMEDIATE: don't wait if the barrier object can't be released                                  *
 *                     INFINITE:  wait indefinitly until the barrier object has been released                         *
 *                                                                                                                    *
 * Return                                                                                                             *
 *   E_SUCCESS       : barrier object was successfully released                                                       *
 *   E_STOPPED       : barrier object was killed before the specified timeout expired                                 *
 *   E_TIMEOUT       : barrier object was not released before the specified timeout expired                           *
 *                                                                                                                    *
 * Note              : use only in thread mode                                                                        *
 *                                                                                                                    *
 **********************************************************************************************************************/

unsigned bar_waitFor( bar_t *bar, uint32_t delay );

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : bar_wait                                                                                       *
 *                                                                                                                    *
 * Description       : wait indefinitly until the barrier object has been released                                    *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   bar             : pointer to barrier object                                                                      *
 *                                                                                                                    *
 * Return                                                                                                             *
 *   E_SUCCESS       : barrier object was successfully released                                                       *
 *   E_STOPPED       : barrier object was killed                                                                      *
 *                                                                                                                    *
 * Note              : use only in thread mode                                                                        *
 *                                                                                                                    *
 **********************************************************************************************************************/

__STATIC_INLINE
unsigned bar_wait( bar_t *bar ) { return bar_waitFor(bar, INFINITE); }

#ifdef __cplusplus
}
#endif

/* -------------------------------------------------------------------------- */

#ifdef __cplusplus

/**********************************************************************************************************************
 *                                                                                                                    *
 * Class             : Barrier                                                                                        *
 *                                                                                                                    *
 * Description       : create and initilize a barrier object                                                          *
 *                                                                                                                    *
 * Constructor parameters                                                                                             *
 *   limit           : number of tasks that must call wait[Until|For] function to release the barrier object          *
 *                                                                                                                    *
 **********************************************************************************************************************/

struct Barrier : public __bar
{
	 explicit
	 Barrier( const unsigned _limit ): __bar _BAR_INIT(_limit) {}
	~Barrier( void ) { assert(queue == nullptr); }

	void     kill     ( void )            {        bar_kill     (this);         }
	unsigned waitUntil( uint32_t _time  ) { return bar_waitUntil(this, _time);  }
	unsigned waitFor  ( uint32_t _delay ) { return bar_waitFor  (this, _delay); }
	unsigned wait     ( void )            { return bar_wait     (this);         }
};

#endif

/* -------------------------------------------------------------------------- */

#endif//__STATEOS_BAR_H
