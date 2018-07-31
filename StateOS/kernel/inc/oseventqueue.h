/******************************************************************************

    @file    StateOS: oseventqueue.h
    @author  Rajmund Szymanski
    @date    31.07.2018
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

#ifndef __STATEOS_EVQ_H
#define __STATEOS_EVQ_H

#include "oskernel.h"

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************
 *
 * Name              : event queue
 *
 ******************************************************************************/

typedef struct __evq evq_t, * const evq_id;

struct __evq
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
 * Name              : _EVQ_INIT
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

#define               _EVQ_INIT( _limit, _data ) { 0, 0, 0, _limit, 0, 0, _data }

/******************************************************************************
 *
 * Name              : _EVQ_DATA
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
#define               _EVQ_DATA( _limit ) (unsigned[_limit]){ 0 }
#endif

/******************************************************************************
 *
 * Name              : OS_EVQ
 *
 * Description       : define and initialize an event queue object
 *
 * Parameters
 *   evq             : name of a pointer to event queue object
 *   limit           : size of a queue (max number of stored events)
 *
 ******************************************************************************/

#define             OS_EVQ( evq, limit )                                \
                       unsigned evq##__buf[limit];                       \
                       evq_t evq##__evq = _EVQ_INIT( limit, evq##__buf ); \
                       evq_id evq = & evq##__evq

/******************************************************************************
 *
 * Name              : static_EVQ
 *
 * Description       : define and initialize a static event queue object
 *
 * Parameters
 *   evq             : name of a pointer to event queue object
 *   limit           : size of a queue (max number of stored events)
 *
 ******************************************************************************/

#define         static_EVQ( evq, limit )                                \
                static unsigned evq##__buf[limit];                       \
                static evq_t evq##__evq = _EVQ_INIT( limit, evq##__buf ); \
                static evq_id evq = & evq##__evq

/******************************************************************************
 *
 * Name              : EVQ_INIT
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
#define                EVQ_INIT( limit ) \
                      _EVQ_INIT( limit, _EVQ_DATA( limit ) )
#endif

/******************************************************************************
 *
 * Name              : EVQ_CREATE
 * Alias             : EVQ_NEW
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
#define                EVQ_CREATE( limit ) \
           (evq_t[]) { EVQ_INIT  ( limit ) }
#define                EVQ_NEW \
                       EVQ_CREATE
#endif

/******************************************************************************
 *
 * Name              : evq_init
 *
 * Description       : initialize an event queue object
 *
 * Parameters
 *   evq             : pointer to event queue object
 *   limit           : size of a queue (max number of stored events)
 *   data            : event queue data buffer
 *
 * Return            : none
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

void evq_init( evq_t *evq, unsigned limit, unsigned *data );

/******************************************************************************
 *
 * Name              : evq_create
 * Alias             : evq_new
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

evq_t *evq_create( unsigned limit );

__STATIC_INLINE
evq_t *evq_new( unsigned limit ) { return evq_create(limit); }

/******************************************************************************
 *
 * Name              : evq_kill
 *
 * Description       : reset the event queue object and wake up all waiting tasks with 'E_STOPPED' event value
 *
 * Parameters
 *   evq             : pointer to event queue object
 *
 * Return            : none
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

void evq_kill( evq_t *evq );

/******************************************************************************
 *
 * Name              : evq_delete
 *
 * Description       : reset the event queue object and free allocated resource
 *
 * Parameters
 *   evq             : pointer to event queue object
 *
 * Return            : none
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

void evq_delete( evq_t *evq );

/******************************************************************************
 *
 * Name              : evq_waitFor
 *
 * Description       : try to transfer event data from the event queue object,
 *                     wait for given duration of time while the event queue object is empty
 *
 * Parameters
 *   evq             : pointer to event queue object
 *   delay           : duration of time (maximum number of ticks to wait while the event queue object is empty)
 *                     IMMEDIATE: don't wait if the event queue object is empty
 *                     INFINITE:  wait indefinitely while the event queue object is empty
 *
 * Return
 *   E_STOPPED       : event queue object was killed before the specified timeout expired
 *   E_TIMEOUT       : event queue object is empty and was not received data before the specified timeout expired
 *   'another'       : task was successfully released
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

unsigned evq_waitFor( evq_t *evq, cnt_t delay );

/******************************************************************************
 *
 * Name              : evq_waitUntil
 *
 * Description       : try to transfer event data from the event queue object,
 *                     wait until given timepoint while the event queue object is empty
 *
 * Parameters
 *   evq             : pointer to event queue object
 *   time            : timepoint value
 *
 * Return
 *   E_STOPPED       : event queue object was killed before the specified timeout expired
 *   E_TIMEOUT       : event queue object is empty and was not received data before the specified timeout expired
 *   'another'       : task was successfully released
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

unsigned evq_waitUntil( evq_t *evq, cnt_t time );

/******************************************************************************
 *
 * Name              : evq_wait
 *
 * Description       : try to transfer event data from the event queue object,
 *                     wait indefinitely while the event queue object is empty
 *
 * Parameters
 *   evq             : pointer to event queue object
 *
 * Return
 *   E_STOPPED       : event queue object was killed
 *   'another'       : task was successfully released
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

__STATIC_INLINE
unsigned evq_wait( evq_t *evq ) { return evq_waitFor(evq, INFINITE); }

/******************************************************************************
 *
 * Name              : evq_take
 * ISR alias         : evq_takeISR
 *
 * Description       : try to transfer event data from the event queue object,
 *                     don't wait if the event queue object is empty
 *
 * Parameters
 *   evq             : pointer to event queue object
 *
 * Return
 *   E_TIMEOUT       : event queue object is empty
 *   'another'       : task was successfully released
 *
 * Note              : may be used both in thread and handler mode
 *
 ******************************************************************************/

unsigned evq_take( evq_t *evq );

__STATIC_INLINE
unsigned evq_takeISR( evq_t *evq ) { return evq_take(evq); }

/******************************************************************************
 *
 * Name              : evq_sendFor
 *
 * Description       : try to transfer event data to the event queue object,
 *                     wait for given duration of time while the event queue object is full
 *
 * Parameters
 *   evq             : pointer to event queue object
 *   event           : event value
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

unsigned evq_sendFor( evq_t *evq, unsigned event, cnt_t delay );

/******************************************************************************
 *
 * Name              : evq_sendUntil
 *
 * Description       : try to transfer event data to the event queue object,
 *                     wait until given timepoint while the event queue object is full
 *
 * Parameters
 *   evq             : pointer to event queue object
 *   event           : event value
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

unsigned evq_sendUntil( evq_t *evq, unsigned event, cnt_t time );

/******************************************************************************
 *
 * Name              : evq_send
 *
 * Description       : try to transfer event data to the event queue object,
 *                     wait indefinitely while the event queue object is full
 *
 * Parameters
 *   evq             : pointer to event queue object
 *   event           : event value
 *
 * Return
 *   E_SUCCESS       : event data was successfully transfered to the event queue object
 *   E_STOPPED       : event queue object was killed
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

__STATIC_INLINE
unsigned evq_send( evq_t *evq, unsigned event ) { return evq_sendFor(evq, event, INFINITE); }

/******************************************************************************
 *
 * Name              : evq_give
 * ISR alias         : evq_giveISR
 *
 * Description       : try to transfer event data to the event queue object,
 *                     don't wait if the event queue object is full
 *
 * Parameters
 *   evq             : pointer to event queue object
 *   event           : event value
 *
 * Return
 *   E_SUCCESS       : event data was successfully transfered to the event queue object
 *   E_TIMEOUT       : event queue object is full
 *
 * Note              : may be used both in thread and handler mode
 *
 ******************************************************************************/

unsigned evq_give( evq_t *evq, unsigned event );

__STATIC_INLINE
unsigned evq_giveISR( evq_t *evq, unsigned event ) { return evq_give(evq, event); }

/******************************************************************************
 *
 * Name              : evq_push
 * ISR alias         : evq_pushISR
 *
 * Description       : try to transfer event data to the event queue object,
 *                     remove the oldest event data if the event queue object is full
 *
 * Parameters
 *   evq             : pointer to event queue object
 *   event           : event value
 *
 * Return
 *   E_SUCCESS       : event data was successfully transfered to the event queue object
 *   E_TIMEOUT       : there are tasks waiting for writing to the event queue object
 *
 * Note              : may be used both in thread and handler mode
 *
 ******************************************************************************/

unsigned evq_push( evq_t *evq, unsigned event );

__STATIC_INLINE
unsigned evq_pushISR( evq_t *evq, unsigned event ) { return evq_push(evq, event); }

#ifdef __cplusplus
}
#endif

/* -------------------------------------------------------------------------- */

#ifdef __cplusplus

/******************************************************************************
 *
 * Class             : baseEventQueue
 *
 * Description       : create and initialize an event queue object
 *
 * Constructor parameters
 *   limit           : size of a queue (max number of stored events)
 *   data            : event queue data buffer
 *
 * Note              : for internal use
 *
 ******************************************************************************/

struct baseEventQueue : public __evq
{
	 explicit
	 baseEventQueue( const unsigned _limit, unsigned * const _data ): __evq _EVQ_INIT(_limit, _data) {}
	~baseEventQueue( void ) { assert(queue == nullptr); }

	void     kill     ( void )                          {        evq_kill     (this);                 }
	unsigned waitFor  (                  cnt_t _delay ) { return evq_waitFor  (this,         _delay); }
	unsigned waitUntil(                  cnt_t _time  ) { return evq_waitUntil(this,         _time);  }
	unsigned wait     ( void )                          { return evq_wait     (this);                 }
	unsigned take     ( void )                          { return evq_take     (this);                 }
	unsigned takeISR  ( void )                          { return evq_takeISR  (this);                 }
	unsigned sendFor  ( unsigned _event, cnt_t _delay ) { return evq_sendFor  (this, _event, _delay); }
	unsigned sendUntil( unsigned _event, cnt_t _time  ) { return evq_sendUntil(this, _event, _time);  }
	unsigned send     ( unsigned _event )               { return evq_send     (this, _event);         }
	unsigned give     ( unsigned _event )               { return evq_give     (this, _event);         }
	unsigned giveISR  ( unsigned _event )               { return evq_giveISR  (this, _event);         }
	unsigned push     ( unsigned _event )               { return evq_push     (this, _event);         }
	unsigned pushISR  ( unsigned _event )               { return evq_pushISR  (this, _event);         }
};

/******************************************************************************
 *
 * Class             : EventQueue
 *
 * Description       : create and initialize an event queue object
 *
 * Constructor parameters
 *   limit           : size of a queue (max number of stored events)
 *
 ******************************************************************************/

template<unsigned _limit>
struct EventQueueT : public baseEventQueue
{
	explicit
	EventQueueT( void ): baseEventQueue(_limit, data_) {}

	private:
	unsigned data_[_limit];
};

#endif

/* -------------------------------------------------------------------------- */

#endif//__STATEOS_EVQ_H
