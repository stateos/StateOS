/******************************************************************************

    @file    StateOS: os_box.h
    @author  Rajmund Szymanski
    @date    21.03.2016
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

#pragma once

#include <oskernel.h>

#ifdef __cplusplus
extern "C" {
#endif

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : mailbox queue                                                                                  *
 *                                                                                                                    *
 **********************************************************************************************************************/

typedef struct __box
{
	tsk_id   queue; // inherited from semaphore
	unsigned count; // inherited from semaphore
	unsigned limit; // inherited from semaphore

	unsigned first; // first element to read from queue
	unsigned next;  // next element to write into queue
	char    *data;  // queue data
	unsigned size;  // size of a single mail (in bytes)

}	box_t, *box_id;

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

#define               _BOX_INIT( _limit, _size, _data ) { 0, 0, _limit, 0, 0, _data, _size }

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

#define               _BOX_DATA( _limit, _size ) (char[_limit*_size]){ 0 }

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

#define                BOX_INIT( limit, size ) \
                      _BOX_INIT( limit, size, _BOX_DATA( limit, size ) )

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : BOX_CREATE                                                                                     *
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
               &(box_t)BOX_INIT( limit, size )
#endif

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : box_create                                                                                     *
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

              box_id   box_create( unsigned limit, unsigned size );

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

              void     box_kill( box_id box );

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
 *   'another'       : task was resumed with 'another' event value                                                    *
 *                                                                                                                    *
 * Note              : use only in thread mode                                                                        *
 *                                                                                                                    *
 **********************************************************************************************************************/

              unsigned box_waitUntil( box_id box, void *data, unsigned time );

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
 *   'another'       : task was resumed with 'another' event value                                                    *
 *                                                                                                                    *
 * Note              : use only in thread mode                                                                        *
 *                                                                                                                    *
 **********************************************************************************************************************/

              unsigned box_waitFor( box_id box, void *data, unsigned delay );

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
 *   'another'       : task was resumed with 'another' event value                                                    *
 *                                                                                                                    *
 * Note              : use only in thread mode                                                                        *
 *                                                                                                                    *
 **********************************************************************************************************************/

static inline unsigned box_wait( box_id box, void *data ) { return box_waitFor(box, data, INFINITE); }

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

static inline unsigned box_take( box_id box, void *data ) { return box_waitFor(box, data, IMMEDIATE); }

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

static inline unsigned box_takeISR( box_id box, void *data ) { return box_waitFor(box, data, IMMEDIATE); }

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
 *   'another'       : task was resumed with 'another' event value                                                    *
 *                                                                                                                    *
 * Note              : use only in thread mode                                                                        *
 *                                                                                                                    *
 **********************************************************************************************************************/

              unsigned box_sendUntil( box_id box, void *data, unsigned time );

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
 *   'another'       : task was resumed with 'another' event value                                                    *
 *                                                                                                                    *
 * Note              : use only in thread mode                                                                        *
 *                                                                                                                    *
 **********************************************************************************************************************/

              unsigned box_sendFor( box_id box, void *data, unsigned delay );

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
 *   'another'       : task was resumed with 'another' event value                                                    *
 *                                                                                                                    *
 * Note              : use only in thread mode                                                                        *
 *                                                                                                                    *
 **********************************************************************************************************************/

static inline unsigned box_send( box_id box, void *data ) { return box_sendFor(box, data, INFINITE); }

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

static inline unsigned box_give( box_id box, void *data ) { return box_sendFor(box, data, IMMEDIATE); }

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

static inline unsigned box_giveISR( box_id box, void *data ) { return box_sendFor(box, data, IMMEDIATE); }

#ifdef __cplusplus
}
#endif

/* -------------------------------------------------------------------------- */

#ifdef __cplusplus

/**********************************************************************************************************************
 *                                                                                                                    *
 * Class             : MailBoxQueue                                                                                   *
 *                                                                                                                    *
 * Description       : create and initilize a mailbox queue object                                                    *
 *                                                                                                                    *
 * Constructor parameters                                                                                             *
 *   T               : class of a single mail                                                                         *
 *   limit           : size of a queue (max number of stored mails)                                                   *
 *                                                                                                                    *
 **********************************************************************************************************************/

template<class T, unsigned _limit>
class MailBoxQueueT : public __box, private EventGuard<__box>
{
	T _data[_limit];

public:

	explicit
	MailBoxQueueT( void ): __box(_BOX_INIT(_limit, sizeof(T), reinterpret_cast<char *>(_data))) {}

	void     kill     ( void )                      {        box_kill     (this);                }
	unsigned waitUntil( T *_data, unsigned _time  ) { return box_waitUntil(this, _data, _time);  }
	unsigned waitFor  ( T *_data, unsigned _delay ) { return box_waitFor  (this, _data, _delay); }
	unsigned wait     ( T *_data )                  { return box_wait     (this, _data);         }
	unsigned take     ( T *_data )                  { return box_take     (this, _data);         }
	unsigned takeISR  ( T *_data )                  { return box_takeISR  (this, _data);         }
	unsigned sendUntil( T *_data, unsigned _time  ) { return box_sendUntil(this, _data, _time);  }
	unsigned sendFor  ( T *_data, unsigned _delay ) { return box_sendFor  (this, _data, _delay); }
	unsigned send     ( T *_data )                  { return box_send     (this, _data);         }
	unsigned give     ( T *_data )                  { return box_give     (this, _data);         }
	unsigned giveISR  ( T *_data )                  { return box_giveISR  (this, _data);         }
};

#endif

/* -------------------------------------------------------------------------- */
