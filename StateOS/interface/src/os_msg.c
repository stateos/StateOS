/******************************************************************************

    @file    StateOS: os_msg.c
    @author  Rajmund Szymanski
    @date    01.03.2017
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
void msg_init( msg_t *msg, unsigned limit, void *data )
/* -------------------------------------------------------------------------- */
{
	assert(!port_isr_inside());
	assert(msg);
	assert(limit);
	assert(data);

	port_sys_lock();

	memset(msg, 0, sizeof(msg_t));

	msg->limit = limit;
	msg->data  = data;

	port_sys_unlock();
}

/* -------------------------------------------------------------------------- */
msg_t *msg_create( unsigned limit )
/* -------------------------------------------------------------------------- */
{
	msg_t *msg;

	assert(!port_isr_inside());
	assert(limit);

	port_sys_lock();

	msg = core_sys_alloc(sizeof(msg_t) + limit * sizeof(unsigned));

	if (msg)
		msg_init(msg, limit, msg + 1);

	port_sys_unlock();

	return msg;
}

/* -------------------------------------------------------------------------- */
void msg_kill( msg_t *msg )
/* -------------------------------------------------------------------------- */
{
	assert(!port_isr_inside());
	assert(msg);

	port_sys_lock();

	msg->count = 0;
	msg->first = 0;
	msg->next  = 0;

	core_all_wakeup(msg, E_STOPPED);

	port_sys_unlock();
}

/* -------------------------------------------------------------------------- */
static
void priv_msg_get( msg_t *msg, unsigned *data )
/* -------------------------------------------------------------------------- */
{
	*data = msg->data[msg->first];

	msg->first = (msg->first + 1) % msg->limit;
	msg->count--;
}

/* -------------------------------------------------------------------------- */
static
void priv_msg_put( msg_t *msg, unsigned data )
/* -------------------------------------------------------------------------- */
{
	msg->data[msg->next] = data;

	msg->next = (msg->next + 1) % msg->limit;
	msg->count++;
}

/* -------------------------------------------------------------------------- */
static
unsigned priv_msg_wait( msg_t *msg, unsigned *data, unsigned time, unsigned(*wait)() )
/* -------------------------------------------------------------------------- */
{
	unsigned event = E_SUCCESS;

	assert(!port_isr_inside() || !time);
	assert(msg);
	assert(data);

	port_sys_lock();

	if (msg->count == 0)
	{
		Current->data = data;

		event = wait(msg, time);
	}
	else
	{
		priv_msg_get(msg, data);

		tsk_t *tsk = core_one_wakeup(msg, E_SUCCESS);

		if (tsk) priv_msg_put(msg, tsk->msg);
	}

	port_sys_unlock();

	return event;
}

/* -------------------------------------------------------------------------- */
unsigned msg_waitUntil( msg_t *msg, unsigned *data, unsigned time )
/* -------------------------------------------------------------------------- */
{
	return priv_msg_wait(msg, data, time, core_tsk_waitUntil);
}

/* -------------------------------------------------------------------------- */
unsigned msg_waitFor( msg_t *msg, unsigned *data, unsigned delay )
/* -------------------------------------------------------------------------- */
{
	return priv_msg_wait(msg, data, delay, core_tsk_waitFor);
}

/* -------------------------------------------------------------------------- */
static
unsigned priv_msg_send( msg_t *msg, unsigned data, unsigned time, unsigned(*wait)() )
/* -------------------------------------------------------------------------- */
{
	unsigned event = E_SUCCESS;

	assert(!port_isr_inside() || !time);
	assert(msg);

	port_sys_lock();

	if (msg->count >= msg->limit)
	{
		Current->msg = data;

		event = wait(msg, time);
	}
	else
	{
		priv_msg_put(msg, data);

		tsk_t *tsk = core_one_wakeup(msg, E_SUCCESS);

		if (tsk) priv_msg_get(msg, tsk->data);
	}

	port_sys_unlock();

	return event;
}

/* -------------------------------------------------------------------------- */
unsigned msg_sendUntil( msg_t *msg, unsigned data, unsigned time )
/* -------------------------------------------------------------------------- */
{
	return priv_msg_send(msg, data, time, core_tsk_waitUntil);
}

/* -------------------------------------------------------------------------- */
unsigned msg_sendFor( msg_t *msg, unsigned data, unsigned delay )
/* -------------------------------------------------------------------------- */
{
	return priv_msg_send(msg, data, delay, core_tsk_waitFor);
}

/* -------------------------------------------------------------------------- */
