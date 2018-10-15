/******************************************************************************

    @file    StateOS: oseventqueue.c
    @author  Rajmund Szymanski
    @date    15.10.2018
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
#include "osalloc.h"

/* -------------------------------------------------------------------------- */
void evq_init( evq_t *evq, unsigned *data, unsigned bufsize )
/* -------------------------------------------------------------------------- */
{
	assert_tsk_context();
	assert(evq);
	assert(data);
	assert(bufsize);

	sys_lock();
	{
		memset(evq, 0, sizeof(evq_t));

		core_obj_init(&evq->obj);

		evq->limit = bufsize / sizeof(unsigned);
		evq->data  = data;
	}
	sys_unlock();
}

/* -------------------------------------------------------------------------- */
evq_t *evq_create( unsigned limit )
/* -------------------------------------------------------------------------- */
{
	evq_t  * evq;
	unsigned bufsize;

	assert_tsk_context();
	assert(limit);

	sys_lock();
	{
		bufsize = limit * sizeof(unsigned);
		evq = sys_alloc(SEG_OVER(sizeof(evq_t)) + bufsize);
		evq_init(evq, (void *)((size_t)evq + SEG_OVER(sizeof(evq_t))), bufsize);
		evq->obj.res = evq;
	}
	sys_unlock();

	return evq;
}

/* -------------------------------------------------------------------------- */
void evq_reset( evq_t *evq )
/* -------------------------------------------------------------------------- */
{
	assert_tsk_context();
	assert(evq);
	assert(evq->obj.res!=RELEASED);

	sys_lock();
	{
		evq->count = 0;
		evq->head  = 0;
		evq->tail  = 0;

		core_all_wakeup(evq->obj.queue, E_STOPPED);
	}
	sys_unlock();
}

/* -------------------------------------------------------------------------- */
void evq_delete( evq_t *evq )
/* -------------------------------------------------------------------------- */
{
	assert_tsk_context();
	assert(evq);
	assert(evq->obj.res!=RELEASED);

	sys_lock();
	{
		evq_reset(evq);
		core_res_free(&evq->obj.res);
	}
	sys_unlock();
}

/* -------------------------------------------------------------------------- */
static
void priv_evq_get( evq_t *evq, unsigned *data )
/* -------------------------------------------------------------------------- */
{
	unsigned i = evq->head;

	*data = evq->data[i++];

	evq->head = (i < evq->limit) ? i : 0;
	evq->count--;
}

/* -------------------------------------------------------------------------- */
static
void priv_evq_put( evq_t *evq, const unsigned data )
/* -------------------------------------------------------------------------- */
{
	unsigned i = evq->tail;

	evq->data[i++] = data;

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
void priv_evq_getUpdate( evq_t *evq, unsigned *data )
/* -------------------------------------------------------------------------- */
{
	tsk_t *tsk;

	priv_evq_get(evq, data);
	tsk = core_one_wakeup(evq->obj.queue, E_SUCCESS);
	if (tsk) priv_evq_put(evq, tsk->tmp.evq.data.out);
}

/* -------------------------------------------------------------------------- */
static
void priv_evq_putUpdate( evq_t *evq, const unsigned data )
/* -------------------------------------------------------------------------- */
{
	tsk_t *tsk;

	priv_evq_put(evq, data);
	tsk = core_one_wakeup(evq->obj.queue, E_SUCCESS);
	if (tsk) priv_evq_get(evq, tsk->tmp.evq.data.in);
}

/* -------------------------------------------------------------------------- */
static
void priv_evq_skipUpdate( evq_t *evq )
/* -------------------------------------------------------------------------- */
{
	tsk_t *tsk;

	while (evq->count == evq->limit)
	{
		priv_evq_skip(evq);
		tsk = core_one_wakeup(evq->obj.queue, E_SUCCESS);
		if (tsk) priv_evq_put(evq, tsk->tmp.evq.data.out);
	}
}

/* -------------------------------------------------------------------------- */
static
unsigned priv_evq_take( evq_t *evq, unsigned *data )
/* -------------------------------------------------------------------------- */
{
	if (evq->count > 0)
	{
		priv_evq_getUpdate(evq, data);
		return E_SUCCESS;
	}

	return E_TIMEOUT;
}

/* -------------------------------------------------------------------------- */
unsigned evq_take( evq_t *evq, unsigned *data )
/* -------------------------------------------------------------------------- */
{
	unsigned event;

	assert(evq);
	assert(evq->obj.res!=RELEASED);
	assert(evq->data);
	assert(evq->limit);

	sys_lock();
	{
		event = priv_evq_take(evq, data);
	}
	sys_unlock();

	return event;
}

/* -------------------------------------------------------------------------- */
unsigned evq_waitFor( evq_t *evq, unsigned *data, cnt_t delay )
/* -------------------------------------------------------------------------- */
{
	unsigned event;

	assert_tsk_context();
	assert(evq);
	assert(evq->obj.res!=RELEASED);
	assert(evq->data);
	assert(evq->limit);

	sys_lock();
	{
		event = priv_evq_take(evq, data);

		if (event == E_TIMEOUT)
		{
			System.cur->tmp.evq.data.in = data;
			event = core_tsk_waitFor(&evq->obj.queue, delay);
		}
	}
	sys_unlock();

	return event;
}

/* -------------------------------------------------------------------------- */
unsigned evq_waitUntil( evq_t *evq, unsigned *data, cnt_t time )
/* -------------------------------------------------------------------------- */
{
	unsigned event;

	assert_tsk_context();
	assert(evq);
	assert(evq->obj.res!=RELEASED);
	assert(evq->data);
	assert(evq->limit);

	sys_lock();
	{
		event = priv_evq_take(evq, data);

		if (event == E_TIMEOUT)
		{
			System.cur->tmp.evq.data.in = data;
			event = core_tsk_waitUntil(&evq->obj.queue, time);
		}
	}
	sys_unlock();

	return event;
}

/* -------------------------------------------------------------------------- */
static
unsigned priv_evq_give( evq_t *evq, unsigned data )
/* -------------------------------------------------------------------------- */
{
	if (evq->count < evq->limit)
	{
		priv_evq_putUpdate(evq, data);
		return E_SUCCESS;
	}

	return E_TIMEOUT;
}

/* -------------------------------------------------------------------------- */
unsigned evq_give( evq_t *evq, unsigned data )
/* -------------------------------------------------------------------------- */
{
	unsigned event;

	assert(evq);
	assert(evq->obj.res!=RELEASED);
	assert(evq->data);
	assert(evq->limit);

	sys_lock();
	{
		event = priv_evq_give(evq, data);
	}
	sys_unlock();

	return event;
}

/* -------------------------------------------------------------------------- */
unsigned evq_sendFor( evq_t *evq, unsigned data, cnt_t delay )
/* -------------------------------------------------------------------------- */
{
	unsigned event;

	assert_tsk_context();
	assert(evq);
	assert(evq->obj.res!=RELEASED);
	assert(evq->data);
	assert(evq->limit);

	sys_lock();
	{
		event = priv_evq_give(evq, data);

		if (event == E_TIMEOUT)
		{
			System.cur->tmp.evq.data.out = data;
			event = core_tsk_waitFor(&evq->obj.queue, delay);
		}
	}
	sys_unlock();

	return event;
}

/* -------------------------------------------------------------------------- */
unsigned evq_sendUntil( evq_t *evq, unsigned data, cnt_t time )
/* -------------------------------------------------------------------------- */
{
	unsigned event;

	assert_tsk_context();
	assert(evq);
	assert(evq->obj.res!=RELEASED);
	assert(evq->data);
	assert(evq->limit);

	sys_lock();
	{
		event = priv_evq_give(evq, data);

		if (event == E_TIMEOUT)
		{
			System.cur->tmp.evq.data.out = data;
			event = core_tsk_waitUntil(&evq->obj.queue, time);
		}
	}
	sys_unlock();

	return event;
}

/* -------------------------------------------------------------------------- */
void evq_push( evq_t *evq, unsigned data )
/* -------------------------------------------------------------------------- */
{
	assert(evq);
	assert(evq->obj.res!=RELEASED);
	assert(evq->data);
	assert(evq->limit);

	sys_lock();
	{
		priv_evq_skipUpdate(evq);
		priv_evq_putUpdate(evq, data);
	}
	sys_unlock();
}

/* -------------------------------------------------------------------------- */
unsigned evq_count( evq_t *evq )
/* -------------------------------------------------------------------------- */
{
	unsigned count;

	assert(evq);
	assert(evq->obj.res!=RELEASED);

	sys_lock();
	{
		count = evq->count;
	}
	sys_unlock();

	return count;
}

/* -------------------------------------------------------------------------- */
unsigned evq_space( evq_t *evq )
/* -------------------------------------------------------------------------- */
{
	unsigned space;

	assert(evq);
	assert(evq->obj.res!=RELEASED);

	sys_lock();
	{
		space = evq->limit - evq->count;
	}
	sys_unlock();

	return space;
}

/* -------------------------------------------------------------------------- */
unsigned evq_limit( evq_t *evq )
/* -------------------------------------------------------------------------- */
{
	unsigned limit;

	assert(evq);
	assert(evq->obj.res!=RELEASED);

	sys_lock();
	{
		limit = evq->limit;
	}
	sys_unlock();

	return limit;
}

/* -------------------------------------------------------------------------- */
