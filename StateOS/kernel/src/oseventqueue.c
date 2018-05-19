/******************************************************************************

    @file    StateOS: oseventqueue.c
    @author  Rajmund Szymanski
    @date    19.05.2018
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

/* -------------------------------------------------------------------------- */
void evq_init( evq_t *evq, unsigned limit, unsigned *data )
/* -------------------------------------------------------------------------- */
{
	assert(!port_isr_inside());
	assert(evq);
	assert(limit);
	assert(data);

	port_sys_lock();

	memset(evq, 0, sizeof(evq_t));

	evq->limit = limit;
	evq->data  = data;

	port_sys_unlock();
}

/* -------------------------------------------------------------------------- */
evq_t *evq_create( unsigned limit )
/* -------------------------------------------------------------------------- */
{
	evq_t *evq;

	assert(!port_isr_inside());
	assert(limit);

	port_sys_lock();

	evq = core_sys_alloc(ABOVE(sizeof(evq_t)) + limit * sizeof(unsigned));
	evq_init(evq, limit, (void *)((size_t)evq + ABOVE(sizeof(evq_t))));
	evq->res = evq;

	port_sys_unlock();

	return evq;
}

/* -------------------------------------------------------------------------- */
void evq_kill( evq_t *evq )
/* -------------------------------------------------------------------------- */
{
	assert(!port_isr_inside());
	assert(evq);

	port_sys_lock();

	evq->count = 0;
	evq->head  = 0;
	evq->tail  = 0;

	core_all_wakeup(evq, E_STOPPED);

	port_sys_unlock();
}

/* -------------------------------------------------------------------------- */
void evq_delete( evq_t *evq )
/* -------------------------------------------------------------------------- */
{
	port_sys_lock();

	evq_kill(evq);
	core_sys_free(evq->res);

	port_sys_unlock();
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
unsigned evq_take( evq_t *evq )
/* -------------------------------------------------------------------------- */
{
	tsk_t  * tsk;
	unsigned event = E_TIMEOUT;

	assert(evq);

	port_sys_lock();

	if (evq->count > 0)
	{
		event = priv_evq_get(evq);
		tsk = core_one_wakeup(evq, E_SUCCESS);
		if (tsk) priv_evq_put(evq, tsk->tmp.evq.event);
	}

	port_sys_unlock();

	return event;
}

/* -------------------------------------------------------------------------- */
static
unsigned priv_evq_wait( evq_t *evq, cnt_t time, unsigned(*wait)(void*,cnt_t) )
/* -------------------------------------------------------------------------- */
{
	tsk_t  * tsk;
	unsigned event;

	assert(!port_isr_inside());
	assert(evq);

	port_sys_lock();

	if (evq->count > 0)
	{
		event = priv_evq_get(evq);
		tsk = core_one_wakeup(evq, E_SUCCESS);
		if (tsk) priv_evq_put(evq, tsk->tmp.evq.event);
	}
	else
	{
		event = wait(evq, time);
	}

	port_sys_unlock();

	return event;
}

/* -------------------------------------------------------------------------- */
unsigned evq_waitUntil( evq_t *evq, cnt_t time )
/* -------------------------------------------------------------------------- */
{
	return priv_evq_wait(evq, time, core_tsk_waitUntil);
}

/* -------------------------------------------------------------------------- */
unsigned evq_waitFor( evq_t *evq, cnt_t delay )
/* -------------------------------------------------------------------------- */
{
	return priv_evq_wait(evq, delay, core_tsk_waitFor);
}

/* -------------------------------------------------------------------------- */
unsigned evq_give( evq_t *evq, unsigned data )
/* -------------------------------------------------------------------------- */
{
	unsigned event = E_TIMEOUT;

	assert(evq);

	port_sys_lock();

	if (evq->count < evq->limit)
	{
		priv_evq_put(evq, data);
		if (evq->queue)
			core_one_wakeup(evq, priv_evq_get(evq));
		event = E_SUCCESS;
	}

	port_sys_unlock();

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

	port_sys_lock();

	if (evq->count < evq->limit)
	{
		priv_evq_put(evq, data);
		if (evq->queue)
			core_one_wakeup(evq, priv_evq_get(evq));
		event = E_SUCCESS;
	}
	else
	{
		event = wait(evq, time);
	}

	port_sys_unlock();

	return event;
}

/* -------------------------------------------------------------------------- */
unsigned evq_sendUntil( evq_t *evq, unsigned data, cnt_t time )
/* -------------------------------------------------------------------------- */
{
	return priv_evq_send(evq, data, time, core_tsk_waitUntil);
}

/* -------------------------------------------------------------------------- */
unsigned evq_sendFor( evq_t *evq, unsigned data, cnt_t delay )
/* -------------------------------------------------------------------------- */
{
	return priv_evq_send(evq, data, delay, core_tsk_waitFor);
}

/* -------------------------------------------------------------------------- */
void evq_push( evq_t *evq, unsigned data )
/* -------------------------------------------------------------------------- */
{
	assert(evq);

	port_sys_lock();

	priv_evq_put(evq, data);

	if (evq->count > evq->limit)
	{
		evq->count = evq->limit;
		evq->head = evq->tail;
	}
	else
	{
		if (evq->queue)
			core_one_wakeup(evq, priv_evq_get(evq));
	}

	port_sys_unlock();
}

/* -------------------------------------------------------------------------- */
