/******************************************************************************

    @file    State Machine OS: os_msg.h
    @author  Rajmund Szymanski
    @date    14.12.2015
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

/* -------------------------------------------------------------------------- */

// deklaracja kolejki komunikatów 'msg'
// limit: rozmiar kolejki (maksymalna iloœæ komunikatów)

#define OS_MSG( msg, limit )                                   \
               unsigned msg##__data[limit];                     \
               msg_t msg##__msg = _MSG_INIT(limit, msg##__data); \
               msg_id msg = & msg##__msg

#define static_MSG( msg, limit )                               \
        static unsigned msg##__data[limit];                     \
        static msg_t msg##__msg = _MSG_INIT(limit, msg##__data); \
        static msg_id msg = & msg##__msg

/* -------------------------------------------------------------------------- */

// obs³uga obiektu: message queue (msg),
// który jest pochodn¹ obiektu mailbox queue (box)
// 'msg' jest kolejk¹ danych typu 'unsigned int'

              msg_id   msg_create( unsigned limit );

              void     msg_kill( msg_id msg );

              unsigned msg_waitUntil( msg_id msg, unsigned *data, unsigned time );

              unsigned msg_waitFor( msg_id msg, unsigned *data, unsigned delay );

static inline unsigned msg_wait( msg_id msg, unsigned *data ) { return msg_waitFor(msg, data, INFINITE); }

static inline unsigned msg_take   ( msg_id msg, unsigned *data ) { return msg_waitFor(msg, data, IMMEDIATE); }
static inline unsigned msg_takeISR( msg_id msg, unsigned *data ) { return msg_waitFor(msg, data, IMMEDIATE); }

              unsigned msg_sendUntil( msg_id msg, unsigned data, unsigned time );

              unsigned msg_sendFor( msg_id msg, unsigned data, unsigned delay );

static inline unsigned msg_send( msg_id msg, unsigned data ) { return msg_sendFor(msg, data, INFINITE); }

static inline unsigned msg_give   ( msg_id msg, unsigned data ) { return msg_sendFor(msg, data, IMMEDIATE); }
static inline unsigned msg_giveISR( msg_id msg, unsigned data ) { return msg_sendFor(msg, data, IMMEDIATE); }

/* -------------------------------------------------------------------------- */

#ifdef __cplusplus
}
#endif

/* -------------------------------------------------------------------------- */

#ifdef __cplusplus

#include <string.h>

// definicja klasy kolejki komunikatów
// limit: rozmiar kolejki (ilosc porcji danych)

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
