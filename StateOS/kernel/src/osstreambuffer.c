/******************************************************************************

    @file    StateOS: osstreambuffer.c
    @author  Rajmund Szymanski
    @date    19.09.2018
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

#include "inc/osstreambuffer.h"
#include "inc/ostask.h"
#include "inc/oscriticalsection.h"
#include "osalloc.h"

/* -------------------------------------------------------------------------- */
void stm_init( stm_t *stm, void *data, unsigned bufsize )
/* -------------------------------------------------------------------------- */
{
	assert(!port_isr_context());
	assert(stm);
	assert(data);
	assert(bufsize);

	sys_lock();
	{
		memset(stm, 0, sizeof(stm_t));

		core_obj_init(&stm->obj);

		stm->limit = bufsize;
		stm->data  = data;
	}
	sys_unlock();
}

/* -------------------------------------------------------------------------- */
stm_t *stm_create( unsigned limit )
/* -------------------------------------------------------------------------- */
{
	stm_t  * stm;
	unsigned bufsize;

	assert(!port_isr_context());
	assert(limit);

	sys_lock();
	{
		bufsize = limit;
		stm = sys_alloc(SEG_OVER(sizeof(stm_t)) + bufsize);
		stm_init(stm, (void *)((size_t)stm + SEG_OVER(sizeof(stm_t))), bufsize);
		stm->obj.res = stm;
	}
	sys_unlock();

	return stm;
}

/* -------------------------------------------------------------------------- */
void stm_kill( stm_t *stm )
/* -------------------------------------------------------------------------- */
{
	assert(!port_isr_context());
	assert(stm);

	sys_lock();
	{
		stm->count = 0;
		stm->head  = 0;
		stm->tail  = 0;

		core_all_wakeup(&stm->obj.queue, E_STOPPED);
	}
	sys_unlock();
}

/* -------------------------------------------------------------------------- */
void stm_delete( stm_t *stm )
/* -------------------------------------------------------------------------- */
{
	sys_lock();
	{
		stm_kill(stm);
		sys_free(stm->obj.res);
	}
	sys_unlock();
}

/* -------------------------------------------------------------------------- */
static
unsigned priv_stm_count( stm_t *stm )
/* -------------------------------------------------------------------------- */
{
	return stm->count;
}

/* -------------------------------------------------------------------------- */
static
unsigned priv_stm_space( stm_t *stm )
/* -------------------------------------------------------------------------- */
{
	return (stm->count == 0 || stm->obj.queue == 0) ? stm->limit - stm->count : 0;
}

/* -------------------------------------------------------------------------- */
static
unsigned priv_stm_limit( stm_t *stm )
/* -------------------------------------------------------------------------- */
{
	return stm->limit;
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
unsigned priv_stm_getUpdate( stm_t *stm, char *data, unsigned size )
/* -------------------------------------------------------------------------- */
{
	if (size > stm->count)
		size = stm->count;
	priv_stm_get(stm, data, size);

	while (stm->obj.queue != 0 && stm->count + stm->obj.queue->tmp.stm.size <= stm->limit)
	{
		priv_stm_put(stm, stm->obj.queue->tmp.stm.data.out, stm->obj.queue->tmp.stm.size);
		stm->obj.queue->tmp.stm.size = 0;
		core_tsk_wakeup(stm->obj.queue, E_SUCCESS);
	}

	return size;
}

/* -------------------------------------------------------------------------- */
static
void priv_stm_putUpdate( stm_t *stm, const char *data, unsigned size )
/* -------------------------------------------------------------------------- */
{
	priv_stm_put(stm, data, size);

	while (stm->obj.queue != 0 && stm->count > 0)
	{
		size = stm->obj.queue->tmp.stm.size;
		if (size > stm->count)
			size = stm->count;
		priv_stm_get(stm, stm->obj.queue->tmp.stm.data.in, size);
		stm->obj.queue->tmp.stm.size -= size;
		core_tsk_wakeup(stm->obj.queue, E_SUCCESS);
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
		stm->obj.queue->tmp.stm.size = 0;
		core_tsk_wakeup(stm->obj.queue, E_SUCCESS);
	}

	if (stm->count + size > stm->limit)
		priv_stm_skip(stm, stm->count + size - stm->limit);
}

/* -------------------------------------------------------------------------- */
static
unsigned priv_stm_take( stm_t *stm, char *data, unsigned size )
/* -------------------------------------------------------------------------- */
{
	assert(stm);
	assert(stm->data);
	assert(stm->limit);
	assert(data);

	if (size > 0 && stm->count > 0)
		return priv_stm_getUpdate(stm, data, size);

	return 0;
}

/* -------------------------------------------------------------------------- */
unsigned stm_take( stm_t *stm, void *data, unsigned size )
/* -------------------------------------------------------------------------- */
{
	unsigned len;

	sys_lock();
	{
		len = priv_stm_take(stm, data, size);
	}
	sys_unlock();

	return len;
}

/* -------------------------------------------------------------------------- */
unsigned stm_waitFor( stm_t *stm, void *data, unsigned size, cnt_t delay )
/* -------------------------------------------------------------------------- */
{
	unsigned len;

	assert(!port_isr_context());

	sys_lock();
	{
		len = priv_stm_take(stm, data, size);

		if (len == 0 && size > 0)
		{
			System.cur->tmp.stm.data.in = data;
			System.cur->tmp.stm.size = size;
			core_tsk_waitFor(&stm->obj.queue, delay);
			len = size - System.cur->tmp.stm.size;
		}
	}
	sys_unlock();

	return len;
}

/* -------------------------------------------------------------------------- */
unsigned stm_waitUntil( stm_t *stm, void *data, unsigned size, cnt_t time )
/* -------------------------------------------------------------------------- */
{
	unsigned len;

	assert(!port_isr_context());

	sys_lock();
	{
		len = priv_stm_take(stm, data, size);

		if (len == 0 && size > 0)
		{
			System.cur->tmp.stm.data.in = data;
			System.cur->tmp.stm.size = size;
			core_tsk_waitUntil(&stm->obj.queue, time);
			len = size - System.cur->tmp.stm.size;
		}
	}
	sys_unlock();

	return len;
}

/* -------------------------------------------------------------------------- */
static
unsigned priv_stm_give( stm_t *stm, const char *data, unsigned size )
/* -------------------------------------------------------------------------- */
{
	assert(stm);
	assert(stm->data);
	assert(stm->limit);
	assert(data);

	if (size > 0 && size <= priv_stm_space(stm))
	{
		priv_stm_putUpdate(stm, data, size);
		return size;
	}

	return 0;
}

/* -------------------------------------------------------------------------- */
unsigned stm_give( stm_t *stm, const void *data, unsigned size )
/* -------------------------------------------------------------------------- */
{
	unsigned len;

	sys_lock();
	{
		len = priv_stm_give(stm, data, size);
	}
	sys_unlock();

	return len;
}

/* -------------------------------------------------------------------------- */
unsigned stm_sendFor( stm_t *stm, const void *data, unsigned size, cnt_t delay )
/* -------------------------------------------------------------------------- */
{
	unsigned len;

	assert(!port_isr_context());

	sys_lock();
	{
		len = priv_stm_give(stm, data, size);

		if (len == 0 && size > 0 && size <= priv_stm_limit(stm))
		{
			System.cur->tmp.stm.data.out = data;
			System.cur->tmp.stm.size = size;
			core_tsk_waitFor(&stm->obj.queue, delay);
			len = size - System.cur->tmp.stm.size;
		}
	}
	sys_unlock();

	return len;
}

/* -------------------------------------------------------------------------- */
unsigned stm_sendUntil( stm_t *stm, const void *data, unsigned size, cnt_t time )
/* -------------------------------------------------------------------------- */
{
	unsigned len;

	assert(!port_isr_context());

	sys_lock();
	{
		len = priv_stm_give(stm, data, size);

		if (len == 0 && size > 0 && size <= priv_stm_limit(stm))
		{
			System.cur->tmp.stm.data.out = data;
			System.cur->tmp.stm.size = size;
			core_tsk_waitUntil(&stm->obj.queue, time);
			len = size - System.cur->tmp.stm.size;
		}
	}
	sys_unlock();

	return len;
}

/* -------------------------------------------------------------------------- */
unsigned stm_push( stm_t *stm, const void *data, unsigned size )
/* -------------------------------------------------------------------------- */
{
	unsigned len = 0;

	assert(stm);
	assert(stm->data);
	assert(stm->limit);
	assert(data);

	sys_lock();
	{
		if (size > 0 && size <= priv_stm_limit(stm))
		{
			priv_stm_skipUpdate(stm, size);
			priv_stm_putUpdate(stm, data, size);
			len = size;
		}
	}
	sys_unlock();

	return len;
}

/* -------------------------------------------------------------------------- */
unsigned stm_count( stm_t *stm )
/* -------------------------------------------------------------------------- */
{
	unsigned count;

	assert(stm);

	sys_lock();
	{
		count = stm->count;
	}
	sys_unlock();

	return count;
}

/* -------------------------------------------------------------------------- */
unsigned stm_space( stm_t *stm )
/* -------------------------------------------------------------------------- */
{
	unsigned space;

	assert(stm);

	sys_lock();
	{
		space = stm->limit - stm->count;
	}
	sys_unlock();

	return space;
}

/* -------------------------------------------------------------------------- */
unsigned stm_limit( stm_t *stm )
/* -------------------------------------------------------------------------- */
{
	unsigned limit;

	assert(stm);

	sys_lock();
	{
		limit = stm->limit;
	}
	sys_unlock();

	return limit;
}

/* -------------------------------------------------------------------------- */
