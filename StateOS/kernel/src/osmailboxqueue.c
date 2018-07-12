/******************************************************************************

    @file    StateOS: osmailboxqueue.c
    @author  Rajmund Szymanski
    @date    11.07.2018
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

#include "inc/osmailboxqueue.h"
#include "inc/ostask.h"

/* -------------------------------------------------------------------------- */
void box_init( box_t *box, unsigned limit, void *data, unsigned size )
/* -------------------------------------------------------------------------- */
{
	assert(!port_isr_inside());
	assert(box);
	assert(limit);
	assert(data);
	assert(size);

	core_sys_lock();

	memset(box, 0, sizeof(box_t));
	
	box->limit = limit * size;
	box->data  = data;
	box->size  = size;

	core_sys_unlock();
}

/* -------------------------------------------------------------------------- */
box_t *box_create( unsigned limit, unsigned size )
/* -------------------------------------------------------------------------- */
{
	box_t *box;

	assert(!port_isr_inside());
	assert(limit);
	assert(size);

	core_sys_lock();

	box = core_sys_alloc(ABOVE(sizeof(box_t)) + limit * size);
	box_init(box, limit, (void *)((size_t)box + ABOVE(sizeof(box_t))), size);
	box->res = box;

	core_sys_unlock();

	return box;
}

/* -------------------------------------------------------------------------- */
void box_kill( box_t *box )
/* -------------------------------------------------------------------------- */
{
	assert(!port_isr_inside());
	assert(box);

	core_sys_lock();

	box->count = 0;
	box->head  = 0;
	box->tail  = 0;

	core_all_wakeup(box, E_STOPPED);

	core_sys_unlock();
}

/* -------------------------------------------------------------------------- */
void box_delete( box_t *box )
/* -------------------------------------------------------------------------- */
{
	core_sys_lock();

	box_kill(box);
	core_sys_free(box->res);

	core_sys_unlock();
}

/* -------------------------------------------------------------------------- */
static
unsigned priv_box_count( box_t *box )
/* -------------------------------------------------------------------------- */
{
	return box->count / box->size;
}

/* -------------------------------------------------------------------------- */
static
unsigned priv_box_space( box_t *box )
/* -------------------------------------------------------------------------- */
{
	return (box->limit - box->count) / box->size;
}

/* -------------------------------------------------------------------------- */
static
void priv_box_skip( box_t *box )
/* -------------------------------------------------------------------------- */
{
	box->count -= box->size;
	box->head  += box->size;
	if (box->head == box->limit) box->head = 0;
}

/* -------------------------------------------------------------------------- */
static
void priv_box_get( box_t *box, char *data )
/* -------------------------------------------------------------------------- */
{
	unsigned i = box->head;
	unsigned j = 0;

	do data[j++] = box->data[i++]; while (j < box->size);

	box->head = (i < box->limit) ? i : 0;
	box->count -= j;
}

/* -------------------------------------------------------------------------- */
static
void priv_box_put( box_t *box, const char *data )
/* -------------------------------------------------------------------------- */
{
	unsigned i = box->tail;
	unsigned j = 0;

	do box->data[i++] = data[j++]; while (j < box->size);

	box->tail = (i < box->limit) ? i : 0;
	box->count += j;
}

/* -------------------------------------------------------------------------- */
static
void priv_box_getUpdate( box_t *box, char *data )
/* -------------------------------------------------------------------------- */
{
	tsk_t *tsk;

	priv_box_get(box, data);
	tsk = core_one_wakeup(box, E_SUCCESS);
	if (tsk) priv_box_put(box, tsk->tmp.box.data.out);
}

/* -------------------------------------------------------------------------- */
static
void priv_box_putUpdate( box_t *box, const char *data )
/* -------------------------------------------------------------------------- */
{
	tsk_t *tsk;

	priv_box_put(box, data);
	tsk = core_one_wakeup(box, E_SUCCESS);
	if (tsk) priv_box_get(box, tsk->tmp.box.data.in);
}

/* -------------------------------------------------------------------------- */
unsigned box_take( box_t *box, void *data )
/* -------------------------------------------------------------------------- */
{
	unsigned event = E_TIMEOUT;

	assert(box);
	assert(data);

	core_sys_lock();

	if (box->count > 0)
	{
		priv_box_getUpdate(box, data);
		event = E_SUCCESS;
	}

	core_sys_unlock();

	return event;
}

/* -------------------------------------------------------------------------- */
static
unsigned priv_box_wait( box_t *box, void *data, cnt_t time, unsigned(*wait)(void*,cnt_t) )
/* -------------------------------------------------------------------------- */
{
	unsigned event;

	assert(!port_isr_inside());
	assert(box);
	assert(data);

	core_sys_lock();

	if (box->count > 0)
	{
		priv_box_getUpdate(box, data);
		event = E_SUCCESS;
	}
	else
	{
		System.cur->tmp.box.data.in = data;
		event = wait(box, time);
	}

	core_sys_unlock();

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
unsigned box_give( box_t *box, const void *data )
/* -------------------------------------------------------------------------- */
{
	unsigned event = E_TIMEOUT;

	assert(box);
	assert(data);

	core_sys_lock();

	if (box->count < box->limit)
	{
		priv_box_putUpdate(box, data);
		event = E_SUCCESS;
	}

	core_sys_unlock();

	return event;
}

/* -------------------------------------------------------------------------- */
static
unsigned priv_box_send( box_t *box, const void *data, cnt_t time, unsigned(*wait)(void*,cnt_t) )
/* -------------------------------------------------------------------------- */
{
	unsigned event;

	assert(!port_isr_inside());
	assert(box);
	assert(data);

	core_sys_lock();

	if (box->count < box->limit)
	{
		priv_box_putUpdate(box, data);
		event = E_SUCCESS;
	}
	else
	{
		System.cur->tmp.box.data.out = data;
		event = wait(box, time);
	}

	core_sys_unlock();

	return event;
}

/* -------------------------------------------------------------------------- */
unsigned box_sendUntil( box_t *box, const void *data, cnt_t time )
/* -------------------------------------------------------------------------- */
{
	return priv_box_send(box, data, time, core_tsk_waitUntil);
}

/* -------------------------------------------------------------------------- */
unsigned box_sendFor( box_t *box, const void *data, cnt_t delay )
/* -------------------------------------------------------------------------- */
{
	return priv_box_send(box, data, delay, core_tsk_waitFor);
}

/* -------------------------------------------------------------------------- */
unsigned box_push( box_t *box, const void *data )
/* -------------------------------------------------------------------------- */
{
	unsigned event = E_TIMEOUT;

	assert(box);
	assert(data);

	core_sys_lock();

	if (box->count == 0 || box->queue == 0)
	{
		if (box->count == box->limit)
			priv_box_skip(box);
		priv_box_putUpdate(box, data);
		event = E_SUCCESS;
	}

	core_sys_unlock();

	return event;
}

/* -------------------------------------------------------------------------- */
unsigned box_count( box_t *box )
/* -------------------------------------------------------------------------- */
{
	unsigned cnt;

	assert(box);

	core_sys_lock();

	cnt = priv_box_count(box);

	core_sys_unlock();

	return cnt;
}

/* -------------------------------------------------------------------------- */
unsigned box_space( box_t *box )
/* -------------------------------------------------------------------------- */
{
	unsigned cnt;

	assert(box);

	core_sys_lock();

	cnt = priv_box_space(box);

	core_sys_unlock();

	return cnt;
}

/* -------------------------------------------------------------------------- */
