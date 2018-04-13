/******************************************************************************

    @file    StateOS: os_box.c
    @author  Rajmund Szymanski
    @date    13.04.2018
    @brief   This file provides set of functions for StateOS.

 ******************************************************************************

   Copyright (c) 2018 Rajmund Szymanski. All rights reserved.

   Permission is hereby granted, free of charge, to any person obtaining a copy
   of this software and associated documentation files (the "Software"), to
   deal in the Software without restriction, including without limitation the
   rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
   sell copies of the Software, and to permit persons to whom the Software is
   furnished to do so, subject to the following conditions:

   The above copyright notice and this permission notice shall be included
   in all copies or substantial portions of the Software.

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
   OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
   THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
   FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
   IN THE SOFTWARE.

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
void priv_box_get( box_t *box, char *data )
/* -------------------------------------------------------------------------- */
{
	unsigned i;
	char   * buf = box->data + box->size * box->first;

	for (i = 0; i < box->size; i++) data[i] = buf[i];

	box->first = (box->first + 1) % box->limit;
	box->count--;
}

/* -------------------------------------------------------------------------- */
static
void priv_box_put( box_t *box, char *data )
/* -------------------------------------------------------------------------- */
{
	unsigned i;
	char   * buf = box->data + box->size * box->next;

	for (i = 0; i < box->size; i++) buf[i] = data[i];

	box->next = (box->next + 1) % box->limit;
	box->count++;
}

/* -------------------------------------------------------------------------- */
unsigned box_take( box_t *box, void *data )
/* -------------------------------------------------------------------------- */
{
	tsk_t  * tsk;
	unsigned event = E_TIMEOUT;

	assert(box);
	assert(data);

	port_sys_lock();

	if (box->count > 0)
	{
		priv_box_get(box, data);
		tsk = core_one_wakeup(box, E_SUCCESS);
		if (tsk) priv_box_put(box, tsk->tmp.data);
		event = E_SUCCESS;
	}

	port_sys_unlock();

	return event;
}

/* -------------------------------------------------------------------------- */
static
unsigned priv_box_wait( box_t *box, void *data, cnt_t time, unsigned(*wait)(void*,cnt_t) )
/* -------------------------------------------------------------------------- */
{
	tsk_t  * tsk;
	unsigned event = E_SUCCESS;

	assert(!port_isr_inside());
	assert(box);
	assert(data);

	port_sys_lock();

	if (box->count > 0)
	{
		priv_box_get(box, data);
		tsk = core_one_wakeup(box, E_SUCCESS);
		if (tsk) priv_box_put(box, tsk->tmp.data);
	}
	else
	{
		System.cur->tmp.data = data;
		event = wait(box, time);
	}

	port_sys_unlock();

	return event;
}

/* -------------------------------------------------------------------------- */
unsigned box_waitUntil( box_t *box, void *data, cnt_t time )
/* -------------------------------------------------------------------------- */
{
	return priv_box_wait(box, data, time, core_tsk_waitUntil);
}

/* -------------------------------------------------------------------------- */
unsigned box_waitFor( box_t *box, void *data, cnt_t delay )
/* -------------------------------------------------------------------------- */
{
	return priv_box_wait(box, data, delay, core_tsk_waitFor);
}

/* -------------------------------------------------------------------------- */
unsigned box_give( box_t *box, void *data )
/* -------------------------------------------------------------------------- */
{
	tsk_t  * tsk;
	unsigned event = E_TIMEOUT;

	assert(box);
	assert(data);

	port_sys_lock();

	if (box->count < box->limit)
	{
		priv_box_put(box, data);
		tsk = core_one_wakeup(box, E_SUCCESS);
		if (tsk) priv_box_get(box, tsk->tmp.data);
		event = E_SUCCESS;
	}

	port_sys_unlock();

	return event;
}

/* -------------------------------------------------------------------------- */
static
unsigned priv_box_send( box_t *box, void *data, cnt_t time, unsigned(*wait)(void*,cnt_t) )
/* -------------------------------------------------------------------------- */
{
	tsk_t  * tsk;
	unsigned event = E_SUCCESS;

	assert(!port_isr_inside());
	assert(box);
	assert(data);

	port_sys_lock();

	if (box->count < box->limit)
	{
		priv_box_put(box, data);
		tsk = core_one_wakeup(box, E_SUCCESS);
		if (tsk) priv_box_get(box, tsk->tmp.data);
	}
	else
	{
		System.cur->tmp.data = data;
		event = wait(box, time);
	}

	port_sys_unlock();

	return event;
}

/* -------------------------------------------------------------------------- */
unsigned box_sendUntil( box_t *box, void *data, cnt_t time )
/* -------------------------------------------------------------------------- */
{
	return priv_box_send(box, data, time, core_tsk_waitUntil);
}

/* -------------------------------------------------------------------------- */
unsigned box_sendFor( box_t *box, void *data, cnt_t delay )
/* -------------------------------------------------------------------------- */
{
	return priv_box_send(box, data, delay, core_tsk_waitFor);
}

/* -------------------------------------------------------------------------- */
void box_push( box_t *box, void *data )
/* -------------------------------------------------------------------------- */
{
	tsk_t *tsk;

	assert(box);
	assert(data);

	port_sys_lock();

	priv_box_put(box, data);

	if (box->count > box->limit)
	{
		box->count = box->limit;
		box->first = box->next;
	}
	else
	{
		tsk = core_one_wakeup(box, E_SUCCESS);
		if (tsk) priv_box_get(box, tsk->tmp.data);
	}

	port_sys_unlock();
}

/* -------------------------------------------------------------------------- */
