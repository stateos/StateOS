/******************************************************************************

    @file    StateOS: osstreambuffer.h
    @author  Rajmund Szymanski
    @date    15.08.2018
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

#ifndef __STATEOS_STM_H
#define __STATEOS_STM_H

#include "oskernel.h"

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************
 *
 * Name              : stream buffer
 *
 ******************************************************************************/

typedef struct __stm stm_t, * const stm_id;

struct __stm
{
	tsk_t  * queue; // inherited from semaphore
	void   * res;   // allocated stream buffer object's resource
	unsigned count; // inherited from semaphore
	unsigned limit; // inherited from semaphore

	unsigned head;  // first element to read from data buffer
	unsigned tail;  // first element to write into data buffer
	char   * data;  // data buffer
};

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

#define               _STM_INIT( _limit, _data ) { 0, 0, 0, _limit, 0, 0, _data }

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

#define               _VA_STM( _limit, _size ) \
                       ( (_size + 0) ? ((_limit) * (_size + 0)) : (_limit) )

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
 *   limit           : size of a buffer (max number of stored bytes)
 *   data            : stream buffer data
 *
 * Return            : none
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

void stm_init( stm_t *stm, unsigned limit, void *data );

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
 * Return            : pointer to stream buffer object (stream buffer successfully created)
 *   0               : stream buffer not created (not enough free memory)
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

stm_t *stm_create( unsigned limit );

__STATIC_INLINE
stm_t *stm_new( unsigned limit ) { return stm_create(limit); }

/******************************************************************************
 *
 * Name              : stm_kill
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

void stm_kill( stm_t *stm );

/******************************************************************************
 *
 * Name              : stm_delete
 *
 * Description       : reset the stream buffer object and free allocated resource
 *
 * Parameters
 *   stm             : pointer to stream buffer object
 *
 * Return            : none
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

void stm_delete( stm_t *stm );

/******************************************************************************
 *
 * Name              : stm_waitFor
 *
 * Description       : try to transfer data from the stream buffer object,
 *                     wait for given duration of time while the stream buffer object is empty
 *
 * Parameters
 *   stm             : pointer to stream buffer object
 *   data            : pointer to write buffer
 *   size            : size of write buffer
 *   delay           : duration of time (maximum number of ticks to wait while the stream buffer object is empty)
 *                     IMMEDIATE: don't wait if the stream buffer object is empty
 *                     INFINITE:  wait indefinitely while the stream buffer object is empty
 *
 * Return
 *   E_SUCCESS       : data was successfully transfered from the stream buffer object
 *   E_STOPPED       : stream buffer queue object was killed before the specified timeout expired
 *   E_TIMEOUT       : stream buffer object was not received enough data before the specified timeout expired
 *                     or write buffer has an incorrect size
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

unsigned stm_waitFor( stm_t *stm, void *data, unsigned size, cnt_t delay );

/******************************************************************************
 *
 * Name              : stm_waitUntil
 *
 * Description       : try to transfer data from the stream buffer object,
 *                     wait until given timepoint while the stream buffer object is empty
 *
 * Parameters
 *   stm             : pointer to stream buffer object
 *   data            : pointer to write buffer
 *   size            : size of write buffer
 *   time            : timepoint value
 *
 * Return
 *   E_SUCCESS       : data was successfully transfered from the stream buffer object
 *   E_STOPPED       : stream buffer queue object was killed before the specified timeout expired
 *   E_TIMEOUT       : stream buffer object was not received enough data before the specified timeout expired
 *                     or write buffer has an incorrect size
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

unsigned stm_waitUntil( stm_t *stm, void *data, unsigned size, cnt_t time );

/******************************************************************************
 *
 * Name              : stm_wait
 *
 * Description       : try to transfer data from the stream buffer object,
 *                     wait indefinitely while the stream buffer object is empty
 *
 * Parameters
 *   stm             : pointer to stream buffer object
 *   data            : pointer to write buffer
 *   size            : size of write buffer
 *
 * Return
 *   E_SUCCESS       : data was successfully transfered from the stream buffer object
 *   E_STOPPED       : stream buffer queue object was killed
 *   E_TIMEOUT       : write buffer has an incorrect size
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

__STATIC_INLINE
unsigned stm_wait( stm_t *stm, void *data, unsigned size ) { return stm_waitFor(stm, data, size, INFINITE); }

/******************************************************************************
 *
 * Name              : stm_take
 * ISR alias         : stm_takeISR
 *
 * Description       : try to transfer data from the stream buffer object,
 *                     don't wait if the stream buffer object is empty
 *
 * Parameters
 *   stm             : pointer to stream buffer object
 *   data            : pointer to write buffer
 *   size            : size of write buffer
 *
 * Return
 *   E_SUCCESS       : data was successfully transfered from the stream buffer object
 *   E_TIMEOUT       : stream buffer object does not have enough data
 *
 * Note              : may be used both in thread and handler mode
 *
 ******************************************************************************/

unsigned stm_take( stm_t *stm, void *data, unsigned size );

__STATIC_INLINE
unsigned stm_takeISR( stm_t *stm, void *data, unsigned size ) { return stm_take(stm, data, size); }

/******************************************************************************
 *
 * Name              : stm_sendFor
 *
 * Description       : try to transfer data to the stream buffer object,
 *                     wait for given duration of time while the stream buffer object is full
 *
 * Parameters
 *   stm             : pointer to stream buffer object
 *   data            : pointer to read buffer
 *   size            : size of read buffer
 *   delay           : duration of time (maximum number of ticks to wait while the stream buffer object is full)
 *                     IMMEDIATE: don't wait if the stream buffer object is full
 *                     INFINITE:  wait indefinitely while the stream buffer object is full
 *
 * Return
 *   E_SUCCESS       : data was successfully transfered to the stream buffer object
 *   E_STOPPED       : stream buffer queue object was killed before the specified timeout expired
 *   E_TIMEOUT       : stream buffer object did not free enough space before the specified timeout expired
 *                     or read buffer has an incorrect size
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

unsigned stm_sendFor( stm_t *stm, const void *data, unsigned size, cnt_t delay );

/******************************************************************************
 *
 * Name              : stm_sendUntil
 *
 * Description       : try to transfer data to the stream buffer object,
 *                     wait until given timepoint while the stream buffer object is full
 *
 * Parameters
 *   stm             : pointer to stream buffer object
 *   data            : pointer to read buffer
 *   size            : size of read buffer
 *   time            : timepoint value
 *
 * Return
 *   E_SUCCESS       : data was successfully transfered to the stream buffer object
 *   E_STOPPED       : stream buffer queue object was killed before the specified timeout expired
 *   E_TIMEOUT       : stream buffer object did not free enough space before the specified timeout expired
 *                     or read buffer has an incorrect size
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

unsigned stm_sendUntil( stm_t *stm, const void *data, unsigned size, cnt_t time );

/******************************************************************************
 *
 * Name              : stm_send
 *
 * Description       : try to transfer data to the stream buffer object,
 *                     wait indefinitely while the stream buffer object is full
 *
 * Parameters
 *   stm             : pointer to stream buffer object
 *   data            : pointer to read buffer
 *   size            : size of read buffer
 *
 * Return
 *   E_SUCCESS       : data was successfully transfered to the stream buffer object
 *   E_STOPPED       : stream buffer queue object was killed
 *   E_TIMEOUT       : read buffer has an incorrect size
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

__STATIC_INLINE
unsigned stm_send( stm_t *stm, const void *data, unsigned size ) { return stm_sendFor(stm, data, size, INFINITE); }

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
 *   data            : pointer to read buffer
 *   size            : size of read buffer
 *
 * Return
 *   E_SUCCESS       : data was successfully transfered to the stream buffer object
 *   E_TIMEOUT       : stream buffer object does not have enough space
 *
 * Note              : may be used both in thread and handler mode
 *
 ******************************************************************************/

unsigned stm_give( stm_t *stm, const void *data, unsigned size );

__STATIC_INLINE
unsigned stm_giveISR( stm_t *stm, const void *data, unsigned size ) { return stm_give(stm, data, size); }

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
 *   data            : pointer to read buffer
 *   size            : size of read buffer
 *
 * Return
 *   E_SUCCESS       : data was successfully transfered to the stream buffer object
 *   E_TIMEOUT       : read buffer has an incorrect size or
 *                     there are tasks waiting for writing to the stream buffer object
 *
 * Note              : may be used both in thread and handler mode
 *
 ******************************************************************************/

unsigned stm_push( stm_t *stm, const void *data, unsigned size );

__STATIC_INLINE
unsigned stm_pushISR( stm_t *stm, const void *data, unsigned size ) { return stm_push(stm, data, size); }

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
 ******************************************************************************/

unsigned stm_count( stm_t *stm );

__STATIC_INLINE
unsigned stm_countISR( stm_t *stm ) { return stm_count(stm); }

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
 ******************************************************************************/

unsigned stm_space( stm_t *stm );

__STATIC_INLINE
unsigned stm_spaceISR( stm_t *stm ) { return stm_space(stm); }

#ifdef __cplusplus
}
#endif

/* -------------------------------------------------------------------------- */

#ifdef __cplusplus

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

template<unsigned limit_>
struct StreamBufferT : public __stm
{
	 StreamBufferT( void ): __stm _STM_INIT(limit_, data_) {}
	~StreamBufferT( void ) { assert(__stm::queue == nullptr); }

	void     kill     ( void )                                            {        stm_kill     (this);                       }
	unsigned waitFor  (       void *_data, unsigned _size, cnt_t _delay ) { return stm_waitFor  (this, _data, _size, _delay); }
	unsigned waitUntil(       void *_data, unsigned _size, cnt_t _time )  { return stm_waitUntil(this, _data, _size, _time);  }
	unsigned wait     (       void *_data, unsigned _size )               { return stm_wait     (this, _data, _size);         }
	unsigned take     (       void *_data, unsigned _size )               { return stm_take     (this, _data, _size);         }
	unsigned takeISR  (       void *_data, unsigned _size )               { return stm_takeISR  (this, _data, _size);         }
	unsigned sendFor  ( const void *_data, unsigned _size, cnt_t _delay ) { return stm_sendFor  (this, _data, _size, _delay); }
	unsigned sendUntil( const void *_data, unsigned _size, cnt_t _time )  { return stm_sendUntil(this, _data, _size, _time);  }
	unsigned send     ( const void *_data, unsigned _size )               { return stm_send     (this, _data, _size);         }
	unsigned give     ( const void *_data, unsigned _size )               { return stm_give     (this, _data, _size);         }
	unsigned giveISR  ( const void *_data, unsigned _size )               { return stm_giveISR  (this, _data, _size);         }
	unsigned push     ( const void *_data, unsigned _size )               { return stm_push     (this, _data, _size);         }
	unsigned pushISR  ( const void *_data, unsigned _size )               { return stm_pushISR  (this, _data, _size);         }
	unsigned count    ( void )                                            { return stm_count    (this);                       }
	unsigned countISR ( void )                                            { return stm_countISR (this);                       }
	unsigned space    ( void )                                            { return stm_space    (this);                       }
	unsigned spaceISR ( void )                                            { return stm_spaceISR (this);                       }

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
 *   T               : class of an object
 *
 ******************************************************************************/

template<unsigned limit_, class T>
struct StreamBufferTT : public StreamBufferT<limit_*sizeof(T)>
{
	StreamBufferTT( void ): StreamBufferT<limit_*sizeof(T)>() {}

	unsigned waitFor  (       T *_data, cnt_t _delay ) { return stm_waitFor  (this, _data, sizeof(T), _delay); }
	unsigned waitUntil(       T *_data, cnt_t _time )  { return stm_waitUntil(this, _data, sizeof(T), _time);  }
	unsigned wait     (       T *_data )               { return stm_wait     (this, _data, sizeof(T));         }
	unsigned take     (       T *_data )               { return stm_take     (this, _data, sizeof(T));         }
	unsigned takeISR  (       T *_data )               { return stm_takeISR  (this, _data, sizeof(T));         }
	unsigned sendFor  ( const T *_data, cnt_t _delay ) { return stm_sendFor  (this, _data, sizeof(T), _delay); }
	unsigned sendUntil( const T *_data, cnt_t _time )  { return stm_sendUntil(this, _data, sizeof(T), _time);  }
	unsigned send     ( const T *_data )               { return stm_send     (this, _data, sizeof(T));         }
	unsigned give     ( const T *_data )               { return stm_give     (this, _data, sizeof(T));         }
	unsigned giveISR  ( const T *_data )               { return stm_giveISR  (this, _data, sizeof(T));         }
	unsigned push     ( const T *_data )               { return stm_push     (this, _data, sizeof(T));         }
	unsigned pushISR  ( const T *_data )               { return stm_pushISR  (this, _data, sizeof(T));         }
};

#endif//__cplusplus

/* -------------------------------------------------------------------------- */

#endif//__STATEOS_STM_H
