/******************************************************************************

    @file    StateOS: os_lst.h
    @author  Rajmund Szymanski
    @date    29.03.2017
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

#ifndef __STATEOS_LST_H
#define __STATEOS_LST_H

#include <oskernel.h>

#ifdef __cplusplus
extern "C" {
#endif

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : list                                                                                           *
 *                                                                                                                    *
 **********************************************************************************************************************/

typedef struct __lst lst_t;

struct __lst
{
	tsk_t  * queue; // next process in the DELAYED queue
	que_t  * next;  // next memory object in the queue, previously created in the memory pool
};

typedef struct __lst lst_id[];

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : _LST_INIT                                                                                      *
 *                                                                                                                    *
 * Description       : create and initilize a list object                                                             *
 *                                                                                                                    *
 * Parameters        : none                                                                                           *
 *                                                                                                                    *
 * Return            : list object                                                                                    *
 *                                                                                                                    *
 * Note              : for internal use                                                                               *
 *                                                                                                                    *
 **********************************************************************************************************************/

#define               _LST_INIT() { 0, 0 }

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : OS_LST                                                                                         *
 *                                                                                                                    *
 * Description       : define and initilize a list object                                                             *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   lst             : name of a pointer to list object                                                               *
 *                                                                                                                    *
 **********************************************************************************************************************/

#define             OS_LST( lst ) \
                       lst_t lst[1] = { _LST_INIT() }

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : static_LST                                                                                     *
 *                                                                                                                    *
 * Description       : define and initilize a static list object                                                      *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   lst             : name of a pointer to list object                                                               *
 *                                                                                                                    *
 **********************************************************************************************************************/

#define         static_LST( lst ) \
                static lst_t[1] lst = { _LST_INIT() }

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : LST_INIT                                                                                       *
 *                                                                                                                    *
 * Description       : create and initilize a list object                                                             *
 *                                                                                                                    *
 * Parameters        : none                                                                                           *
 *                                                                                                                    *
 * Return            : list object                                                                                    *
 *                                                                                                                    *
 * Note              : use only in 'C' code                                                                           *
 *                                                                                                                    *
 **********************************************************************************************************************/

#ifndef __cplusplus
#define                LST_INIT() \
                      _LST_INIT()
#endif

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : LST_CREATE                                                                                     *
 *                                                                                                                    *
 * Description       : create and initilize a list object                                                             *
 *                                                                                                                    *
 * Parameters        : none                                                                                           *
 *                                                                                                                    *
 * Return            : pointer to list object                                                                         *
 *                                                                                                                    *
 * Note              : use only in 'C' code                                                                           *
 *                                                                                                                    *
 **********************************************************************************************************************/

#ifndef __cplusplus
#define                LST_CREATE() \
             & (lst_t) LST_INIT()
#endif

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : lst_init                                                                                       *
 *                                                                                                                    *
 * Description       : initilize a list object                                                                        *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   lst             : pointer to list object                                                                         *
 *                                                                                                                    *
 * Return            : none                                                                                           *
 *                                                                                                                    *
 * Note              : use only in thread mode                                                                        *
 *                                                                                                                    *
 **********************************************************************************************************************/

void lst_init( lst_t *lst );

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : lst_create                                                                                     *
 *                                                                                                                    *
 * Description       : create and initilize a new list object                                                         *
 *                                                                                                                    *
 * Parameters        : none                                                                                           *
 *                                                                                                                    *
 * Return            : pointer to list object (list successfully created)                                             *
 *   0               : list not created (not enough free memory)                                                      *
 *                                                                                                                    *
 * Note              : use only in thread mode                                                                        *
 *                                                                                                                    *
 **********************************************************************************************************************/

lst_t *lst_create( void );

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : lst_kill                                                                                       *
 *                                                                                                                    *
 * Description       : wake up all waiting tasks with 'E_STOPPED' event value                                         *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   lst             : pointer to list object                                                                         *
 *                                                                                                                    *
 * Return            : none                                                                                           *
 *                                                                                                                    *
 * Note              : use only in thread mode                                                                        *
 *                                                                                                                    *
 **********************************************************************************************************************/

void lst_kill( lst_t *lst );

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : lst_waitUntil                                                                                  *
 *                                                                                                                    *
 * Description       : try to get memory object from the list object,                                                 *
 *                     wait until given timepoint while the list object is empty                                      *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   lst             : pointer to list object                                                                         *
 *   data            : pointer to store the pointer to the memory object                                              *
 *   time            : timepoint value                                                                                *
 *                                                                                                                    *
 * Return                                                                                                             *
 *   E_SUCCESS       : pointer to memory object was successfully transfered to the data pointer                       *
 *   E_STOPPED       : list object was killed before the specified timeout expired                                    *
 *   E_TIMEOUT       : list object is empty and was not received data before the specified timeout expired            *
 *                                                                                                                    *
 * Note              : use only in thread mode                                                                        *
 *                                                                                                                    *
 **********************************************************************************************************************/

unsigned lst_waitUntil( lst_t *lst, void **data, unsigned time );

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : lst_waitFor                                                                                    *
 *                                                                                                                    *
 * Description       : try to get memory object from the list object,                                                 *
 *                     wait for given duration of time while the list object is empty                                 *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   lst             : pointer to list object                                                                         *
 *   data            : pointer to store the pointer to the memory object                                              *
 *   delay           : duration of time (maximum number of ticks to wait while the list object is empty)              *
 *                     IMMEDIATE: don't wait if the list object is empty                                              *
 *                     INFINITE:  wait indefinitly while the list object is empty                                     *
 *                                                                                                                    *
 * Return                                                                                                             *
 *   E_SUCCESS       : pointer to memory object was successfully transfered to the data pointer                       *
 *   E_STOPPED       : list object was killed before the specified timeout expired                                    *
 *   E_TIMEOUT       : list object is empty and was not received data before the specified timeout expired            *
 *                                                                                                                    *
 * Note              : use only in thread mode                                                                        *
 *                                                                                                                    *
 **********************************************************************************************************************/

unsigned lst_waitFor( lst_t *lst, void **data, unsigned delay );

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : lst_wait                                                                                       *
 *                                                                                                                    *
 * Description       : try to get memory object from the list object,                                                 *
 *                     wait indefinitly while the list object is empty                                                *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   lst             : pointer to list object                                                                         *
 *   data            : pointer to store the pointer to the memory object                                              *
 *                                                                                                                    *
 * Return                                                                                                             *
 *   E_SUCCESS       : pointer to memory object was successfully transfered to the data pointer                       *
 *   E_STOPPED       : list object was killed                                                                         *
 *                                                                                                                    *
 * Note              : use only in thread mode                                                                        *
 *                                                                                                                    *
 **********************************************************************************************************************/

__STATIC_INLINE
unsigned lst_wait( lst_t *lst, void **data ) { return lst_waitFor(lst, data, INFINITE); }

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : lst_take                                                                                       *
 *                                                                                                                    *
 * Description       : try to get memory object from the list object,                                                 *
 *                     don't wait if the list object is empty                                                         *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   lst             : pointer to list object                                                                         *
 *   data            : pointer to store the pointer to the memory object                                              *
 *                                                                                                                    *
 * Return                                                                                                             *
 *   E_SUCCESS       : pointer to memory object was successfully transfered to the data pointer                       *
 *   E_TIMEOUT       : list object is empty                                                                           *
 *                                                                                                                    *
 * Note              : use only in thread mode                                                                        *
 *                                                                                                                    *
 **********************************************************************************************************************/

__STATIC_INLINE
unsigned lst_take( lst_t *lst, void **data ) { return lst_waitFor(lst, data, IMMEDIATE); }

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : lst_takeISR                                                                                    *
 *                                                                                                                    *
 * Description       : try to get memory object from the list object,                                                 *
 *                     don't wait if the list object is empty                                                         *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   lst             : pointer to list object                                                                         *
 *   data            : pointer to store the pointer to the memory object                                              *
 *                                                                                                                    *
 * Return                                                                                                             *
 *   E_SUCCESS       : pointer to memory object was successfully transfered to the data pointer                       *
 *   E_TIMEOUT       : list object is empty                                                                           *
 *                                                                                                                    *
 * Note              : use only in handler mode                                                                       *
 *                                                                                                                    *
 **********************************************************************************************************************/

__STATIC_INLINE
unsigned lst_takeISR( lst_t *lst, void **data ) { return lst_waitFor(lst, data, IMMEDIATE); }

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : lst_give                                                                                       *
 *                                                                                                                    *
 * Description       : transfer memory object to the list object,                                                     *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   lst             : pointer to list object                                                                         *
 *   data            : pointer to memory object                                                                       *
 *                                                                                                                    *
 * Return            : none                                                                                           *
 *                                                                                                                    *
 * Note              : use only in thread mode                                                                        *
 *                                                                                                                    *
 **********************************************************************************************************************/

void lst_give( lst_t *lst, void *data );

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : lst_giveISR                                                                                    *
 *                                                                                                                    *
 * Description       : transfer memory object to the list object,                                                     *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   lst             : pointer to list object                                                                         *
 *   data            : pointer to memory object                                                                       *
 *                                                                                                                    *
 * Return            : none                                                                                           *
 *                                                                                                                    *
 * Note              : use only in handler mode                                                                       *
 *                                                                                                                    *
 **********************************************************************************************************************/

__STATIC_INLINE
void lst_giveISR( lst_t *lst, void *data ) { lst_give(lst, data); }

#ifdef __cplusplus
}
#endif

/* -------------------------------------------------------------------------- */

#ifdef __cplusplus

/**********************************************************************************************************************
 *                                                                                                                    *
 * Class             : List                                                                                           *
 *                                                                                                                    *
 * Description       : create and initilize a list object                                                             *
 *                                                                                                                    *
 * Constructor parameters                                                                                             *
 *                   : none                                                                                           *
 *                                                                                                                    *
 **********************************************************************************************************************/

struct List : public __lst
{
	explicit
	 List( void ): __lst _LST_INIT() {}
	~List( void ) { assert(queue == nullptr); }

	void     kill     ( void )                          {        lst_kill     (this);                }
	unsigned waitUntil( void **_data, unsigned _time )  { return lst_waitUntil(this, _data, _time);  }
	unsigned waitFor  ( void **_data, unsigned _delay ) { return lst_waitFor  (this, _data, _delay); }
	unsigned wait     ( void **_data )                  { return lst_wait     (this, _data);         }
	unsigned take     ( void **_data )                  { return lst_take     (this, _data);         }
	unsigned takeISR  ( void **_data )                  { return lst_takeISR  (this, _data);         }
	void     give     ( void  *_data )                  {        lst_give     (this, _data);         }
	void     giveISR  ( void  *_data )                  {        lst_giveISR  (this, _data);         }
};

#endif

/* -------------------------------------------------------------------------- */

#endif//__STATEOS_LST_H
