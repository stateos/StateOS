/******************************************************************************

    @file    State Machine OS: os_msg.h
    @author  Rajmund Szymanski
    @date    08.01.2016
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
 * Name              : message queue                                                                                  *
 *                     message is an 'unsigned int' data type; for other data types use mailbox queue                 *
 *                                                                                                                    *
 **********************************************************************************************************************/

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : OS_MSG                                                                                         *
 *                                                                                                                    *
 * Description       : define and initilize a message queue object                                                    *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   msg             : name of a pointer to message queue object                                                      *
 *   limit           : size of a queue (max number of stored messages)                                                *
 *                                                                                                                    *
 **********************************************************************************************************************/

#define     OS_MSG( msg, limit )                               \
               unsigned msg##__data[limit];                     \
               msg_t msg##__msg = _MSG_INIT(limit, msg##__data); \
               msg_id msg = & msg##__msg

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : static_MSG                                                                                     *
 *                                                                                                                    *
 * Description       : define and initilize a static message queue object                                             *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   msg             : name of a pointer to message queue object                                                      *
 *   limit           : size of a queue (max number of stored messages)                                                *
 *                                                                                                                    *
 **********************************************************************************************************************/

#define static_MSG( msg, limit )                               \
        static unsigned msg##__data[limit];                     \
        static msg_t msg##__msg = _MSG_INIT(limit, msg##__data); \
        static msg_id msg = & msg##__msg

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : msg_create                                                                                     *
 *                                                                                                                    *
 * Description       : create and initilize a new message queue object                                                *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   limit           : size of a queue (max number of stored messages)                                                *
 *                                                                                                                    *
 * Return            : pointer to message queue object (message queue successfully created)                           *
 *   0               : message queue not created (not enough free memory)                                             *
 *                                                                                                                    *
 * Note              : use only in thread mode                                                                        *
 *                                                                                                                    *
 **********************************************************************************************************************/

              msg_id   msg_create( unsigned limit );

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : msg_kill                                                                                       *
 *                                                                                                                    *
 * Description       : reset the message queue object and wake up all waiting tasks with 'E_STOPPED' event value      *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   msg             : pointer to message queue object                                                                *
 *                                                                                                                    *
 * Return            : none                                                                                           *
 *                                                                                                                    *
 * Note              : use only in thread mode                                                                        *
 *                                                                                                                    *
 **********************************************************************************************************************/

              void     msg_kill( msg_id msg );

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : msg_waitUntil                                                                                  *
 *                                                                                                                    *
 * Description       : try to transfer message data from the message queue object,                                    *
 *                     wait until given timepoint while the message queue object is empty                             *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   msg             : pointer to message queue object                                                                *
 *   data            : pointer to store message data                                                                  *
 *   time            : timepoint value                                                                                *
 *                                                                                                                    *
 * Return                                                                                                             *
 *   E_SUCCESS       : message data was successfully transfered from the message queue object                         *
 *   E_STOPPED       : message queue object was killed before the specified timeout expired                           *
 *   E_TIMEOUT       : message queue object is empty and was not received data before the specified timeout expired   *
 *   'another'       : task was resumed with 'another' event value                                                    *
 *                                                                                                                    *
 * Note              : use only in thread mode                                                                        *
 *                                                                                                                    *
 **********************************************************************************************************************/

              unsigned msg_waitUntil( msg_id msg, unsigned *data, unsigned time );

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : msg_waitFor                                                                                    *
 *                                                                                                                    *
 * Description       : try to transfer message data from the message queue object,                                    *
 *                     wait for given duration of time while the message queue object is empty                        *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   msg             : pointer to message queue object                                                                *
 *   data            : pointer to store message data                                                                  *
 *   delay           : duration of time (maximum number of ticks to wait while the message queue object is empty)     *
 *                     IMMEDIATE: don't wait if the message queue object is empty                                     *
 *                     INFINITE:  wait indefinitly while the message queue object is empty                            *
 *                                                                                                                    *
 * Return                                                                                                             *
 *   E_SUCCESS       : message data was successfully transfered from the message queue object                         *
 *   E_STOPPED       : message queue object was killed before the specified timeout expired                           *
 *   E_TIMEOUT       : message queue object is empty and was not received data before the specified timeout expired   *
 *   'another'       : task was resumed with 'another' event value                                                    *
 *                                                                                                                    *
 * Note              : use only in thread mode                                                                        *
 *                                                                                                                    *
 **********************************************************************************************************************/

              unsigned msg_waitFor( msg_id msg, unsigned *data, unsigned delay );

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : msg_wait                                                                                       *
 *                                                                                                                    *
 * Description       : try to transfer message data from the message queue object,                                    *
 *                     wait indefinitly while the message queue object is empty                                       *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   msg             : pointer to message queue object                                                                *
 *   data            : pointer to store message data                                                                  *
 *                                                                                                                    *
 * Return                                                                                                             *
 *   E_SUCCESS       : message data was successfully transfered from the message queue object                         *
 *   E_STOPPED       : message queue object was killed                                                                *
 *   'another'       : task was resumed with 'another' event value                                                    *
 *                                                                                                                    *
 * Note              : use only in thread mode                                                                        *
 *                                                                                                                    *
 **********************************************************************************************************************/

static inline unsigned msg_wait( msg_id msg, unsigned *data ) { return msg_waitFor(msg, data, INFINITE); }

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : msg_take                                                                                       *
 *                                                                                                                    *
 * Description       : try to transfer message data from the message queue object,                                    *
 *                     don't wait if the message queue object is empty                                                *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   msg             : pointer to message queue object                                                                *
 *   data            : pointer to store message data                                                                  *
 *                                                                                                                    *
 * Return                                                                                                             *
 *   E_SUCCESS       : message data was successfully transfered from the message queue object                         *
 *   E_TIMEOUT       : message queue object is empty                                                                  *
 *                                                                                                                    *
 * Note              : use only in thread mode                                                                        *
 *                                                                                                                    *
 **********************************************************************************************************************/

static inline unsigned msg_take   ( msg_id msg, unsigned *data ) { return msg_waitFor(msg, data, IMMEDIATE); }

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : msg_takeISR                                                                                    *
 *                                                                                                                    *
 * Description       : try to transfer message data from the message queue object,                                    *
 *                     don't wait if the message queue object is empty                                                *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   msg             : pointer to message queue object                                                                *
 *   data            : pointer to store message data                                                                  *
 *                                                                                                                    *
 * Return                                                                                                             *
 *   E_SUCCESS       : message data was successfully transfered from the message queue object                         *
 *   E_TIMEOUT       : message queue object is empty                                                                  *
 *                                                                                                                    *
 * Note              : use only in handler mode                                                                       *
 *                                                                                                                    *
 **********************************************************************************************************************/

static inline unsigned msg_takeISR( msg_id msg, unsigned *data ) { return msg_waitFor(msg, data, IMMEDIATE); }

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : msg_sendUntil                                                                                  *
 *                                                                                                                    *
 * Description       : try to transfer message data to the message queue object,                                      *
 *                     wait until given timepoint while the message queue object is full                              *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   msg             : pointer to message queue object                                                                *
 *   data            : message data                                                                                   *
 *   time            : timepoint value                                                                                *
 *                                                                                                                    *
 * Return                                                                                                             *
 *   E_SUCCESS       : message data was successfully transfered to the message queue object                           *
 *   E_STOPPED       : message queue object was killed before the specified timeout expired                           *
 *   E_TIMEOUT       : message queue object is full and was not issued data before the specified timeout expired      *
 *   'another'       : task was resumed with 'another' event value                                                    *
 *                                                                                                                    *
 * Note              : use only in thread mode                                                                        *
 *                                                                                                                    *
 **********************************************************************************************************************/

              unsigned msg_sendUntil( msg_id msg, unsigned data, unsigned time );

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : msg_sendFor                                                                                    *
 *                                                                                                                    *
 * Description       : try to transfer message data to the message queue object,                                      *
 *                     wait for given duration of time while the message queue object is full                         *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   msg             : pointer to message queue object                                                                *
 *   data            : message data                                                                                   *
 *   delay           : duration of time (maximum number of ticks to wait while the message queue object is full)      *
 *                     IMMEDIATE: don't wait if the message queue object is full                                      *
 *                     INFINITE:  wait indefinitly while the message queue object is full                             *
 *                                                                                                                    *
 * Return                                                                                                             *
 *   E_SUCCESS       : message data was successfully transfered to the message queue object                           *
 *   E_STOPPED       : message queue object was killed before the specified timeout expired                           *
 *   E_TIMEOUT       : message queue object is full and was not issued data before the specified timeout expired      *
 *   'another'       : task was resumed with 'another' event value                                                    *
 *                                                                                                                    *
 * Note              : use only in thread mode                                                                        *
 *                                                                                                                    *
 **********************************************************************************************************************/

              unsigned msg_sendFor( msg_id msg, unsigned data, unsigned delay );

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : msg_send                                                                                       *
 *                                                                                                                    *
 * Description       : try to transfer message data to the message queue object,                                      *
 *                     wait indefinitly while the message queue object is full                                        *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   msg             : pointer to message queue object                                                                *
 *   data            : message data                                                                                   *
 *                                                                                                                    *
 * Return                                                                                                             *
 *   E_SUCCESS       : message data was successfully transfered to the message queue object                           *
 *   E_STOPPED       : message queue object was killed                                                                *
 *   'another'       : task was resumed with 'another' event value                                                    *
 *                                                                                                                    *
 * Note              : use only in thread mode                                                                        *
 *                                                                                                                    *
 **********************************************************************************************************************/

static inline unsigned msg_send( msg_id msg, unsigned data ) { return msg_sendFor(msg, data, INFINITE); }

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : msg_give                                                                                       *
 *                                                                                                                    *
 * Description       : try to transfer message data to the message queue object,                                      *
 *                     don't wait if the message queue object is full                                                 *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   msg             : pointer to message queue object                                                                *
 *   data            : message data                                                                                   *
 *                                                                                                                    *
 * Return                                                                                                             *
 *   E_SUCCESS       : message data was successfully transfered to the message queue object                           *
 *   E_TIMEOUT       : message queue object is full                                                                   *
 *                                                                                                                    *
 * Note              : use only in thread mode                                                                        *
 *                                                                                                                    *
 **********************************************************************************************************************/

static inline unsigned msg_give   ( msg_id msg, unsigned data ) { return msg_sendFor(msg, data, IMMEDIATE); }

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : msg_giveISR                                                                                    *
 *                                                                                                                    *
 * Description       : try to transfer message data to the message queue object,                                      *
 *                     don't wait if the message queue object is full                                                 *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   msg             : pointer to message queue object                                                                *
 *   data            : message data                                                                                   *
 *                                                                                                                    *
 * Return                                                                                                             *
 *   E_SUCCESS       : message data was successfully transfered to the message queue object                           *
 *   E_TIMEOUT       : message queue object is full                                                                   *
 *                                                                                                                    *
 * Note              : use only in handler mode                                                                       *
 *                                                                                                                    *
 **********************************************************************************************************************/

static inline unsigned msg_giveISR( msg_id msg, unsigned data ) { return msg_sendFor(msg, data, IMMEDIATE); }

#ifdef __cplusplus
}
#endif

/* -------------------------------------------------------------------------- */

#ifdef __cplusplus

#include <string.h>

template<unsigned _limit>
class MessageQueueT : public msg_t
{
	unsigned _data[_limit];

public:

	 MessageQueueT( void ): msg_t(_MSG_INIT(_limit, _data)) {}

	~MessageQueueT( void ) { msg_kill(this); }

	unsigned waitUntil( unsigned _time  )                 { return msg_waitUntil(this,        _time);  }
	unsigned waitFor  ( unsigned _delay )                 { return msg_waitFor  (this,        _delay); }
	unsigned wait     ( void )                            { return msg_wait     (this);                }
	unsigned take     ( void )                            { return msg_take     (this);                }
	unsigned takeISR  ( void )                            { return msg_takeISR  (this);                }
	unsigned sendUntil( unsigned _data, unsigned _time  ) { return msg_sendUntil(this, _data, _time);  }
	unsigned sendFor  ( unsigned _data, unsigned _delay ) { return msg_sendFor  (this, _data, _delay); }
	unsigned send     ( unsigned _data )                  { return msg_send     (this, _data);         }
	unsigned give     ( unsigned _data )                  { return msg_give     (this, _data);         }
	unsigned giveISR  ( unsigned _data )                  { return msg_giveISR  (this, _data);         }
};

#endif

/* -------------------------------------------------------------------------- */
