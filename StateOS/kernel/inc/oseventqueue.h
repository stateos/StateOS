/******************************************************************************

    @file    StateOS: oseventqueue.h
    @author  Rajmund Szymanski
    @date    27.08.2018
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
 * Name              : event queue
 *
 ******************************************************************************/

typedef struct __evt evt_t, * const evt_id;

struct __evt
{
	tsk_t  * queue; // inherited from semaphore
	void   * res;   // allocated event queue object's resource
	unsigned count; // inherited from semaphore
	unsigned limit; // inherited from semaphore

	unsigned head;  // first element to read from data buffer
	unsigned tail;  // first element to write into data buffer
	unsigned*data;  // data buffer
};

/******************************************************************************
 *
 * Name              : _EVT_INIT
 *
 * Description       : create and initialize an event queue object
 *
 * Parameters
 *   limit           : size of a queue (max number of stored events)
 *   data            : event queue data buffer
 *
 * Return            : event queue object
 *
 * Note              : for internal use
 *
 ******************************************************************************/

#define               _EVT_INIT( _limit, _data ) { 0, 0, 0, _limit, 0, 0, _data }

/******************************************************************************
 *
 * Name              : _EVT_DATA
 *
 * Description       : create an event queue data buffer
 *
 * Parameters
 *   limit           : size of a queue (max number of stored events)
 *
 * Return            : event queue data buffer
 *
 * Note              : for internal use
 *
 ******************************************************************************/

#ifndef __cplusplus
#define               _EVT_DATA( _limit ) (unsigned[_limit]){ 0 }
#endif

/******************************************************************************
 *
 * Name              : OS_EVT
 *
 * Description       : define and initialize an event queue object
 *
 * Parameters
 *   evt             : name of a pointer to event queue object
 *   limit           : size of a queue (max number of stored events)
 *
 ******************************************************************************/

#define             OS_EVT( evt, limit )                                \
                       unsigned evt##__buf[limit];                       \
                       evt_t evt##__evt = _EVT_INIT( limit, evt##__buf ); \
                       evt_id evt = & evt##__evt

/******************************************************************************
 *
 * Name              : static_EVT
 *
 * Description       : define and initialize a static event queue object
 *
 * Parameters
 *   evt             : name of a pointer to event queue object
 *   limit           : size of a queue (max number of stored events)
 *
 ******************************************************************************/

#define         static_EVT( evt, limit )                                \
                static unsigned evt##__buf[limit];                       \
                static evt_t evt##__evt = _EVT_INIT( limit, evt##__buf ); \
                static evt_id evt = & evt##__evt

/******************************************************************************
 *
 * Name              : EVT_INIT
 *
 * Description       : create and initialize an event queue object
 *
 * Parameters
 *   limit           : size of a queue (max number of stored events)
 *
 * Return            : event queue object
 *
 * Note              : use only in 'C' code
 *
 ******************************************************************************/

#ifndef __cplusplus
#define                EVT_INIT( limit ) \
                      _EVT_INIT( limit, _EVT_DATA( limit ) )
#endif

/******************************************************************************
 *
 * Name              : EVT_CREATE
 * Alias             : EVT_NEW
 *
 * Description       : create and initialize an event queue object
 *
 * Parameters
 *   limit           : size of a queue (max number of stored events)
 *
 * Return            : pointer to event queue object
 *
 * Note              : use only in 'C' code
 *
 ******************************************************************************/

#ifndef __cplusplus
#define                EVT_CREATE( limit ) \
           (evt_t[]) { EVT_INIT  ( limit ) }
#define                EVT_NEW \
                       EVT_CREATE
#endif

/******************************************************************************
 *
 * Name              : evt_init
 *
 * Description       : initialize an event queue object
 *
 * Parameters
 *   evt             : pointer to event queue object
 *   data            : event queue data buffer
 *   bufsize         : size of the data buffer (in bytes)
 *
 * Return            : none
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

void evt_init( evt_t *evt, unsigned *data, unsigned bufsize );

/******************************************************************************
 *
 * Name              : evt_create
 * Alias             : evt_new
 *
 * Description       : create and initialize a new event queue object
 *
 * Parameters
 *   limit           : size of a queue (max number of stored events)
 *
 * Return            : pointer to event queue object (event queue successfully created)
 *   0               : event queue not created (not enough free memory)
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

evt_t *evt_create( unsigned limit );

__STATIC_INLINE
evt_t *evt_new( unsigned limit ) { return evt_create(limit); }

/******************************************************************************
 *
 * Name              : evt_kill
 *
 * Description       : reset the event queue object and wake up all waiting tasks with 'E_STOPPED' event value
 *
 * Parameters
 *   evt             : pointer to event queue object
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
 * Description       : reset the event queue object and free allocated resource
 *
 * Parameters
 *   evt             : pointer to event queue object
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
 * Description       : try to transfer event data from the event queue object,
 *                     wait for given duration of time while the event queue object is empty
 *
 * Parameters
 *   evt             : pointer to event queue object
 *   data            : pointer to store event data
 *   delay           : duration of time (maximum number of ticks to wait while the event queue object is empty)
 *                     IMMEDIATE: don't wait if the event queue object is empty
 *                     INFINITE:  wait indefinitely while the event queue object is empty
 *
 * Return
 *   E_SUCCESS       : event data was successfully transfered from the event queue object
 *   E_STOPPED       : event queue object was killed before the specified timeout expired
 *   E_TIMEOUT       : event queue object is empty and was not received data before the specified timeout expired
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

unsigned evt_waitFor( evt_t *evt, unsigned *data, cnt_t delay );

/******************************************************************************
 *
 * Name              : evt_waitUntil
 *
 * Description       : try to transfer event data from the event queue object,
 *                     wait until given timepoint while the event queue object is empty
 *
 * Parameters
 *   evt             : pointer to event queue object
 *   data            : pointer to store event data
 *   time            : timepoint value
 *
 * Return
 *   E_SUCCESS       : event data was successfully transfered from the event queue object
 *   E_STOPPED       : event queue object was killed before the specified timeout expired
 *   E_TIMEOUT       : event queue object is empty and was not received data before the specified timeout expired
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

unsigned evt_waitUntil( evt_t *evt, unsigned *data, cnt_t time );

/******************************************************************************
 *
 * Name              : evt_wait
 *
 * Description       : try to transfer event data from the event queue object,
 *                     wait indefinitely while the event queue object is empty
 *
 * Parameters
 *   evt             : pointer to event queue object
 *   data            : pointer to store event data
 *
 * Return
 *   E_SUCCESS       : event data was successfully transfered from the event queue object
 *   E_STOPPED       : event queue object was killed
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

__STATIC_INLINE
unsigned evt_wait( evt_t *evt, unsigned *data ) { return evt_waitFor(evt, data, INFINITE); }

/******************************************************************************
 *
 * Name              : evt_take
 * ISR alias         : evt_takeISR
 *
 * Description       : try to transfer event data from the event queue object,
 *                     don't wait if the event queue object is empty
 *
 * Parameters
 *   evt             : pointer to event queue object
 *   data            : pointer to store event data
 *
 * Return
 *   E_SUCCESS       : event data was successfully transfered from the event queue object
 *   E_TIMEOUT       : event queue object is empty
 *
 * Note              : may be used both in thread and handler mode
 *
 ******************************************************************************/

unsigned evt_take( evt_t *evt, unsigned *data );

__STATIC_INLINE
unsigned evt_takeISR( evt_t *evt, unsigned *data ) { return evt_take(evt, data); }

/******************************************************************************
 *
 * Name              : evt_sendFor
 *
 * Description       : try to transfer event data to the event queue object,
 *                     wait for given duration of time while the event queue object is full
 *
 * Parameters
 *   evt             : pointer to event queue object
 *   data            : event value
 *   delay           : duration of time (maximum number of ticks to wait while the event queue object is full)
 *                     IMMEDIATE: don't wait if the event queue object is full
 *                     INFINITE:  wait indefinitely while the event queue object is full
 *
 * Return
 *   E_SUCCESS       : event data was successfully transfered to the event queue object
 *   E_STOPPED       : event queue object was killed before the specified timeout expired
 *   E_TIMEOUT       : event queue object is full and was not issued data before the specified timeout expired
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

unsigned evt_sendFor( evt_t *evt, unsigned data, cnt_t delay );

/******************************************************************************
 *
 * Name              : evt_sendUntil
 *
 * Description       : try to transfer event data to the event queue object,
 *                     wait until given timepoint while the event queue object is full
 *
 * Parameters
 *   evt             : pointer to event queue object
 *   data            : event value
 *   time            : timepoint value
 *
 * Return
 *   E_SUCCESS       : event data was successfully transfered to the event queue object
 *   E_STOPPED       : event queue object was killed before the specified timeout expired
 *   E_TIMEOUT       : event queue object is full and was not issued data before the specified timeout expired
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

unsigned evt_sendUntil( evt_t *evt, unsigned data, cnt_t time );

/******************************************************************************
 *
 * Name              : evt_send
 *
 * Description       : try to transfer event data to the event queue object,
 *                     wait indefinitely while the event queue object is full
 *
 * Parameters
 *   evt             : pointer to event queue object
 *   data            : event value
 *
 * Return
 *   E_SUCCESS       : event data was successfully transfered to the event queue object
 *   E_STOPPED       : event queue object was killed
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

__STATIC_INLINE
unsigned evt_send( evt_t *evt, unsigned data ) { return evt_sendFor(evt, data, INFINITE); }

/******************************************************************************
 *
 * Name              : evt_give
 * ISR alias         : evt_giveISR
 *
 * Description       : try to transfer event data to the event queue object,
 *                     don't wait if the event queue object is full
 *
 * Parameters
 *   evt             : pointer to event queue object
 *   data            : event value
 *
 * Return
 *   E_SUCCESS       : event data was successfully transfered to the event queue object
 *   E_TIMEOUT       : event queue object is full
 *
 * Note              : may be used both in thread and handler mode
 *
 ******************************************************************************/

unsigned evt_give( evt_t *evt, unsigned data );

__STATIC_INLINE
unsigned evt_giveISR( evt_t *evt, unsigned data ) { return evt_give(evt, data); }

/******************************************************************************
 *
 * Name              : evt_push
 * ISR alias         : evt_pushISR
 *
 * Description       : try to transfer event data to the event queue object,
 *                     remove the oldest event data if the event queue object is full
 *
 * Parameters
 *   evt             : pointer to event queue object
 *   data            : event value
 *
 * Return
 *   E_SUCCESS       : event data was successfully transfered to the event queue object
 *   E_TIMEOUT       : there are tasks waiting for writing to the event queue object
 *
 * Note              : may be used both in thread and handler mode
 *
 ******************************************************************************/

unsigned evt_push( evt_t *evt, unsigned data );

__STATIC_INLINE
unsigned evt_pushISR( evt_t *evt, unsigned data ) { return evt_push(evt, data); }

#ifdef __cplusplus
}
#endif

/* -------------------------------------------------------------------------- */

#ifdef __cplusplus

/******************************************************************************
 *
 * Class             : EventQueueT<>
 *
 * Description       : create and initialize an event queue object
 *
 * Constructor parameters
 *   limit           : size of a queue (max number of stored events)
 *
 ******************************************************************************/

template<unsigned limit_>
struct EventQueueT : public __evt
{
	 EventQueueT( void ): __evt _EVT_INIT(limit_, data_) {}
	~EventQueueT( void ) { assert(__evt::queue == nullptr); }

	void     kill     ( void )                          {        evt_kill     (this);                }
	unsigned waitFor  ( unsigned *_data, cnt_t _delay ) { return evt_waitFor  (this, _data, _delay); }
	unsigned waitUntil( unsigned *_data, cnt_t _time )  { return evt_waitUntil(this, _data, _time);  }
	unsigned wait     ( unsigned *_data )               { return evt_wait     (this, _data);         }
	unsigned take     ( unsigned *_data )               { return evt_take     (this, _data);         }
	unsigned takeISR  ( unsigned *_data )               { return evt_takeISR  (this, _data);         }
	unsigned sendFor  ( unsigned  _data, cnt_t _delay ) { return evt_sendFor  (this, _data, _delay); }
	unsigned sendUntil( unsigned  _data, cnt_t _time )  { return evt_sendUntil(this, _data, _time);  }
	unsigned send     ( unsigned  _data )               { return evt_send     (this, _data);         }
	unsigned give     ( unsigned  _data )               { return evt_give     (this, _data);         }
	unsigned giveISR  ( unsigned  _data )               { return evt_giveISR  (this, _data);         }
	unsigned push     ( unsigned  _data )               { return evt_push     (this, _data);         }
	unsigned pushISR  ( unsigned  _data )               { return evt_pushISR  (this, _data);         }

	private:
	unsigned data_[limit_];
};

#endif//__cplusplus

/* -------------------------------------------------------------------------- */

#endif//__STATEOS_EVT_H
