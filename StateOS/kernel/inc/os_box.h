/******************************************************************************

    @file    StateOS: os_box.h
    @author  Rajmund Szymanski
    @date    22.10.2017
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

#ifndef __STATEOS_BOX_H
#define __STATEOS_BOX_H

#include "oskernel.h"

#ifdef __cplusplus
extern "C" {
#endif

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : mailbox queue                                                                                  *
 *                                                                                                                    *
 **********************************************************************************************************************/

typedef struct __box box_t, * const box_id;

struct __box
{
	tsk_t  * queue; // inherited from semaphore
	void   * res;   // allocated mailbox queue object's resource
	unsigned count; // inherited from semaphore
	unsigned limit; // inherited from semaphore

	unsigned first; // first element to read from queue
	unsigned next;  // next element to write into queue
	char   * data;  // queue data
	unsigned size;  // size of a single mail (in bytes)
};

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : _BOX_INIT                                                                                      *
 *                                                                                                                    *
 * Description       : create and initilize a mailbox queue object                                                    *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   limit           : size of a queue (max number of stored mails)                                                   *
 *   size            : size of a single mail (in bytes)                                                               *
 *   data            : mailbox queue data buffer                                                                      *
 *                                                                                                                    *
 * Return            : mailbox queue object                                                                           *
 *                                                                                                                    *
 * Note              : for internal use                                                                               *
 *                                                                                                                    *
 **********************************************************************************************************************/

#define               _BOX_INIT( _limit, _size, _data ) { 0, 0, 0, _limit, 0, 0, _data, _size }

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : _BOX_DATA                                                                                      *
 *                                                                                                                    *
 * Description       : create a mailbox queue data buffer                                                             *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   limit           : size of a queue (max number of stored mails)                                                   *
 *   size            : size of a single mail (in bytes)                                                               *
 *                                                                                                                    *
 * Return            : mailbox queue data buffer                                                                      *
 *                                                                                                                    *
 * Note              : for internal use                                                                               *
 *                                                                                                                    *
 **********************************************************************************************************************/

#ifndef __cplusplus
#define               _BOX_DATA( _limit, _size ) (char[_limit * _size]){ 0 }
#endif

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : OS_BOX                                                                                         *
 *                                                                                                                    *
 * Description       : define and initilize a mailbox queue object                                                    *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   box             : name of a pointer to mailbox queue object                                                      *
 *   limit           : size of a queue (max number of stored mails)                                                   *
 *   size            : size of a single mail (in bytes)                                                               *
 *                                                                                                                    *
 **********************************************************************************************************************/

#define             OS_BOX( box, limit, size )                                \
                       char box##__buf[limit*size];                            \
                       box_t box##__box = _BOX_INIT( limit, size, box##__buf ); \
                       box_id box = & box##__box

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : static_BOX                                                                                     *
 *                                                                                                                    *
 * Description       : define and initilize a static mailbox queue object                                             *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   box             : name of a pointer to mailbox queue object                                                      *
 *   limit           : size of a queue (max number of stored mails)                                                   *
 *   size            : size of a single mail (in bytes)                                                               *
 *                                                                                                                    *
 **********************************************************************************************************************/

#define         static_BOX( box, limit, size )                                \
                static char box##__buf[limit*size];                            \
                static box_t box##__box = _BOX_INIT( limit, size, box##__buf ); \
                static box_id box = & box##__box

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : BOX_INIT                                                                                       *
 *                                                                                                                    *
 * Description       : create and initilize a mailbox queue object                                                    *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   limit           : size of a queue (max number of stored mails)                                                   *
 *   size            : size of a single mail (in bytes)                                                               *
 *                                                                                                                    *
 * Return            : mailbox queue object                                                                           *
 *                                                                                                                    *
 * Note              : use only in 'C' code                                                                           *
 *                                                                                                                    *
 **********************************************************************************************************************/

#ifndef __cplusplus
#define                BOX_INIT( limit, size ) \
                      _BOX_INIT( limit, size, _BOX_DATA( limit, size ) )
#endif

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : BOX_CREATE                                                                                     *
 * Alias             : BOX_NEW                                                                                        *
 *                                                                                                                    *
 * Description       : create and initilize a mailbox queue object                                                    *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   limit           : size of a queue (max number of stored mails)                                                   *
 *   size            : size of a single mail (in bytes)                                                               *
 *                                                                                                                    *
 * Return            : pointer to mailbox queue object                                                                *
 *                                                                                                                    *
 * Note              : use only in 'C' code                                                                           *
 *                                                                                                                    *
 **********************************************************************************************************************/

#ifndef __cplusplus
#define                BOX_CREATE( limit, size ) \
             & (box_t) BOX_INIT  ( limit, size )
#define                BOX_NEW \
                       BOX_CREATE
#endif

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : box_init                                                                                       *
 *                                                                                                                    *
 * Description       : initilize a mailbox queue object                                                               *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   box             : pointer to mailbox queue object                                                                *
 *   limit           : size of a queue (max number of stored mails)                                                   *
 *   size            : size of a single mail (in bytes)                                                               *
 *   data            : mailbox queue data buffer                                                                      *
 *                                                                                                                    *
 * Return            : none                                                                                           *
 *                                                                                                                    *
 * Note              : use only in thread mode                                                                        *
 *                                                                                                                    *
 **********************************************************************************************************************/

void box_init( box_t *box, unsigned limit, unsigned size, void *data );

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : box_create                                                                                     *
 * Alias             : box_new                                                                                        *
 *                                                                                                                    *
 * Description       : create and initilize a new mailbox queue object                                                *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   limit           : size of a queue (max number of stored mails)                                                   *
 *   size            : size of a single mail (in bytes)                                                               *
 *                                                                                                                    *
 * Return            : pointer to mailbox queue object (mailbox queue successfully created)                           *
 *   0               : mailbox queue not created (not enough free memory)                                             *
 *                                                                                                                    *
 * Note              : use only in thread mode                                                                        *
 *                                                                                                                    *
 **********************************************************************************************************************/

box_t *box_create( unsigned limit, unsigned size );
__STATIC_INLINE
box_t *box_new   ( unsigned limit, unsigned size ) { return box_create(limit, size); }

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : box_kill                                                                                       *
 *                                                                                                                    *
 * Description       : reset the mailbox queue object and wake up all waiting tasks with 'E_STOPPED' event value      *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   box             : pointer to mailbox queue object                                                                *
 *                                                                                                                    *
 * Return            : none                                                                                           *
 *                                                                                                                    *
 * Note              : use only in thread mode                                                                        *
 *                                                                                                                    *
 **********************************************************************************************************************/

void box_kill( box_t *box );

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : box_delete                                                                                     *
 *                                                                                                                    *
 * Description       : reset the mailbox queue object and free allocated resource                                     *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   box             : pointer to mailbox queue object                                                                *
 *                                                                                                                    *
 * Return            : none                                                                                           *
 *                                                                                                                    *
 * Note              : use only in thread mode                                                                        *
 *                                                                                                                    *
 **********************************************************************************************************************/

void box_delete( box_t *box );

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : box_waitUntil                                                                                  *
 *                                                                                                                    *
 * Description       : try to transfer mailbox data from the mailbox queue object,                                    *
 *                     wait until given timepoint while the mailbox queue object is empty                             *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   box             : pointer to mailbox queue object                                                                *
 *   data            : pointer to store mailbox data                                                                  *
 *   time            : timepoint value                                                                                *
 *                                                                                                                    *
 * Return                                                                                                             *
 *   E_SUCCESS       : mailbox data was successfully transfered from the mailbox queue object                         *
 *   E_STOPPED       : mailbox queue object was killed before the specified timeout expired                           *
 *   E_TIMEOUT       : mailbox queue object is empty and was not received data before the specified timeout expired   *
 *                                                                                                                    *
 * Note              : use only in thread mode                                                                        *
 *                                                                                                                    *
 **********************************************************************************************************************/

unsigned box_waitUntil( box_t *box, void *data, uint32_t time );

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : box_waitFor                                                                                    *
 *                                                                                                                    *
 * Description       : try to transfer mailbox data from the mailbox queue object,                                    *
 *                     wait for given duration of time while the mailbox queue object is empty                        *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   box             : pointer to mailbox queue object                                                                *
 *   data            : pointer to store mailbox data                                                                  *
 *   delay           : duration of time (maximum number of ticks to wait while the mailbox queue object is empty)     *
 *                     IMMEDIATE: don't wait if the mailbox queue object is empty                                     *
 *                     INFINITE:  wait indefinitly while the mailbox queue object is empty                            *
 *                                                                                                                    *
 * Return                                                                                                             *
 *   E_SUCCESS       : mailbox data was successfully transfered from the mailbox queue object                         *
 *   E_STOPPED       : mailbox queue object was killed before the specified timeout expired                           *
 *   E_TIMEOUT       : mailbox queue object is empty and was not received data before the specified timeout expired   *
 *                                                                                                                    *
 * Note              : use only in thread mode                                                                        *
 *                                                                                                                    *
 **********************************************************************************************************************/

unsigned box_waitFor( box_t *box, void *data, uint32_t delay );

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : box_wait                                                                                       *
 *                                                                                                                    *
 * Description       : try to transfer mailbox data from the mailbox queue object,                                    *
 *                     wait indefinitly while the mailbox queue object is empty                                       *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   box             : pointer to mailbox queue object                                                                *
 *   data            : pointer to store mailbox data                                                                  *
 *                                                                                                                    *
 * Return                                                                                                             *
 *   E_SUCCESS       : mailbox data was successfully transfered from the mailbox queue object                         *
 *   E_STOPPED       : mailbox queue object was killed                                                                *
 *                                                                                                                    *
 * Note              : use only in thread mode                                                                        *
 *                                                                                                                    *
 **********************************************************************************************************************/

__STATIC_INLINE
unsigned box_wait( box_t *box, void *data ) { return box_waitFor(box, data, INFINITE); }

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : box_take                                                                                       *
 *                                                                                                                    *
 * Description       : try to transfer mailbox data from the mailbox queue object,                                    *
 *                     don't wait if the mailbox queue object is empty                                                *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   box             : pointer to mailbox queue object                                                                *
 *   data            : pointer to store mailbox data                                                                  *
 *                                                                                                                    *
 * Return                                                                                                             *
 *   E_SUCCESS       : mailbox data was successfully transfered from the mailbox queue object                         *
 *   E_TIMEOUT       : mailbox queue object is empty                                                                  *
 *                                                                                                                    *
 * Note              : use only in thread mode                                                                        *
 *                                                                                                                    *
 **********************************************************************************************************************/

__STATIC_INLINE
unsigned box_take( box_t *box, void *data ) { return box_waitFor(box, data, IMMEDIATE); }

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : box_takeISR                                                                                    *
 *                                                                                                                    *
 * Description       : try to transfer mailbox data from the mailbox queue object,                                    *
 *                     don't wait if the mailbox queue object is empty                                                *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   box             : pointer to mailbox queue object                                                                *
 *   data            : pointer to store mailbox data                                                                  *
 *                                                                                                                    *
 * Return                                                                                                             *
 *   E_SUCCESS       : mailbox data was successfully transfered from the mailbox queue object                         *
 *   E_TIMEOUT       : mailbox queue object is empty                                                                  *
 *                                                                                                                    *
 * Note              : use only in handler mode                                                                       *
 *                                                                                                                    *
 **********************************************************************************************************************/

__STATIC_INLINE
unsigned box_takeISR( box_t *box, void *data ) { return box_waitFor(box, data, IMMEDIATE); }

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : box_sendUntil                                                                                  *
 *                                                                                                                    *
 * Description       : try to transfer mailbox data to the mailbox queue object,                                      *
 *                     wait until given timepoint while the mailbox queue object is full                              *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   box             : pointer to mailbox queue object                                                                *
 *   data            : pointer to mailbox data                                                                        *
 *   time            : timepoint value                                                                                *
 *                                                                                                                    *
 * Return                                                                                                             *
 *   E_SUCCESS       : mailbox data was successfully transfered to the mailbox queue object                           *
 *   E_STOPPED       : mailbox queue object was killed before the specified timeout expired                           *
 *   E_TIMEOUT       : mailbox queue object is full and was not issued data before the specified timeout expired      *
 *                                                                                                                    *
 * Note              : use only in thread mode                                                                        *
 *                                                                                                                    *
 **********************************************************************************************************************/

unsigned box_sendUntil( box_t *box, const void *data, uint32_t time );

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : box_sendFor                                                                                    *
 *                                                                                                                    *
 * Description       : try to transfer mailbox data to the mailbox queue object,                                      *
 *                     wait for given duration of time while the mailbox queue object is full                         *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   box             : pointer to mailbox queue object                                                                *
 *   data            : pointer to mailbox data                                                                        *
 *   delay           : duration of time (maximum number of ticks to wait while the mailbox queue object is full)      *
 *                     IMMEDIATE: don't wait if the mailbox queue object is full                                      *
 *                     INFINITE:  wait indefinitly while the mailbox queue object is full                             *
 *                                                                                                                    *
 * Return                                                                                                             *
 *   E_SUCCESS       : mailbox data was successfully transfered to the mailbox queue object                           *
 *   E_STOPPED       : mailbox queue object was killed before the specified timeout expired                           *
 *   E_TIMEOUT       : mailbox queue object is full and was not issued data before the specified timeout expired      *
 *                                                                                                                    *
 * Note              : use only in thread mode                                                                        *
 *                                                                                                                    *
 **********************************************************************************************************************/

unsigned box_sendFor( box_t *box, const void *data, uint32_t delay );

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : box_send                                                                                       *
 *                                                                                                                    *
 * Description       : try to transfer mailbox data to the mailbox queue object,                                      *
 *                     wait indefinitly while the mailbox queue object is full                                        *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   box             : pointer to mailbox queue object                                                                *
 *   data            : pointer to mailbox data                                                                        *
 *                                                                                                                    *
 * Return                                                                                                             *
 *   E_SUCCESS       : mailbox data was successfully transfered to the mailbox queue object                           *
 *   E_STOPPED       : mailbox queue object was killed                                                                *
 *                                                                                                                    *
 * Note              : use only in thread mode                                                                        *
 *                                                                                                                    *
 **********************************************************************************************************************/

__STATIC_INLINE
unsigned box_send( box_t *box, const void *data ) { return box_sendFor(box, data, INFINITE); }

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : box_give                                                                                       *
 *                                                                                                                    *
 * Description       : try to transfer mailbox data to the mailbox queue object,                                      *
 *                     don't wait if the mailbox queue object is full                                                 *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   box             : pointer to mailbox queue object                                                                *
 *   data            : pointer to mailbox data                                                                        *
 *                                                                                                                    *
 * Return                                                                                                             *
 *   E_SUCCESS       : mailbox data was successfully transfered to the mailbox queue object                           *
 *   E_TIMEOUT       : mailbox queue object is full                                                                   *
 *                                                                                                                    *
 * Note              : use only in thread mode                                                                        *
 *                                                                                                                    *
 **********************************************************************************************************************/

__STATIC_INLINE
unsigned box_give( box_t *box, const void *data ) { return box_sendFor(box, data, IMMEDIATE); }

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : box_giveISR                                                                                    *
 *                                                                                                                    *
 * Description       : try to transfer mailbox data to the mailbox queue object,                                      *
 *                     don't wait if the mailbox queue object is full                                                 *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   box             : pointer to mailbox queue object                                                                *
 *   data            : pointer to mailbox data                                                                        *
 *                                                                                                                    *
 * Return                                                                                                             *
 *   E_SUCCESS       : mailbox data was successfully transfered to the mailbox queue object                           *
 *   E_TIMEOUT       : mailbox queue object is full                                                                   *
 *                                                                                                                    *
 * Note              : use only in handler mode                                                                       *
 *                                                                                                                    *
 **********************************************************************************************************************/

__STATIC_INLINE
unsigned box_giveISR( box_t *box, const void *data ) { return box_sendFor(box, data, IMMEDIATE); }

#ifdef __cplusplus
}
#endif

/* -------------------------------------------------------------------------- */

#ifdef __cplusplus

/**********************************************************************************************************************
 *                                                                                                                    *
 * Class             : baseMailBoxQueue                                                                               *
 *                                                                                                                    *
 * Description       : create and initilize a mailbox queue object                                                    *
 *                                                                                                                    *
 * Constructor parameters                                                                                             *
 *   limit           : size of a queue (max number of stored mails)                                                   *
 *   size            : size of a single mail (in bytes)                                                               *
 *   data            : mailbox queue data buffer                                                                      *
 *                                                                                                                    *
 * Note              : for internal use                                                                               *
 *                                                                                                                    *
 **********************************************************************************************************************/

struct baseMailBoxQueue : public __box
{
	 explicit
	 baseMailBoxQueue( const unsigned _limit, const unsigned _size, char * const _data ): __box _BOX_INIT(_limit, _size, _data) {}
	~baseMailBoxQueue( void ) { assert(queue == nullptr); }

	void     kill     ( void )                               {        box_kill     (this);                }
	unsigned waitUntil(       void *_data, uint32_t _time  ) { return box_waitUntil(this, _data, _time);  }
	unsigned waitFor  (       void *_data, uint32_t _delay ) { return box_waitFor  (this, _data, _delay); }
	unsigned wait     (       void *_data )                  { return box_wait     (this, _data);         }
	unsigned take     (       void *_data )                  { return box_take     (this, _data);         }
	unsigned takeISR  (       void *_data )                  { return box_takeISR  (this, _data);         }
	unsigned sendUntil( const void *_data, uint32_t _time  ) { return box_sendUntil(this, _data, _time);  }
	unsigned sendFor  ( const void *_data, uint32_t _delay ) { return box_sendFor  (this, _data, _delay); }
	unsigned send     ( const void *_data )                  { return box_send     (this, _data);         }
	unsigned give     ( const void *_data )                  { return box_give     (this, _data);         }
	unsigned giveISR  ( const void *_data )                  { return box_giveISR  (this, _data);         }
};

/**********************************************************************************************************************
 *                                                                                                                    *
 * Class             : MailBoxQueue                                                                                   *
 *                                                                                                                    *
 * Description       : create and initilize a mailbox queue object                                                    *
 *                                                                                                                    *
 * Constructor parameters                                                                                             *
 *   limit           : size of a queue (max number of stored mails)                                                   *
 *   size            : size of a single mail (in bytes)                                                               *
 *                                                                                                                    *
 **********************************************************************************************************************/

template<unsigned _limit, unsigned _size>
struct MailBoxQueueT : public baseMailBoxQueue
{
	explicit
	MailBoxQueueT( void ): baseMailBoxQueue(_limit, _size, _data) {}

	private:
	char _data[_limit * _size];
};

/**********************************************************************************************************************
 *                                                                                                                    *
 * Class             : MailBoxQueue                                                                                   *
 *                                                                                                                    *
 * Description       : create and initilize a mailbox queue object                                                    *
 *                                                                                                                    *
 * Constructor parameters                                                                                             *
 *   limit           : size of a queue (max number of stored mails)                                                   *
 *   T               : class of a single mail                                                                         *
 *                                                                                                                    *
 **********************************************************************************************************************/

template<unsigned _limit, class T>
struct MailBoxQueueTT : public MailBoxQueueT<_limit, sizeof(T)>
{
	explicit
	MailBoxQueueTT( void ): MailBoxQueueT<_limit, sizeof(T)>() {}
};

#endif

/* -------------------------------------------------------------------------- */

#endif//__STATEOS_BOX_H
