/******************************************************************************

    @file    StateOS: oseventqueue.c
    @author  Rajmund Szymanski
    @date    31.08.2018
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

#include "inc/oseventqueue.h"
#include "inc/ostask.h"
#include "inc/oscriticalsection.h"

/* -------------------------------------------------------------------------- */
void evt_init( evt_t *evt, unsigned *data, unsigned bufsize )
/* -------------------------------------------------------------------------- */
{
	assert(!port_isr_context());
	assert(evt);
	assert(data);
	assert(bufsize);

	sys_lock();
	{
		memset(evt, 0, sizeof(evt_t));

		evt->limit = bufsize / sizeof(unsigned);
		evt->data  = data;
	}
	sys_unlock();
}

/* -------------------------------------------------------------------------- */
evt_t *evt_create( unsigned limit )
/* -------------------------------------------------------------------------- */
{
	evt_t  * evt;
	unsigned bufsize;

	assert(!port_isr_context());
	assert(limit);

	sys_lock();
	{
		bufsize = limit * sizeof(unsigned);
		evt = core_sys_alloc(ABOVE(sizeof(evt_t)) + bufsize);
		evt_init(evt, (void *)((size_t)evt + ABOVE(sizeof(evt_t))), bufsize);
		evt->obj.res = evt;
	}
	sys_unlock();

	return evt;
}

/* -------------------------------------------------------------------------- */
void evt_kill( evt_t *evt )
/* -------------------------------------------------------------------------- */
{
	assert(!port_isr_context());
	assert(evt);

	sys_lock();
	{
		evt->count = 0;
		evt->head  = 0;
		evt->tail  = 0;

		core_all_wakeup(&evt->obj.queue, E_STOPPED);
	}
	sys_unlock();
}

/* -------------------------------------------------------------------------- */
void evt_delete( evt_t *evt )
/* -------------------------------------------------------------------------- */
{
	sys_lock();
	{
		evt_kill(evt);
		core_sys_free(evt->obj.res);
	}
	sys_unlock();
}

/* -------------------------------------------------------------------------- */
static
void priv_evt_get( evt_t *evt, unsigned *data )
/* -------------------------------------------------------------------------- */
{
	unsigned i = evt->head;

	*data = evt->data[i++];

	evt->head = (i < evt->limit) ? i : 0;
	evt->count--;
}

/* -------------------------------------------------------------------------- */
static
void priv_evt_put( evt_t *evt, const unsigned data )
/* -------------------------------------------------------------------------- */
{
	unsigned i = evt->tail;

	evt->data[i++] = data;

	evt->tail = (i < evt->limit) ? i : 0;
	evt->count++;
}

/* -------------------------------------------------------------------------- */
static
void priv_evt_skip( evt_t *evt )
/* -------------------------------------------------------------------------- */
{
	evt->count--;
	evt->head++;
	if (evt->head == evt->limit) evt->head = 0;
}

/* -------------------------------------------------------------------------- */
static
void priv_evt_getUpdate( evt_t *evt, unsigned *data )
/* -------------------------------------------------------------------------- */
{
	tsk_t *tsk;

	priv_evt_get(evt, data);
	tsk = core_one_wakeup(&evt->obj.queue, E_SUCCESS);
	if (tsk) priv_evt_put(evt, tsk->tmp.evt.data.out);
}

/* -------------------------------------------------------------------------- */
static
void priv_evt_putUpdate( evt_t *evt, const unsigned data )
/* -------------------------------------------------------------------------- */
{
	tsk_t *tsk;

	priv_evt_put(evt, data);
	tsk = core_one_wakeup(&evt->obj.queue, E_SUCCESS);
	if (tsk) priv_evt_get(evt, tsk->tmp.evt.data.in);
}

/* -------------------------------------------------------------------------- */
unsigned evt_take( evt_t *evt, unsigned *data )
/* -------------------------------------------------------------------------- */
{
	unsigned event;

	assert(evt);

	sys_lock();
	{
		if (evt->count > 0)
		{
			priv_evt_getUpdate(evt, data);
			event = E_SUCCESS;
		}
		else
		{
			event = E_TIMEOUT;
		}
	}
	sys_unlock();

	return event;
}

/* -------------------------------------------------------------------------- */
static
unsigned priv_evt_wait( evt_t *evt, unsigned *data, cnt_t time, unsigned(*wait)(tsk_t**,cnt_t) )
/* -------------------------------------------------------------------------- */
{
	assert(!port_isr_context());
	assert(evt);

	if (evt->count > 0)
	{
		priv_evt_getUpdate(evt, data);
		return E_SUCCESS;
	}

	System.cur->tmp.evt.data.in = data;
	return wait(&evt->obj.queue, time);
}

/* -------------------------------------------------------------------------- */
unsigned evt_waitFor( evt_t *evt, unsigned *data, cnt_t delay )
/* -------------------------------------------------------------------------- */
{
	unsigned event;

	sys_lock();
	{
		event = priv_evt_wait(evt, data, delay, core_tsk_waitFor);
	}
	sys_unlock();

	return event;
}

/* -------------------------------------------------------------------------- */
unsigned evt_waitUntil( evt_t *evt, unsigned *data, cnt_t time )
/* -------------------------------------------------------------------------- */
{
	unsigned event;

	sys_lock();
	{
		event = priv_evt_wait(evt, data, time, core_tsk_waitUntil);
	}
	sys_unlock();

	return event;
}

/* -------------------------------------------------------------------------- */
unsigned evt_give( evt_t *evt, unsigned data )
/* -------------------------------------------------------------------------- */
{
	unsigned event;

	assert(evt);

	sys_lock();
	{
		if (evt->count < evt->limit)
		{
			priv_evt_putUpdate(evt, data);
			event = E_SUCCESS;
		}
		else
		{
			event = E_TIMEOUT;
		}
	}
	sys_unlock();

	return event;
}

/* -------------------------------------------------------------------------- */
static
unsigned priv_evt_send( evt_t *evt, unsigned data, cnt_t time, unsigned(*wait)(tsk_t**,cnt_t) )
/* -------------------------------------------------------------------------- */
{
	assert(!port_isr_context());
	assert(evt);

	if (evt->count < evt->limit)
	{
		priv_evt_putUpdate(evt, data);
		return E_SUCCESS;
	}

	System.cur->tmp.evt.data.out = data;
	return wait(&evt->obj.queue, time);
}

/* -------------------------------------------------------------------------- */
unsigned evt_sendFor( evt_t *evt, unsigned data, cnt_t delay )
/* -------------------------------------------------------------------------- */
{
	unsigned event;

	sys_lock();
	{
		event = priv_evt_send(evt, data, delay, core_tsk_waitFor);
	}
	sys_unlock();

	return event;
}

/* -------------------------------------------------------------------------- */
unsigned evt_sendUntil( evt_t *evt, unsigned data, cnt_t time )
/* -------------------------------------------------------------------------- */
{
	unsigned event;

	sys_lock();
	{
		event = priv_evt_send(evt, data, time, core_tsk_waitUntil);
	}
	sys_unlock();

	return event;
}

/* -------------------------------------------------------------------------- */
unsigned evt_push( evt_t *evt, unsigned data )
/* -------------------------------------------------------------------------- */
{
	unsigned event;

	assert(evt);

	sys_lock();
	{
		if (evt->count == 0 || evt->obj.queue == 0)
		{
			if (evt->count == evt->limit)
				priv_evt_skip(evt);
			priv_evt_putUpdate(evt, data);
			event = E_SUCCESS;
		}
		else
		{
			event = E_TIMEOUT;
		}
	}
	sys_unlock();

	return event;
}

/* -------------------------------------------------------------------------- */
