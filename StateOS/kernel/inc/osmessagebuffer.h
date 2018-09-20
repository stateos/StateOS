/******************************************************************************

    @file    StateOS: osmessagebuffer.h
    @author  Rajmund Szymanski
    @date    20.09.2018
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
 * Name              : message buffer
 *
 ******************************************************************************/

typedef struct __msg msg_t, * const msg_id;

struct __msg
{
	obj_t    obj;   // object header

	unsigned count; // inherited from stream buffer
	unsigned limit; // inherited from stream buffer

	unsigned head;  // inherited from stream buffer
	unsigned tail;  // inherited from stream buffer
	char   * data;  // inherited from stream buffer
};

/******************************************************************************
 *
 * Name              : _MSG_INIT
 *
 * Description       : create and initialize a message buffer object
 *
 * Parameters
 *   limit           : size of a buffer (max number of stored bytes)
 *   data            : message buffer data
 *
 * Return            : message buffer object
 *
 * Note              : for internal use
 *
 ******************************************************************************/

#define               _MSG_INIT( _limit, _data ) { _OBJ_INIT(), 0, _limit, 0, 0, _data }

/******************************************************************************
 *
 * Name              : _MSG_DATA
 *
 * Description       : create a message buffer data
 *
 * Parameters
 *   limit           : size of a buffer (max number of stored bytes)
 *
 * Return            : message buffer data
 *
 * Note              : for internal use
 *
 ******************************************************************************/

#ifndef __cplusplus
#define               _MSG_DATA( _limit ) (char[_limit]){ 0 }
#endif

/******************************************************************************
 *
 * Name              : _VA_MSG
 *
 * Description       : calculate buffer size from optional parameter
 *
 * Note              : for internal use
 *
 ******************************************************************************/

#define               _VA_MSG( _limit, _size ) ( (_size + 0) ? ((_limit) * (sizeof(unsigned) + (_size + 0))) : (_limit) )

/******************************************************************************
 *
 * Name              : OS_MSG
 *
 * Description       : define and initialize a message buffer object
 *
 * Parameters
 *   msg             : name of a pointer to message buffer object
 *   limit           : size of a buffer (max number of stored bytes / objects)
 *   type            : (optional) size of the object (in bytes)
 *
 ******************************************************************************/

#define             OS_MSG( msg, limit, ... )                                                 \
                       char msg##__buf[_VA_MSG(limit, __VA_ARGS__)];                           \
                       msg_t msg##__msg = _MSG_INIT( _VA_MSG(limit, __VA_ARGS__), msg##__buf ); \
                       msg_id msg = & msg##__msg

/******************************************************************************
 *
 * Name              : static_MSG
 *
 * Description       : define and initialize a static message buffer object
 *
 * Parameters
 *   msg             : name of a pointer to message buffer object
 *   limit           : size of a buffer (max number of stored bytes / objects)
 *   type            : (optional) size of the object (in bytes)
 *
 ******************************************************************************/

#define         static_MSG( msg, limit, ... )                                                 \
                static char msg##__buf[_VA_MSG(limit, __VA_ARGS__)];                           \
                static msg_t msg##__msg = _MSG_INIT( _VA_MSG(limit, __VA_ARGS__), msg##__buf ); \
                static msg_id msg = & msg##__msg

/******************************************************************************
 *
 * Name              : MSG_INIT
 *
 * Description       : create and initialize a message buffer object
 *
 * Parameters
 *   limit           : size of a buffer (max number of stored bytes / objects)
 *   type            : (optional) size of the object (in bytes)
 *
 * Return            : message buffer object
 *
 * Note              : use only in 'C' code
 *
 ******************************************************************************/

#ifndef __cplusplus
#define                MSG_INIT( limit, ... ) \
                      _MSG_INIT( _VA_MSG(limit, __VA_ARGS__), _MSG_DATA( _VA_MSG(limit, __VA_ARGS__) ) )
#endif

/******************************************************************************
 *
 * Name              : MSG_CREATE
 * Alias             : MSG_NEW
 *
 * Description       : create and initialize a message buffer object
 *
 * Parameters
 *   limit           : size of a buffer (max number of stored bytes / objects)
 *   type            : (optional) size of the object (in bytes)
 *
 * Return            : pointer to message buffer object
 *
 * Note              : use only in 'C' code
 *
 ******************************************************************************/

#ifndef __cplusplus
#define                MSG_CREATE( limit, ... ) \
           (msg_t[]) { MSG_INIT  ( _VA_MSG(limit, __VA_ARGS__) ) }
#define                MSG_NEW \
                       MSG_CREATE
#endif

/******************************************************************************
 *
 * Name              : msg_init
 *
 * Description       : initialize a message buffer object
 *
 * Parameters
 *   msg             : pointer to message buffer object
 *   data            : message buffer data
 *   bufsize         : size of the data buffer (in bytes)
 *
 * Return            : none
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

void msg_init( msg_t *msg, void *data, unsigned bufsize );

/******************************************************************************
 *
 * Name              : msg_create
 * Alias             : msg_new
 *
 * Description       : create and initialize a new message buffer object
 *
 * Parameters
 *   limit           : size of a buffer (max number of stored bytes)
 *
 * Return            : pointer to message buffer object (message buffer successfully created)
 *   0               : message buffer not created (not enough free memory)
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
 * Description       : reset the message buffer object and wake up all waiting tasks with 'E_STOPPED' event value
 *
 * Parameters
 *   msg             : pointer to message buffer object
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
 * Description       : reset the message buffer object and free allocated resource
 *
 * Parameters
 *   msg             : pointer to message buffer object
 *
 * Return            : none
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

void msg_delete( msg_t *msg );

/******************************************************************************
 *
 * Name              : msg_take
 * Alias             : msg_tryWait
 * ISR alias         : msg_takeISR
 *
 * Description       : try to transfer data from the message buffer object,
 *                     don't wait if the message buffer object is empty
 *
 * Parameters
 *   msg             : pointer to message buffer object
 *   data            : pointer to write buffer
 *   size            : size of write buffer
 *
 * Return            : number of bytes read from the message buffer or
 *   E_FAILURE       : not enough space in the write buffer
 *   E_TIMEOUT       : message buffer object is empty, try wait
 *
 * Note              : may be used both in thread and handler mode
 *
 ******************************************************************************/

unsigned msg_take( msg_t *msg, void *data, unsigned size );

__STATIC_INLINE
unsigned msg_tryWait( msg_t *msg, void *data, unsigned size ) { return msg_take(msg, data, size); }

__STATIC_INLINE
unsigned msg_takeISR( msg_t *msg, void *data, unsigned size ) { return msg_take(msg, data, size); }

/******************************************************************************
 *
 * Name              : msg_waitFor
 *
 * Description       : try to transfer data from the message buffer object,
 *                     wait for given duration of time while the message buffer object is empty
 *
 * Parameters
 *   msg             : pointer to message buffer object
 *   data            : pointer to write buffer
 *   size            : size of write buffer
 *   delay           : duration of time (maximum number of ticks to wait while the message buffer object is empty)
 *                     IMMEDIATE: don't wait if the message buffer object is empty
 *                     INFINITE:  wait indefinitely while the message buffer object is empty
 *
 * Return            : number of bytes read from the message buffer or
 *   E_FAILURE       : not enough space in the write buffer
 *   E_STOPPED       : message buffer object was killed before the specified timeout expired
 *   E_TIMEOUT       : message buffer object is empty and was not received data before the specified timeout expired
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

unsigned msg_waitFor( msg_t *msg, void *data, unsigned size, cnt_t delay );

/******************************************************************************
 *
 * Name              : msg_waitUntil
 *
 * Description       : try to transfer data from the message buffer object,
 *                     wait until given timepoint while the message buffer object is empty
 *
 * Parameters
 *   msg             : pointer to message buffer object
 *   data            : pointer to write buffer
 *   size            : size of write buffer
 *   time            : timepoint value
 *
 * Return            : number of bytes read from the message buffer or
 *   E_FAILURE       : not enough space in the write buffer
 *   E_STOPPED       : message buffer object was killed before the specified timeout expired
 *   E_TIMEOUT       : message buffer object is empty and was not received data before the specified timeout expired
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

unsigned msg_waitUntil( msg_t *msg, void *data, unsigned size, cnt_t time );

/******************************************************************************
 *
 * Name              : msg_wait
 *
 * Description       : try to transfer data from the message buffer object,
 *                     wait indefinitely while the message buffer object is empty
 *
 * Parameters
 *   msg             : pointer to message buffer object
 *   data            : pointer to write buffer
 *   size            : size of write buffer
 *
 * Return            : number of bytes read from the message buffer or
 *   E_FAILURE       : not enough space in the write buffer
 *   E_STOPPED       : message buffer object was killed
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

__STATIC_INLINE
unsigned msg_wait( msg_t *msg, void *data, unsigned size ) { return msg_waitFor(msg, data, size, INFINITE); }

/******************************************************************************
 *
 * Name              : msg_give
 * ISR alias         : msg_giveISR
 *
 * Description       : try to transfer data to the message buffer object,
 *                     don't wait if the message buffer object is full
 *
 * Parameters
 *   msg             : pointer to message buffer object
 *   data            : pointer to read buffer
 *   size            : size of read buffer
 *
 * Return
 *   E_SUCCESS       : message data was successfully transfered to the message buffer object
 *   E_FAILURE       : size of the message data is out of the limit
 *   E_TIMEOUT       : not enough space in the message buffer, try wait
 *
 * Note              : may be used both in thread and handler mode
 *
 ******************************************************************************/

unsigned msg_give( msg_t *msg, const void *data, unsigned size );

__STATIC_INLINE
unsigned msg_giveISR( msg_t *msg, const void *data, unsigned size ) { return msg_give(msg, data, size); }

/******************************************************************************
 *
 * Name              : msg_sendFor
 *
 * Description       : try to transfer data to the message buffer object,
 *                     wait for given duration of time while the message buffer object is full
 *
 * Parameters
 *   msg             : pointer to message buffer object
 *   data            : pointer to read buffer
 *   size            : size of read buffer
 *   delay           : duration of time (maximum number of ticks to wait while the message buffer object is full)
 *                     IMMEDIATE: don't wait if the message buffer object is full
 *                     INFINITE:  wait indefinitely while the message buffer object is full
 *
 * Return
 *   E_SUCCESS       : message data was successfully transfered to the message buffer object
 *   E_FAILURE       : size of the message data is out of the limit
 *   E_STOPPED       : message buffer object was killed before the specified timeout expired
 *   E_TIMEOUT       : message buffer object is full and was not issued data before the specified timeout expired
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

unsigned msg_sendFor( msg_t *msg, const void *data, unsigned size, cnt_t delay );

/******************************************************************************
 *
 * Name              : msg_sendUntil
 *
 * Description       : try to transfer data to the message buffer object,
 *                     wait until given timepoint while the message buffer object is full
 *
 * Parameters
 *   msg             : pointer to message buffer object
 *   data            : pointer to read buffer
 *   size            : size of read buffer
 *   time            : timepoint value
 *
 * Return
 *   E_SUCCESS       : message data was successfully transfered to the message buffer object
 *   E_FAILURE       : size of the message data is out of the limit
 *   E_STOPPED       : message buffer object was killed before the specified timeout expired
 *   E_TIMEOUT       : message buffer object is full and was not issued data before the specified timeout expired
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

unsigned msg_sendUntil( msg_t *msg, const void *data, unsigned size, cnt_t time );

/******************************************************************************
 *
 * Name              : msg_send
 *
 * Description       : try to transfer data to the message buffer object,
 *                     wait indefinitely while the message buffer object is full
 *
 * Parameters
 *   msg             : pointer to message buffer object
 *   data            : pointer to read buffer
 *   size            : size of read buffer
 *
 * Return
 *   E_SUCCESS       : message data was successfully transfered to the message buffer object
 *   E_FAILURE       : size of the message data is out of the limit
 *   E_STOPPED       : message buffer object was killed
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

__STATIC_INLINE
unsigned msg_send( msg_t *msg, const void *data, unsigned size ) { return msg_sendFor(msg, data, size, INFINITE); }

/******************************************************************************
 *
 * Name              : msg_push
 * ISR alias         : msg_pushISR
 *
 * Description       : try to transfer data to the message buffer object,
 *                     remove the oldest data if the message buffer object is full
 *
 * Parameters
 *   msg             : pointer to message buffer object
 *   data            : pointer to read buffer
 *   size            : size of read buffer
 *
 * Return
 *   E_SUCCESS       : message data was successfully transfered to the message buffer object
 *   E_FAILURE       : size of the message data is out of the limit
 *
 * Note              : may be used both in thread and handler mode
 *
 ******************************************************************************/

unsigned msg_push( msg_t *msg, const void *data, unsigned size );

__STATIC_INLINE
unsigned msg_pushISR( msg_t *msg, const void *data, unsigned size ) { return msg_push(msg, data, size); }

/******************************************************************************
 *
 * Name              : msg_count
 * ISR alias         : msg_countISR
 *
 * Description       : return the size of the first message in the buffer
 *
 * Parameters
 *   msg             : pointer to message buffer object
 *
 * Return            : amount of data contained in the first message
 *
 * Note              : may be used both in thread and handler mode
 *
 ******************************************************************************/

unsigned msg_count( msg_t *msg );

__STATIC_INLINE
unsigned msg_countISR( msg_t *msg ) { return msg_count(msg); }

/******************************************************************************
 *
 * Name              : msg_space
 * ISR alias         : msg_spaceISR
 *
 * Description       : return the amount of free space in the message buffer
 *
 * Parameters
 *   msg             : pointer to message buffer object
 *
 * Return            : amount of free space in the message buffer
 *
 * Note              : may be used both in thread and handler mode
 *
 ******************************************************************************/

unsigned msg_space( msg_t *msg );

__STATIC_INLINE
unsigned msg_spaceISR( msg_t *msg ) { return msg_space(msg); }

/******************************************************************************
 *
 * Name              : msg_limit
 * ISR alias         : msg_limitISR
 *
 * Description       : return the size of the message buffer
 *
 * Parameters
 *   msg             : pointer to message buffer object
 *
 * Return            : size of the message buffer
 *
 * Note              : may be used both in thread and handler mode
 *
 ******************************************************************************/

unsigned msg_limit( msg_t *msg );

__STATIC_INLINE
unsigned msg_limitISR( msg_t *msg ) { return msg_limit(msg); }

/******************************************************************************
 *
 * Name              : msg_size
 * ISR alias         : msg_sizeISR
 *
 * Description       : return the size of first message in the message buffer
 *
 * Parameters
 *   msg             : pointer to message buffer object
 *
 * Return            : size of first message in the message buffer
 *
 * Note              : may be used both in thread and handler mode
 *
 ******************************************************************************/

unsigned msg_size( msg_t *msg );

__STATIC_INLINE
unsigned msg_sizeISR( msg_t *msg ) { return msg_size(msg); }

#ifdef __cplusplus
}
#endif

/* -------------------------------------------------------------------------- */

#ifdef __cplusplus

/******************************************************************************
 *
 * Class             : MessageBufferT<>
 *
 * Description       : create and initialize a message buffer object
 *
 * Constructor parameters
 *   limit           : size of a buffer (max number of stored bytes)
 *
 ******************************************************************************/

template<unsigned limit_>
struct MessageBufferT : public __msg
{
	 MessageBufferT( void ): __msg _MSG_INIT(limit_, data_) {}
	~MessageBufferT( void ) { assert(__msg::obj.queue == nullptr); }

	void     kill     ( void )                                            {        msg_kill     (this);                       }
	unsigned take     (       void *_data, unsigned _size )               { return msg_take     (this, _data, _size);         }
	unsigned tryWait  (       void *_data, unsigned _size )               { return msg_tryWait  (this, _data, _size);         }
	unsigned takeISR  (       void *_data, unsigned _size )               { return msg_takeISR  (this, _data, _size);         }
	unsigned waitFor  (       void *_data, unsigned _size, cnt_t _delay ) { return msg_waitFor  (this, _data, _size, _delay); }
	unsigned waitUntil(       void *_data, unsigned _size, cnt_t _time )  { return msg_waitUntil(this, _data, _size, _time);  }
	unsigned wait     (       void *_data, unsigned _size )               { return msg_wait     (this, _data, _size);         }
	unsigned give     ( const void *_data, unsigned _size )               { return msg_give     (this, _data, _size);         }
	unsigned giveISR  ( const void *_data, unsigned _size )               { return msg_giveISR  (this, _data, _size);         }
	unsigned sendFor  ( const void *_data, unsigned _size, cnt_t _delay ) { return msg_sendFor  (this, _data, _size, _delay); }
	unsigned sendUntil( const void *_data, unsigned _size, cnt_t _time )  { return msg_sendUntil(this, _data, _size, _time);  }
	unsigned send     ( const void *_data, unsigned _size )               { return msg_send     (this, _data, _size);         }
	unsigned push     ( const void *_data, unsigned _size )               { return msg_push     (this, _data, _size);         }
	unsigned pushISR  ( const void *_data, unsigned _size )               { return msg_pushISR  (this, _data, _size);         }
	unsigned count    ( void )                                            { return msg_count    (this);                       }
	unsigned countISR ( void )                                            { return msg_countISR (this);                       }
	unsigned space    ( void )                                            { return msg_space    (this);                       }
	unsigned spaceISR ( void )                                            { return msg_spaceISR (this);                       }
	unsigned limit    ( void )                                            { return msg_limit    (this);                       }
	unsigned limitISR ( void )                                            { return msg_limitISR (this);                       }
	unsigned size     ( void )                                            { return msg_size     (this);                       }
	unsigned sizeISR  ( void )                                            { return msg_sizeISR  (this);                       }

	private:
	char data_[limit_];
};

/******************************************************************************
 *
 * Class             : MessageBufferTT<>
 *
 * Description       : create and initialize a message buffer object
 *
 * Constructor parameters
 *   limit           : size of a buffer (max number of stored objects)
 *   T               : class of an object
 *
 ******************************************************************************/

template<unsigned limit_, class T>
struct MessageBufferTT : public MessageBufferT<limit_*(sizeof(unsigned)+sizeof(T))>
{
	MessageBufferTT( void ): MessageBufferT<limit_*(sizeof(unsigned)+sizeof(T))>() {}

	unsigned take     (       T *_data )               { return msg_take     (this, _data, sizeof(T));         }
	unsigned tryWait  (       T *_data )               { return msg_tryWait  (this, _data, sizeof(T));         }
	unsigned takeISR  (       T *_data )               { return msg_takeISR  (this, _data, sizeof(T));         }
	unsigned waitFor  (       T *_data, cnt_t _delay ) { return msg_waitFor  (this, _data, sizeof(T), _delay); }
	unsigned waitUntil(       T *_data, cnt_t _time )  { return msg_waitUntil(this, _data, sizeof(T), _time);  }
	unsigned wait     (       T *_data )               { return msg_wait     (this, _data, sizeof(T));         }
	unsigned give     ( const T *_data )               { return msg_give     (this, _data, sizeof(T));         }
	unsigned giveISR  ( const T *_data )               { return msg_giveISR  (this, _data, sizeof(T));         }
	unsigned sendFor  ( const T *_data, cnt_t _delay ) { return msg_sendFor  (this, _data, sizeof(T), _delay); }
	unsigned sendUntil( const T *_data, cnt_t _time )  { return msg_sendUntil(this, _data, sizeof(T), _time);  }
	unsigned send     ( const T *_data )               { return msg_send     (this, _data, sizeof(T));         }
	unsigned push     ( const T *_data )               { return msg_push     (this, _data, sizeof(T));         }
	unsigned pushISR  ( const T *_data )               { return msg_pushISR  (this, _data, sizeof(T));         }
};

#endif//__cplusplus

/* -------------------------------------------------------------------------- */

#endif//__STATEOS_MSG_H
