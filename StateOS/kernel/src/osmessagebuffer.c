/******************************************************************************

    @file    StateOS: osmessagebuffer.c
    @author  Rajmund Szymanski
    @date    04.09.2018
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
	assert(!port_isr_context());
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

	assert(!port_isr_context());
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
	assert(!port_isr_context());
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
unsigned priv_msg_count( msg_t *msg )
/* -------------------------------------------------------------------------- */
{
	unsigned count = msg->count;

	if (count > 0)
		priv_msg_peek(msg, (void *)&count, sizeof(unsigned));

	return count;
}

/* -------------------------------------------------------------------------- */
static
unsigned priv_msg_space( msg_t *msg )
/* -------------------------------------------------------------------------- */
{
	return ((msg->count == 0 || msg->obj.queue == 0) && msg->limit - msg->count > sizeof(unsigned)) ? msg->limit - msg->count - sizeof(unsigned) : 0;
}

/* -------------------------------------------------------------------------- */
static
unsigned priv_msg_limit( msg_t *msg )
/* -------------------------------------------------------------------------- */
{
	return (msg->limit > sizeof(unsigned)) ? msg->limit - sizeof(unsigned) : 0;
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
unsigned priv_msg_getSize( msg_t *msg )
/* -------------------------------------------------------------------------- */
{
	assert(msg->count);

	unsigned size;

	priv_msg_get(msg, (void *)&size, sizeof(unsigned));

	return size;
}

/* -------------------------------------------------------------------------- */
static
void priv_msg_putSize( msg_t *msg, unsigned size )
/* -------------------------------------------------------------------------- */
{
	assert(size);

	priv_msg_put(msg, (const void *)&size, sizeof(unsigned));
}

/* -------------------------------------------------------------------------- */
static
unsigned priv_msg_getUpdate( msg_t *msg, char *data, unsigned size )
/* -------------------------------------------------------------------------- */
{
	size = priv_msg_getSize(msg);
	priv_msg_get(msg, data, size);

	while (msg->obj.queue != 0 && msg->obj.queue->tmp.msg.size <= priv_msg_space(msg))
	{
		priv_msg_putSize(msg, msg->obj.queue->tmp.msg.size);
		priv_msg_put(msg, msg->obj.queue->tmp.msg.data.out, msg->obj.queue->tmp.msg.size);
		msg->obj.queue->tmp.msg.size = 0;
		core_tsk_wakeup(msg->obj.queue, E_SUCCESS);
	}

	return size;
}

/* -------------------------------------------------------------------------- */
static
void priv_msg_putUpdate( msg_t *msg, const char *data, unsigned size )
/* -------------------------------------------------------------------------- */
{
	assert(size <= priv_msg_space(msg));

	priv_msg_putSize(msg, size);
	priv_msg_put(msg, data, size);

	while (msg->obj.queue != 0)
	{
		if (msg->obj.queue->tmp.msg.size >= priv_msg_count(msg))
		{
			size = priv_msg_getSize(msg);
			priv_msg_get(msg, msg->obj.queue->tmp.msg.data.in, size);
			msg->obj.queue->tmp.msg.size -= size;
			core_tsk_wakeup(msg->obj.queue, E_SUCCESS);
		}
		else
		{
			core_tsk_wakeup(msg->obj.queue, E_TIMEOUT);
		}
	}
}

/* -------------------------------------------------------------------------- */
unsigned msg_take( msg_t *msg, void *data, unsigned size )
/* -------------------------------------------------------------------------- */
{
	unsigned len = 0;

	assert(msg);
	assert(data);

	sys_lock();
	{
		if (msg->count > 0)
		{
			if (size >= priv_msg_count(msg))
				len = priv_msg_getUpdate(msg, data, size);
		}
	}
	sys_unlock();

	return len;
}

/* -------------------------------------------------------------------------- */
static
unsigned priv_msg_wait( msg_t *msg, char *data, unsigned size, cnt_t time, unsigned(*wait)(tsk_t**,cnt_t) )
/* -------------------------------------------------------------------------- */
{
	assert(!port_isr_context());
	assert(msg);
	assert(data);

	if (msg->count > 0)
	{
		if (size >= priv_msg_count(msg))
			return priv_msg_getUpdate(msg, data, size);
		return 0;
	}

	System.cur->tmp.msg.data.in = data;
	System.cur->tmp.msg.size = size;

	if (size > 0)
		wait(&msg->obj.queue, time);

	return size - System.cur->tmp.msg.size;
}

/* -------------------------------------------------------------------------- */
unsigned msg_waitFor( msg_t *msg, void *data, unsigned size, cnt_t delay )
/* -------------------------------------------------------------------------- */
{
	unsigned len;

	sys_lock();
	{
		len = priv_msg_wait(msg, data, size, delay, core_tsk_waitFor);
	}
	sys_unlock();

	return len;
}

/* -------------------------------------------------------------------------- */
unsigned msg_waitUntil( msg_t *msg, void *data, unsigned size, cnt_t time )
/* -------------------------------------------------------------------------- */
{
	unsigned len;

	sys_lock();
	{
		len = priv_msg_wait(msg, data, size, time, core_tsk_waitUntil);
	}
	sys_unlock();

	return len;
}

/* -------------------------------------------------------------------------- */
unsigned msg_give( msg_t *msg, const void *data, unsigned size )
/* -------------------------------------------------------------------------- */
{
	unsigned len = 0;

	assert(msg);
	assert(data);

	sys_lock();
	{
		if (size <= priv_msg_space(msg))
		{
			if (size > 0)
				priv_msg_putUpdate(msg, data, size);
			len = size;
		}
	}
	sys_unlock();

	return len;
}

/* -------------------------------------------------------------------------- */
static
unsigned priv_msg_send( msg_t *msg, const char *data, unsigned size, cnt_t time, unsigned(*wait)(tsk_t**,cnt_t) )
/* -------------------------------------------------------------------------- */
{
	assert(!port_isr_context());
	assert(msg);
	assert(data);

	if (size <= priv_msg_space(msg))
	{
		if (size > 0)
			priv_msg_putUpdate(msg, data, size);
		return size;
	}

	System.cur->tmp.msg.data.out = data;
	System.cur->tmp.msg.size = size;

	if (size <= priv_msg_limit(msg))
		wait(&msg->obj.queue, time);

	return size - System.cur->tmp.msg.size;
}

/* -------------------------------------------------------------------------- */
unsigned msg_sendFor( msg_t *msg, const void *data, unsigned size, cnt_t delay )
/* -------------------------------------------------------------------------- */
{
	unsigned len;

	sys_lock();
	{
		len = priv_msg_send(msg, data, size, delay, core_tsk_waitFor);
	}
	sys_unlock();

	return len;
}

/* -------------------------------------------------------------------------- */
unsigned msg_sendUntil( msg_t *msg, const void *data, unsigned size, cnt_t time )
/* -------------------------------------------------------------------------- */
{
	unsigned len;

	sys_lock();
	{
		len = priv_msg_send(msg, data, size, time, core_tsk_waitUntil);
	}
	sys_unlock();

	return len;
}

/* -------------------------------------------------------------------------- */
unsigned msg_push( msg_t *msg, const void *data, unsigned size )
/* -------------------------------------------------------------------------- */
{
	unsigned len = 0;

	assert(msg);
	assert(data);

	sys_lock();
	{
		if ((msg->count == 0 || msg->obj.queue == 0) && size <= priv_msg_limit(msg))
		{
			while (size > priv_msg_space(msg))
				priv_msg_skip(msg, priv_msg_getSize(msg));
			if (size > 0)
				priv_msg_putUpdate(msg, data, size);
			len = size;
		}
	}
	sys_unlock();

	return len;
}

/* -------------------------------------------------------------------------- */
unsigned msg_count( msg_t *msg )
/* -------------------------------------------------------------------------- */
{
	unsigned cnt;

	assert(msg);

	sys_lock();
	{
		cnt = priv_msg_count(msg);
	}
	sys_unlock();

	return cnt;
}

/* -------------------------------------------------------------------------- */
unsigned msg_space( msg_t *msg )
/* -------------------------------------------------------------------------- */
{
	unsigned cnt;

	assert(msg);

	sys_lock();
	{
		cnt = priv_msg_space(msg);
	}
	sys_unlock();

	return cnt;
}

/* -------------------------------------------------------------------------- */
unsigned msg_limit( msg_t *msg )
/* -------------------------------------------------------------------------- */
{
	unsigned cnt;

	assert(msg);

	sys_lock();
	{
		cnt = priv_msg_limit(msg);
	}
	sys_unlock();

	return cnt;
}

/* -------------------------------------------------------------------------- */
