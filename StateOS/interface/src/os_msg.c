/******************************************************************************

    @file    State Machine OS: os_msg.c
    @author  Rajmund Szymanski
    @date    26.10.2015
    @brief   This file provides set of functions for StateOS.

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

#include <os.h>

/* -------------------------------------------------------------------------- */
msg_id msg_create( unsigned limit )
/* -------------------------------------------------------------------------- */
{
	msg_id msg;

	port_sys_lock();

	msg = core_sys_alloc(sizeof(msg_t) + limit * sizeof(unsigned));

	if (msg)
	{
		msg->limit = limit;
		msg->data  = (unsigned *)(msg + 1);
	}

	port_sys_unlock();

	return msg;
}

/* -------------------------------------------------------------------------- */
void msg_kill( msg_id msg )
/* -------------------------------------------------------------------------- */
{
	port_sys_lock();

	msg->count = 0;
	msg->first = 0;
	msg->next  = 0;

	core_all_wakeup(msg, E_STOPPED);

	port_sys_unlock();
}

/* -------------------------------------------------------------------------- */
static unsigned priv_msg_get( msg_id msg )
/* -------------------------------------------------------------------------- */
{
	unsigned data = msg->data[msg->first];

	msg->first = (msg->first + 1) % msg->limit;

	return data;
}

/* -------------------------------------------------------------------------- */
__attribute__((always_inline)) static inline
unsigned priv_msg_wait( msg_id msg, unsigned time, unsigned(*wait)() )
/* -------------------------------------------------------------------------- */
{
	unsigned event;

	port_sys_lock();

	event = wait(msg, time);

	if (event == E_SUCCESS)
	{
		event = priv_msg_get(msg);
	}

	port_sys_unlock();

	return event;
}

/* -------------------------------------------------------------------------- */
unsigned msg_waitUntil( msg_id msg, unsigned time )
/* -------------------------------------------------------------------------- */
{
	return priv_msg_wait(msg, time, sem_waitUntil);
}

/* -------------------------------------------------------------------------- */
unsigned msg_waitFor( msg_id msg, unsigned delay )
/* -------------------------------------------------------------------------- */
{
	return priv_msg_wait(msg, delay, sem_waitFor);
}

/* -------------------------------------------------------------------------- */
static void priv_msg_put( msg_id msg, unsigned data )
/* -------------------------------------------------------------------------- */
{
	msg->data[msg->next] = data;

	msg->next = (msg->next + 1) % msg->limit;
}

/* -------------------------------------------------------------------------- */
__attribute__((always_inline)) static inline
unsigned priv_msg_send( msg_id msg, unsigned data, unsigned time, unsigned(*wait)() )
/* -------------------------------------------------------------------------- */
{
	unsigned event;

	port_sys_lock();

	event = wait(msg, time);

	if (event == E_SUCCESS)
	{
		priv_msg_put(msg, data);
	}

	port_sys_unlock();

	return event;
}

/* -------------------------------------------------------------------------- */
unsigned msg_sendUntil( msg_id msg, unsigned data, unsigned time )
/* -------------------------------------------------------------------------- */
{
	return priv_msg_send(msg, data, time, sem_sendUntil);
}

/* -------------------------------------------------------------------------- */
unsigned msg_sendFor( msg_id msg, unsigned data, unsigned delay )
/* -------------------------------------------------------------------------- */
{
	return priv_msg_send(msg, data, delay, sem_sendFor);
}

/* -------------------------------------------------------------------------- */
