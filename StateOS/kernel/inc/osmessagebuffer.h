/******************************************************************************

    @file    StateOS: osmessagebuffer.h
    @author  Rajmund Szymanski
    @date    23.12.2020
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

#ifndef __STATEOS_MSG_H
#define __STATEOS_MSG_H

#include "oskernel.h"
#include "osclock.h"

/******************************************************************************
 *
 * Name              : message buffer
 *
 ******************************************************************************/

typedef struct __msg msg_t, * const msg_id;

struct __msg
{
	obj_t    obj;   // object header

	size_t   count; // inherited from stream buffer
	size_t   limit; // inherited from stream buffer

	size_t   head;  // inherited from stream buffer
	size_t   tail;  // inherited from stream buffer
	char *   data;  // inherited from stream buffer
};

#ifdef __cplusplus
extern "C" {
#endif

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

#define               _VA_MSG( _limit, _size ) ( (_size + 0) ? ((_limit) * (sizeof(size_t) + (_size + 0))) : (_limit) )

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

void msg_init( msg_t *msg, void *data, size_t bufsize );

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
 * Return            : pointer to message buffer object
 *   NULL            : object not created (not enough free memory)
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

msg_t *msg_create( size_t limit );

__STATIC_INLINE
msg_t *msg_new( size_t limit ) { return msg_create(limit); }

/******************************************************************************
 *
 * Name              : msg_reset
 * Alias             : msg_kill
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

void msg_reset( msg_t *msg );

__STATIC_INLINE
void msg_kill( msg_t *msg ) { msg_reset(msg); }

/******************************************************************************
 *
 * Name              : msg_destroy
 * Alias             : msg_delete
 *
 * Description       : reset the message buffer object, wake up all waiting tasks with 'E_DELETED' event value and free allocated resource
 *
 * Parameters
 *   msg             : pointer to message buffer object
 *
 * Return            : none
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

void msg_destroy( msg_t *msg );

__STATIC_INLINE
void msg_delete( msg_t *msg ) { msg_destroy(msg); }

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
 *   data            : pointer to the buffer
 *   size            : size of the buffer
 *   read            : pointer to the variable getting number of read bytes
 *
 * Return
 *   E_SUCCESS       : variable 'read' contains the number of bytes read from the message buffer
 *   E_FAILURE       : not enough space in the buffer
 *   E_TIMEOUT       : message buffer object is empty, try again
 *
 * Note              : can be used in both thread and handler mode (for blockable interrupts)
 *                     use ISR alias in blockable interrupt handlers
 *
 ******************************************************************************/

int msg_take( msg_t *msg, void *data, size_t size, size_t *read );

__STATIC_INLINE
int msg_tryWait( msg_t *msg, void *data, size_t size, size_t *read ) { return msg_take(msg, data, size, read); }

__STATIC_INLINE
int msg_takeISR( msg_t *msg, void *data, size_t size, size_t *read ) { return msg_take(msg, data, size, read); }

/******************************************************************************
 *
 * Name              : msg_waitFor
 *
 * Description       : try to transfer data from the message buffer object,
 *                     wait for given duration of time while the message buffer object is empty
 *
 * Parameters
 *   msg             : pointer to message buffer object
 *   data            : pointer to the buffer
 *   size            : size of the buffer
 *   read            : pointer to the variable getting number of read bytes
 *   delay           : duration of time (maximum number of ticks to wait while the message buffer object is empty)
 *                     IMMEDIATE: don't wait if the message buffer object is empty
 *                     INFINITE:  wait indefinitely while the message buffer object is empty
 *
 * Return
 *   E_SUCCESS       : variable 'read' contains the number of bytes read from the message buffer
 *   E_FAILURE       : not enough space in the buffer
 *   E_STOPPED       : message buffer object was reseted before the specified timeout expired
 *   E_DELETED       : message buffer object was deleted before the specified timeout expired
 *   E_TIMEOUT       : message buffer object is empty and was not received data before the specified timeout expired
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

int msg_waitFor( msg_t *msg, void *data, size_t size, size_t *read, cnt_t delay );

/******************************************************************************
 *
 * Name              : msg_waitUntil
 *
 * Description       : try to transfer data from the message buffer object,
 *                     wait until given timepoint while the message buffer object is empty
 *
 * Parameters
 *   msg             : pointer to message buffer object
 *   data            : pointer to the buffer
 *   size            : size of the buffer
 *   read            : pointer to the variable getting number of read bytes
 *   time            : timepoint value
 *
 * Return
 *   E_SUCCESS       : variable 'read' contains the number of bytes read from the message buffer
 *   E_FAILURE       : not enough space in the buffer
 *   E_STOPPED       : message buffer object was reseted before the specified timeout expired
 *   E_DELETED       : message buffer object was deleted before the specified timeout expired
 *   E_TIMEOUT       : message buffer object is empty and was not received data before the specified timeout expired
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

int msg_waitUntil( msg_t *msg, void *data, size_t size, size_t *read, cnt_t time );

/******************************************************************************
 *
 * Name              : msg_wait
 *
 * Description       : try to transfer data from the message buffer object,
 *                     wait indefinitely while the message buffer object is empty
 *
 * Parameters
 *   msg             : pointer to message buffer object
 *   data            : pointer to the buffer
 *   size            : size of the buffer
 *   read            : pointer to the variable getting number of read bytes
 *
 * Return
 *   E_SUCCESS       : variable 'read' contains the number of bytes read from the message buffer
 *   E_FAILURE       : not enough space in the buffer
 *   E_STOPPED       : message buffer object was reseted
 *   E_DELETED       : message buffer object was deleted
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

__STATIC_INLINE
int msg_wait( msg_t *msg, void *data, size_t size, size_t *read ) { return msg_waitFor(msg, data, size, read, INFINITE); }

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
 *   data            : pointer to the buffer
 *   size            : size of the buffer
 *
 * Return
 *   E_SUCCESS       : message data was successfully transferred to the message buffer object
 *   E_FAILURE       : size of the message data is out of the limit
 *   E_TIMEOUT       : not enough space in the message buffer, try again
 *
 * Note              : can be used in both thread and handler mode (for blockable interrupts)
 *                     use ISR alias in blockable interrupt handlers
 *
 ******************************************************************************/

int msg_give( msg_t *msg, const void *data, size_t size );

__STATIC_INLINE
int msg_giveISR( msg_t *msg, const void *data, size_t size ) { return msg_give(msg, data, size); }

/******************************************************************************
 *
 * Name              : msg_sendFor
 *
 * Description       : try to transfer data to the message buffer object,
 *                     wait for given duration of time while the message buffer object is full
 *
 * Parameters
 *   msg             : pointer to message buffer object
 *   data            : pointer to the buffer
 *   size            : size of the buffer
 *   delay           : duration of time (maximum number of ticks to wait while the message buffer object is full)
 *                     IMMEDIATE: don't wait if the message buffer object is full
 *                     INFINITE:  wait indefinitely while the message buffer object is full
 *
 * Return
 *   E_SUCCESS       : message data was successfully transferred to the message buffer object
 *   E_FAILURE       : size of the message data is out of the limit
 *   E_STOPPED       : message buffer object was reseted before the specified timeout expired
 *   E_DELETED       : message buffer object was deleted before the specified timeout expired
 *   E_TIMEOUT       : message buffer object is full and was not issued data before the specified timeout expired
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

int msg_sendFor( msg_t *msg, const void *data, size_t size, cnt_t delay );

/******************************************************************************
 *
 * Name              : msg_sendUntil
 *
 * Description       : try to transfer data to the message buffer object,
 *                     wait until given timepoint while the message buffer object is full
 *
 * Parameters
 *   msg             : pointer to message buffer object
 *   data            : pointer to the buffer
 *   size            : size of the buffer
 *   time            : timepoint value
 *
 * Return
 *   E_SUCCESS       : message data was successfully transferred to the message buffer object
 *   E_FAILURE       : size of the message data is out of the limit
 *   E_STOPPED       : message buffer object was reseted before the specified timeout expired
 *   E_DELETED       : message buffer object was deleted before the specified timeout expired
 *   E_TIMEOUT       : message buffer object is full and was not issued data before the specified timeout expired
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

int msg_sendUntil( msg_t *msg, const void *data, size_t size, cnt_t time );

/******************************************************************************
 *
 * Name              : msg_send
 *
 * Description       : try to transfer data to the message buffer object,
 *                     wait indefinitely while the message buffer object is full
 *
 * Parameters
 *   msg             : pointer to message buffer object
 *   data            : pointer to the buffer
 *   size            : size of the buffer
 *
 * Return
 *   E_SUCCESS       : message data was successfully transferred to the message buffer object
 *   E_FAILURE       : size of the message data is out of the limit
 *   E_STOPPED       : message buffer object was reseted
 *   E_DELETED       : message buffer object was deleted
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

__STATIC_INLINE
int msg_send( msg_t *msg, const void *data, size_t size ) { return msg_sendFor(msg, data, size, INFINITE); }

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
 *   data            : pointer to the buffer
 *   size            : size of the buffer
 *
 * Return
 *   E_SUCCESS       : message data was successfully transferred to the message buffer object
 *   E_FAILURE       : size of the message data is out of the limit
 *
 * Note              : can be used in both thread and handler mode (for blockable interrupts)
 *                     use ISR alias in blockable interrupt handlers
 *
 ******************************************************************************/

int msg_push( msg_t *msg, const void *data, size_t size );

__STATIC_INLINE
int msg_pushISR( msg_t *msg, const void *data, size_t size ) { return msg_push(msg, data, size); }

/******************************************************************************
 *
 * Name              : msg_count
 * ISR alias         : msg_countISR
 *
 * Description       : return the amount of data contained in the message buffer
 *
 * Parameters
 *   msg             : pointer to message buffer object
 *
 * Return            : amount of data contained in the message buffer
 *
 * Note              : can be used in both thread and handler mode (for blockable interrupts)
 *                     use ISR alias in blockable interrupt handlers
 *
 ******************************************************************************/

size_t msg_count( msg_t *msg );

__STATIC_INLINE
size_t msg_countISR( msg_t *msg ) { return msg_count(msg); }

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
 * Note              : can be used in both thread and handler mode (for blockable interrupts)
 *                     use ISR alias in blockable interrupt handlers
 *
 ******************************************************************************/

size_t msg_space( msg_t *msg );

__STATIC_INLINE
size_t msg_spaceISR( msg_t *msg ) { return msg_space(msg); }

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
 * Note              : can be used in both thread and handler mode (for blockable interrupts)
 *                     use ISR alias in blockable interrupt handlers
 *
 ******************************************************************************/

size_t msg_limit( msg_t *msg );

__STATIC_INLINE
size_t msg_limitISR( msg_t *msg ) { return msg_limit(msg); }

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
 * Note              : can be used in both thread and handler mode (for blockable interrupts)
 *                     use ISR alias in blockable interrupt handlers
 *
 ******************************************************************************/

size_t msg_size( msg_t *msg );

__STATIC_INLINE
size_t msg_sizeISR( msg_t *msg ) { return msg_size(msg); }

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

template<size_t limit_>
struct MessageBufferT : public __msg
{
	constexpr
	MessageBufferT( void ): __msg _MSG_INIT(limit_, data_) {}

	MessageBufferT( MessageBufferT&& ) = default;
	MessageBufferT( const MessageBufferT& ) = delete;
	MessageBufferT& operator=( MessageBufferT&& ) = delete;
	MessageBufferT& operator=( const MessageBufferT& ) = delete;

	~MessageBufferT( void ) { assert(__msg::obj.queue == nullptr); }

#if __cplusplus >= 201402
	using Ptr = std::unique_ptr<MessageBufferT<limit_>>;
#else
	using Ptr = MessageBufferT<limit_> *;
#endif

/******************************************************************************
 *
 * Name              : MessageBufferT<>::Create
 *
 * Description       : create dynamic object with manageable resources
 *
 * Parameters
 *   limit           : size of a buffer (max number of stored bytes)
 *
 * Return            : std::unique_pointer / pointer to MessageBufferT<> object
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

	static
	Ptr Create( void )
	{
		auto msg = new MessageBufferT<limit_>();
		if (msg != nullptr)
			msg->__msg::obj.res = msg;
		return Ptr(msg);
	}

	void   reset    ( void )                                                            {        msg_reset    (this); }
	void   kill     ( void )                                                            {        msg_kill     (this); }
	void   destroy  ( void )                                                            {        msg_destroy  (this); }
	int    take     (       void *_data, size_t _size, size_t *_read = nullptr )        { return msg_take     (this, _data, _size, _read); }
	int    tryWait  (       void *_data, size_t _size, size_t *_read = nullptr )        { return msg_tryWait  (this, _data, _size, _read); }
	int    takeISR  (       void *_data, size_t _size, size_t *_read = nullptr )        { return msg_takeISR  (this, _data, _size, _read); }
	template<typename T>
	int    waitFor  (       void *_data, size_t _size, size_t *_read,  const T _delay ) { return msg_waitFor  (this, _data, _size, _read, Clock::count(_delay)); }
	template<typename T>
	int    waitUntil(       void *_data, size_t _size, size_t *_read,  const T _time )  { return msg_waitUntil(this, _data, _size, _read, Clock::until(_time)); }
	int    wait     (       void *_data, size_t _size, size_t *_read = nullptr )        { return msg_wait     (this, _data, _size, _read); }
	int    give     ( const void *_data, size_t _size )                                 { return msg_give     (this, _data, _size); }
	int    giveISR  ( const void *_data, size_t _size )                                 { return msg_giveISR  (this, _data, _size); }
	template<typename T>
	int    sendFor  ( const void *_data, size_t _size, const T _delay )                 { return msg_sendFor  (this, _data, _size, Clock::count(_delay)); }
	template<typename T>
	int    sendUntil( const void *_data, size_t _size, const T _time )                  { return msg_sendUntil(this, _data, _size, Clock::until(_time)); }
	int    send     ( const void *_data, size_t _size )                                 { return msg_send     (this, _data, _size); }
	int    push     ( const void *_data, size_t _size )                                 { return msg_push     (this, _data, _size); }
	int    pushISR  ( const void *_data, size_t _size )                                 { return msg_pushISR  (this, _data, _size); }
	size_t count    ( void )                                                            { return msg_count    (this); }
	size_t countISR ( void )                                                            { return msg_countISR (this); }
	size_t space    ( void )                                                            { return msg_space    (this); }
	size_t spaceISR ( void )                                                            { return msg_spaceISR (this); }
	size_t limit    ( void )                                                            { return msg_limit    (this); }
	size_t limitISR ( void )                                                            { return msg_limitISR (this); }
	size_t size     ( void )                                                            { return msg_size     (this); }
	size_t sizeISR  ( void )                                                            { return msg_sizeISR  (this); }

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
 *   C               : class of an object
 *
 ******************************************************************************/

template<unsigned limit_, class C>
struct MessageBufferTT : public MessageBufferT<limit_*(sizeof(size_t)+sizeof(C))>
{
	constexpr
	MessageBufferTT( void ): MessageBufferT<limit_*(sizeof(size_t)+sizeof(C))>() {}

#if __cplusplus >= 201402
	using Ptr = std::unique_ptr<MessageBufferTT<limit_, C>>;
#else
	using Ptr = MessageBufferTT<limit_, C> *;
#endif

/******************************************************************************
 *
 * Name              : MessageBufferTT<>::Create
 *
 * Description       : create dynamic object with manageable resources
 *
 * Parameters
 *   limit           : size of a buffer (max number of stored objects)
 *   C               : class of an object
 *
 * Return            : std::unique_pointer / pointer to MessageBufferTT<> object
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

	static
	Ptr Create( void )
	{
		auto msg = new MessageBufferTT<limit_, C>();
		if (msg != nullptr)
			msg->__msg::obj.res = msg;
		return Ptr(msg);
	}

	int take     (       C *_data )                 { return msg_take     (this, _data, sizeof(C), nullptr); }
	int tryWait  (       C *_data )                 { return msg_tryWait  (this, _data, sizeof(C), nullptr); }
	int takeISR  (       C *_data )                 { return msg_takeISR  (this, _data, sizeof(C), nullptr); }
	template<typename T>
	int waitFor  (       C *_data, const T _delay ) { return msg_waitFor  (this, _data, sizeof(C), nullptr, Clock::count(_delay)); }
	template<typename T>
	int waitUntil(       C *_data, const T _time )  { return msg_waitUntil(this, _data, sizeof(C), nullptr, Clock::until(_time)); }
	int wait     (       C *_data )                 { return msg_wait     (this, _data, sizeof(C), nullptr); }
	int give     ( const C *_data )                 { return msg_give     (this, _data, sizeof(C)); }
	int giveISR  ( const C *_data )                 { return msg_giveISR  (this, _data, sizeof(C)); }
	template<typename T>
	int sendFor  ( const C *_data, const T _delay ) { return msg_sendFor  (this, _data, sizeof(C), Clock::count(_delay)); }
	template<typename T>
	int sendUntil( const C *_data, const T _time )  { return msg_sendUntil(this, _data, sizeof(C), Clock::until(_time)); }
	int send     ( const C *_data )                 { return msg_send     (this, _data, sizeof(C)); }
	int push     ( const C *_data )                 { return msg_push     (this, _data, sizeof(C)); }
	int pushISR  ( const C *_data )                 { return msg_pushISR  (this, _data, sizeof(C)); }
};

#endif//__cplusplus

/* -------------------------------------------------------------------------- */

#endif//__STATEOS_MSG_H
