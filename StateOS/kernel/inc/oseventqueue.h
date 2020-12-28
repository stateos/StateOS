/******************************************************************************

    @file    StateOS: oseventqueue.h
    @author  Rajmund Szymanski
    @date    28.12.2020
    @brief   This file contains definitions for StateOS.

 ******************************************************************************

   Copyright (c) 2018-2020 Rajmund Szymanski. All rights reserved.

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
#include "osclock.h"

/******************************************************************************
 *
 * Name              : event queue
 *
 ******************************************************************************/

typedef struct __evq evq_t, * const evq_id;

struct __evq
{
	obj_t    obj;   // object header

	unsigned count; // inherited from semaphore
	unsigned limit; // inherited from semaphore

	unsigned head;  // first element to read from data buffer
	unsigned tail;  // first element to write into data buffer
	unsigned*data;  // data buffer
};

#ifdef __cplusplus
extern "C" {
#endif

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

#define               _EVQ_INIT( _limit, _data ) { _OBJ_INIT(), 0, _limit, 0, 0, _data }

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
 *   data            : event queue data buffer
 *   bufsize         : size of the data buffer (in bytes)
 *
 * Return            : none
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

void evq_init( evq_t *evq, unsigned *data, size_t bufsize );

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
 * Return            : pointer to event queue object
 *   NULL            : object not created (not enough free memory)
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

evq_t *evq_create( unsigned limit );

__STATIC_INLINE
evq_t *evq_new( unsigned limit ) { return evq_create(limit); }

/******************************************************************************
 *
 * Name              : evq_reset
 * Alias             : evq_kill
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

void evq_reset( evq_t *evq );

__STATIC_INLINE
void evq_kill( evq_t *evq ) { evq_reset(evq); }

/******************************************************************************
 *
 * Name              : evq_destroy
 * Alias             : evq_delete
 *
 * Description       : reset the event queue object, wake up all waiting tasks with 'E_DELETED' event value and free allocated resource
 *
 * Parameters
 *   evq             : pointer to event queue object
 *
 * Return            : none
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

void evq_destroy( evq_t *evq );

__STATIC_INLINE
void evq_delete( evq_t *evq ) { evq_destroy(evq); }

/******************************************************************************
 *
 * Name              : evq_take
 * Alias             : evq_tryWait
 * ISR alias         : evq_takeISR
 * Async alias       : evq_takeAsync
 *
 * Description       : try to transfer event value from the event queue object,
 *                     don't wait if the event queue object is empty
 *
 * Parameters
 *   evq             : pointer to event queue object
 *   event           : pointer to store event value
 *
 * Return
 *   E_SUCCESS       : event value was successfully transferred from the event queue object
 *   E_TIMEOUT       : event queue object is empty, try again
 *
 * Note              : can be used in both thread and handler mode
 *                     use ISR alias in blockable interrupt handlers
 *                     use Async alias for communication with unmasked interrupt handlers
 *
 ******************************************************************************/

int evq_take( evq_t *evq, unsigned *event );

__STATIC_INLINE
int evq_tryWait( evq_t *evq, unsigned *event ) { return evq_take(evq, event); }

__STATIC_INLINE
int evq_takeISR( evq_t *evq, unsigned *event ) { return evq_take(evq, event); }

#if OS_ATOMICS
int evq_takeAsync( evq_t *evq, unsigned *event );
#endif

/******************************************************************************
 *
 * Name              : evq_waitFor
 *
 * Description       : try to transfer event value from the event queue object,
 *                     wait for given duration of time while the event queue object is empty
 *
 * Parameters
 *   evq             : pointer to event queue object
 *   event           : pointer to store event value
 *   delay           : duration of time (maximum number of ticks to wait while the event queue object is empty)
 *                     IMMEDIATE: don't wait if the event queue object is empty
 *                     INFINITE:  wait indefinitely while the event queue object is empty
 *
 * Return
 *   E_SUCCESS       : event value was successfully transferred from the event queue object
 *   E_STOPPED       : event queue object was reseted before the specified timeout expired
 *   E_DELETED       : event queue object was deleted before the specified timeout expired
 *   E_TIMEOUT       : event queue object is empty and was not received data before the specified timeout expired
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

int evq_waitFor( evq_t *evq, unsigned *event, cnt_t delay );

/******************************************************************************
 *
 * Name              : evq_waitUntil
 *
 * Description       : try to transfer event value from the event queue object,
 *                     wait until given timepoint while the event queue object is empty
 *
 * Parameters
 *   evq             : pointer to event queue object
 *   event           : pointer to store event value
 *   time            : timepoint value
 *
 * Return
 *   E_SUCCESS       : event value was successfully transferred from the event queue object
 *   E_STOPPED       : event queue object was reseted before the specified timeout expired
 *   E_DELETED       : event queue object was deleted before the specified timeout expired
 *   E_TIMEOUT       : event queue object is empty and was not received data before the specified timeout expired
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

int evq_waitUntil( evq_t *evq, unsigned *event, cnt_t time );

/******************************************************************************
 *
 * Name              : evq_wait
 * Async alias       : evq_waitAsync
 *
 * Description       : try to transfer event value from the event queue object,
 *                     wait indefinitely while the event queue object is empty
 *
 * Parameters
 *   evq             : pointer to event queue object
 *   event           : pointer to store event value
 *
 * Return
 *   E_SUCCESS       : event value was successfully transferred from the event queue object
 *   E_STOPPED       : event queue object was reseted (unavailable for async version)
 *   E_DELETED       : event queue object was deleted (unavailable for async version)
 *
 * Note              : use only in thread mode
 *                     use Async alias for communication with unmasked interrupt handlers
 *
 ******************************************************************************/

__STATIC_INLINE
int evq_wait( evq_t *evq, unsigned *event ) { return evq_waitFor(evq, event, INFINITE); }

#if OS_ATOMICS
int evq_waitAsync( evq_t *evq, unsigned *event );
#endif

/******************************************************************************
 *
 * Name              : evq_give
 * ISR alias         : evq_giveISR
 * Async alias       : evq_giveAsync
 *
 * Description       : try to transfer event value to the event queue object,
 *                     don't wait if the event queue object is full
 *
 * Parameters
 *   evq             : pointer to event queue object
 *   event           : event value
 *
 * Return
 *   E_SUCCESS       : event value was successfully transferred to the event queue object
 *   E_TIMEOUT       : event queue object is full, try again
 *
 * Note              : can be used in both thread and handler mode
 *                     use ISR alias in blockable interrupt handlers
 *                     use Async alias for communication with unmasked interrupt handlers
 *
 ******************************************************************************/

int evq_give( evq_t *evq, unsigned event );

__STATIC_INLINE
int evq_giveISR( evq_t *evq, unsigned event ) { return evq_give(evq, event); }

#if OS_ATOMICS
int evq_giveAsync( evq_t *evq, unsigned event );
#endif

/******************************************************************************
 *
 * Name              : evq_sendFor
 *
 * Description       : try to transfer event value to the event queue object,
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
 *   E_SUCCESS       : event value was successfully transferred to the event queue object
 *   E_STOPPED       : event queue object was reseted before the specified timeout expired
 *   E_DELETED       : event queue object was deleted before the specified timeout expired
 *   E_TIMEOUT       : event queue object is full and was not issued data before the specified timeout expired
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

int evq_sendFor( evq_t *evq, unsigned event, cnt_t delay );

/******************************************************************************
 *
 * Name              : evq_sendUntil
 *
 * Description       : try to transfer event value to the event queue object,
 *                     wait until given timepoint while the event queue object is full
 *
 * Parameters
 *   evq             : pointer to event queue object
 *   event           : event value
 *   time            : timepoint value
 *
 * Return
 *   E_SUCCESS       : event value was successfully transferred to the event queue object
 *   E_STOPPED       : event queue object was reseted before the specified timeout expired
 *   E_DELETED       : event queue object was deleted before the specified timeout expired
 *   E_TIMEOUT       : event queue object is full and was not issued data before the specified timeout expired
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

int evq_sendUntil( evq_t *evq, unsigned event, cnt_t time );

/******************************************************************************
 *
 * Name              : evq_send
 * Async alias       : evq_sendAsync
 *
 * Description       : try to transfer event value to the event queue object,
 *                     wait indefinitely while the event queue object is full
 *
 * Parameters
 *   evq             : pointer to event queue object
 *   event           : event value
 *
 * Return
 *   E_SUCCESS       : event value was successfully transferred to the event queue object
 *   E_STOPPED       : event queue object was reseted (unavailable for async version)
 *   E_DELETED       : event queue object was deleted (unavailable for async version)
 *
 * Note              : use only in thread mode
 *                     use Async alias for communication with unmasked interrupt handlers
 *
 ******************************************************************************/

__STATIC_INLINE
int evq_send( evq_t *evq, unsigned event ) { return evq_sendFor(evq, event, INFINITE); }

#if OS_ATOMICS
int evq_sendAsync( evq_t *evq, unsigned event );
#endif

/******************************************************************************
 *
 * Name              : evq_push
 * ISR alias         : evq_pushISR
 *
 * Description       : try to transfer event value to the event queue object,
 *                     remove the oldest event value if the event queue object is full
 *
 * Parameters
 *   evq             : pointer to event queue object
 *   event           : event value
 *
 * Return            : none
 *
 * Note              : can be used in both thread and handler mode (for blockable interrupts)
 *                     use ISR alias in blockable interrupt handlers
 *
 ******************************************************************************/

void evq_push( evq_t *evq, unsigned event );

__STATIC_INLINE
void evq_pushISR( evq_t *evq, unsigned event ) { evq_push(evq, event); }

/******************************************************************************
 *
 * Name              : evq_count
 * ISR alias         : evq_countISR
 *
 * Description       : return the amount of data contained in the event queue
 *
 * Parameters
 *   evq             : pointer to event queue object
 *
 * Return            : amount of data contained in the event queue
 *
 * Note              : can be used in both thread and handler mode (for blockable interrupts)
 *                     use ISR alias in blockable interrupt handlers
 *
 ******************************************************************************/

unsigned evq_count( evq_t *evq );

__STATIC_INLINE
unsigned evq_countISR( evq_t *evq ) { return evq_count(evq); }

/******************************************************************************
 *
 * Name              : evq_space
 * ISR alias         : evq_spaceISR
 *
 * Description       : return the amount of free space in the event queue
 *
 * Parameters
 *   evq             : pointer to event queue object
 *
 * Return            : amount of free space in the event queue
 *
 * Note              : can be used in both thread and handler mode (for blockable interrupts)
 *                     use ISR alias in blockable interrupt handlers
 *
 ******************************************************************************/

unsigned evq_space( evq_t *evq );

__STATIC_INLINE
unsigned evq_spaceISR( evq_t *evq ) { return evq_space(evq); }

/******************************************************************************
 *
 * Name              : evq_limit
 * ISR alias         : evq_limitISR
 *
 * Description       : return the size of the event queue
 *
 * Parameters
 *   evq             : pointer to event queue object
 *
 * Return            : size of the event queue
 *
 * Note              : can be used in both thread and handler mode (for blockable interrupts)
 *                     use ISR alias in blockable interrupt handlers
 *
 ******************************************************************************/

unsigned evq_limit( evq_t *evq );

__STATIC_INLINE
unsigned evq_limitISR( evq_t *evq ) { return evq_limit(evq); }

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
struct EventQueueT : public __evq
{
	constexpr
	EventQueueT( void ): __evq _EVQ_INIT(limit_, data_) {}

	EventQueueT( EventQueueT&& ) = default;
	EventQueueT( const EventQueueT& ) = delete;
	EventQueueT& operator=( EventQueueT&& ) = delete;
	EventQueueT& operator=( const EventQueueT& ) = delete;

	~EventQueueT( void ) { assert(__evq::obj.queue == nullptr); }

#if __cplusplus >= 201402
	using Ptr = std::unique_ptr<EventQueueT<limit_>>;
#else
	using Ptr = EventQueueT<limit_> *;
#endif

/******************************************************************************
 *
 * Name              : EventQueueT<>::Create
 *
 * Description       : create dynamic object with manageable resources
 *
 * Parameters
 *   limit           : size of a queue (max number of stored events)
 *
 * Return            : std::unique_pointer / pointer to EventQueueT<> object
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

	static
	Ptr Create( void )
	{
		auto evq = new EventQueueT<limit_>();
		if (evq != nullptr)
			evq->__evq::obj.res = evq;
		return Ptr(evq);
	}

	void     reset    ( void )                             {        evq_reset    (this); }
	void     kill     ( void )                             {        evq_kill     (this); }
	void     destroy  ( void )                             {        evq_destroy  (this); }
	int      take     ( unsigned *_event )                 { return evq_take     (this, _event); }
	int      tryWait  ( unsigned *_event )                 { return evq_tryWait  (this, _event); }
	int      takeISR  ( unsigned *_event )                 { return evq_takeISR  (this, _event); }
	template<typename T>
	int      waitFor  ( unsigned *_event, const T _delay ) { return evq_waitFor  (this, _event, Clock::count(_delay)); }
	template<typename T>
	int      waitUntil( unsigned *_event, const T _time )  { return evq_waitUntil(this, _event, Clock::until(_time)); }
	int      wait     ( unsigned *_event )                 { return evq_wait     (this, _event); }
	int      give     ( unsigned  _event )                 { return evq_give     (this, _event); }
	int      giveISR  ( unsigned  _event )                 { return evq_giveISR  (this, _event); }
	template<typename T>
	int      sendFor  ( unsigned  _event, const T _delay ) { return evq_sendFor  (this, _event, Clock::count(_delay)); }
	template<typename T>
	int      sendUntil( unsigned  _event, const T _time )  { return evq_sendUntil(this, _event, Clock::until(_time)); }
	int      send     ( unsigned  _event )                 { return evq_send     (this, _event); }
	void     push     ( unsigned  _event )                 {        evq_push     (this, _event); }
	void     pushISR  ( unsigned  _event )                 {        evq_pushISR  (this, _event); }
	unsigned count    ( void )                             { return evq_count    (this); }
	unsigned countISR ( void )                             { return evq_countISR (this); }
	unsigned space    ( void )                             { return evq_space    (this); }
	unsigned spaceISR ( void )                             { return evq_spaceISR (this); }
	unsigned limit    ( void )                             { return evq_limit    (this); }
	unsigned limitISR ( void )                             { return evq_limitISR (this); }
#if OS_ATOMICS
	int      takeAsync( unsigned *_event )                 { return evq_takeAsync(this, _event); }
	int      waitAsync( unsigned *_event )                 { return evq_waitAsync(this, _event); }
	int      giveAsync( unsigned  _event )                 { return evq_giveAsync(this, _event); }
	int      sendAsync( unsigned  _event )                 { return evq_sendAsync(this, _event); }
#endif

	private:
	unsigned data_[limit_];
};

#endif//__cplusplus

/* -------------------------------------------------------------------------- */

#endif//__STATEOS_EVQ_H
