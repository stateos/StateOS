/******************************************************************************

    @file    StateOS: osevent.h
    @author  Rajmund Szymanski
    @date    14.08.2018
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

#ifndef __STATEOS_EVT_H
#define __STATEOS_EVT_H

#include "oskernel.h"

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************
 *
 * Name              : event
 *
 ******************************************************************************/

typedef struct __evt evt_t, * const evt_id;

struct __evt
{
	tsk_t  * queue; // next process in the DELAYED queue
	void   * res;   // allocated event object's resource
};

/******************************************************************************
 *
 * Name              : _EVT_INIT
 *
 * Description       : create and initialize an event object
 *
 * Parameters        : none
 *
 * Return            : event object
 *
 * Note              : for internal use
 *
 ******************************************************************************/

#define               _EVT_INIT() { 0, 0 }

/******************************************************************************
 *
 * Name              : OS_EVT
 *
 * Description       : define and initialize an event object
 *
 * Parameters
 *   evt             : name of a pointer to event object
 *
 ******************************************************************************/

#define             OS_EVT( evt )                     \
                       evt_t evt##__evt = _EVT_INIT(); \
                       evt_id evt = & evt##__evt

/******************************************************************************
 *
 * Name              : static_EVT
 *
 * Description       : define and initialize a static event object
 *
 * Parameters
 *   evt             : name of a pointer to event object
 *
 ******************************************************************************/

#define         static_EVT( evt )                     \
                static evt_t evt##__evt = _EVT_INIT(); \
                static evt_id evt = & evt##__evt

/******************************************************************************
 *
 * Name              : EVT_INIT
 *
 * Description       : create and initialize an event object
 *
 * Parameters        : none
 *
 * Return            : event object
 *
 * Note              : use only in 'C' code
 *
 ******************************************************************************/

#ifndef __cplusplus
#define                EVT_INIT() \
                      _EVT_INIT()
#endif

/******************************************************************************
 *
 * Name              : EVT_CREATE
 * Alias             : EVT_NEW
 *
 * Description       : create and initialize an event object
 *
 * Parameters        : none
 *
 * Return            : pointer to event object
 *
 * Note              : use only in 'C' code
 *
 ******************************************************************************/

#ifndef __cplusplus
#define                EVT_CREATE() \
           (evt_t[]) { EVT_INIT  () }
#define                EVT_NEW \
                       EVT_CREATE
#endif

/******************************************************************************
 *
 * Name              : evt_init
 *
 * Description       : initialize an event object
 *
 * Parameters
 *   evt             : pointer to event object
 *
 * Return            : none
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

void evt_init( evt_t *evt );

/******************************************************************************
 *
 * Name              : evt_create
 * Alias             : evt_new
 *
 * Description       : create and initialize a new event object
 *
 * Parameters        : none
 *
 * Return            : pointer to event object (event successfully created)
 *   0               : event not created (not enough free memory)
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

evt_t *evt_create( void );

__STATIC_INLINE
evt_t *evt_new( void ) { return evt_create(); }

/******************************************************************************
 *
 * Name              : evt_kill
 *
 * Description       : reset the event object and wake up all waiting tasks with 'E_STOPPED' event value
 *
 * Parameters
 *   evt             : pointer to event object
 *
 * Return            : none
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

void evt_kill( evt_t *evt );

/******************************************************************************
 *
 * Name              : evt_delete
 *
 * Description       : reset the event object and free allocated resource
 *
 * Parameters
 *   evt             : pointer to event object
 *
 * Return            : none
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

void evt_delete( evt_t *evt );

/******************************************************************************
 *
 * Name              : evt_waitFor
 *
 * Description       : wait for release the event object for given duration of time
 *
 * Parameters
 *   evt             : pointer to event object
 *   delay           : duration of time (maximum number of ticks to wait for release the event object)
 *                     IMMEDIATE: don't wait until the event object has been released
 *                     INFINITE:  wait indefinitely until the event object has been released
 *
 * Return
 *   E_STOPPED       : event object was killed before the specified timeout expired
 *   E_TIMEOUT       : event object was not released before the specified timeout expired
 *   'another'       : event object was successfully released
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

unsigned evt_waitFor( evt_t *evt, cnt_t delay );

/******************************************************************************
 *
 * Name              : evt_waitUntil
 *
 * Description       : wait for release the event object until given timepoint
 *
 * Parameters
 *   evt             : pointer to event object
 *   time            : timepoint value
 *
 * Return
 *   E_STOPPED       : event object was killed before the specified timeout expired
 *   E_TIMEOUT       : event object was not released before the specified timeout expired
 *   'another'       : event object was successfully released
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

unsigned evt_waitUntil( evt_t *evt, cnt_t time );

/******************************************************************************
 *
 * Name              : evt_wait
 *
 * Description       : wait indefinitely until the event object has been released
 *
 * Parameters
 *   evt             : pointer to event object
 *
 * Return
 *   E_STOPPED       : event object was killed
 *   'another'       : event object was successfully released
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

__STATIC_INLINE
unsigned evt_wait( evt_t *evt ) { return evt_waitFor(evt, INFINITE); }

/******************************************************************************
 *
 * Name              : evt_give
 * ISR alias         : evt_giveISR
 *
 * Description       : resume all tasks that are waiting on the event object
 *
 * Parameters
 *   evt             : pointer to event object
 *   event           : all waiting tasks are resumed with the 'event' value
 *
 * Return            : none
 *
 * Note              : may be used both in thread and handler mode
 *
 ******************************************************************************/

void evt_give( evt_t *evt, unsigned event );

__STATIC_INLINE
void evt_giveISR( evt_t *evt, unsigned event ) { evt_give(evt, event); }

#ifdef __cplusplus
}
#endif

/* -------------------------------------------------------------------------- */

#ifdef __cplusplus

/******************************************************************************
 *
 * Class             : Event
 *
 * Description       : create and initialize an event object
 *
 * Constructor parameters
 *                   : none
 *
 ******************************************************************************/

struct Event : public __evt
{
	 Event( void ): __evt _EVT_INIT() {}
	~Event( void ) { assert(__evt::queue == nullptr); }

	void     kill     ( void )            {        evt_kill     (this);         }
	unsigned waitFor  ( cnt_t _delay )    { return evt_waitFor  (this, _delay); }
	unsigned waitUntil( cnt_t _time )     { return evt_waitUntil(this, _time);  }
	unsigned wait     ( void )            { return evt_wait     (this);         }
	void     give     ( unsigned _event ) {        evt_give     (this, _event); }
	void     giveISR  ( unsigned _event ) {        evt_giveISR  (this, _event); }
};

#endif//__cplusplus

/* -------------------------------------------------------------------------- */

#endif//__STATEOS_EVT_H
