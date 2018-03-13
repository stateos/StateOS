/******************************************************************************

    @file    StateOS: os_lst.h
    @author  Rajmund Szymanski
    @date    13.03.2018
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

#ifndef __STATEOS_LST_H
#define __STATEOS_LST_H

#include "oskernel.h"

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************
 *
 * Name              : list
 *
 ******************************************************************************/

typedef struct __lst lst_t, * const lst_id;

struct __lst
{
	tsk_t  * queue; // next process in the DELAYED queue
	void   * res;   // allocated list object's resource
	que_t  * next;  // next memory object in the queue, previously created in the memory pool
};

/******************************************************************************
 *
 * Name              : _LST_INIT
 *
 * Description       : create and initialize a list object
 *
 * Parameters        : none
 *
 * Return            : list object
 *
 * Note              : for internal use
 *
 ******************************************************************************/

#define               _LST_INIT() { 0, 0, 0 }

/******************************************************************************
 *
 * Name              : OS_LST
 *
 * Description       : define and initialize a list object
 *
 * Parameters
 *   lst             : name of a pointer to list object
 *
 ******************************************************************************/

#define             OS_LST( lst )                     \
                       lst_t lst##__lst = _LST_INIT(); \
                       lst_id lst = & lst##__lst

/******************************************************************************
 *
 * Name              : static_LST
 *
 * Description       : define and initialize a static list object
 *
 * Parameters
 *   lst             : name of a pointer to list object
 *
 ******************************************************************************/

#define         static_LST( lst )                     \
                static lst_t lst##__lst = _LST_INIT(); \
                static lst_id lst = & lst##__lst

/******************************************************************************
 *
 * Name              : LST_INIT
 *
 * Description       : create and initialize a list object
 *
 * Parameters        : none
 *
 * Return            : list object
 *
 * Note              : use only in 'C' code
 *
 ******************************************************************************/

#ifndef __cplusplus
#define                LST_INIT() \
                      _LST_INIT()
#endif

/******************************************************************************
 *
 * Name              : LST_CREATE
 * Alias             : LST_NEW
 *
 * Description       : create and initialize a list object
 *
 * Parameters        : none
 *
 * Return            : pointer to list object
 *
 * Note              : use only in 'C' code
 *
 ******************************************************************************/

#ifndef __cplusplus
#define                LST_CREATE() \
             & (lst_t) LST_INIT  ()
#define                LST_NEW \
                       LST_CREATE
#endif

/******************************************************************************
 *
 * Name              : lst_init
 *
 * Description       : initialize a list object
 *
 * Parameters
 *   lst             : pointer to list object
 *
 * Return            : none
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

void lst_init( lst_t *lst );

/******************************************************************************
 *
 * Name              : lst_create
 * Alias             : lst_new
 *
 * Description       : create and initialize a new list object
 *
 * Parameters        : none
 *
 * Return            : pointer to list object (list successfully created)
 *   0               : list not created (not enough free memory)
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

lst_t *lst_create( void );

__STATIC_INLINE
lst_t *lst_new( void ) { return lst_create(); }

/******************************************************************************
 *
 * Name              : lst_kill
 *
 * Description       : wake up all waiting tasks with 'E_STOPPED' event value
 *
 * Parameters
 *   lst             : pointer to list object
 *
 * Return            : none
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

void lst_kill( lst_t *lst );

/******************************************************************************
 *
 * Name              : lst_delete
 *
 * Description       : reset the list object and free allocated resource
 *
 * Parameters
 *   lst             : pointer to list object
 *
 * Return            : none
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

void lst_delete( lst_t *lst );

/******************************************************************************
 *
 * Name              : lst_waitUntil
 *
 * Description       : try to get memory object from the list object,
 *                     wait until given timepoint while the list object is empty
 *
 * Parameters
 *   lst             : pointer to list object
 *   data            : pointer to store the pointer to the memory object
 *   time            : timepoint value
 *
 * Return
 *   E_SUCCESS       : pointer to memory object was successfully transfered to the data pointer
 *   E_STOPPED       : list object was killed before the specified timeout expired
 *   E_TIMEOUT       : list object is empty and was not received data before the specified timeout expired
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

unsigned lst_waitUntil( lst_t *lst, void **data, cnt_t time );

/******************************************************************************
 *
 * Name              : lst_waitFor
 *
 * Description       : try to get memory object from the list object,
 *                     wait for given duration of time while the list object is empty
 *
 * Parameters
 *   lst             : pointer to list object
 *   data            : pointer to store the pointer to the memory object
 *   delay           : duration of time (maximum number of ticks to wait while the list object is empty)
 *                     IMMEDIATE: don't wait if the list object is empty
 *                     INFINITE:  wait indefinitely while the list object is empty
 *
 * Return
 *   E_SUCCESS       : pointer to memory object was successfully transfered to the data pointer
 *   E_STOPPED       : list object was killed before the specified timeout expired
 *   E_TIMEOUT       : list object is empty and was not received data before the specified timeout expired
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

unsigned lst_waitFor( lst_t *lst, void **data, cnt_t delay );

/******************************************************************************
 *
 * Name              : lst_wait
 *
 * Description       : try to get memory object from the list object,
 *                     wait indefinitely while the list object is empty
 *
 * Parameters
 *   lst             : pointer to list object
 *   data            : pointer to store the pointer to the memory object
 *
 * Return
 *   E_SUCCESS       : pointer to memory object was successfully transfered to the data pointer
 *   E_STOPPED       : list object was killed
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

__STATIC_INLINE
unsigned lst_wait( lst_t *lst, void **data ) { return lst_waitFor(lst, data, INFINITE); }

/******************************************************************************
 *
 * Name              : lst_take
 * ISR alias         : lst_takeISR
 *
 * Description       : try to get memory object from the list object,
 *                     don't wait if the list object is empty
 *
 * Parameters
 *   lst             : pointer to list object
 *   data            : pointer to store the pointer to the memory object
 *
 * Return
 *   E_SUCCESS       : pointer to memory object was successfully transfered to the data pointer
 *   E_TIMEOUT       : list object is empty
 *
 * Note              : may be used both in thread and handler mode
 *
 ******************************************************************************/

__STATIC_INLINE
unsigned lst_take( lst_t *lst, void **data ) { return lst_waitFor(lst, data, IMMEDIATE); }

__STATIC_INLINE
unsigned lst_takeISR( lst_t *lst, void **data ) { return lst_waitFor(lst, data, IMMEDIATE); }

/******************************************************************************
 *
 * Name              : lst_give
 * ISR alias         : lst_giveISR
 *
 * Description       : transfer memory object to the list object,
 *
 * Parameters
 *   lst             : pointer to list object
 *   data            : pointer to memory object
 *
 * Return            : none
 *
 * Note              : may be used both in thread and handler mode
 *
 ******************************************************************************/

void lst_give( lst_t *lst, void *data );

__STATIC_INLINE
void lst_giveISR( lst_t *lst, void *data ) { lst_give(lst, data); }

#ifdef __cplusplus
}
#endif

/* -------------------------------------------------------------------------- */

#ifdef __cplusplus

/******************************************************************************
 *
 * Class             : List
 *
 * Description       : create and initialize a list object
 *
 * Constructor parameters
 *                   : none
 *
 ******************************************************************************/

struct List : public __lst
{
	 explicit
	 List( void ): __lst _LST_INIT() {}
	~List( void ) { assert(queue == nullptr); }

	void     kill     ( void )                       {        lst_kill     (this);                }
	unsigned waitUntil( void **_data, cnt_t _time )  { return lst_waitUntil(this, _data, _time);  }
	unsigned waitFor  ( void **_data, cnt_t _delay ) { return lst_waitFor  (this, _data, _delay); }
	unsigned wait     ( void **_data )               { return lst_wait     (this, _data);         }
	unsigned take     ( void **_data )               { return lst_take     (this, _data);         }
	unsigned takeISR  ( void **_data )               { return lst_takeISR  (this, _data);         }
	void     give     ( void  *_data )               {        lst_give     (this, _data);         }
	void     giveISR  ( void  *_data )               {        lst_giveISR  (this, _data);         }
};

/******************************************************************************
 *
 * Class             : List
 *
 * Description       : create and initialize a list object
 *
 * Constructor parameters
 *   T               : class of a memory object
 *
 ******************************************************************************/

template<class T>
struct ListT : public List
{
	explicit
	ListT( void ): List() {}

	unsigned waitUntil( T **_data, cnt_t _time )  { return lst_waitUntil(this, reinterpret_cast<void **>(_data), _time);  }
	unsigned waitFor  ( T **_data, cnt_t _delay ) { return lst_waitFor  (this, reinterpret_cast<void **>(_data), _delay); }
	unsigned wait     ( T **_data )               { return lst_wait     (this, reinterpret_cast<void **>(_data));         }
	unsigned take     ( T **_data )               { return lst_take     (this, reinterpret_cast<void **>(_data));         }
	unsigned takeISR  ( T **_data )               { return lst_takeISR  (this, reinterpret_cast<void **>(_data));         }
};

#endif

/* -------------------------------------------------------------------------- */

#endif//__STATEOS_LST_H
