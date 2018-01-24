/******************************************************************************

    @file    StateOS: os_msg.h
    @author  Rajmund Szymanski
    @date    24.01.2018
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

#ifndef __STATEOS_MSG_H
#define __STATEOS_MSG_H

#include "oskernel.h"

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************
 *
 * Name              : message queue
 *                     message is an 'unsigned int' data type; for other data types use mailbox queue
 *
 ******************************************************************************/

typedef struct __msg msg_t, * const msg_id;

struct __msg
{
	tsk_t  * queue; // inherited from semaphore
	void   * res;   // allocated message queue object's resource
	unsigned count; // inherited from semaphore
	unsigned limit; // inherited from semaphore

	unsigned first; // first element to read from queue
	unsigned next;  // next element to write into queue
	unsigned*data;  // queue data
};

/******************************************************************************
 *
 * Name              : _MSG_INIT
 *
 * Description       : create and initialize a message queue object
 *
 * Parameters
 *   limit           : size of a queue (max number of stored messages)
 *   data            : message queue data buffer
 *
 * Return            : message queue object
 *
 * Note              : for internal use
 *
 ******************************************************************************/

#define               _MSG_INIT( _limit, _data ) { 0, 0, 0, _limit, 0, 0, _data }

/******************************************************************************
 *
 * Name              : _MSG_DATA
 *
 * Description       : create a message queue data buffer
 *
 * Parameters
 *   limit           : size of a queue (max number of stored messages)
 *
 * Return            : message queue data buffer
 *
 * Note              : for internal use
 *
 ******************************************************************************/

#ifndef __cplusplus
#define               _MSG_DATA( _limit ) (unsigned[_limit]){ 0 }
#endif

/******************************************************************************
 *
 * Name              : OS_MSG
 *
 * Description       : define and initialize a message queue object
 *
 * Parameters
 *   msg             : name of a pointer to message queue object
 *   limit           : size of a queue (max number of stored messages)
 *
 ******************************************************************************/

#define             OS_MSG( msg, limit )                                \
                       unsigned msg##__buf[limit];                       \
                       msg_t msg##__msg = _MSG_INIT( limit, msg##__buf ); \
                       msg_id msg = & msg##__msg

/******************************************************************************
 *
 * Name              : static_MSG
 *
 * Description       : define and initialize a static message queue object
 *
 * Parameters
 *   msg             : name of a pointer to message queue object
 *   limit           : size of a queue (max number of stored messages)
 *
 ******************************************************************************/

#define         static_MSG( msg, limit )                                \
                static unsigned msg##__buf[limit];                       \
                static msg_t msg##__msg = _MSG_INIT( limit, msg##__buf ); \
                static msg_id msg = & msg##__msg

/******************************************************************************
 *
 * Name              : MSG_INIT
 *
 * Description       : create and initialize a message queue object
 *
 * Parameters
 *   limit           : size of a queue (max number of stored messages)
 *
 * Return            : message queue object
 *
 * Note              : use only in 'C' code
 *
 ******************************************************************************/

#ifndef __cplusplus
#define                MSG_INIT( limit ) \
                      _MSG_INIT( limit, _MSG_DATA( limit ) )
#endif

/******************************************************************************
 *
 * Name              : MSG_CREATE
 * Alias             : MSG_NEW
 *
 * Description       : create and initialize a message queue object
 *
 * Parameters
 *   limit           : size of a queue (max number of stored messages)
 *
 * Return            : pointer to message queue object
 *
 * Note              : use only in 'C' code
 *
 ******************************************************************************/

#ifndef __cplusplus
#define                MSG_CREATE( limit ) \
             & (msg_t) MSG_INIT  ( limit )
#define                MSG_NEW \
                       MSG_CREATE
#endif

/******************************************************************************
 *
 * Name              : msg_init
 *
 * Description       : initialize a message queue object
 *
 * Parameters
 *   msg             : pointer to message queue object
 *   limit           : size of a queue (max number of stored messages)
 *   data            : message queue data buffer
 *
 * Return            : none
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

void msg_init( msg_t *msg, unsigned limit, unsigned *data );

/******************************************************************************
 *
 * Name              : msg_create
 * Alias             : msg_new
 *
 * Description       : create and initialize a new message queue object
 *
 * Parameters
 *   limit           : size of a queue (max number of stored messages)
 *
 * Return            : pointer to message queue object (message queue successfully created)
 *   0               : message queue not created (not enough free memory)
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

msg_t *msg_create( unsigned limit );

__STATIC_INLINE
msg_t *msg_new( unsigned limit ) { return msg_create(limit); }

/******************************************************************************
 *
 * Name              : msg_kill
 *
 * Description       : reset the message queue object and wake up all waiting tasks with 'E_STOPPED' event value
 *
 * Parameters
 *   msg             : pointer to message queue object
 *
 * Return            : none
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

void msg_kill( msg_t *msg );

/******************************************************************************
 *
 * Name              : msg_delete
 *
 * Description       : reset the message queue object and free allocated resource
 *
 * Parameters
 *   msg             : pointer to message queue object
 *
 * Return            : none
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

void msg_delete( msg_t *msg );

/******************************************************************************
 *
 * Name              : msg_waitUntil
 *
 * Description       : try to transfer message data from the message queue object,
 *                     wait until given timepoint while the message queue object is empty
 *
 * Parameters
 *   msg             : pointer to message queue object
 *   data            : pointer to store message data
 *   time            : timepoint value
 *
 * Return
 *   E_SUCCESS       : message data was successfully transfered from the message queue object
 *   E_STOPPED       : message queue object was killed before the specified timeout expired
 *   E_TIMEOUT       : message queue object is empty and was not received data before the specified timeout expired
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

unsigned msg_waitUntil( msg_t *msg, unsigned *data, cnt_t time );

/******************************************************************************
 *
 * Name              : msg_waitFor
 *
 * Description       : try to transfer message data from the message queue object,
 *                     wait for given duration of time while the message queue object is empty
 *
 * Parameters
 *   msg             : pointer to message queue object
 *   data            : pointer to store message data
 *   delay           : duration of time (maximum number of ticks to wait while the message queue object is empty)
 *                     IMMEDIATE: don't wait if the message queue object is empty
 *                     INFINITE:  wait indefinitely while the message queue object is empty
 *
 * Return
 *   E_SUCCESS       : message data was successfully transfered from the message queue object
 *   E_STOPPED       : message queue object was killed before the specified timeout expired
 *   E_TIMEOUT       : message queue object is empty and was not received data before the specified timeout expired
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

unsigned msg_waitFor( msg_t *msg, unsigned *data, cnt_t delay );

/******************************************************************************
 *
 * Name              : msg_wait
 *
 * Description       : try to transfer message data from the message queue object,
 *                     wait indefinitely while the message queue object is empty
 *
 * Parameters
 *   msg             : pointer to message queue object
 *   data            : pointer to store message data
 *
 * Return
 *   E_SUCCESS       : message data was successfully transfered from the message queue object
 *   E_STOPPED       : message queue object was killed
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

__STATIC_INLINE
unsigned msg_wait( msg_t *msg, unsigned *data ) { return msg_waitFor(msg, data, INFINITE); }

/******************************************************************************
 *
 * Name              : msg_take
 * ISR alias         : msg_takeISR
 *
 * Description       : try to transfer message data from the message queue object,
 *                     don't wait if the message queue object is empty
 *
 * Parameters
 *   msg             : pointer to message queue object
 *   data            : pointer to store message data
 *
 * Return
 *   E_SUCCESS       : message data was successfully transfered from the message queue object
 *   E_TIMEOUT       : message queue object is empty
 *
 * Note              : may be used both in thread and handler mode
 *
 ******************************************************************************/

__STATIC_INLINE
unsigned msg_take( msg_t *msg, unsigned *data ) { return msg_waitFor(msg, data, IMMEDIATE); }

__STATIC_INLINE
unsigned msg_takeISR( msg_t *msg, unsigned *data ) { return msg_waitFor(msg, data, IMMEDIATE); }

/******************************************************************************
 *
 * Name              : msg_sendUntil
 *
 * Description       : try to transfer message data to the message queue object,
 *                     wait until given timepoint while the message queue object is full
 *
 * Parameters
 *   msg             : pointer to message queue object
 *   data            : message data
 *   time            : timepoint value
 *
 * Return
 *   E_SUCCESS       : message data was successfully transfered to the message queue object
 *   E_STOPPED       : message queue object was killed before the specified timeout expired
 *   E_TIMEOUT       : message queue object is full and was not issued data before the specified timeout expired
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

unsigned msg_sendUntil( msg_t *msg, unsigned data, cnt_t time );

/******************************************************************************
 *
 * Name              : msg_sendFor
 *
 * Description       : try to transfer message data to the message queue object,
 *                     wait for given duration of time while the message queue object is full
 *
 * Parameters
 *   msg             : pointer to message queue object
 *   data            : message data
 *   delay           : duration of time (maximum number of ticks to wait while the message queue object is full)
 *                     IMMEDIATE: don't wait if the message queue object is full
 *                     INFINITE:  wait indefinitely while the message queue object is full
 *
 * Return
 *   E_SUCCESS       : message data was successfully transfered to the message queue object
 *   E_STOPPED       : message queue object was killed before the specified timeout expired
 *   E_TIMEOUT       : message queue object is full and was not issued data before the specified timeout expired
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

unsigned msg_sendFor( msg_t *msg, unsigned data, cnt_t delay );

/******************************************************************************
 *
 * Name              : msg_send
 *
 * Description       : try to transfer message data to the message queue object,
 *                     wait indefinitely while the message queue object is full
 *
 * Parameters
 *   msg             : pointer to message queue object
 *   data            : message data
 *
 * Return
 *   E_SUCCESS       : message data was successfully transfered to the message queue object
 *   E_STOPPED       : message queue object was killed
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

__STATIC_INLINE
unsigned msg_send( msg_t *msg, unsigned data ) { return msg_sendFor(msg, data, INFINITE); }

/******************************************************************************
 *
 * Name              : msg_give
 * ISR alias         : msg_giveISR
 *
 * Description       : try to transfer message data to the message queue object,
 *                     don't wait if the message queue object is full
 *
 * Parameters
 *   msg             : pointer to message queue object
 *   data            : message data
 *
 * Return
 *   E_SUCCESS       : message data was successfully transfered to the message queue object
 *   E_TIMEOUT       : message queue object is full
 *
 * Note              : may be used both in thread and handler mode
 *
 ******************************************************************************/

__STATIC_INLINE
unsigned msg_give( msg_t *msg, unsigned data ) { return msg_sendFor(msg, data, IMMEDIATE); }

__STATIC_INLINE
unsigned msg_giveISR( msg_t *msg, unsigned data ) { return msg_sendFor(msg, data, IMMEDIATE); }

#ifdef __cplusplus
}
#endif

/* -------------------------------------------------------------------------- */

#ifdef __cplusplus

/******************************************************************************
 *
 * Class             : baseMessageQueue
 *
 * Description       : create and initialize a message queue object
 *
 * Constructor parameters
 *   limit           : size of a queue (max number of stored messages)
 *   data            : message queue data buffer
 *
 * Note              : for internal use
 *
 ******************************************************************************/

struct baseMessageQueue : public __msg
{
	 explicit
	 baseMessageQueue( const unsigned _limit, unsigned * const _data ): __msg _MSG_INIT(_limit, _data) {}
	~baseMessageQueue( void ) { assert(queue == nullptr); }

	void     kill     ( void )                         {        msg_kill     (this);                }
	unsigned waitUntil( unsigned*_data, cnt_t _time  ) { return msg_waitUntil(this, _data, _time);  }
	unsigned waitFor  ( unsigned*_data, cnt_t _delay ) { return msg_waitFor  (this, _data, _delay); }
	unsigned wait     ( unsigned*_data )               { return msg_wait     (this, _data);         }
	unsigned take     ( unsigned*_data )               { return msg_take     (this, _data);         }
	unsigned takeISR  ( unsigned*_data )               { return msg_takeISR  (this, _data);         }
	unsigned sendUntil( unsigned _data, cnt_t _time  ) { return msg_sendUntil(this, _data, _time);  }
	unsigned sendFor  ( unsigned _data, cnt_t _delay ) { return msg_sendFor  (this, _data, _delay); }
	unsigned send     ( unsigned _data )               { return msg_send     (this, _data);         }
	unsigned give     ( unsigned _data )               { return msg_give     (this, _data);         }
	unsigned giveISR  ( unsigned _data )               { return msg_giveISR  (this, _data);         }
};

/******************************************************************************
 *
 * Class             : MessageQueue
 *
 * Description       : create and initialize a message queue object
 *
 * Constructor parameters
 *   limit           : size of a queue (max number of stored messages)
 *
 ******************************************************************************/

template<unsigned _limit>
struct MessageQueueT : public baseMessageQueue
{
	explicit
	MessageQueueT( void ): baseMessageQueue(_limit, data_) {}

	private:
	unsigned data_[_limit];
};

#endif

/* -------------------------------------------------------------------------- */

#endif//__STATEOS_MSG_H
