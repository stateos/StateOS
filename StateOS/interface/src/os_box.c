/******************************************************************************

    @file    StateOS: os_box.c
    @author  Rajmund Szymanski
    @date    28.10.2016
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
box_id box_create( unsigned limit, unsigned size )
/* -------------------------------------------------------------------------- */
{
	box_id box;

	port_sys_lock();

	box = core_sys_alloc(sizeof(box_t) + limit * size);

	if (box)
	{
		box->limit = limit;
		box->size  = size;
		box->data  = (char *)(box + 1);
	}

	port_sys_unlock();

	return box;
}

/* -------------------------------------------------------------------------- */
void box_kill( box_id box )
/* -------------------------------------------------------------------------- */
{
	port_sys_lock();

	box->count = 0;
	box->first = 0;
	box->next  = 0;

	core_all_wakeup(box, E_STOPPED);

	port_sys_unlock();
}

/* -------------------------------------------------------------------------- */
static inline
void priv_box_get( box_id box, void *data )
/* -------------------------------------------------------------------------- */
{
	unsigned i;
	char*buf = box->data + box->size * box->first;

	for (i = 0; i < box->size; i++) ((char*)data)[i] = buf[i];

	box->first = (box->first + 1) % box->limit;
	box->count--;
}

/* -------------------------------------------------------------------------- */
static inline
void priv_box_put( box_id box, void *data )
/* -------------------------------------------------------------------------- */
{
	unsigned i;
	char*buf = box->data + box->size * box->next;

	for (i = 0; i < box->size; i++) buf[i] = ((char*)data)[i];

	box->next = (box->next + 1) % box->limit;
	box->count++;
}

/* -------------------------------------------------------------------------- */
static inline
unsigned priv_box_wait( box_id box, void *data, unsigned time, unsigned(*wait)() )
/* -------------------------------------------------------------------------- */
{
	unsigned event = E_SUCCESS;

	port_sys_lock();

	if (box->count == 0)
	{
		Current->data = data;

		event = wait(box, time);
	}
	else
	{
		priv_box_get(box, data);

		tsk_id tsk = core_one_wakeup(box, E_SUCCESS);

		if (tsk) priv_box_put(box, tsk->data);
	}

	port_sys_unlock();

	return event;
}

/* -------------------------------------------------------------------------- */
unsigned box_waitUntil( box_id box, void *data, unsigned time )
/* -------------------------------------------------------------------------- */
{
	return priv_box_wait(box, data, time, core_tsk_waitUntil);
}

/* -------------------------------------------------------------------------- */
unsigned box_waitFor( box_id box, void *data, unsigned delay )
/* -------------------------------------------------------------------------- */
{
	return priv_box_wait(box, data, delay, core_tsk_waitFor);
}

/* -------------------------------------------------------------------------- */
static inline
unsigned priv_box_send( box_id box, void *data, unsigned time, unsigned(*wait)() )
/* -------------------------------------------------------------------------- */
{
	unsigned event = E_SUCCESS;

	port_sys_lock();

	if (box->count >= box->limit)
	{
		Current->data = data;

		event = wait(box, time);
	}
	else
	{
		priv_box_put(box, data);

		tsk_id tsk = core_one_wakeup(box, E_SUCCESS);

		if (tsk) priv_box_get(box, tsk->data);
	}

	port_sys_unlock();

	return event;
}

/* -------------------------------------------------------------------------- */
unsigned box_sendUntil( box_id box, void *data, unsigned time )
/* -------------------------------------------------------------------------- */
{
	return priv_box_send(box, data, time, core_tsk_waitUntil);
}

/* -------------------------------------------------------------------------- */
unsigned box_sendFor( box_id box, void *data, unsigned delay )
/* -------------------------------------------------------------------------- */
{
	return priv_box_send(box, data, delay, core_tsk_waitFor);
}

/* -------------------------------------------------------------------------- */
