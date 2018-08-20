/******************************************************************************

    @file    StateOS: oseventqueue.c
    @author  Rajmund Szymanski
    @date    20.08.2018
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
void evq_init( evq_t *evq, unsigned limit, unsigned *data )
/* -------------------------------------------------------------------------- */
{
	assert(!port_isr_inside());
	assert(evq);
	assert(limit);
	assert(data);

	sys_lock();
	{
		memset(evq, 0, sizeof(evq_t));

		evq->limit = limit;
		evq->data  = data;
	}
	sys_unlock();
}

/* -------------------------------------------------------------------------- */
evq_t *evq_create( unsigned limit )
/* -------------------------------------------------------------------------- */
{
	evq_t *evq;

	assert(!port_isr_inside());
	assert(limit);

	sys_lock();
	{
		evq = core_sys_alloc(ABOVE(sizeof(evq_t)) + limit * sizeof(unsigned));
		evq_init(evq, limit, (void *)((size_t)evq + ABOVE(sizeof(evq_t))));
		evq->res = evq;
	}
	sys_unlock();

	return evq;
}

/* -------------------------------------------------------------------------- */
void evq_kill( evq_t *evq )
/* -------------------------------------------------------------------------- */
{
	assert(!port_isr_inside());
	assert(evq);

	sys_lock();
	{
		evq->count = 0;
		evq->head  = 0;
		evq->tail  = 0;

		core_all_wakeup(evq, E_STOPPED);
	}
	sys_unlock();
}

/* -------------------------------------------------------------------------- */
void evq_delete( evq_t *evq )
/* -------------------------------------------------------------------------- */
{
	sys_lock();
	{
		evq_kill(evq);
		core_sys_free(evq->res);
	}
	sys_unlock();
}

/* -------------------------------------------------------------------------- */
static
unsigned priv_evq_get( evq_t *evq )
/* -------------------------------------------------------------------------- */
{
	unsigned event;
	unsigned i = evq->head;

	event = evq->data[i++];

	evq->head = (i < evq->limit) ? i : 0;
	evq->count--;

	return event;
}

/* -------------------------------------------------------------------------- */
static
void priv_evq_put( evq_t *evq, unsigned event )
/* -------------------------------------------------------------------------- */
{
	unsigned i = evq->tail;

	evq->data[i++] = event;

	evq->tail = (i < evq->limit) ? i : 0;
	evq->count++;
}

/* -------------------------------------------------------------------------- */
static
void priv_evq_skip( evq_t *evq )
/* -------------------------------------------------------------------------- */
{
	evq->count--;
	evq->head++;
	if (evq->head == evq->limit) evq->head = 0;
}

/* -------------------------------------------------------------------------- */
static
unsigned priv_evq_getUpdate( evq_t *evq )
/* -------------------------------------------------------------------------- */
{
	tsk_t   *tsk;
	unsigned event;

	event = priv_evq_get(evq);
	tsk = core_one_wakeup(evq, E_SUCCESS);
	if (tsk) priv_evq_put(evq, tsk->tmp.evq.event);

	return event;
}

/* -------------------------------------------------------------------------- */
static
void priv_evq_putUpdate( evq_t *evq, const unsigned data )
/* -------------------------------------------------------------------------- */
{
	priv_evq_put(evq, data);
	if (evq->queue)
		core_one_wakeup(evq, priv_evq_get(evq));
}

/* -------------------------------------------------------------------------- */
unsigned evq_take( evq_t *evq )
/* -------------------------------------------------------------------------- */
{
	unsigned event = E_TIMEOUT;

	assert(evq);

	sys_lock();
	{
		if (evq->count > 0)
		{
			event = priv_evq_getUpdate(evq);
		}
	}
	sys_unlock();

	return event;
}

/* -------------------------------------------------------------------------- */
static
unsigned priv_evq_wait( evq_t *evq, cnt_t time, unsigned(*wait)(void*,cnt_t) )
/* -------------------------------------------------------------------------- */
{
	unsigned event;

	assert(!port_isr_inside());
	assert(evq);

	sys_lock();
	{
		if (evq->count > 0)
		{
			event = priv_evq_getUpdate(evq);
		}
		else
		{
			event = wait(evq, time);
		}
	}
	sys_unlock();

	return event;
}

/* -------------------------------------------------------------------------- */
unsigned evq_waitFor( evq_t *evq, cnt_t delay )
/* -------------------------------------------------------------------------- */
{
	return priv_evq_wait(evq, delay, core_tsk_waitFor);
}

/* -------------------------------------------------------------------------- */
unsigned evq_waitUntil( evq_t *evq, cnt_t time )
/* -------------------------------------------------------------------------- */
{
	return priv_evq_wait(evq, time, core_tsk_waitUntil);
}

/* -------------------------------------------------------------------------- */
unsigned evq_give( evq_t *evq, unsigned data )
/* -------------------------------------------------------------------------- */
{
	unsigned event = E_TIMEOUT;

	assert(evq);

	sys_lock();
	{
		if (evq->count < evq->limit)
		{
			priv_evq_putUpdate(evq, data);
			event = E_SUCCESS;
		}
	}
	sys_unlock();

	return event;
}

/* -------------------------------------------------------------------------- */
static
unsigned priv_evq_send( evq_t *evq, unsigned data, cnt_t time, unsigned(*wait)(void*,cnt_t) )
/* -------------------------------------------------------------------------- */
{
	unsigned event;

	assert(!port_isr_inside());
	assert(evq);

	sys_lock();
	{
		if (evq->count < evq->limit)
		{
			priv_evq_putUpdate(evq, data);
			event = E_SUCCESS;
		}
		else
		{
			System.cur->tmp.evq.event = data;
			event = wait(evq, time);
		}
	}
	sys_unlock();

	return event;
}

/* -------------------------------------------------------------------------- */
unsigned evq_sendFor( evq_t *evq, unsigned data, cnt_t delay )
/* -------------------------------------------------------------------------- */
{
	return priv_evq_send(evq, data, delay, core_tsk_waitFor);
}

/* -------------------------------------------------------------------------- */
unsigned evq_sendUntil( evq_t *evq, unsigned data, cnt_t time )
/* -------------------------------------------------------------------------- */
{
	return priv_evq_send(evq, data, time, core_tsk_waitUntil);
}

/* -------------------------------------------------------------------------- */
unsigned evq_push( evq_t *evq, unsigned data )
/* -------------------------------------------------------------------------- */
{
	unsigned event = E_TIMEOUT;

	assert(evq);

	sys_lock();
	{
		if (evq->count == 0 || evq->queue == 0)
		{
			if (evq->count == evq->limit)
				priv_evq_skip(evq);
			priv_evq_putUpdate(evq, data);
			event = E_SUCCESS;
		}
	}
	sys_unlock();

	return event;
}

/* -------------------------------------------------------------------------- */
