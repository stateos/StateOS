/******************************************************************************

    @file    StateOS: osmessagebuffer.c
    @author  Rajmund Szymanski
    @date    21.09.2018
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

#include "inc/osmessagebuffer.h"
#include "inc/ostask.h"
#include "inc/oscriticalsection.h"
#include "osalloc.h"

/* -------------------------------------------------------------------------- */
void msg_init( msg_t *msg, void *data, unsigned bufsize )
/* -------------------------------------------------------------------------- */
{
	assert_tsk_context();
	assert(msg);
	assert(data);
	assert(bufsize);

	sys_lock();
	{
		memset(msg, 0, sizeof(msg_t));

		core_obj_init(&msg->obj);

		msg->limit = bufsize;
		msg->data  = data;
	}
	sys_unlock();
}

/* -------------------------------------------------------------------------- */
msg_t *msg_create( unsigned limit )
/* -------------------------------------------------------------------------- */
{
	msg_t  * msg;
	unsigned bufsize;

	assert_tsk_context();
	assert(limit);

	sys_lock();
	{
		bufsize = limit;
		msg = sys_alloc(SEG_OVER(sizeof(msg_t)) + bufsize);
		msg_init(msg, (void *)((size_t)msg + SEG_OVER(sizeof(msg_t))), bufsize);
		msg->obj.res = msg;
	}
	sys_unlock();

	return msg;
}

/* -------------------------------------------------------------------------- */
void msg_kill( msg_t *msg )
/* -------------------------------------------------------------------------- */
{
	assert_tsk_context();
	assert(msg);

	sys_lock();
	{
		msg->count = 0;
		msg->head  = 0;
		msg->tail  = 0;

		core_all_wakeup(&msg->obj.queue, E_STOPPED);
	}
	sys_unlock();
}

/* -------------------------------------------------------------------------- */
void msg_delete( msg_t *msg )
/* -------------------------------------------------------------------------- */
{
	sys_lock();
	{
		msg_kill(msg);
		sys_free(msg->obj.res);
	}
	sys_unlock();
}

/* -------------------------------------------------------------------------- */
static
void priv_msg_peek( msg_t *msg, char *data, unsigned size )
/* -------------------------------------------------------------------------- */
{
	unsigned i = msg->head;

	while (size--)
	{
		*data++ = msg->data[i++];
		if (i >= msg->limit) i = 0;
	}
}

/* -------------------------------------------------------------------------- */
static
void priv_msg_get( msg_t *msg, char *data, unsigned size )
/* -------------------------------------------------------------------------- */
{
	unsigned i = msg->head;

	msg->count -= size;
	while (size--)
	{
		*data++ = msg->data[i++];
		if (i >= msg->limit) i = 0;
	}
	msg->head = i;
}

/* -------------------------------------------------------------------------- */
static
void priv_msg_put( msg_t *msg, const char *data, unsigned size )
/* -------------------------------------------------------------------------- */
{
	unsigned i = msg->tail;

	msg->count += size;
	while (size--)
	{
		msg->data[i++] = *data++;
		if (i >= msg->limit) i = 0;
	}
	msg->tail = i;
}

/* -------------------------------------------------------------------------- */
static
void priv_msg_skip( msg_t *msg, unsigned size )
/* -------------------------------------------------------------------------- */
{
	msg->count -= size;
	msg->head  += size;
	if (msg->head >= msg->limit) msg->head -= msg->limit;
}

/* -------------------------------------------------------------------------- */
static
unsigned priv_msg_size( msg_t *msg )
/* -------------------------------------------------------------------------- */
{
	unsigned size;

	assert(msg->count);

	priv_msg_peek(msg, (void *)&size, sizeof(unsigned));

	return size;
}

/* -------------------------------------------------------------------------- */
static
unsigned priv_msg_getSize( msg_t *msg )
/* -------------------------------------------------------------------------- */
{
	unsigned size;

	assert(msg->count);

	priv_msg_get(msg, (void *)&size, sizeof(unsigned));

	return size;
}

/* -------------------------------------------------------------------------- */
static
void priv_msg_putSize( msg_t *msg, unsigned size )
/* -------------------------------------------------------------------------- */
{
	priv_msg_put(msg, (const void *)&size, sizeof(unsigned));
}

/* -------------------------------------------------------------------------- */
static
unsigned priv_msg_getUpdate( msg_t *msg, char *data, unsigned size )
/* -------------------------------------------------------------------------- */
{
	size = priv_msg_getSize(msg);
	priv_msg_get(msg, data, size);

	while (msg->obj.queue != 0 && msg->count + sizeof(unsigned) + msg->obj.queue->tmp.msg.size <= msg->limit)
	{
		priv_msg_putSize(msg, msg->obj.queue->tmp.msg.size);
		priv_msg_put(msg, msg->obj.queue->tmp.msg.data.out, msg->obj.queue->tmp.msg.size);
		core_tsk_wakeup(msg->obj.queue, E_SUCCESS);
	}

	return size;
}

/* -------------------------------------------------------------------------- */
static
void priv_msg_putUpdate( msg_t *msg, const char *data, unsigned size )
/* -------------------------------------------------------------------------- */
{
	priv_msg_putSize(msg, size);
	priv_msg_put(msg, data, size);

	while (msg->obj.queue != 0)
	{
		if (msg->obj.queue->tmp.msg.size >= priv_msg_size(msg))
		{
			size = priv_msg_getSize(msg);
			priv_msg_get(msg, msg->obj.queue->tmp.msg.data.in, size);
			core_tsk_wakeup(msg->obj.queue, size);
		}
		else
		{
			core_tsk_wakeup(msg->obj.queue, E_FAILURE);
		}
	}
}

/* -------------------------------------------------------------------------- */
static
void priv_msg_skipUpdate( msg_t *msg, unsigned size )
/* -------------------------------------------------------------------------- */
{
	while (msg->count + sizeof(unsigned) + size > msg->limit)
	{
		priv_msg_skip(msg, priv_msg_getSize(msg));

		while (msg->obj.queue != 0 && msg->count + sizeof(unsigned) + msg->obj.queue->tmp.msg.size <= msg->limit)
		{
			priv_msg_putSize(msg, msg->obj.queue->tmp.msg.size);
			priv_msg_put(msg, msg->obj.queue->tmp.msg.data.out, msg->obj.queue->tmp.msg.size);
			core_tsk_wakeup(msg->obj.queue, E_SUCCESS);
		}
	}
}

/* -------------------------------------------------------------------------- */
static
unsigned priv_msg_take( msg_t *msg, char *data, unsigned size )
/* -------------------------------------------------------------------------- */
{
	assert(msg);
	assert(msg->data);
	assert(msg->limit);
	assert(data);

	if (msg->count > 0)
	{
		if (size >= priv_msg_size(msg))
			return priv_msg_getUpdate(msg, data, size);

		return E_FAILURE;
	}

	return E_TIMEOUT;
}

/* -------------------------------------------------------------------------- */
unsigned msg_take( msg_t *msg, void *data, unsigned size )
/* -------------------------------------------------------------------------- */
{
	unsigned len;

	sys_lock();
	{
		len = priv_msg_take(msg, data, size);
	}
	sys_unlock();

	return len;
}

/* -------------------------------------------------------------------------- */
unsigned msg_waitFor( msg_t *msg, void *data, unsigned size, cnt_t delay )
/* -------------------------------------------------------------------------- */
{
	unsigned len;

	assert_tsk_context();

	sys_lock();
	{
		len = priv_msg_take(msg, data, size);

		if (len == E_TIMEOUT)
		{
			System.cur->tmp.msg.data.in = data;
			System.cur->tmp.msg.size = size;
			len = core_tsk_waitFor(&msg->obj.queue, delay);
		}
	}
	sys_unlock();

	return len;
}

/* -------------------------------------------------------------------------- */
unsigned msg_waitUntil( msg_t *msg, void *data, unsigned size, cnt_t time )
/* -------------------------------------------------------------------------- */
{
	unsigned len;

	assert_tsk_context();

	sys_lock();
	{
		len = priv_msg_take(msg, data, size);

		if (len == E_TIMEOUT)
		{
			System.cur->tmp.msg.data.in = data;
			System.cur->tmp.msg.size = size;
			len = core_tsk_waitUntil(&msg->obj.queue, time);
		}
	}
	sys_unlock();

	return len;
}

/* -------------------------------------------------------------------------- */
static
unsigned priv_msg_give( msg_t *msg, const char *data, unsigned size )
/* -------------------------------------------------------------------------- */
{
	assert(msg);
	assert(msg->data);
	assert(msg->limit);
	assert(data);

	if (msg->count + sizeof(unsigned) + size <= msg->limit)
	{
		priv_msg_putUpdate(msg, data, size);
		return E_SUCCESS;
	}

	if (sizeof(unsigned) + size <= msg->limit)
		return E_TIMEOUT;

	return E_FAILURE;
}

/* -------------------------------------------------------------------------- */
unsigned msg_give( msg_t *msg, const void *data, unsigned size )
/* -------------------------------------------------------------------------- */
{
	unsigned event;

	sys_lock();
	{
		event = priv_msg_give(msg, data, size);
	}
	sys_unlock();

	return event;
}

/* -------------------------------------------------------------------------- */
unsigned msg_sendFor( msg_t *msg, const void *data, unsigned size, cnt_t delay )
/* -------------------------------------------------------------------------- */
{
	unsigned event;

	assert_tsk_context();

	sys_lock();
	{
		event = priv_msg_give(msg, data, size);

		if (event == E_TIMEOUT)
		{
			System.cur->tmp.msg.data.out = data;
			System.cur->tmp.msg.size = size;
			event = core_tsk_waitFor(&msg->obj.queue, delay);
		}
	}
	sys_unlock();

	return event;
}

/* -------------------------------------------------------------------------- */
unsigned msg_sendUntil( msg_t *msg, const void *data, unsigned size, cnt_t time )
/* -------------------------------------------------------------------------- */
{
	unsigned event;

	assert_tsk_context();

	sys_lock();
	{
		event = priv_msg_give(msg, data, size);

		if (event == E_TIMEOUT)
		{
			System.cur->tmp.msg.data.out = data;
			System.cur->tmp.msg.size = size;
			event = core_tsk_waitUntil(&msg->obj.queue, time);
		}
	}
	sys_unlock();

	return event;
}

/* -------------------------------------------------------------------------- */
unsigned msg_push( msg_t *msg, const void *data, unsigned size )
/* -------------------------------------------------------------------------- */
{
	unsigned event;

	assert(msg);
	assert(msg->data);
	assert(msg->limit);
	assert(data);

	sys_lock();
	{
		if (sizeof(unsigned) + size <= msg->limit)
		{
			priv_msg_skipUpdate(msg, size);
			priv_msg_putUpdate(msg, data, size);
			event = E_SUCCESS;
		}
		else
		{
			event = E_FAILURE;
		}
	}
	sys_unlock();

	return event;
}

/* -------------------------------------------------------------------------- */
unsigned msg_count( msg_t *msg )
/* -------------------------------------------------------------------------- */
{
	unsigned count;

	assert(msg);

	sys_lock();
	{
		count = msg->count;
	}
	sys_unlock();

	return count;
}

/* -------------------------------------------------------------------------- */
unsigned msg_space( msg_t *msg )
/* -------------------------------------------------------------------------- */
{
	unsigned space;

	assert(msg);

	sys_lock();
	{
		space = (msg->limit >= msg->count + sizeof(unsigned)) ? msg->limit - msg->count - sizeof(unsigned) : 0;
	}
	sys_unlock();

	return space;
}

/* -------------------------------------------------------------------------- */
unsigned msg_limit( msg_t *msg )
/* -------------------------------------------------------------------------- */
{
	unsigned limit;

	assert(msg);

	sys_lock();
	{
		limit = (msg->limit >= sizeof(unsigned)) ? msg->limit - sizeof(unsigned) : 0;
	}
	sys_unlock();

	return limit;
}

/* -------------------------------------------------------------------------- */
unsigned msg_size( msg_t *msg )
/* -------------------------------------------------------------------------- */
{
	unsigned size = 0;

	assert(msg);

	sys_lock();
	{
		if (msg->count > 0)
			size = priv_msg_size(msg);
	}
	sys_unlock();

	return size;
}

/* -------------------------------------------------------------------------- */
