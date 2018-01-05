/******************************************************************************

    @file    StateOS: os_box.c
    @author  Rajmund Szymanski
    @date    01.01.2018
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

#include "inc/os_box.h"
#include "inc/os_tsk.h"

/* -------------------------------------------------------------------------- */
void box_init( box_t *box, unsigned limit, unsigned size, void *data )
/* -------------------------------------------------------------------------- */
{
	assert(!port_isr_inside());
	assert(box);
	assert(limit);
	assert(size);
	assert(data);

	port_sys_lock();

	memset(box, 0, sizeof(box_t));
	
	box->limit = limit;
	box->size  = size;
	box->data  = data;

	port_sys_unlock();
}

/* -------------------------------------------------------------------------- */
box_t *box_create( unsigned limit, unsigned size )
/* -------------------------------------------------------------------------- */
{
	box_t *box;

	assert(!port_isr_inside());
	assert(limit);
	assert(size);

	port_sys_lock();

	box = core_sys_alloc(ABOVE(sizeof(box_t)) + limit * size);
	box_init(box, limit, size, (void *)((size_t)box + ABOVE(sizeof(box_t))));
	box->res = box;

	port_sys_unlock();

	return box;
}

/* -------------------------------------------------------------------------- */
void box_kill( box_t *box )
/* -------------------------------------------------------------------------- */
{
	assert(!port_isr_inside());
	assert(box);

	port_sys_lock();

	box->count = 0;
	box->first = 0;
	box->next  = 0;

	core_all_wakeup(box, E_STOPPED);

	port_sys_unlock();
}

/* -------------------------------------------------------------------------- */
void box_delete( box_t *box )
/* -------------------------------------------------------------------------- */
{
	port_sys_lock();

	box_kill(box);
	core_sys_free(box->res);

	port_sys_unlock();
}

/* -------------------------------------------------------------------------- */
static
void priv_box_get( box_t *box, void *data )
/* -------------------------------------------------------------------------- */
{
	unsigned i;
	char*buf = box->data + box->size * box->first;

	for (i = 0; i < box->size; i++) ((char*)data)[i] = buf[i];

	box->first = (box->first + 1) % box->limit;
	box->count--;
}

/* -------------------------------------------------------------------------- */
static
void priv_box_put( box_t *box, void *data )
/* -------------------------------------------------------------------------- */
{
	unsigned i;
	char*buf = box->data + box->size * box->next;

	for (i = 0; i < box->size; i++) buf[i] = ((char*)data)[i];

	box->next = (box->next + 1) % box->limit;
	box->count++;
}

/* -------------------------------------------------------------------------- */
static
unsigned priv_box_wait( box_t *box, void *data, cnt_t time, unsigned(*wait)(void*,cnt_t) )
/* -------------------------------------------------------------------------- */
{
	tsk_t  * tsk;
	unsigned event = E_SUCCESS;

	assert(box);
	assert(data);

	port_sys_lock();

	if (box->count == 0)
	{
		System.cur->tmp.data = data;

		event = wait(box, time);
	}
	else
	{
		priv_box_get(box, data);

		tsk = core_one_wakeup(box, E_SUCCESS);

		if (tsk) priv_box_put(box, tsk->tmp.data);
	}

	port_sys_unlock();

	return event;
}

/* -------------------------------------------------------------------------- */
unsigned box_waitUntil( box_t *box, void *data, cnt_t time )
/* -------------------------------------------------------------------------- */
{
	assert(!port_isr_inside());

	return priv_box_wait(box, data, time, core_tsk_waitUntil);
}

/* -------------------------------------------------------------------------- */
unsigned box_waitFor( box_t *box, void *data, cnt_t delay )
/* -------------------------------------------------------------------------- */
{
	assert(!port_isr_inside() || !delay);

	return priv_box_wait(box, data, delay, core_tsk_waitFor);
}

/* -------------------------------------------------------------------------- */
static
unsigned priv_box_send( box_t *box, void *data, cnt_t time, unsigned(*wait)(void*,cnt_t) )
/* -------------------------------------------------------------------------- */
{
	tsk_t  * tsk;
	unsigned event = E_SUCCESS;

	assert(box);
	assert(data);

	port_sys_lock();

	if (box->count >= box->limit)
	{
		System.cur->tmp.data = data;

		event = wait(box, time);
	}
	else
	{
		priv_box_put(box, data);

		tsk = core_one_wakeup(box, E_SUCCESS);

		if (tsk) priv_box_get(box, tsk->tmp.data);
	}

	port_sys_unlock();

	return event;
}

/* -------------------------------------------------------------------------- */
unsigned box_sendUntil( box_t *box, const void *data, cnt_t time )
/* -------------------------------------------------------------------------- */
{
	assert(!port_isr_inside());

	return priv_box_send(box, (void*)data, time, core_tsk_waitUntil);
}

/* -------------------------------------------------------------------------- */
unsigned box_sendFor( box_t *box, const void *data, cnt_t delay )
/* -------------------------------------------------------------------------- */
{
	assert(!port_isr_inside() || !delay);

	return priv_box_send(box, (void*)data, delay, core_tsk_waitFor);
}

/* -------------------------------------------------------------------------- */
