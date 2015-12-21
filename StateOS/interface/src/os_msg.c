/******************************************************************************

    @file    State Machine OS: os_msg.c
    @author  Rajmund Szymanski
    @date    21.12.2015
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
static void priv_msg_get( msg_id msg, unsigned *data )
/* -------------------------------------------------------------------------- */
{
	*data = msg->data[msg->first];

	msg->first = (msg->first + 1) % msg->limit;
}

/* -------------------------------------------------------------------------- */
static void priv_msg_put( msg_id msg, unsigned data )
/* -------------------------------------------------------------------------- */
{
	msg->data[msg->next] = data;

	msg->next = (msg->next + 1) % msg->limit;
}

/* -------------------------------------------------------------------------- */
static inline __attribute__((always_inline))
unsigned priv_msg_wait( msg_id msg, unsigned *data, unsigned time, unsigned(*wait)() )
/* -------------------------------------------------------------------------- */
{
	unsigned event = E_SUCCESS;

	port_sys_lock();

	if (msg->count == 0)
	{
		System.cur->data = data;

		event = wait(msg, time);
	}
	else
	{
		priv_msg_get(msg, data);

	    tsk_id tsk = core_one_wakeup(msg, E_SUCCESS);

	    if (tsk)
			priv_msg_put(msg, tsk->msg);
	    else
			msg->count--;
	}

	port_sys_unlock();

	return event;
}

/* -------------------------------------------------------------------------- */
unsigned msg_waitUntil( msg_id msg, unsigned *data, unsigned time )
/* -------------------------------------------------------------------------- */
{
	return priv_msg_wait(msg, data, time, core_tsk_waitUntil);
}

/* -------------------------------------------------------------------------- */
unsigned msg_waitFor( msg_id msg, unsigned *data, unsigned delay )
/* -------------------------------------------------------------------------- */
{
	return priv_msg_wait(msg, data, delay, core_tsk_waitFor);
}

/* -------------------------------------------------------------------------- */
static inline __attribute__((always_inline))
unsigned priv_msg_send( msg_id msg, unsigned data, unsigned time, unsigned(*wait)() )
/* -------------------------------------------------------------------------- */
{
	unsigned event = E_SUCCESS;

	port_sys_lock();

	if (msg->count >= msg->limit)
	{
		System.cur->msg = data;

		event = wait(msg, time);
	}
	else
	{
		priv_msg_put(msg, data);

	    tsk_id tsk = core_one_wakeup(msg, E_SUCCESS);

	    if (tsk)
			priv_msg_get(msg, tsk->data);
	    else
			msg->count++;
	}

	port_sys_unlock();

	return event;
}

/* -------------------------------------------------------------------------- */
unsigned msg_sendUntil( msg_id msg, unsigned data, unsigned time )
/* -------------------------------------------------------------------------- */
{
	return priv_msg_send(msg, data, time, core_tsk_waitUntil);
}

/* -------------------------------------------------------------------------- */
unsigned msg_sendFor( msg_id msg, unsigned data, unsigned delay )
/* -------------------------------------------------------------------------- */
{
	return priv_msg_send(msg, data, delay, core_tsk_waitFor);
}

/* -------------------------------------------------------------------------- */
