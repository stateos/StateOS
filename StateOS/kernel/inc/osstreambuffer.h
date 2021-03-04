/******************************************************************************

    @file    StateOS: osstreambuffer.h
    @author  Rajmund Szymanski
    @date    04.03.2021
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

#ifndef __STATEOS_STM_H
#define __STATEOS_STM_H

#include "oskernel.h"
#include "osclock.h"

/******************************************************************************
 *
 * Name              : stream buffer
 *
 ******************************************************************************/

typedef struct __stm stm_t, * const stm_id;

struct __stm
{
	obj_t    obj;   // object header

	size_t   count; // size of used memory in the stream buffer (in bytes)
	size_t   limit; // size of the stream buffer (in bytes)

	size_t   head;  // first element to read from data buffer
	size_t   tail;  // first element to write into data buffer
	char *   data;  // data buffer
};

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************
 *
 * Name              : _STM_INIT
 *
 * Description       : create and initialize a stream buffer object
 *
 * Parameters
 *   limit           : size of a buffer (max number of stored bytes)
 *   data            : stream buffer data
 *
 * Return            : stream buffer object
 *
 * Note              : for internal use
 *
 ******************************************************************************/

#define               _STM_INIT( _limit, _data ) { _OBJ_INIT(), 0, _limit, 0, 0, _data }

/******************************************************************************
 *
 * Name              : _STM_DATA
 *
 * Description       : create a stream buffer data
 *
 * Parameters
 *   limit           : size of a buffer (max number of stored bytes)
 *
 * Return            : stream buffer data
 *
 * Note              : for internal use
 *
 ******************************************************************************/

#ifndef __cplusplus
#define               _STM_DATA( _limit ) (char[_limit]){ 0 }
#endif

/******************************************************************************
 *
 * Name              : _VA_STM
 *
 * Description       : calculate buffer size from optional parameter
 *
 * Note              : for internal use
 *
 ******************************************************************************/

#define               _VA_STM( _limit, _size ) ( (_size + 0) ? ((_limit) * (_size + 0)) : (_limit) )

/******************************************************************************
 *
 * Name              : OS_STM
 *
 * Description       : define and initialize a stream buffer object
 *
 * Parameters
 *   stm             : name of a pointer to stream buffer object
 *   limit           : size of a buffer (max number of stored bytes / objects)
 *   type            : (optional) size of the object (in bytes); default: 1
 *
 ******************************************************************************/

#define             OS_STM( stm, limit, ... )                                                 \
                       char stm##__buf[_VA_STM(limit, __VA_ARGS__)];                           \
                       stm_t stm##__stm = _STM_INIT( _VA_STM(limit, __VA_ARGS__), stm##__buf ); \
                       stm_id stm = & stm##__stm

/******************************************************************************
 *
 * Name              : static_STM
 *
 * Description       : define and initialize a static stream buffer object
 *
 * Parameters
 *   stm             : name of a pointer to stream buffer object
 *   limit           : size of a buffer (max number of stored bytes / objects)
 *   type            : (optional) size of the object (in bytes); default: 1
 *
 ******************************************************************************/

#define         static_STM( stm, limit, ... )                                                 \
                static char stm##__buf[_VA_STM(limit, __VA_ARGS__)];                           \
                static stm_t stm##__stm = _STM_INIT( _VA_STM(limit, __VA_ARGS__), stm##__buf ); \
                static stm_id stm = & stm##__stm

/******************************************************************************
 *
 * Name              : STM_INIT
 *
 * Description       : create and initialize a stream buffer object
 *
 * Parameters
 *   limit           : size of a buffer (max number of stored bytes / objects)
 *   type            : (optional) size of the object (in bytes); default: 1
 *
 * Return            : stream buffer object
 *
 * Note              : use only in 'C' code
 *
 ******************************************************************************/

#ifndef __cplusplus
#define                STM_INIT( limit, ... ) \
                      _STM_INIT( _VA_STM(limit, __VA_ARGS__), _STM_DATA( _VA_STM(limit, __VA_ARGS__) ) )
#endif

/******************************************************************************
 *
 * Name              : STM_CREATE
 * Alias             : STM_NEW
 *
 * Description       : create and initialize a stream buffer object
 *
 * Parameters
 *   limit           : size of a buffer (max number of stored bytes / objects)
 *   type            : (optional) size of the object (in bytes); default: 1
 *
 * Return            : pointer to stream buffer object
 *
 * Note              : use only in 'C' code
 *
 ******************************************************************************/

#ifndef __cplusplus
#define                STM_CREATE( limit, ... ) \
           (stm_t[]) { STM_INIT  ( _VA_STM(limit, __VA_ARGS__) ) }
#define                STM_NEW \
                       STM_CREATE
#endif

/******************************************************************************
 *
 * Name              : stm_init
 *
 * Description       : initialize a stream buffer object
 *
 * Parameters
 *   stm             : pointer to stream buffer object
 *   data            : stream buffer data
 *   bufsize         : size of the data buffer (in bytes)
 *
 * Return            : none
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

void stm_init( stm_t *stm, void *data, size_t bufsize );

/******************************************************************************
 *
 * Name              : stm_create
 * Alias             : stm_new
 *
 * Description       : create and initialize a new stream buffer object
 *
 * Parameters
 *   limit           : size of a buffer (max number of stored bytes)
 *
 * Return            : pointer to stream buffer object
 *   NULL            : object not created (not enough free memory)
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

stm_t *stm_create( size_t limit );

__STATIC_INLINE
stm_t *stm_new( size_t limit ) { return stm_create(limit); }

/******************************************************************************
 *
 * Name              : stm_reset
 * Alias             : stm_kill
 *
 * Description       : reset the stream buffer object and wake up all waiting tasks with 'E_STOPPED' event value
 *
 * Parameters
 *   stm             : pointer to stream buffer object
 *
 * Return            : none
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

void stm_reset( stm_t *stm );

__STATIC_INLINE
void stm_kill( stm_t *stm ) { stm_reset(stm); }

/******************************************************************************
 *
 * Name              : stm_destroy
 * Alias             : stm_delete
 *
 * Description       : reset the stream buffer object, wake up all waiting tasks with 'E_DELETED' event value and free allocated resource
 *
 * Parameters
 *   stm             : pointer to stream buffer object
 *
 * Return            : none
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

void stm_destroy( stm_t *stm );

__STATIC_INLINE
void stm_delete( stm_t *stm ) { stm_destroy(stm); }

/******************************************************************************
 *
 * Name              : stm_take
 * Alias             : stm_tryWait
 * ISR alias         : stm_takeISR
 *
 * Description       : try to transfer data from the stream buffer object,
 *                     don't wait if the stream buffer object is empty
 *
 * Parameters
 *   stm             : pointer to stream buffer object
 *   data            : pointer to the buffer
 *   size            : size of the buffer
 *   read            : pointer to the variable getting number of read bytes
 *
 * Return
 *   E_SUCCESS       : variable 'read' contains the number of bytes read from the stream buffer
 *   E_TIMEOUT       : stream buffer object is empty, try again
 *
 * Note              : can be used in both thread and handler mode (for blockable interrupts)
 *                     use ISR alias in blockable interrupt handlers
 *
 ******************************************************************************/

int stm_take( stm_t *stm, void *data, size_t size, size_t *read );

__STATIC_INLINE
int stm_tryWait( stm_t *stm, void *data, size_t size, size_t *read ) { return stm_take(stm, data, size, read); }

__STATIC_INLINE
int stm_takeISR( stm_t *stm, void *data, size_t size, size_t *read ) { return stm_take(stm, data, size, read); }

/******************************************************************************
 *
 * Name              : stm_waitFor
 *
 * Description       : try to transfer data from the stream buffer object,
 *                     wait for given duration of time while the stream buffer object is empty
 *
 * Parameters
 *   stm             : pointer to stream buffer object
 *   data            : pointer to the buffer
 *   size            : size of the buffer
 *   read            : pointer to the variable getting number of read bytes
 *   delay           : duration of time (maximum number of ticks to wait while the stream buffer object is empty)
 *                     IMMEDIATE: don't wait if the stream buffer object is empty
 *                     INFINITE:  wait indefinitely while the stream buffer object is empty
 *
 * Return
 *   E_SUCCESS       : variable 'read' contains the number of bytes read from the stream buffer
 *   E_STOPPED       : stream buffer object was reseted before the specified timeout expired
 *   E_DELETED       : stream buffer object was deleted before the specified timeout expired
 *   E_TIMEOUT       : stream buffer object is empty and was not received data before the specified timeout expired
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

int stm_waitFor( stm_t *stm, void *data, size_t size, size_t *read, cnt_t delay );

/******************************************************************************
 *
 * Name              : stm_waitUntil
 *
 * Description       : try to transfer data from the stream buffer object,
 *                     wait until given timepoint while the stream buffer object is empty
 *
 * Parameters
 *   stm             : pointer to stream buffer object
 *   data            : pointer to the buffer
 *   size            : size of the buffer
 *   read            : pointer to the variable getting number of read bytes
 *   time            : timepoint value
 *
 * Return
 *   E_SUCCESS       : variable 'read' contains the number of bytes read from the stream buffer
 *   E_STOPPED       : stream buffer object was reseted before the specified timeout expired
 *   E_DELETED       : stream buffer object was deleted before the specified timeout expired
 *   E_TIMEOUT       : stream buffer object is empty and was not received data before the specified timeout expired
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

int stm_waitUntil( stm_t *stm, void *data, size_t size, size_t *read, cnt_t time );

/******************************************************************************
 *
 * Name              : stm_wait
 *
 * Description       : try to transfer data from the stream buffer object,
 *                     wait indefinitely while the stream buffer object is empty
 *
 * Parameters
 *   stm             : pointer to stream buffer object
 *   data            : pointer to the buffer
 *   size            : size of the buffer
 *   read            : pointer to the variable getting number of read bytes
 *
 * Return
 *   E_SUCCESS       : variable 'read' contains the number of bytes read from the stream buffer
 *   E_STOPPED       : stream buffer object was reseted
 *   E_DELETED       : stream buffer object was deleted
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

__STATIC_INLINE
int stm_wait( stm_t *stm, void *data, size_t size, size_t *read ) { return stm_waitFor(stm, data, size, read, INFINITE); }

/******************************************************************************
 *
 * Name              : stm_give
 * ISR alias         : stm_giveISR
 *
 * Description       : try to transfer data to the stream buffer object,
 *                     don't wait if the stream buffer object is full
 *
 * Parameters
 *   stm             : pointer to stream buffer object
 *   data            : pointer to the buffer
 *   size            : size of the buffer
 *
 * Return
 *   E_SUCCESS       : stream data was successfully transferred to the stream buffer object
 *   E_FAILURE       : size of the stream data is out of the limit
 *   E_TIMEOUT       : not enough space in the stream buffer, try again
 *
 * Note              : can be used in both thread and handler mode (for blockable interrupts)
 *                     use ISR alias in blockable interrupt handlers
 *
 ******************************************************************************/

int stm_give( stm_t *stm, const void *data, size_t size );

__STATIC_INLINE
int stm_giveISR( stm_t *stm, const void *data, size_t size ) { return stm_give(stm, data, size); }

/******************************************************************************
 *
 * Name              : stm_sendFor
 *
 * Description       : try to transfer data to the stream buffer object,
 *                     wait for given duration of time while the stream buffer object is full
 *
 * Parameters
 *   stm             : pointer to stream buffer object
 *   data            : pointer to the buffer
 *   size            : size of the buffer
 *   delay           : duration of time (maximum number of ticks to wait while the stream buffer object is full)
 *                     IMMEDIATE: don't wait if the stream buffer object is full
 *                     INFINITE:  wait indefinitely while the stream buffer object is full
 *
 * Return
 *   E_SUCCESS       : stream data was successfully transferred to the stream buffer object
 *   E_FAILURE       : size of the stream data is out of the limit
 *   E_STOPPED       : stream buffer object was reseted before the specified timeout expired
 *   E_DELETED       : stream buffer object was deleted before the specified timeout expired
 *   E_TIMEOUT       : stream buffer object is full and was not issued data before the specified timeout expired
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

int stm_sendFor( stm_t *stm, const void *data, size_t size, cnt_t delay );

/******************************************************************************
 *
 * Name              : stm_sendUntil
 *
 * Description       : try to transfer data to the stream buffer object,
 *                     wait until given timepoint while the stream buffer object is full
 *
 * Parameters
 *   stm             : pointer to stream buffer object
 *   data            : pointer to the buffer
 *   size            : size of the buffer
 *   time            : timepoint value
 *
 * Return
 *   E_SUCCESS       : stream data was successfully transferred to the stream buffer object
 *   E_FAILURE       : size of the stream data is out of the limit
 *   E_STOPPED       : stream buffer object was reseted before the specified timeout expired
 *   E_DELETED       : stream buffer object was deleted before the specified timeout expired
 *   E_TIMEOUT       : stream buffer object is full and was not issued data before the specified timeout expired
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

int stm_sendUntil( stm_t *stm, const void *data, size_t size, cnt_t time );

/******************************************************************************
 *
 * Name              : stm_send
 *
 * Description       : try to transfer data to the stream buffer object,
 *                     wait indefinitely while the stream buffer object is full
 *
 * Parameters
 *   stm             : pointer to stream buffer object
 *   data            : pointer to the buffer
 *   size            : size of the buffer
 *
 * Return
 *   E_SUCCESS       : stream data was successfully transferred to the stream buffer object
 *   E_FAILURE       : size of the stream data is out of the limit
 *   E_STOPPED       : stream buffer object was reseted
 *   E_DELETED       : stream buffer object was deleted
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

__STATIC_INLINE
int stm_send( stm_t *stm, const void *data, size_t size ) { return stm_sendFor(stm, data, size, INFINITE); }

/******************************************************************************
 *
 * Name              : stm_push
 * ISR alias         : stm_pushISR
 *
 * Description       : try to transfer data to the stream buffer object,
 *                     remove the oldest data if the stream buffer object is full
 *
 * Parameters
 *   stm             : pointer to stream buffer object
 *   data            : pointer to the buffer
 *   size            : size of the buffer
 *
 * Return
 *   E_SUCCESS       : stream data was successfully transferred to the stream buffer object
 *   E_FAILURE       : size of the stream data is out of the limit
 *
 * Note              : can be used in both thread and handler mode (for blockable interrupts)
 *                     use ISR alias in blockable interrupt handlers
 *
 ******************************************************************************/

int stm_push( stm_t *stm, const void *data, size_t size );

__STATIC_INLINE
int stm_pushISR( stm_t *stm, const void *data, size_t size ) { return stm_push(stm, data, size); }

/******************************************************************************
 *
 * Name              : stm_count
 * ISR alias         : stm_countISR
 *
 * Description       : return the amount of data contained in the stream buffer
 *
 * Parameters
 *   stm             : pointer to stream buffer object
 *
 * Return            : amount of data contained in the stream buffer
 *
 * Note              : can be used in both thread and handler mode (for blockable interrupts)
 *                     use ISR alias in blockable interrupt handlers
 *
 ******************************************************************************/

size_t stm_count( stm_t *stm );

__STATIC_INLINE
size_t stm_countISR( stm_t *stm ) { return stm_count(stm); }

/******************************************************************************
 *
 * Name              : stm_space
 * ISR alias         : stm_spaceISR
 *
 * Description       : return the amount of free space in the stream buffer
 *
 * Parameters
 *   stm             : pointer to stream buffer object
 *
 * Return            : amount of free space in the stream buffer
 *
 * Note              : can be used in both thread and handler mode (for blockable interrupts)
 *                     use ISR alias in blockable interrupt handlers
 *
 ******************************************************************************/

size_t stm_space( stm_t *stm );

__STATIC_INLINE
size_t stm_spaceISR( stm_t *stm ) { return stm_space(stm); }

/******************************************************************************
 *
 * Name              : stm_limit
 * ISR alias         : stm_limitISR
 *
 * Description       : return the size of the stream buffer
 *
 * Parameters
 *   stm             : pointer to stream buffer object
 *
 * Return            : size of the stream buffer
 *
 * Note              : can be used in both thread and handler mode (for blockable interrupts)
 *                     use ISR alias in blockable interrupt handlers
 *
 ******************************************************************************/

size_t stm_limit( stm_t *stm );

__STATIC_INLINE
size_t stm_limitISR( stm_t *stm ) { return stm_limit(stm); }

#ifdef __cplusplus
}
#endif

/* -------------------------------------------------------------------------- */

#ifdef __cplusplus
namespace stateos {

/******************************************************************************
 *
 * Class             : StreamBufferT<>
 *
 * Description       : create and initialize a stream buffer object
 *
 * Constructor parameters
 *   limit           : size of a buffer (max number of stored bytes)
 *
 ******************************************************************************/

template<size_t limit_>
struct StreamBufferT : public __stm
{
	constexpr
	StreamBufferT( void ): __stm _STM_INIT(limit_, data_) {}

	StreamBufferT( StreamBufferT&& ) = default;
	StreamBufferT( const StreamBufferT& ) = delete;
	StreamBufferT& operator=( StreamBufferT&& ) = delete;
	StreamBufferT& operator=( const StreamBufferT& ) = delete;

	~StreamBufferT( void ) { assert(__stm::obj.queue == nullptr); }

#if __cplusplus >= 201402
	using Ptr = std::unique_ptr<StreamBufferT<limit_>>;
#else
	using Ptr = StreamBufferT<limit_> *;
#endif

/******************************************************************************
 *
 * Name              : StreamBufferT<>::Create
 *
 * Description       : create dynamic object with manageable resources
 *
 * Parameters
 *   limit           : size of a buffer (max number of stored bytes)
 *
 * Return            : std::unique_pointer / pointer to StreamBufferT<> object
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

	static
	Ptr Create( void )
	{
		auto stm = new StreamBufferT<limit_>();
		if (stm != nullptr)
			stm->__stm::obj.res = stm;
		return Ptr(stm);
	}

	void   reset    ( void )                                                            {        stm_reset    (this); }
	void   kill     ( void )                                                            {        stm_kill     (this); }
	void   destroy  ( void )                                                            {        stm_destroy  (this); }
	int    take     (       void *_data, size_t _size, size_t *_read = nullptr )        { return stm_take     (this, _data, _size, _read); }
	int    tryWait  (       void *_data, size_t _size, size_t *_read = nullptr )        { return stm_tryWait  (this, _data, _size, _read); }
	int    takeISR  (       void *_data, size_t _size, size_t *_read = nullptr )        { return stm_takeISR  (this, _data, _size, _read); }
	template<typename T>
	int    waitFor  (       void *_data, size_t _size, size_t *_read,  const T _delay ) { return stm_waitFor  (this, _data, _size, _read, _delay); }
	template<typename T>
	int    waitUntil(       void *_data, size_t _size, size_t *_read,  const T _time )  { return stm_waitUntil(this, _data, _size, _read, _time); }
	int    wait     (       void *_data, size_t _size, size_t *_read = nullptr )        { return stm_wait     (this, _data, _size, _read); }
	int    give     ( const void *_data, size_t _size )                                 { return stm_give     (this, _data, _size); }
	int    giveISR  ( const void *_data, size_t _size )                                 { return stm_giveISR  (this, _data, _size); }
	template<typename T>
	int    sendFor  ( const void *_data, size_t _size, const T _delay )                 { return stm_sendFor  (this, _data, _size, _delay); }
	template<typename T>
	int    sendUntil( const void *_data, size_t _size, const T _time )                  { return stm_sendUntil(this, _data, _size, _time); }
	int    send     ( const void *_data, size_t _size )                                 { return stm_send     (this, _data, _size); }
	int    push     ( const void *_data, size_t _size )                                 { return stm_push     (this, _data, _size); }
	int    pushISR  ( const void *_data, size_t _size )                                 { return stm_pushISR  (this, _data, _size); }
	size_t count    ( void )                                                            { return stm_count    (this); }
	size_t countISR ( void )                                                            { return stm_countISR (this); }
	size_t space    ( void )                                                            { return stm_space    (this); }
	size_t spaceISR ( void )                                                            { return stm_spaceISR (this); }
	size_t limit    ( void )                                                            { return stm_limit    (this); }
	size_t limitISR ( void )                                                            { return stm_limitISR (this); }

	private:
	char data_[limit_];
};

/******************************************************************************
 *
 * Class             : StreamBufferTT<>
 *
 * Description       : create and initialize a stream buffer object
 *
 * Constructor parameters
 *   limit           : size of a buffer (max number of stored objects)
 *   C               : class of an object
 *
 ******************************************************************************/

template<unsigned limit_, class C>
struct StreamBufferTT : public StreamBufferT<limit_*sizeof(C)>
{
	constexpr
	StreamBufferTT( void ): StreamBufferT<limit_*sizeof(C)>() {}

#if __cplusplus >= 201402
	using Ptr = std::unique_ptr<StreamBufferTT<limit_, C>>;
#else
	using Ptr = StreamBufferTT<limit_, C> *;
#endif

/******************************************************************************
 *
 * Name              : StreamBufferTT<>::Create
 *
 * Description       : create dynamic object with manageable resources
 *
 * Parameters
 *   limit           : size of a buffer (max number of stored objects)
 *   C               : class of an object
 *
 * Return            : std::unique_pointer / pointer to StreamBufferTT<> object
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

	static
	Ptr Create( void )
	{
		auto stm = new StreamBufferTT<limit_, C>();
		if (stm != nullptr)
			stm->__stm::obj.res = stm;
		return Ptr(stm);
	}

	int take     (       C *_data )                 { return stm_take     (this, _data, sizeof(C), nullptr); }
	int tryWait  (       C *_data )                 { return stm_tryWait  (this, _data, sizeof(C), nullptr); }
	int takeISR  (       C *_data )                 { return stm_takeISR  (this, _data, sizeof(C), nullptr); }
	template<typename T>                               
	int waitFor  (       C *_data, const T _delay ) { return stm_waitFor  (this, _data, sizeof(C), nullptr, _delay); }
	template<typename T>                               
	int waitUntil(       C *_data, const T _time )  { return stm_waitUntil(this, _data, sizeof(C), nullptr, _time); }
	int wait     (       C *_data )                 { return stm_wait     (this, _data, sizeof(C), nullptr); }
	int give     ( const C *_data )                 { return stm_give     (this, _data, sizeof(C)); }
	int giveISR  ( const C *_data )                 { return stm_giveISR  (this, _data, sizeof(C)); }
	template<typename T>                               
	int sendFor  ( const C *_data, const T _delay ) { return stm_sendFor  (this, _data, sizeof(C), _delay); }
	template<typename T>                               
	int sendUntil( const C *_data, const T _time )  { return stm_sendUntil(this, _data, sizeof(C), _time); }
	int send     ( const C *_data )                 { return stm_send     (this, _data, sizeof(C)); }
	int push     ( const C *_data )                 { return stm_push     (this, _data, sizeof(C)); }
	int pushISR  ( const C *_data )                 { return stm_pushISR  (this, _data, sizeof(C)); }
};

}     //  namespace
#endif//__cplusplus

/* -------------------------------------------------------------------------- */

#endif//__STATEOS_STM_H
