/******************************************************************************

    @file    StateOS: osmemorypool.h
    @author  Rajmund Szymanski
    @date    19.11.2018
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

#ifndef __STATEOS_MEM_H
#define __STATEOS_MEM_H

#include "oskernel.h"
#include "oslist.h"

#ifdef __cplusplus
extern "C" {
#endif

/* -------------------------------------------------------------------------- */

#define MEM_SIZE( size ) \
    ALIGNED_SIZE( size, que_t )

/******************************************************************************
 *
 * Name              : memory pool
 *
 ******************************************************************************/

typedef struct __mem mem_t, * const mem_id;

struct __mem
{
	lst_t    lst;   // memory pool list

	unsigned limit; // size of a memory pool (max number of objects)
	unsigned size;  // size of memory object (in sizeof(que_t) units)
	que_t  * data;  // pointer to memory pool buffer

	intptr_t :0;
};

/******************************************************************************
 *
 * Name              : _MEM_INIT
 *
 * Description       : create and initialize a memory pool object
 *
 * Parameters
 *   limit           : size of a buffer (max number of objects)
 *   size            : size of memory object (in sizeof(que_t) units)
 *   data            : memory pool data buffer
 *
 * Return            : memory pool object
 *
 * Note              : for internal use
 *
 ******************************************************************************/

#define               _MEM_INIT( _limit, _size, _data ) { _LST_INIT(), _limit, _size, _data }

/******************************************************************************
 *
 * Name              : _MEM_DATA
 *
 * Description       : create a memory pool data buffer
 *
 * Parameters
 *   limit           : size of a buffer (max number of objects)
 *   size            : size of memory object (in sizeof(que_t) units)
 *
 * Return            : memory pool data buffer
 *
 * Note              : for internal use
 *
 ******************************************************************************/

#ifndef __cplusplus
#define               _MEM_DATA( _limit, _size ) (que_t[_limit * (1 + _size)]){ { 0 } }
#endif

/******************************************************************************
 *
 * Name              : OS_MEM
 *
 * Description       : define and initialize a memory pool object
 *
 * Parameters
 *   mem             : name of a pointer to memory pool object
 *   limit           : size of a buffer (max number of objects)
 *   size            : size of memory object (in bytes)
 *
 ******************************************************************************/

#define             OS_MEM( mem, limit, size )                                                    \
                       struct { mem_t mem; que_t buf[limit * (1 + MEM_SIZE(size))]; } mem##__wrk = \
                       { _MEM_INIT( limit, MEM_SIZE(size), mem##__wrk.buf ), { { 0 } } };           \
                       mem_id mem = & mem##__wrk.mem

/******************************************************************************
 *
 * Name              : static_MEM
 *
 * Description       : define and initialize a static memory pool object
 *
 * Parameters
 *   mem             : name of a pointer to memory pool object
 *   limit           : size of a buffer (max number of objects)
 *   size            : size of memory object (in bytes)
 *
 ******************************************************************************/

#define         static_MEM( mem, limit, size )                                                    \
                static struct { mem_t mem; que_t buf[limit * (1 + MEM_SIZE(size))]; } mem##__wrk = \
                       { _MEM_INIT( limit, MEM_SIZE(size), mem##__wrk.buf ), { { 0 } } };           \
                static mem_id mem = & mem##__wrk.mem

/******************************************************************************
 *
 * Name              : MEM_INIT
 *
 * Description       : create and initialize a memory pool object
 *
 * Parameters
 *   limit           : size of a buffer (max number of objects)
 *   size            : size of memory object (in bytes)
 *
 * Return            : memory pool object
 *
 * Note              : use only in 'C' code
 *
 ******************************************************************************/

#ifndef __cplusplus
#define                MEM_INIT( limit, size ) \
                      _MEM_INIT( limit, MEM_SIZE(size), _MEM_DATA(limit, MEM_SIZE(size)) )
#endif

/******************************************************************************
 *
 * Name              : MEM_CREATE
 * Alias             : MEM_NEW
 *
 * Description       : create and initialize a memory pool object
 *
 * Parameters
 *   limit           : size of a buffer (max number of objects)
 *   size            : size of memory object (in bytes)
 *
 * Return            : pointer to memory pool object
 *
 * Note              : use only in 'C' code
 *
 ******************************************************************************/

#ifndef __cplusplus
#define                MEM_CREATE( limit, size ) \
           (mem_t[]) { MEM_INIT  ( limit, size ) }
#define                MEM_NEW \
                       MEM_CREATE
#endif

/******************************************************************************
 *
 * Name              : mem_bind
 *
 * Description       : initialize data buffer of a memory pool object
 *
 * Parameters
 *   mem             : pointer to memory pool object
 *
 * Return            : none
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

void mem_bind( mem_t *mem );

/******************************************************************************
 *
 * Name              : mem_init
 *
 * Description       : initialize a memory pool object
 *
 * Parameters
 *   mem             : pointer to memory pool object
 *   size            : size of memory object (in bytes)
 *   data            : memory pool data buffer
 *   bufsize         : size of the data buffer (in bytes)
 *
 * Return            : none
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

void mem_init( mem_t *mem, unsigned size, que_t *data, unsigned bufsize );

/******************************************************************************
 *
 * Name              : mem_create
 * Alias             : mem_new
 *
 * Description       : create and initialize a new memory pool object
 *
 * Parameters
 *   limit           : size of a buffer (max number of objects)
 *   size            : size of memory object (in bytes)
 *
 * Return            : pointer to memory pool object (memory pool successfully created)
 *   0               : memory pool not created (not enough free memory)
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

mem_t *mem_create( unsigned limit, unsigned size );

__STATIC_INLINE
mem_t *mem_new( unsigned limit, unsigned size ) { return mem_create(limit, size); }

/******************************************************************************
 *
 * Name              : mem_reset
 * Alias             : mem_kill
 *
 * Description       : reset the memory pool object and wake up all waiting tasks with 'E_STOPPED' event value
 *
 * Parameters
 *   mem             : pointer to memory pool object
 *
 * Return            : none
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

void mem_reset( mem_t *mem );

__STATIC_INLINE
void mem_kill( mem_t *mem ) { mem_reset(mem); }

/******************************************************************************
 *
 * Name              : mem_destroy
 * Alias             : mem_delete
 *
 * Description       : reset the memory pool object, wake up all waiting tasks with 'E_DELETED' event value and free allocated resource
 *
 * Parameters
 *   mem             : pointer to memory pool object
 *
 * Return            : none
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

void mem_destroy( mem_t *mem );

__STATIC_INLINE
void mem_delete( mem_t *mem ) { mem_destroy(mem); }

/******************************************************************************
 *
 * Name              : mem_take
 * Alias             : mem_tryWait
 * ISR alias         : mem_takeISR
 *
 * Description       : try to get memory object from the memory pool object,
 *                     don't wait if the memory pool object is empty
 *
 * Parameters
 *   mem             : pointer to memory pool object
 *   data            : pointer to store the pointer to the memory object
 *
 * Return
 *   E_SUCCESS       : pointer to memory object was successfully transferred to the data pointer
 *   E_TIMEOUT       : memory pool object is empty
 *
 * Note              : may be used both in thread and handler mode
 *
 ******************************************************************************/

__STATIC_INLINE
unsigned mem_take( mem_t *mem, void **data ) { return lst_take(&mem->lst, data); }

__STATIC_INLINE
unsigned mem_tryWait( mem_t *mem, void **data ) { return lst_take(&mem->lst, data); }

__STATIC_INLINE
unsigned mem_takeISR( mem_t *mem, void **data ) { return lst_takeISR(&mem->lst, data); }

/******************************************************************************
 *
 * Name              : mem_waitFor
 *
 * Description       : try to get memory object from the memory pool object,
 *                     wait for given duration of time while the memory pool object is empty
 *
 * Parameters
 *   mem             : pointer to memory pool object
 *   data            : pointer to store the pointer to the memory object
 *   delay           : duration of time (maximum number of ticks to wait while the memory pool object is empty)
 *                     IMMEDIATE: don't wait if the memory pool object is empty
 *                     INFINITE:  wait indefinitely while the memory pool object is empty
 *
 * Return
 *   E_SUCCESS       : pointer to memory object was successfully transferred to the data pointer
 *   E_STOPPED       : memory pool object was reseted before the specified timeout expired
 *   E_DELETED       : memory pool object was deleted before the specified timeout expired
 *   E_TIMEOUT       : memory pool object is empty and was not received data before the specified timeout expired
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

__STATIC_INLINE
unsigned mem_waitFor( mem_t *mem, void **data, cnt_t delay ) { return lst_waitFor(&mem->lst, data, delay); }

/******************************************************************************
 *
 * Name              : mem_waitUntil
 *
 * Description       : try to get memory object from the memory pool object,
 *                     wait until given timepoint while the memory pool object is empty
 *
 * Parameters
 *   mem             : pointer to memory pool object
 *   data            : pointer to store the pointer to the memory object
 *   time            : timepoint value
 *
 * Return
 *   E_SUCCESS       : pointer to memory object was successfully transferred to the data pointer
 *   E_STOPPED       : memory pool object was reseted before the specified timeout expired
 *   E_DELETED       : memory pool object was deleted before the specified timeout expired
 *   E_TIMEOUT       : memory pool object is empty and was not received data before the specified timeout expired
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

__STATIC_INLINE
unsigned mem_waitUntil( mem_t *mem, void **data, cnt_t time ) { return lst_waitUntil(&mem->lst, data, time); }

/******************************************************************************
 *
 * Name              : mem_wait
 *
 * Description       : try to get memory object from the memory pool object,
 *                     wait indefinitely while the memory pool object is empty
 *
 * Parameters
 *   mem             : pointer to memory pool object
 *   data            : pointer to store the pointer to the memory object
 *
 * Return
 *   E_SUCCESS       : pointer to memory object was successfully transferred to the data pointer
 *   E_STOPPED       : memory pool object was reseted
 *   E_DELETED       : memory pool object was deleted
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

__STATIC_INLINE
unsigned mem_wait( mem_t *mem, void **data ) { return lst_wait(&mem->lst, data); }

/******************************************************************************
 *
 * Name              : mem_give
 * ISR alias         : mem_giveISR
 *
 * Description       : transfer memory object to the memory pool object,
 *
 * Parameters
 *   mem             : pointer to memory pool object
 *   data            : pointer to memory object
 *
 * Return            : none
 *
 * Note              : may be used both in thread and handler mode
 *
 ******************************************************************************/

__STATIC_INLINE
void mem_give( mem_t *mem, const void *data ) { lst_give(&mem->lst, data); }

__STATIC_INLINE
void mem_giveISR( mem_t *mem, const void *data ) { lst_giveISR(&mem->lst, data); }

#ifdef __cplusplus
}
#endif

/* -------------------------------------------------------------------------- */

#ifdef __cplusplus

/******************************************************************************
 *
 * Class             : MemoryPoolT<>
 *
 * Description       : create and initialize a memory pool object
 *
 * Constructor parameters
 *   limit           : size of a buffer (max number of objects)
 *   size            : size of memory object (in bytes)
 *
 ******************************************************************************/

template<unsigned limit_, unsigned size_>
struct MemoryPoolT : public __mem
{
	 MemoryPoolT( void ): __mem _MEM_INIT(limit_, MEM_SIZE(size_), data_) { mem_bind(this); }
	~MemoryPoolT( void ) { assert(__mem::lst.obj.queue == nullptr); }

	static
	MemoryPoolT<limit_, size_> *create( void )
	{
		static_assert(sizeof(__mem) + limit_ * (1 + MEM_SIZE(size_)) * sizeof(que_t) == sizeof(MemoryPoolT<limit_, size_>), "unexpected error!");
		return reinterpret_cast<MemoryPoolT<limit_, size_> *>(mem_create(limit_, size_));
	}

	void     reset    ( void )                             {        mem_reset    (this);                }
	void     kill     ( void )                             {        mem_kill     (this);                }
	void     destroy  ( void )                             {        mem_destroy  (this);                }
	unsigned take     (       void **_data )               { return mem_take     (this, _data);         }
	unsigned tryWait  (       void **_data )               { return mem_tryWait  (this, _data);         }
	unsigned takeISR  (       void **_data )               { return mem_takeISR  (this, _data);         }
	unsigned waitFor  (       void **_data, cnt_t _delay ) { return mem_waitFor  (this, _data, _delay); }
	unsigned waitUntil(       void **_data, cnt_t _time )  { return mem_waitUntil(this, _data, _time);  }
	unsigned wait     (       void **_data )               { return mem_wait     (this, _data);         }
	void     give     ( const void  *_data )               {        mem_give     (this, _data);         }
	void     giveISR  ( const void  *_data )               {        mem_giveISR  (this, _data);         }

	private:
	que_t data_[limit_ * (1 + MEM_SIZE(size_))];
};

/******************************************************************************
 *
 * Class             : MemoryPoolTT<>
 *
 * Description       : create and initialize a memory pool object
 *
 * Constructor parameters
 *   limit           : size of a buffer (max number of objects)
 *   T               : class of a memory object
 *
 ******************************************************************************/

template<unsigned limit_, class T>
struct MemoryPoolTT : public MemoryPoolT<limit_, sizeof(T)>
{
	MemoryPoolTT( void ): MemoryPoolT<limit_, sizeof(T)>() {}

	static
	MemoryPoolTT<limit_, T> *create( void )
	{
		static_assert(sizeof(__mem) + limit_ * (1 + MEM_SIZE(sizeof(T))) * sizeof(que_t) == sizeof(MemoryPoolTT<limit_, T>), "unexpected error!");
		return reinterpret_cast<MemoryPoolTT<limit_, T> *>(mem_create(limit_, sizeof(T)));
	}

	unsigned take     ( T **_data )               { return mem_take     (this, reinterpret_cast<void **>(_data));         }
	unsigned tryWait  ( T **_data )               { return mem_tryWait  (this, reinterpret_cast<void **>(_data));         }
	unsigned takeISR  ( T **_data )               { return mem_takeISR  (this, reinterpret_cast<void **>(_data));         }
	unsigned waitFor  ( T **_data, cnt_t _delay ) { return mem_waitFor  (this, reinterpret_cast<void **>(_data), _delay); }
	unsigned waitUntil( T **_data, cnt_t _time )  { return mem_waitUntil(this, reinterpret_cast<void **>(_data), _time);  }
	unsigned wait     ( T **_data )               { return mem_wait     (this, reinterpret_cast<void **>(_data));         }
};

#endif//__cplusplus

/* -------------------------------------------------------------------------- */

#endif//__STATEOS_MEM_H
