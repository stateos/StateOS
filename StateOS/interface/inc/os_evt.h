/******************************************************************************

    @file    StateOS: os_evt.h
    @author  Rajmund Szymanski
    @date    10.11.2016
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

#ifndef __STATEOS_EVT_H
#define __STATEOS_EVT_H

#include <oskernel.h>

#ifdef __cplusplus
extern "C" {
#endif

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : event                                                                                          *
 *                                                                                                                    *
 **********************************************************************************************************************/

typedef struct __evt
{
	tsk_id   queue; // next process in the DELAYED queue

}	evt_t, *evt_id;

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : _EVT_INIT                                                                                      *
 *                                                                                                                    *
 * Description       : create and initilize an event object                                                           *
 *                                                                                                                    *
 * Parameters        : none                                                                                           *
 *                                                                                                                    *
 * Return            : event object                                                                                   *
 *                                                                                                                    *
 * Note              : for internal use                                                                               *
 *                                                                                                                    *
 **********************************************************************************************************************/

#define               _EVT_INIT() { 0 }

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : OS_EVT                                                                                         *
 *                                                                                                                    *
 * Description       : define and initilize an event object                                                           *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   evt             : name of a pointer to event object                                                              *
 *                                                                                                                    *
 **********************************************************************************************************************/

#define             OS_EVT( evt )                     \
                       evt_t evt##__evt = _EVT_INIT(); \
                       evt_id evt = & evt##__evt

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : static_EVT                                                                                     *
 *                                                                                                                    *
 * Description       : define and initilize a static event object                                                     *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   evt             : name of a pointer to event object                                                              *
 *                                                                                                                    *
 **********************************************************************************************************************/

#define         static_EVT( evt )                     \
                static evt_t evt##__evt = _EVT_INIT(); \
                static evt_id evt = & evt##__evt

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : EVT_INIT                                                                                       *
 *                                                                                                                    *
 * Description       : create and initilize an event object                                                           *
 *                                                                                                                    *
 * Parameters        : none                                                                                           *
 *                                                                                                                    *
 * Return            : event object                                                                                   *
 *                                                                                                                    *
 * Note              : use only in 'C' code                                                                           *
 *                                                                                                                    *
 **********************************************************************************************************************/

#define                EVT_INIT() \
                      _EVT_INIT()

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : EVT_CREATE                                                                                     *
 *                                                                                                                    *
 * Description       : create and initilize an event object                                                           *
 *                                                                                                                    *
 * Parameters        : none                                                                                           *
 *                                                                                                                    *
 * Return            : pointer to event object                                                                        *
 *                                                                                                                    *
 * Note              : use only in 'C' code                                                                           *
 *                                                                                                                    *
 **********************************************************************************************************************/

#ifndef __cplusplus
#define                EVT_CREATE() \
               &(evt_t)EVT_INIT()
#endif

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : evt_create                                                                                     *
 *                                                                                                                    *
 * Description       : create and initilize a new event object                                                        *
 *                                                                                                                    *
 * Parameters        : none                                                                                           *
 *                                                                                                                    *
 * Return            : pointer to event object (event successfully created)                                           *
 *   0               : event not created (not enough free memory)                                                     *
 *                                                                                                                    *
 * Note              : use only in thread mode                                                                        *
 *                                                                                                                    *
 **********************************************************************************************************************/

              evt_id   evt_create( void );

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : evt_kill                                                                                       *
 *                                                                                                                    *
 * Description       : reset the event object and wake up all waiting tasks with 'E_STOPPED' event value              *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   evt             : pointer to event object                                                                        *
 *                                                                                                                    *
 * Return            : none                                                                                           *
 *                                                                                                                    *
 * Note              : use only in thread mode                                                                        *
 *                                                                                                                    *
 **********************************************************************************************************************/

              void     evt_kill( evt_id evt );

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : evt_waitUntil                                                                                  *
 *                                                                                                                    *
 * Description       : wait for release the event object until given timepoint                                        *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   evt             : pointer to event object                                                                        *
 *   time            : timepoint value                                                                                *
 *                                                                                                                    *
 * Return                                                                                                             *
 *   E_STOPPED       : event object was killed before the specified timeout expired                                   *
 *   E_TIMEOUT       : event object was not released before the specified timeout expired                             *
 *   'another'       : event object was successfully released or task was resumed with 'another' event value          *
 *                                                                                                                    *
 * Note              : use only in thread mode                                                                        *
 *                                                                                                                    *
 **********************************************************************************************************************/

              unsigned evt_waitUntil( evt_id evt, unsigned time );

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : evt_waitFor                                                                                    *
 *                                                                                                                    *
 * Description       : wait for release the event object for given duration of time                                   *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   evt             : pointer to event object                                                                        *
 *   delay           : duration of time (maximum number of ticks to wait for release the event object)                *
 *                     IMMEDIATE: don't wait until the event object has been released                                 *
 *                     INFINITE:  wait indefinitly until the event object has been released                           *
 *                                                                                                                    *
 * Return                                                                                                             *
 *   E_STOPPED       : event object was killed before the specified timeout expired                                   *
 *   E_TIMEOUT       : event object was not released before the specified timeout expired                             *
 *   'another'       : event object was successfully released or task was resumed with 'another' event value          *
 *                                                                                                                    *
 * Note              : use only in thread mode                                                                        *
 *                                                                                                                    *
 **********************************************************************************************************************/

              unsigned evt_waitFor( evt_id evt, unsigned delay );

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : evt_wait                                                                                       *
 *                                                                                                                    *
 * Description       : wait indefinitly until the event object has been released                                      *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   evt             : pointer to event object                                                                        *
 *                                                                                                                    *
 * Return                                                                                                             *
 *   E_STOPPED       : event object was killed                                                                        *
 *   'another'       : event object was successfully released or task was resumed with 'another' event value          *
 *                                                                                                                    *
 * Note              : use only in thread mode                                                                        *
 *                                                                                                                    *
 **********************************************************************************************************************/

static inline unsigned evt_wait( evt_id evt ) { return evt_waitFor(evt, INFINITE); }

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : evt_give                                                                                       *
 *                                                                                                                    *
 * Description       : resume all tasks that are waiting on the event object                                          *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   evt             : pointer to event object                                                                        *
 *   event           : all waiting tasks are resumed with the 'event' value                                           *
 *                                                                                                                    *
 * Return            : none                                                                                           *
 *                                                                                                                    *
 * Note              : use only in thread mode                                                                        *
 *                                                                                                                    *
 **********************************************************************************************************************/

              void     evt_give( evt_id evt, unsigned event );

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : evt_giveISR                                                                                    *
 *                                                                                                                    *
 * Description       : resume all tasks that are waiting on the event object                                          *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   evt             : pointer to event object                                                                        *
 *   event           : all waiting tasks are resumed with the 'event' value                                           *
 *                                                                                                                    *
 * Return            : none                                                                                           *
 *                                                                                                                    *
 * Note              : use only in handler mode                                                                       *
 *                                                                                                                    *
 **********************************************************************************************************************/

static inline void     evt_giveISR( evt_id evt, unsigned event ) { evt_give(evt, event); }

#ifdef __cplusplus
}
#endif

/* -------------------------------------------------------------------------- */

#ifdef __cplusplus

/**********************************************************************************************************************
 *                                                                                                                    *
 * Class             : Event                                                                                          *
 *                                                                                                                    *
 * Description       : create and initilize an event object                                                           *
 *                                                                                                                    *
 * Constructor parameters                                                                                             *
 *                   : none                                                                                           *
 *                                                                                                                    *
 **********************************************************************************************************************/

class Event : public __evt, private EventGuard<__evt>
{
public:

	explicit
	Event( void ): __evt _EVT_INIT() {}

	void     kill     ( void )            {        evt_kill     (this);         }
	unsigned waitUntil( unsigned _time  ) { return evt_waitUntil(this, _time);  }
	unsigned waitFor  ( unsigned _delay ) { return evt_waitFor  (this, _delay); }
	unsigned wait     ( void )            { return evt_wait     (this);         }
	void     give     ( unsigned _event ) {        evt_give     (this, _event); }
	void     giveISR  ( unsigned _event ) {        evt_giveISR  (this, _event); }
};

#endif

/* -------------------------------------------------------------------------- */

#endif//__STATEOS_EVT_H
