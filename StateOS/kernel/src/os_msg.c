/******************************************************************************

    @file    StateOS: os_msg.c
    @author  Rajmund Szymanski
    @date    01.05.2018
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

#include "inc/os_msg.h"
#include "inc/os_tsk.h"

/* -------------------------------------------------------------------------- */
void msg_init( msg_t *msg, unsigned limit, void *data )
/* -------------------------------------------------------------------------- */
{
	assert(!port_isr_inside());
	assert(msg);
	assert(limit);
	assert(data);

	port_sys_lock();

	memset(msg, 0, sizeof(msg_t));

	msg->limit = PSIZE(limit);
	msg->data  = data;

	port_sys_unlock();
}

/* -------------------------------------------------------------------------- */
msg_t *msg_create( unsigned limit )
/* -------------------------------------------------------------------------- */
{
	msg_t *msg;

	assert(!port_isr_inside());
	assert(limit);

	port_sys_lock();

	msg = core_sys_alloc(ABOVE(sizeof(msg_t)) + PSIZE(limit));
	msg_init(msg, limit, (void *)((size_t)msg + ABOVE(sizeof(msg_t))));
	msg->res = msg;

	port_sys_unlock();

	return msg;
}

/* -------------------------------------------------------------------------- */
void msg_kill( msg_t *msg )
/* -------------------------------------------------------------------------- */
{
	assert(!port_isr_inside());
	assert(msg);

	port_sys_lock();

	msg->count = 0;
	msg->first = 0;
	msg->next  = 0;
	msg->size  = 0;

	core_all_wakeup(msg, E_STOPPED);

	port_sys_unlock();
}

/* -------------------------------------------------------------------------- */
void msg_delete( msg_t *msg )
/* -------------------------------------------------------------------------- */
{
	port_sys_lock();

	msg_kill(msg);
	core_sys_free(msg->res);

	port_sys_unlock();
}

/* -------------------------------------------------------------------------- */
static
char priv_msg_getc( msg_t *msg )
/* -------------------------------------------------------------------------- */
{
	unsigned i = msg->first;
	char c = msg->data[i++];
	msg->first = (i < msg->limit) ? i : 0;
	msg->count--;
	return c;
}

/* -------------------------------------------------------------------------- */
static
void priv_msg_putc( msg_t *msg, char c )
/* -------------------------------------------------------------------------- */
{
	unsigned i = msg->next;
	msg->data[i++] = c;
	msg->next = (i < msg->limit) ? i : 0;
	msg->count++;
}

/* -------------------------------------------------------------------------- */
static
void priv_msg_get( msg_t *msg, char *data, unsigned size )
/* -------------------------------------------------------------------------- */
{
	while (size-- > 0)
		*data++ = priv_msg_getc(msg);

	while (msg->first % sizeof(unsigned))
		priv_msg_getc(msg);
}

/* -------------------------------------------------------------------------- */
static
void priv_msg_put( msg_t *msg, const char *data, unsigned size )
/* -------------------------------------------------------------------------- */
{
	while (size-- > 0)
		priv_msg_putc(msg, *data++);

	while (msg->next % sizeof(unsigned))
		priv_msg_putc(msg, 0);
}

/* -------------------------------------------------------------------------- */
static
void priv_msg_give( msg_t *msg, tsk_t *tsk, unsigned size )
/* -------------------------------------------------------------------------- */
{
	tsk->evt.size -= size;
	priv_msg_put(msg, tsk->tmp.idata, size);
	core_tsk_wakeup(tsk, E_SUCCESS);
}

/* -------------------------------------------------------------------------- */
static
void priv_tsk_get( tsk_t *tsk, char *data, unsigned size )
/* -------------------------------------------------------------------------- */
{
	tsk->evt.size -= size;
	memcpy(data, tsk->tmp.odata, size);
	core_tsk_wakeup(tsk, E_SUCCESS);
}

/* -------------------------------------------------------------------------- */
static
void priv_tsk_put( tsk_t *tsk, const char *data, unsigned size )
/* -------------------------------------------------------------------------- */
{
	tsk->evt.size -= size;
	memcpy(tsk->tmp.idata, data, size);
	core_tsk_wakeup(tsk, E_SUCCESS);
}

/* -------------------------------------------------------------------------- */
static
void priv_put_size( msg_t *msg, unsigned size )
/* -------------------------------------------------------------------------- */
{
	if (msg->count == 0)
		msg->size = size;
	else
		priv_msg_put(msg, (void *)&size, sizeof(unsigned));
}

/* -------------------------------------------------------------------------- */
static
void priv_msg_update( msg_t *msg, bool force )
/* -------------------------------------------------------------------------- */
{
	if (msg->size > 0 && (msg->count == 0 || force))
	{
		msg->size = 0;
		if (force)
			while (msg->size == 0 && msg->count > 0)
				priv_msg_get(msg, (void *)&msg->size, sizeof(unsigned));
		while (msg->queue != 0 && msg->size == 0)
		{
			msg->size = msg->queue->evt.size;
			if (msg->size <= msg->limit)
			{
				priv_msg_give(msg, msg->queue, msg->queue->evt.size);
				if (msg->size > 0)
				{
					while (msg->queue != 0 && msg->count + msg->queue->evt.size + sizeof(unsigned) <= msg->limit)
					{
						priv_put_size(msg, msg->queue->evt.size);
						priv_msg_give(msg, msg->queue, msg->queue->evt.size);
					}
				}
			}
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

	port_sys_lock();

	priv_msg_update(msg, false);

	if (msg->size > 0)
	{
		if (msg->size <= size)
		{
			if (msg->count > 0)
				priv_msg_get(msg, data, msg->size);
			else
				priv_tsk_get(msg->queue, data, msg->size);
			len = msg->size;
			priv_msg_update(msg, true);
		}
	}

	port_sys_unlock();

	return len;
}

/* -------------------------------------------------------------------------- */
static
unsigned priv_msg_wait( msg_t *msg, char *data, unsigned size, cnt_t time, unsigned(*wait)(void*,cnt_t) )
/* -------------------------------------------------------------------------- */
{
	unsigned len = 0;

	assert(!port_isr_inside());
	assert(msg);
	assert(data);

	port_sys_lock();

	priv_msg_update(msg, false);

	if (msg->size > 0)
	{
		if (msg->size <= size)
		{
			if (msg->count > 0)
				priv_msg_get(msg, data, msg->size);
			else
				priv_tsk_get(msg->queue, data, msg->size);
			len = msg->size;
			priv_msg_update(msg, true);
		}
	}
	else
	{
		System.cur->tmp.idata = data;
		System.cur->evt.size = size;
		wait(msg, time);
		len = size - System.cur->evt.size;
	}

	port_sys_unlock();

	return len;
}

/* -------------------------------------------------------------------------- */
unsigned msg_waitUntil( msg_t *msg, void *data, unsigned size, cnt_t time )
/* -------------------------------------------------------------------------- */
{
	return priv_msg_wait(msg, data, size, time, core_tsk_waitUntil);
}

/* -------------------------------------------------------------------------- */
unsigned msg_waitFor( msg_t *msg, void *data, unsigned size, cnt_t delay )
/* -------------------------------------------------------------------------- */
{
	return priv_msg_wait(msg, data, size, delay, core_tsk_waitFor);
}

/* -------------------------------------------------------------------------- */
unsigned msg_give( msg_t *msg, const void *data, unsigned size )
/* -------------------------------------------------------------------------- */
{
	unsigned len = 0;

	assert(msg);
	assert(data);

	port_sys_lock();

	priv_msg_update(msg, false);

	if (msg->queue != 0 && msg->size == 0)
	{
		if (msg->queue->evt.size >= size)
		{
			priv_tsk_put(msg->queue, data, size);
			len = size;
		}
	}
	else
	if (msg->queue == 0 && ((msg->size == 0 && msg->count + size <= msg->limit) || msg->count + sizeof(unsigned) + size <= msg->limit))
	{
		priv_put_size(msg, size);
		priv_msg_put(msg, data, size);
		len = size;
	}

	port_sys_unlock();

	return len;
}

/* -------------------------------------------------------------------------- */
static
unsigned priv_msg_send( msg_t *msg, const char *data, unsigned size, cnt_t time, unsigned(*wait)(void*,cnt_t) )
/* -------------------------------------------------------------------------- */
{
	unsigned len = 0;

	assert(!port_isr_inside());
	assert(msg);
	assert(data);

	port_sys_lock();

	priv_msg_update(msg, false);

	if (msg->queue != 0 && msg->size == 0)
	{
		if (msg->queue->evt.size >= size)
		{
			priv_tsk_put(msg->queue, data, size);
			len = size;
		}
	}
	else
	if (msg->queue == 0 && ((msg->size == 0 && msg->count + size <= msg->limit) || msg->count + sizeof(unsigned) + size <= msg->limit))
	{
		priv_put_size(msg, size);
		priv_msg_put(msg, data, size);
		len = size;
	}
	else
	{
		if (msg->size == 0)
			msg->size = size;
		System.cur->tmp.odata = data;
		System.cur->evt.size = size;
		wait(msg, time);
		len = size - System.cur->evt.size;
	}

	port_sys_unlock();

	return len;
}

/* -------------------------------------------------------------------------- */
unsigned msg_sendUntil( msg_t *msg, const void *data, unsigned size, cnt_t time )
/* -------------------------------------------------------------------------- */
{
	return priv_msg_send(msg, data, size, time, core_tsk_waitUntil);
}

/* -------------------------------------------------------------------------- */
unsigned msg_sendFor( msg_t *msg, const void *data, unsigned size, cnt_t delay )
/* -------------------------------------------------------------------------- */
{
	return priv_msg_send(msg, data, size, delay, core_tsk_waitFor);
}

/* -------------------------------------------------------------------------- */
unsigned msg_count( msg_t *msg )
/* -------------------------------------------------------------------------- */
{
	unsigned cnt;

	assert(msg);

	port_sys_lock();

	priv_msg_update(msg, false);

	cnt = msg->size;

	port_sys_unlock();

	return cnt;
}

/* -------------------------------------------------------------------------- */
unsigned msg_space( msg_t *msg )
/* -------------------------------------------------------------------------- */
{
	unsigned cnt = 0;

	assert(msg);

	port_sys_lock();

	priv_msg_update(msg, false);

	if (msg->size > 0)
	{
		if (msg->queue == 0)
			if (msg->count < msg->limit)
				cnt = msg->limit - msg->count - sizeof(unsigned);
	}
	else
	{
		if (msg->queue == 0)
			cnt = msg->limit;
		else
			cnt = msg->queue->evt.size;
	}

	port_sys_unlock();

	return cnt;
}

/* -------------------------------------------------------------------------- */
