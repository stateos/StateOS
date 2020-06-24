/******************************************************************************

    @file    StateOS: osstreambuffer.c
    @author  Rajmund Szymanski
    @date    24.06.2020
    @brief   This file provides set of functions for StateOS.

 ******************************************************************************

   Copyright (c) 2018-2020 Rajmund Szymanski. All rights reserved.

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

#include "inc/osstreambuffer.h"
#include "inc/ostask.h"
#include "inc/oscriticalsection.h"

/* -------------------------------------------------------------------------- */
static
void priv_stm_init( stm_t *stm, void *data, size_t bufsize, void *res )
/* -------------------------------------------------------------------------- */
{
	memset(stm, 0, sizeof(stm_t));

	core_obj_init(&stm->obj, res);

	stm->limit = bufsize;
	stm->data  = data;
}

/* -------------------------------------------------------------------------- */
void stm_init( stm_t *stm, void *data, size_t bufsize )
/* -------------------------------------------------------------------------- */
{
	assert_tsk_context();
	assert(stm);
	assert(data);
	assert(bufsize);

	sys_lock();
	{
		priv_stm_init(stm, data, bufsize, NULL);
	}
	sys_unlock();
}

/* -------------------------------------------------------------------------- */
stm_t *stm_create( size_t limit )
/* -------------------------------------------------------------------------- */
{
	struct stm_T { stm_t stm; char buf[]; } *tmp;
	stm_t *stm = NULL;
	size_t bufsize;

	assert_tsk_context();
	assert(limit);

	sys_lock();
	{
		bufsize = limit;
		tmp = malloc(sizeof(struct stm_T) + bufsize);
		if (tmp)
			priv_stm_init(stm = &tmp->stm, tmp->buf, bufsize, tmp);
	}
	sys_unlock();

	return stm;
}

/* -------------------------------------------------------------------------- */
static
void priv_stm_reset( stm_t *stm, unsigned event )
/* -------------------------------------------------------------------------- */
{
	stm->count = 0;
	stm->head  = 0;
	stm->tail  = 0;

	core_all_wakeup(stm->obj.queue, event);
}

/* -------------------------------------------------------------------------- */
void stm_reset( stm_t *stm )
/* -------------------------------------------------------------------------- */
{
	assert_tsk_context();
	assert(stm);
	assert(stm->obj.res!=RELEASED);

	sys_lock();
	{
		priv_stm_reset(stm, E_STOPPED);
	}
	sys_unlock();
}

/* -------------------------------------------------------------------------- */
void stm_destroy( stm_t *stm )
/* -------------------------------------------------------------------------- */
{
	assert_tsk_context();
	assert(stm);
	assert(stm->obj.res!=RELEASED);

	sys_lock();
	{
		priv_stm_reset(stm, stm->obj.res ? E_DELETED : E_STOPPED);
		core_res_free(&stm->obj);
	}
	sys_unlock();
}

/* -------------------------------------------------------------------------- */
static
void priv_stm_get( stm_t *stm, char *data, unsigned size )
/* -------------------------------------------------------------------------- */
{
	unsigned i = stm->head;

	stm->count -= size;
	while (size--)
	{
		*data++ = stm->data[i++];
		if (i >= stm->limit) i = 0;
	}
	stm->head = i;
}

/* -------------------------------------------------------------------------- */
static
void priv_stm_put( stm_t *stm, const char *data, unsigned size )
/* -------------------------------------------------------------------------- */
{
	unsigned i = stm->tail;

	stm->count += size;
	while (size--)
	{
		stm->data[i++] = *data++;
		if (i >= stm->limit) i = 0;
	}
	stm->tail = i;
}

/* -------------------------------------------------------------------------- */
static
void priv_stm_skip( stm_t *stm, unsigned size )
/* -------------------------------------------------------------------------- */
{
	stm->count -= size;
	stm->head  += size;
	if (stm->head >= stm->limit) stm->head -= stm->limit;
}

/* -------------------------------------------------------------------------- */
static
void priv_stm_getUpdate( stm_t *stm, char *data, unsigned size, unsigned *read )
/* -------------------------------------------------------------------------- */
{
	if (size > stm->count) size = stm->count;
	if (read != NULL) *read = size;
	priv_stm_get(stm, data, size);

	while (stm->obj.queue != 0 && stm->count + stm->obj.queue->tmp.stm.size <= stm->limit)
	{
		priv_stm_put(stm, stm->obj.queue->tmp.stm.data.out, stm->obj.queue->tmp.stm.size);
		core_one_wakeup(stm->obj.queue, E_SUCCESS);
	}
}

/* -------------------------------------------------------------------------- */
static
void priv_stm_putUpdate( stm_t *stm, const char *data, unsigned size )
/* -------------------------------------------------------------------------- */
{
	priv_stm_put(stm, data, size);

	while (stm->obj.queue != 0 && stm->count > 0)
	{
		if (stm->obj.queue->tmp.stm.size > stm->count)
			stm->obj.queue->tmp.stm.size = stm->count;
		priv_stm_get(stm, stm->obj.queue->tmp.stm.data.in, stm->obj.queue->tmp.stm.size);
		core_one_wakeup(stm->obj.queue, E_SUCCESS);
	}
}

/* -------------------------------------------------------------------------- */
static
void priv_stm_skipUpdate( stm_t *stm, unsigned size )
/* -------------------------------------------------------------------------- */
{
	while (stm->obj.queue != 0)
	{
		if (stm->count + stm->obj.queue->tmp.stm.size > stm->limit)
			priv_stm_skip(stm, stm->count + stm->obj.queue->tmp.stm.size - stm->limit);
		priv_stm_put(stm, stm->obj.queue->tmp.stm.data.out, stm->obj.queue->tmp.stm.size);
		core_one_wakeup(stm->obj.queue, E_SUCCESS);
	}

	if (stm->count + size > stm->limit)
		priv_stm_skip(stm, stm->count + size - stm->limit);
}

/* -------------------------------------------------------------------------- */
static
unsigned priv_stm_take( stm_t *stm, char *data, unsigned size, unsigned *read )
/* -------------------------------------------------------------------------- */
{
	if (stm->count > 0)
	{
		priv_stm_getUpdate(stm, data, size, read);
		return E_SUCCESS;
	}

	return E_TIMEOUT;
}

/* -------------------------------------------------------------------------- */
unsigned stm_take( stm_t *stm, void *data, unsigned size, unsigned *read )
/* -------------------------------------------------------------------------- */
{
	unsigned event;

	assert(stm);
	assert(stm->obj.res!=RELEASED);
	assert(stm->data);
	assert(stm->limit);
	assert(data);
	assert(size);

	sys_lock();
	{
		event = priv_stm_take(stm, data, size, read);
	}
	sys_unlock();

	return event;
}

/* -------------------------------------------------------------------------- */
unsigned stm_waitFor( stm_t *stm, void *data, unsigned size, unsigned *read, cnt_t delay )
/* -------------------------------------------------------------------------- */
{
	unsigned event;

	assert_tsk_context();
	assert(stm);
	assert(stm->obj.res!=RELEASED);
	assert(stm->data);
	assert(stm->limit);
	assert(data);
	assert(size);

	sys_lock();
	{
		event = priv_stm_take(stm, data, size, read);

		if (event == E_TIMEOUT)
		{
			System.cur->tmp.stm.data.in = data;
			System.cur->tmp.stm.size = size;
			event = core_tsk_waitFor(&stm->obj.queue, delay);
			if (event == E_SUCCESS && read != NULL)
				*read = System.cur->tmp.stm.size;
		}
	}
	sys_unlock();

	return event;
}

/* -------------------------------------------------------------------------- */
unsigned stm_waitUntil( stm_t *stm, void *data, unsigned size, unsigned *read, cnt_t time )
/* -------------------------------------------------------------------------- */
{
	unsigned event;

	assert_tsk_context();
	assert(stm);
	assert(stm->obj.res!=RELEASED);
	assert(stm->data);
	assert(stm->limit);
	assert(data);
	assert(size);

	sys_lock();
	{
		event = priv_stm_take(stm, data, size, read);

		if (event == E_TIMEOUT)
		{
			System.cur->tmp.stm.data.in = data;
			System.cur->tmp.stm.size = size;
			event = core_tsk_waitUntil(&stm->obj.queue, time);
			if (event == E_SUCCESS && read != NULL)
				*read = System.cur->tmp.stm.size;
		}
	}
	sys_unlock();

	return event;
}

/* -------------------------------------------------------------------------- */
static
unsigned priv_stm_give( stm_t *stm, const char *data, unsigned size )
/* -------------------------------------------------------------------------- */
{
	if (stm->count + size <= stm->limit)
	{
		priv_stm_putUpdate(stm, data, size);
		return E_SUCCESS;
	}

	if (size <= stm->limit)
		return E_TIMEOUT;

	return E_FAILURE;
}

/* -------------------------------------------------------------------------- */
unsigned stm_give( stm_t *stm, const void *data, unsigned size )
/* -------------------------------------------------------------------------- */
{
	unsigned event;

	assert(stm);
	assert(stm->obj.res!=RELEASED);
	assert(stm->data);
	assert(stm->limit);
	assert(data);
	assert(size);

	sys_lock();
	{
		event = priv_stm_give(stm, data, size);
	}
	sys_unlock();

	return event;
}

/* -------------------------------------------------------------------------- */
unsigned stm_sendFor( stm_t *stm, const void *data, unsigned size, cnt_t delay )
/* -------------------------------------------------------------------------- */
{
	unsigned event;

	assert_tsk_context();
	assert(stm);
	assert(stm->obj.res!=RELEASED);
	assert(stm->data);
	assert(stm->limit);
	assert(data);
	assert(size);

	sys_lock();
	{
		event = priv_stm_give(stm, data, size);

		if (event == E_TIMEOUT)
		{
			System.cur->tmp.stm.data.out = data;
			System.cur->tmp.stm.size = size;
			event = core_tsk_waitFor(&stm->obj.queue, delay);
		}
	}
	sys_unlock();

	return event;
}

/* -------------------------------------------------------------------------- */
unsigned stm_sendUntil( stm_t *stm, const void *data, unsigned size, cnt_t time )
/* -------------------------------------------------------------------------- */
{
	unsigned event;

	assert_tsk_context();
	assert(stm);
	assert(stm->obj.res!=RELEASED);
	assert(stm->data);
	assert(stm->limit);
	assert(data);
	assert(size);

	sys_lock();
	{
		event = priv_stm_give(stm, data, size);

		if (event == E_TIMEOUT)
		{
			System.cur->tmp.stm.data.out = data;
			System.cur->tmp.stm.size = size;
			event = core_tsk_waitUntil(&stm->obj.queue, time);
		}
	}
	sys_unlock();

	return event;
}

/* -------------------------------------------------------------------------- */
static
unsigned priv_stm_push( stm_t *stm, const void *data, unsigned size )
/* -------------------------------------------------------------------------- */
{
	if (size <= stm->limit)
	{
		priv_stm_skipUpdate(stm, size);
		priv_stm_putUpdate(stm, data, size);

		return E_SUCCESS;
	}

	return E_FAILURE;
}

/* -------------------------------------------------------------------------- */
unsigned stm_push( stm_t *stm, const void *data, unsigned size )
/* -------------------------------------------------------------------------- */
{
	unsigned event;

	assert(stm);
	assert(stm->obj.res!=RELEASED);
	assert(stm->data);
	assert(stm->limit);
	assert(data);
	assert(size);

	sys_lock();
	{
		event = priv_stm_push(stm, data, size);
	}
	sys_unlock();

	return event;
}

/* -------------------------------------------------------------------------- */
size_t stm_count( stm_t *stm )
/* -------------------------------------------------------------------------- */
{
	size_t count;

	assert(stm);
	assert(stm->obj.res!=RELEASED);

	sys_lock();
	{
		count = stm->count;
	}
	sys_unlock();

	return count;
}

/* -------------------------------------------------------------------------- */
size_t stm_space( stm_t *stm )
/* -------------------------------------------------------------------------- */
{
	size_t space;

	assert(stm);
	assert(stm->obj.res!=RELEASED);

	sys_lock();
	{
		space = stm->limit - stm->count;
	}
	sys_unlock();

	return space;
}

/* -------------------------------------------------------------------------- */
size_t stm_limit( stm_t *stm )
/* -------------------------------------------------------------------------- */
{
	size_t limit;

	assert(stm);
	assert(stm->obj.res!=RELEASED);

	sys_lock();
	{
		limit = stm->limit;
	}
	sys_unlock();

	return limit;
}

/* -------------------------------------------------------------------------- */
