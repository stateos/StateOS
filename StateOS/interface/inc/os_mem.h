/******************************************************************************

    @file    StateOS: os_mem.h
    @author  Rajmund Szymanski
    @date    27.02.2017
    @brief   This file contains definitions for StateOS.

 ******************************************************************************

    StateOS - Copyright (C) 2013 Rajmund Szymanski.

    This file is part of StateOS distribution.

    StateOS is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published
    by the Free Software Foundation; either version 3 of the License,
    or (at your option) any later version.

    StateOS is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program. If not, see <http://www.gnu.org/licenses/>.

 ******************************************************************************/

#ifndef __STATEOS_MEM_H
#define __STATEOS_MEM_H

#include <oskernel.h>

#ifdef __cplusplus
extern "C" {
#endif

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : memory pool                                                                                    *
 *                                                                                                                    *
 **********************************************************************************************************************/

struct __mem
{
	tsk_t  * queue; // inherited from list
	que_t  * next;  // inherited from list
	unsigned limit; // size of a memory pool (max number of objects)
	unsigned size;  // size of memory object (in words)
	void   * data;  // pointer to memory pool buffer
};

typedef struct __mem mem_t, mem_id[1];

/* -------------------------------------------------------------------------- */

#define MSIZE( size ) \
 (((unsigned)( size )+(sizeof(void*)-1))/sizeof(void*))

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : _MEM_INIT                                                                                      *
 *                                                                                                                    *
 * Description       : create and initilize a memory pool object                                                      *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   size            : size of memory object (in bytes)                                                               *
 *   data            : memory pool data buffer                                                                        *
 *                                                                                                                    *
 * Return            : memory pool object                                                                             *
 *                                                                                                                    *
 * Note              : for internal use                                                                               *
 *                                                                                                                    *
 **********************************************************************************************************************/

#define               _MEM_INIT( _limit, _size, _data ) { 0, 0, _limit, MSIZE(_size), _data }

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : _MEM_DATA                                                                                      *
 *                                                                                                                    *
 * Description       : create a memory pool data buffer                                                               *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   limit           : size of a buffer (max number of objects)                                                       *
 *   size            : size of memory object (in bytes)                                                               *
 *                                                                                                                    *
 * Return            : memory pool data buffer                                                                        *
 *                                                                                                                    *
 * Note              : for internal use                                                                               *
 *                                                                                                                    *
 **********************************************************************************************************************/

#ifndef __cplusplus
#define               _MEM_DATA( _limit, _size ) (void *[_limit * (1 + MSIZE(_size))]){ 0 }
#endif

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : OS_MEM                                                                                         *
 *                                                                                                                    *
 * Description       : define and initilize a memory pool object                                                      *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   mem             : name of a pointer to memory pool object                                                        *
 *   limit           : size of a buffer (max number of objects)                                                       *
 *   size            : size of memory object (in bytes)                                                               *
 *                                                                                                                    *
 **********************************************************************************************************************/

#define             OS_MEM( mem, limit, size )                     \
                       void *mem##__buf[limit * (1 + MSIZE(size))]; \
                       mem_id mem = { _MEM_INIT( limit, size, mem##__buf ) }

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : static_MEM                                                                                     *
 *                                                                                                                    *
 * Description       : define and initilize a static memory pool object                                               *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   mem             : name of a pointer to memory pool object                                                        *
 *   limit           : size of a buffer (max number of objects)                                                       *
 *   size            : size of memory object (in bytes)                                                               *
 *                                                                                                                    *
 **********************************************************************************************************************/

#define         static_MEM( mem, limit, size )                     \
                static void *mem##__buf[limit * (1 + MSIZE(size))]; \
                static mem_id mem = { _MEM_INIT( limit, size, mem##__buf ) }

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : MEM_INIT                                                                                       *
 *                                                                                                                    *
 * Description       : create and initilize a memory pool object                                                      *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   limit           : size of a buffer (max number of objects)                                                       *
 *   size            : size of memory object (in bytes)                                                               *
 *                                                                                                                    *
 * Return            : memory pool object                                                                             *
 *                                                                                                                    *
 * Note              : use only in 'C' code                                                                           *
 *                                                                                                                    *
 **********************************************************************************************************************/

#ifndef __cplusplus
#define                MEM_INIT( limit, size ) \
                      _MEM_INIT( limit, size, _MEM_DATA( limit, size ) )
#endif

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : MEM_CREATE                                                                                     *
 *                                                                                                                    *
 * Description       : create and initilize a memory pool object                                                      *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   limit           : size of a buffer (max number of objects)                                                       *
 *   size            : size of memory object (in bytes)                                                               *
 *                                                                                                                    *
 * Return            : pointer to memory pool object                                                                  *
 *                                                                                                                    *
 * Note              : use only in 'C' code                                                                           *
 *                                                                                                                    *
 **********************************************************************************************************************/

#ifndef __cplusplus
#define                MEM_CREATE( limit, size ) \
                     { MEM_INIT( limit, size ) }
#endif

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : mem_bind                                                                                       *
 *                                                                                                                    *
 * Description       : initialize data buffer of a memory pool object                                                 *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   mem             : pointer to memory pool object                                                                  *
 *                                                                                                                    *
 * Return            : none                                                                                           *
 *                                                                                                                    *
 * Note              : use only in thread mode                                                                        *
 *                                                                                                                    *
 **********************************************************************************************************************/

void mem_bind( mem_t *mem );

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : mem_init                                                                                       *
 *                                                                                                                    *
 * Description       : initilize a memory pool object                                                                 *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   mem             : pointer to memory pool object                                                                  *
 *   limit           : size of a buffer (max number of objects)                                                       *
 *   size            : size of memory object (in bytes)                                                               *
 *   data            : memory pool data buffer                                                                        *
 *                                                                                                                    *
 * Return            : none                                                                                           *
 *                                                                                                                    *
 * Note              : use only in thread mode                                                                        *
 *                                                                                                                    *
 **********************************************************************************************************************/

void mem_init( mem_t *mem, unsigned limit, unsigned size, void *data );

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : mem_create                                                                                     *
 *                                                                                                                    *
 * Description       : create and initilize a new memory pool object                                                  *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   limit           : size of a buffer (max number of objects)                                                       *
 *   size            : size of memory object (in bytes)                                                               *
 *                                                                                                                    *
 * Return            : pointer to memory pool object (memory pool successfully created)                               *
 *   0               : memory pool not created (not enough free memory)                                               *
 *                                                                                                                    *
 * Note              : use only in thread mode                                                                        *
 *                                                                                                                    *
 **********************************************************************************************************************/

mem_t *mem_create( unsigned limit, unsigned size );

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : mem_kill                                                                                       *
 *                                                                                                                    *
 * Description       : wake up all waiting tasks with 'E_STOPPED' event value                                         *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   mem             : pointer to memory pool object                                                                  *
 *                                                                                                                    *
 * Return            : none                                                                                           *
 *                                                                                                                    *
 * Note              : use only in thread mode                                                                        *
 *                                                                                                                    *
 **********************************************************************************************************************/

void mem_kill( mem_t *mem );

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : mem_waitUntil                                                                                  *
 *                                                                                                                    *
 * Description       : try to get memory object from the memory pool object,                                          *
 *                     wait until given timepoint while the memory pool object is empty                               *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   mem             : pointer to memory pool object                                                                  *
 *   data            : pointer to store the pointer to the memory object                                              *
 *   time            : timepoint value                                                                                *
 *                                                                                                                    *
 * Return                                                                                                             *
 *   E_SUCCESS       : pointer to memory object was successfully transfered to the data pointer                       *
 *   E_STOPPED       : memory pool object was killed before the specified timeout expired                             *
 *   E_TIMEOUT       : memory pool object is empty and was not received data before the specified timeout expired     *
 *                                                                                                                    *
 * Note              : use only in thread mode                                                                        *
 *                                                                                                                    *
 **********************************************************************************************************************/

unsigned mem_waitUntil( mem_t *mem, void **data, unsigned time );

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : mem_waitFor                                                                                    *
 *                                                                                                                    *
 * Description       : try to get memory object from the memory pool object,                                          *
 *                     wait for given duration of time while the memory pool object is empty                          *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   mem             : pointer to memory pool object                                                                  *
 *   data            : pointer to store the pointer to the memory object                                              *
 *   delay           : duration of time (maximum number of ticks to wait while the memory pool object is empty)       *
 *                     IMMEDIATE: don't wait if the memory pool object is empty                                       *
 *                     INFINITE:  wait indefinitly while the memory pool object is empty                              *
 *                                                                                                                    *
 * Return                                                                                                             *
 *   E_SUCCESS       : pointer to memory object was successfully transfered to the data pointer                       *
 *   E_STOPPED       : memory pool object was killed before the specified timeout expired                             *
 *   E_TIMEOUT       : memory pool object is empty and was not received data before the specified timeout expired     *
 *                                                                                                                    *
 * Note              : use only in thread mode                                                                        *
 *                                                                                                                    *
 **********************************************************************************************************************/

unsigned mem_waitFor( mem_t *mem, void **data, unsigned delay );

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : mem_wait                                                                                       *
 *                                                                                                                    *
 * Description       : try to get memory object from the memory pool object,                                          *
 *                     wait indefinitly while the memory pool object is empty                                         *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   mem             : pointer to memory pool object                                                                  *
 *   data            : pointer to store the pointer to the memory object                                              *
 *                                                                                                                    *
 * Return                                                                                                             *
 *   E_SUCCESS       : pointer to memory object was successfully transfered to the data pointer                       *
 *   E_STOPPED       : memory pool object was killed                                                                  *
 *                                                                                                                    *
 * Note              : use only in thread mode                                                                        *
 *                                                                                                                    *
 **********************************************************************************************************************/

__STATIC_INLINE
unsigned mem_wait( mem_t *mem, void **data ) { return mem_waitFor(mem, data, INFINITE); }

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : mem_take                                                                                       *
 *                                                                                                                    *
 * Description       : try to get memory object from the memory pool object,                                          *
 *                     don't wait if the memory pool object is empty                                                  *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   mem             : pointer to memory pool object                                                                  *
 *   data            : pointer to store the pointer to the memory object                                              *
 *                                                                                                                    *
 * Return                                                                                                             *
 *   E_SUCCESS       : pointer to memory object was successfully transfered to the data pointer                       *
 *   E_TIMEOUT       : memory pool object is empty                                                                    *
 *                                                                                                                    *
 * Note              : use only in thread mode                                                                        *
 *                                                                                                                    *
 **********************************************************************************************************************/

__STATIC_INLINE
unsigned mem_take( mem_t *mem, void **data ) { return mem_waitFor(mem, data, IMMEDIATE); }

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : mem_takeISR                                                                                    *
 *                                                                                                                    *
 * Description       : try to get memory object from the memory pool object,                                          *
 *                     don't wait if the memory pool object is empty                                                  *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   mem             : pointer to memory pool object                                                                  *
 *   data            : pointer to store the pointer to the memory object                                              *
 *                                                                                                                    *
 * Return                                                                                                             *
 *   E_SUCCESS       : pointer to memory object was successfully transfered to the data pointer                       *
 *   E_TIMEOUT       : memory pool object is empty                                                                    *
 *                                                                                                                    *
 * Note              : use only in handler mode                                                                       *
 *                                                                                                                    *
 **********************************************************************************************************************/

__STATIC_INLINE
unsigned mem_takeISR( mem_t *mem, void **data ) { return mem_waitFor(mem, data, IMMEDIATE); }

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : mem_give                                                                                       *
 *                                                                                                                    *
 * Description       : transfer memory object to the memory pool object,                                              *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   mem             : pointer to memory pool object                                                                  *
 *   data            : pointer to memory object                                                                       *
 *                                                                                                                    *
 * Return            : none                                                                                           *
 *                                                                                                                    *
 * Note              : use only in thread mode                                                                        *
 *                                                                                                                    *
 **********************************************************************************************************************/

void mem_give( mem_t *mem, void *data );

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : mem_giveISR                                                                                    *
 *                                                                                                                    *
 * Description       : transfer memory object to the memory pool object,                                              *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   mem             : pointer to memory pool object                                                                  *
 *   data            : pointer to memory object                                                                       *
 *                                                                                                                    *
 * Return            : none                                                                                           *
 *                                                                                                                    *
 * Note              : use only in handler mode                                                                       *
 *                                                                                                                    *
 **********************************************************************************************************************/

__STATIC_INLINE
void mem_giveISR( mem_t *mem, void *data ) { mem_give(mem, data); }

#ifdef __cplusplus
}
#endif

/* -------------------------------------------------------------------------- */

#ifdef __cplusplus

/**********************************************************************************************************************
 *                                                                                                                    *
 * Class             : MemoryPool                                                                                     *
 *                                                                                                                    *
 * Description       : create and initilize a memory pool object                                                      *
 *                                                                                                                    *
 * Constructor parameters                                                                                             *
 *   limit           : size of a buffer (max number of objects)                                                       *
 *   size            : size of memory object (in bytes)                                                               *
 *                                                                                                                    *
 **********************************************************************************************************************/

template<unsigned _limit, unsigned _size>
struct MemoryPoolT : public __mem
{
	explicit
	 MemoryPoolT( void ): __mem _MEM_INIT(_limit, _size, _data) { mem_bind(this); }
	~MemoryPoolT( void ) { assert(queue == nullptr); }

	void     kill     ( void )                          {        mem_kill     (this);                }
	unsigned waitUntil( void **_data, unsigned _time )  { return mem_waitUntil(this, _data, _time);  }
	unsigned waitFor  ( void **_data, unsigned _delay ) { return mem_waitFor  (this, _data, _delay); }
	unsigned wait     ( void **_data )                  { return mem_wait     (this, _data);         }
	unsigned take     ( void **_data )                  { return mem_take     (this, _data);         }
	unsigned takeISR  ( void **_data )                  { return mem_takeISR  (this, _data);         }
	void     give     ( void  *_data )                  {        mem_give     (this, _data);         }
	void     giveISR  ( void  *_data )                  {        mem_giveISR  (this, _data);         }

	private:
	void *_data[_limit * (1 + MSIZE(_size))];
};

/**********************************************************************************************************************
 *                                                                                                                    *
 * Class             : MemoryPool                                                                                     *
 *                                                                                                                    *
 * Description       : create and initilize a memory pool object                                                      *
 *                                                                                                                    *
 * Constructor parameters                                                                                             *
 *   limit           : size of a buffer (max number of objects)                                                       *
 *   T               : class of a memory object                                                                       *
 *                                                                                                                    *
 **********************************************************************************************************************/

template<unsigned _limit, class T>
struct MemoryPoolTT : public MemoryPoolT<_limit, sizeof(T)>
{
	unsigned waitUntil( T **_data, unsigned _time )  { return mem_waitUntil(this, (void**)_data, _time);  }
	unsigned waitFor  ( T **_data, unsigned _delay ) { return mem_waitFor  (this, (void**)_data, _delay); }
	unsigned wait     ( T **_data )                  { return mem_wait     (this, (void**)_data);         }
	unsigned take     ( T **_data )                  { return mem_take     (this, (void**)_data);         }
	unsigned takeISR  ( T **_data )                  { return mem_takeISR  (this, (void**)_data);         }
	void     give     ( T  *_data )                  {        mem_give     (this,         _data);         }
	void     giveISR  ( T  *_data )                  {        mem_giveISR  (this,         _data);         }
};

#endif

/* -------------------------------------------------------------------------- */

#endif//__STATEOS_MEM_H
