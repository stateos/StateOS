/******************************************************************************

    @file    StateOS: osmessagebuffer.c
    @author  Rajmund Szymanski
    @date    25.06.2020
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

#include "inc/osmessagebuffer.h"
#include "inc/ostask.h"
#include "inc/oscriticalsection.h"

/* -------------------------------------------------------------------------- */
static
void priv_msg_init( msg_t *msg, void *data, size_t bufsize, void *res )
/* -------------------------------------------------------------------------- */
{
	memset(msg, 0, sizeof(msg_t));

	core_obj_init(&msg->obj, res);

	msg->limit = bufsize;
	msg->data  = data;
}

/* -------------------------------------------------------------------------- */
void msg_init( msg_t *msg, void *data, size_t bufsize )
/* -------------------------------------------------------------------------- */
{
	assert_tsk_context();
	assert(msg);
	assert(data);
	assert(bufsize);

	sys_lock();
	{
		priv_msg_init(msg, data, bufsize, NULL);
	}
	sys_unlock();
}

/* -------------------------------------------------------------------------- */
msg_t *msg_create( size_t limit )
/* -------------------------------------------------------------------------- */
{
	struct msg_T { msg_t msg; char buf[]; } *tmp;
	msg_t *msg = NULL;
	size_t bufsize;

	assert_tsk_context();
	assert(limit);

	sys_lock();
	{
		bufsize = limit;
		tmp = malloc(sizeof(struct msg_T) + bufsize);
		if (tmp)
			priv_msg_init(msg = &tmp->msg, tmp->buf, bufsize, tmp);
	}
	sys_unlock();

	return msg;
}
/* -------------------------------------------------------------------------- */
static
void priv_msg_reset( msg_t *msg, unsigned event )
/* -------------------------------------------------------------------------- */
{
	msg->count = 0;
	msg->head  = 0;
	msg->tail  = 0;

	core_all_wakeup(msg->obj.queue, event);
}


/* -------------------------------------------------------------------------- */
void msg_reset( msg_t *msg )
/* -------------------------------------------------------------------------- */
{
	assert_tsk_context();
	assert(msg);
	assert(msg->obj.res!=RELEASED);

	sys_lock();
	{
		priv_msg_reset(msg, E_STOPPED);
	}
	sys_unlock();
}

/* -------------------------------------------------------------------------- */
void msg_destroy( msg_t *msg )
/* -------------------------------------------------------------------------- */
{
	assert_tsk_context();
	assert(msg);
	assert(msg->obj.res!=RELEASED);

	sys_lock();
	{
		priv_msg_reset(msg, msg->obj.res ? E_DELETED : E_STOPPED);
		core_res_free(&msg->obj);
	}
	sys_unlock();
}

/* -------------------------------------------------------------------------- */
static
void priv_msg_peek( msg_t *msg, char *data, size_t size )
/* -------------------------------------------------------------------------- */
{
	size_t i = msg->head;

	while (size--)
	{
		*data++ = msg->data[i++];
		if (i == msg->limit) i = 0;
	}
}

/* -------------------------------------------------------------------------- */
static
void priv_msg_get( msg_t *msg, char *data, size_t size )
/* -------------------------------------------------------------------------- */
{
	size_t i = msg->head;

	msg->count -= size;
	while (size--)
	{
		*data++ = msg->data[i++];
		if (i == msg->limit) i = 0;
	}
	msg->head = i;
}

/* -------------------------------------------------------------------------- */
static
void priv_msg_put( msg_t *msg, const char *data, size_t size )
/* -------------------------------------------------------------------------- */
{
	size_t i = msg->tail;

	msg->count += size;
	while (size--)
	{
		msg->data[i++] = *data++;
		if (i == msg->limit) i = 0;
	}
	msg->tail = i;
}

/* -------------------------------------------------------------------------- */
static
void priv_msg_skip( msg_t *msg, size_t size )
/* -------------------------------------------------------------------------- */
{
	msg->count -= size;
	msg->head  += size;
	if (msg->head >= msg->limit) msg->head -= msg->limit;
}

/* -------------------------------------------------------------------------- */
static
size_t priv_msg_size( msg_t *msg )
/* -------------------------------------------------------------------------- */
{
	size_t size;

	assert(msg->count);

	priv_msg_peek(msg, (void *)&size, sizeof(size_t));

	return size;
}

/* -------------------------------------------------------------------------- */
static
size_t priv_msg_getSize( msg_t *msg )
/* -------------------------------------------------------------------------- */
{
	size_t size;

	assert(msg->count);

	priv_msg_get(msg, (void *)&size, sizeof(size_t));

	return size;
}

/* -------------------------------------------------------------------------- */
static
void priv_msg_putSize( msg_t *msg, size_t size )
/* -------------------------------------------------------------------------- */
{
	priv_msg_put(msg, (const void *)&size, sizeof(size_t));
}

/* -------------------------------------------------------------------------- */
static
void priv_msg_getUpdate( msg_t *msg, char *data, size_t size, size_t *read )
/* -------------------------------------------------------------------------- */
{
	size = priv_msg_getSize(msg);
	if (read != NULL) *read = size;
	priv_msg_get(msg, data, size);

	while (msg->obj.queue != 0 && msg->count + sizeof(size_t) + msg->obj.queue->tmp.msg.size <= msg->limit)
	{
		priv_msg_putSize(msg, msg->obj.queue->tmp.msg.size);
		priv_msg_put(msg, msg->obj.queue->tmp.msg.data.out, msg->obj.queue->tmp.msg.size);
		core_one_wakeup(msg->obj.queue, E_SUCCESS);
	}
}

/* -------------------------------------------------------------------------- */
static
void priv_msg_putUpdate( msg_t *msg, const char *data, size_t size )
/* -------------------------------------------------------------------------- */
{
	priv_msg_putSize(msg, size);
	priv_msg_put(msg, data, size);

	while (msg->obj.queue != 0 && msg->count > 0)
	{
		if (msg->obj.queue->tmp.msg.size >= priv_msg_size(msg))
		{
			msg->obj.queue->tmp.msg.size = priv_msg_getSize(msg);
			priv_msg_get(msg, msg->obj.queue->tmp.msg.data.in, msg->obj.queue->tmp.msg.size);
			core_one_wakeup(msg->obj.queue, E_SUCCESS);
		}
		else
		{
			core_one_wakeup(msg->obj.queue, E_FAILURE);
		}
	}
}

/* -------------------------------------------------------------------------- */
static
void priv_msg_skipUpdate( msg_t *msg, size_t size )
/* -------------------------------------------------------------------------- */
{
	while (msg->count + sizeof(size_t) + size > msg->limit)
	{
		priv_msg_skip(msg, priv_msg_getSize(msg));

		while (msg->obj.queue != 0 && msg->count + sizeof(size_t) + msg->obj.queue->tmp.msg.size <= msg->limit)
		{
			priv_msg_putSize(msg, msg->obj.queue->tmp.msg.size);
			priv_msg_put(msg, msg->obj.queue->tmp.msg.data.out, msg->obj.queue->tmp.msg.size);
			core_one_wakeup(msg->obj.queue, E_SUCCESS);
		}
	}
}

/* -------------------------------------------------------------------------- */
static
unsigned priv_msg_take( msg_t *msg, char *data, size_t size, size_t *read )
/* -------------------------------------------------------------------------- */
{
	if (msg->count > 0)
	{
		if (size >= priv_msg_size(msg))
		{
			priv_msg_getUpdate(msg, data, size, read);
			return E_SUCCESS;
		}

		return E_FAILURE;
	}

	return E_TIMEOUT;
}

/* -------------------------------------------------------------------------- */
unsigned msg_take( msg_t *msg, void *data, size_t size, size_t *read )
/* -------------------------------------------------------------------------- */
{
	unsigned event;

	assert(msg);
	assert(msg->obj.res!=RELEASED);
	assert(msg->data);
	assert(msg->limit);
	assert(data||size==0);

	sys_lock();
	{
		event = priv_msg_take(msg, data, size, read);
	}
	sys_unlock();

	return event;
}

/* -------------------------------------------------------------------------- */
unsigned msg_waitFor( msg_t *msg, void *data, size_t size, size_t *read, cnt_t delay )
/* -------------------------------------------------------------------------- */
{
	unsigned event;

	assert_tsk_context();
	assert(msg);
	assert(msg->obj.res!=RELEASED);
	assert(msg->data);
	assert(msg->limit);
	assert(data||size==0);

	sys_lock();
	{
		event = priv_msg_take(msg, data, size, read);

		if (event == E_TIMEOUT)
		{
			System.cur->tmp.msg.data.in = data;
			System.cur->tmp.msg.size = size;
			event = core_tsk_waitFor(&msg->obj.queue, delay);
			if (event == E_SUCCESS && read != NULL)
				*read = System.cur->tmp.msg.size;
		}
	}
	sys_unlock();

	return event;
}

/* -------------------------------------------------------------------------- */
unsigned msg_waitUntil( msg_t *msg, void *data, size_t size, size_t *read, cnt_t time )
/* -------------------------------------------------------------------------- */
{
	unsigned event;

	assert_tsk_context();
	assert(msg);
	assert(msg->obj.res!=RELEASED);
	assert(msg->data);
	assert(msg->limit);
	assert(data||size==0);

	sys_lock();
	{
		event = priv_msg_take(msg, data, size, read);

		if (event == E_TIMEOUT)
		{
			System.cur->tmp.msg.data.in = data;
			System.cur->tmp.msg.size = size;
			event = core_tsk_waitUntil(&msg->obj.queue, time);
			if (event == E_SUCCESS && read != NULL)
				*read = System.cur->tmp.msg.size;
		}
	}
	sys_unlock();

	return event;
}

/* -------------------------------------------------------------------------- */
static
unsigned priv_msg_give( msg_t *msg, const char *data, size_t size )
/* -------------------------------------------------------------------------- */
{
	if (msg->count + sizeof(size_t) + size <= msg->limit)
	{
		priv_msg_putUpdate(msg, data, size);
		return E_SUCCESS;
	}

	if (sizeof(size_t) + size <= msg->limit)
		return E_TIMEOUT;

	return E_FAILURE;
}

/* -------------------------------------------------------------------------- */
unsigned msg_give( msg_t *msg, const void *data, size_t size )
/* -------------------------------------------------------------------------- */
{
	unsigned event;

	assert(msg);
	assert(msg->obj.res!=RELEASED);
	assert(msg->data);
	assert(msg->limit);
	assert(data||size==0);

	sys_lock();
	{
		event = priv_msg_give(msg, data, size);
	}
	sys_unlock();

	return event;
}

/* -------------------------------------------------------------------------- */
unsigned msg_sendFor( msg_t *msg, const void *data, size_t size, cnt_t delay )
/* -------------------------------------------------------------------------- */
{
	unsigned event;

	assert_tsk_context();
	assert(msg);
	assert(msg->obj.res!=RELEASED);
	assert(msg->data);
	assert(msg->limit);
	assert(data||size==0);

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
unsigned msg_sendUntil( msg_t *msg, const void *data, size_t size, cnt_t time )
/* -------------------------------------------------------------------------- */
{
	unsigned event;

	assert_tsk_context();
	assert(msg);
	assert(msg->obj.res!=RELEASED);
	assert(msg->data);
	assert(msg->limit);
	assert(data||size==0);

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
static
unsigned priv_msg_push( msg_t *msg, const void *data, size_t size )
/* -------------------------------------------------------------------------- */
{
	if (sizeof(size_t) + size <= msg->limit)
	{
		priv_msg_skipUpdate(msg, size);
		priv_msg_putUpdate(msg, data, size);

		return E_SUCCESS;
	}

	return E_FAILURE;
}

/* -------------------------------------------------------------------------- */
unsigned msg_push( msg_t *msg, const void *data, size_t size )
/* -------------------------------------------------------------------------- */
{
	unsigned event;

	assert(msg);
	assert(msg->obj.res!=RELEASED);
	assert(msg->data);
	assert(msg->limit);
	assert(data||size==0);

	sys_lock();
	{
		event = priv_msg_push(msg, data, size);
	}
	sys_unlock();

	return event;
}

/* -------------------------------------------------------------------------- */
size_t msg_count( msg_t *msg )
/* -------------------------------------------------------------------------- */
{
	size_t count;

	assert(msg);
	assert(msg->obj.res!=RELEASED);

	sys_lock();
	{
		count = msg->count;
	}
	sys_unlock();

	return count;
}

/* -------------------------------------------------------------------------- */
size_t msg_space( msg_t *msg )
/* -------------------------------------------------------------------------- */
{
	size_t space;

	assert(msg);
	assert(msg->obj.res!=RELEASED);

	sys_lock();
	{
		space = (msg->limit >= msg->count + sizeof(size_t)) ? msg->limit - msg->count - sizeof(size_t) : 0;
	}
	sys_unlock();

	return space;
}

/* -------------------------------------------------------------------------- */
size_t msg_limit( msg_t *msg )
/* -------------------------------------------------------------------------- */
{
	size_t limit;

	assert(msg);
	assert(msg->obj.res!=RELEASED);

	sys_lock();
	{
		limit = (msg->limit >= sizeof(size_t)) ? msg->limit - sizeof(size_t) : 0;
	}
	sys_unlock();

	return limit;
}

/* -------------------------------------------------------------------------- */
size_t msg_size( msg_t *msg )
/* -------------------------------------------------------------------------- */
{
	size_t size = 0;

	assert(msg);
	assert(msg->obj.res!=RELEASED);

	sys_lock();
	{
		if (msg->count > 0)
			size = priv_msg_size(msg);
	}
	sys_unlock();

	return size;
}

/* -------------------------------------------------------------------------- */
