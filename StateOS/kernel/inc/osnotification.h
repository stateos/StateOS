/******************************************************************************

    @file    StateOS: osnotification.h
    @author  Rajmund Szymanski
    @date    07.09.2018
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

#ifndef __STATEOS_NFO_H
#define __STATEOS_NFO_H

#include "oskernel.h"

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************
 *
 * Name              : notification of the event
 *
 ******************************************************************************/

typedef struct __nfo nfo_t, * const nfo_id;

struct __nfo
{
	obj_t    obj;   // object header
};

/******************************************************************************
 *
 * Name              : _NFO_INIT
 *
 * Description       : create and initialize an notification object
 *
 * Parameters        : none
 *
 * Return            : notification object
 *
 * Note              : for internal use
 *
 ******************************************************************************/

#define               _NFO_INIT() { _OBJ_INIT() }

/******************************************************************************
 *
 * Name              : OS_NFO
 *
 * Description       : define and initialize an notification object
 *
 * Parameters
 *   nfo             : name of a pointer to notification object
 *
 ******************************************************************************/

#define             OS_NFO( nfo )                     \
                       nfo_t nfo##__nfo = _NFO_INIT(); \
                       nfo_id nfo = & nfo##__nfo

/******************************************************************************
 *
 * Name              : static_NFO
 *
 * Description       : define and initialize a static notification object
 *
 * Parameters
 *   nfo             : name of a pointer to notification object
 *
 ******************************************************************************/

#define         static_NFO( nfo )                     \
                static nfo_t nfo##__nfo = _NFO_INIT(); \
                static nfo_id nfo = & nfo##__nfo

/******************************************************************************
 *
 * Name              : NFO_INIT
 *
 * Description       : create and initialize an notification object
 *
 * Parameters        : none
 *
 * Return            : notification object
 *
 * Note              : use only in 'C' code
 *
 ******************************************************************************/

#ifndef __cplusplus
#define                NFO_INIT() \
                      _NFO_INIT()
#endif

/******************************************************************************
 *
 * Name              : NFO_CREATE
 * Alias             : NFO_NEW
 *
 * Description       : create and initialize an notification object
 *
 * Parameters        : none
 *
 * Return            : pointer to notification object
 *
 * Note              : use only in 'C' code
 *
 ******************************************************************************/

#ifndef __cplusplus
#define                NFO_CREATE() \
           (nfo_t[]) { NFO_INIT  () }
#define                NFO_NEW \
                       NFO_CREATE
#endif

/******************************************************************************
 *
 * Name              : nfo_init
 *
 * Description       : initialize an notification object
 *
 * Parameters
 *   nfo             : pointer to notification object
 *
 * Return            : none
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

void nfo_init( nfo_t *nfo );

/******************************************************************************
 *
 * Name              : nfo_create
 * Alias             : nfo_new
 *
 * Description       : create and initialize a new notification object
 *
 * Parameters        : none
 *
 * Return            : pointer to notification object (notification successfully created)
 *   0               : notification not created (not enough free memory)
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

nfo_t *nfo_create( void );

__STATIC_INLINE
nfo_t *nfo_new( void ) { return nfo_create(); }

/******************************************************************************
 *
 * Name              : nfo_kill
 *
 * Description       : reset the notification object and wake up all waiting tasks with 'E_STOPPED' event value
 *
 * Parameters
 *   nfo             : pointer to notification object
 *
 * Return            : none
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

void nfo_kill( nfo_t *nfo );

/******************************************************************************
 *
 * Name              : nfo_delete
 *
 * Description       : reset the notification object and free allocated resource
 *
 * Parameters
 *   nfo             : pointer to notification object
 *
 * Return            : none
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

void nfo_delete( nfo_t *nfo );

/******************************************************************************
 *
 * Name              : nfo_waitFor
 *
 * Description       : wait for release the notification object for given duration of time
 *
 * Parameters
 *   nfo             : pointer to notification object
 *   delay           : duration of time (maximum number of ticks to wait for release the notification object)
 *                     IMMEDIATE: don't wait until the notification object has been released
 *                     INFINITE:  wait indefinitely until the notification object has been released
 *
 * Return
 *   E_STOPPED       : notification object was killed before the specified timeout expired
 *   E_TIMEOUT       : notification object was not released before the specified timeout expired
 *   'another'       : notification object was successfully released
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

unsigned nfo_waitFor( nfo_t *nfo, cnt_t delay );

/******************************************************************************
 *
 * Name              : nfo_waitUntil
 *
 * Description       : wait for release the notification object until given timepoint
 *
 * Parameters
 *   nfo             : pointer to notification object
 *   time            : timepoint value
 *
 * Return
 *   E_STOPPED       : notification object was killed before the specified timeout expired
 *   E_TIMEOUT       : notification object was not released before the specified timeout expired
 *   'another'       : notification object was successfully released
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

unsigned nfo_waitUntil( nfo_t *nfo, cnt_t time );

/******************************************************************************
 *
 * Name              : nfo_wait
 *
 * Description       : wait indefinitely until the notification object has been released
 *
 * Parameters
 *   nfo             : pointer to notification object
 *
 * Return
 *   E_STOPPED       : notification object was killed
 *   'another'       : notification object was successfully released
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

__STATIC_INLINE
unsigned nfo_wait( nfo_t *nfo ) { return nfo_waitFor(nfo, INFINITE); }

/******************************************************************************
 *
 * Name              : nfo_give
 * ISR alias         : nfo_giveISR
 *
 * Description       : resume all tasks that are waiting on the notification object
 *
 * Parameters
 *   nfo             : pointer to notification object
 *   event           : all waiting tasks are resumed with the 'event' value
 *
 * Return            : none
 *
 * Note              : may be used both in thread and handler mode
 *
 ******************************************************************************/

void nfo_give( nfo_t *nfo, unsigned event );

__STATIC_INLINE
void nfo_giveISR( nfo_t *nfo, unsigned event ) { nfo_give(nfo, event); }

#ifdef __cplusplus
}
#endif

/* -------------------------------------------------------------------------- */

#ifdef __cplusplus

/******************************************************************************
 *
 * Class             : Notification
 *
 * Description       : create and initialize an notification object
 *
 * Constructor parameters
 *                   : none
 *
 ******************************************************************************/

struct Notification : public __nfo
{
	 Notification( void ): __nfo _NFO_INIT() {}
	~Notification( void ) { assert(__nfo::obj.queue == nullptr); }

	void     kill     ( void )            {        nfo_kill     (this);         }
	unsigned waitFor  ( cnt_t _delay )    { return nfo_waitFor  (this, _delay); }
	unsigned waitUntil( cnt_t _time )     { return nfo_waitUntil(this, _time);  }
	unsigned wait     ( void )            { return nfo_wait     (this);         }
	void     give     ( unsigned _event ) {        nfo_give     (this, _event); }
	void     giveISR  ( unsigned _event ) {        nfo_giveISR  (this, _event); }
};

#endif//__cplusplus

/* -------------------------------------------------------------------------- */

#endif//__STATEOS_NFO_H
