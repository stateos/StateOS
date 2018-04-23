/******************************************************************************

    @file    StateOS: os_stm.c
    @author  Rajmund Szymanski
    @date    23.04.2018
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

#include "inc/os_stm.h"
#include "inc/os_tsk.h"

/* -------------------------------------------------------------------------- */
void stm_init( stm_t *stm, unsigned limit, void *data )
/* -------------------------------------------------------------------------- */
{
	assert(!port_isr_inside());
	assert(stm);
	assert(limit);
	assert(data);

	port_sys_lock();

	memset(stm, 0, sizeof(stm_t));

	stm->limit = limit;
	stm->data  = data;

	port_sys_unlock();
}

/* -------------------------------------------------------------------------- */
stm_t *stm_create( unsigned limit )
/* -------------------------------------------------------------------------- */
{
	stm_t *stm;

	assert(!port_isr_inside());
	assert(limit);

	port_sys_lock();

	stm = core_sys_alloc(ABOVE(sizeof(stm_t)) + limit);
	stm_init(stm, limit, (void *)((size_t)stm + ABOVE(sizeof(stm_t))));
	stm->res = stm;

	port_sys_unlock();

	return stm;
}

/* -------------------------------------------------------------------------- */
void stm_kill( stm_t *stm )
/* -------------------------------------------------------------------------- */
{
	assert(!port_isr_inside());
	assert(stm);

	port_sys_lock();

	stm->count = 0;
	stm->first = 0;
	stm->next  = 0;
	stm->owner = 0;

	core_all_wakeup(stm, E_STOPPED);

	port_sys_unlock();
}

/* -------------------------------------------------------------------------- */
void stm_delete( stm_t *stm )
/* -------------------------------------------------------------------------- */
{
	port_sys_lock();

	stm_kill(stm);
	core_sys_free(stm->res);

	port_sys_unlock();
}

/* -------------------------------------------------------------------------- */
static
tsk_t *priv_stm_owner( stm_t *stm, tsk_t *tsk )
/* -------------------------------------------------------------------------- */
{
	if (stm->owner == 0 || stm->owner->guard != stm)
		stm->owner = tsk;

	return stm->owner;
}

/* -------------------------------------------------------------------------- */
static
unsigned priv_stm_get( stm_t *stm, char *data, unsigned size )
/* -------------------------------------------------------------------------- */
{
	unsigned i;
	unsigned len = 0;
	tsk_t  * tsk;

	while (size-- > 0 && stm->count > 0)
	{
		i = stm->first;
		data[len++] = stm->data[i++];
		stm->first = (i < stm->limit) ? i : 0;
		stm->count--;

		tsk = priv_stm_owner(stm, stm->queue);
		if (tsk)
		{
			i = stm->next;
			stm->data[i++] = *(const char *)tsk->tmp.odata;
			tsk->tmp.odata = (const char *)tsk->tmp.odata + 1;
			stm->next = (i < stm->limit) ? i : 0;
			stm->count++;
			tsk->evt.size--;
			if (tsk->evt.size == 0)
			{
				stm->owner = 0;
				core_tsk_wakeup(tsk, E_SUCCESS);
			}
		}
	}

	return len;
}

/* -------------------------------------------------------------------------- */
static
unsigned priv_stm_put( stm_t *stm, const char *data, unsigned size )
/* -------------------------------------------------------------------------- */
{
	unsigned i;
	unsigned len = 0;
	tsk_t  * tsk;

	while (size-- > 0 && stm->count < stm->limit)
	{
		i = stm->next;
		stm->data[i++] = data[len++];
		stm->next = (i < stm->limit) ? i : 0;
		stm->count++;

		tsk = priv_stm_owner(stm, stm->queue);
		if (tsk)
		{
			i = stm->first;
			*(char *)tsk->tmp.idata = stm->data[i++];
			tsk->tmp.idata = (char *)tsk->tmp.idata + 1;
			stm->first = (i < stm->limit) ? i : 0;
			stm->count--;
			tsk->evt.size--;
			if (tsk->evt.size == 0)
			{
				stm->owner = 0;
				core_tsk_wakeup(tsk, E_SUCCESS);
			}
		}
	}

	return len;
}

/* -------------------------------------------------------------------------- */
unsigned stm_count( stm_t *stm )
/* -------------------------------------------------------------------------- */
{
	tsk_t  * tsk;
	unsigned cnt;

	assert(stm);

	port_sys_lock();

	cnt = stm->count;

	if (cnt == stm->limit)
		for (tsk = stm->queue; tsk; tsk = tsk->obj.queue)
			cnt += tsk->evt.size;

	port_sys_unlock();

	return cnt;
}

/* -------------------------------------------------------------------------- */
unsigned stm_space( stm_t *stm )
/* -------------------------------------------------------------------------- */
{
	tsk_t  * tsk;
	unsigned cnt;

	assert(stm);

	port_sys_lock();

	cnt = stm->limit - stm->count;

	if (cnt == stm->limit)
		for (tsk = stm->queue; tsk; tsk = tsk->obj.queue)
			cnt += tsk->evt.size;

	port_sys_unlock();

	return cnt;
}

/* -------------------------------------------------------------------------- */
unsigned stm_take( stm_t *stm, void *data, unsigned size )
/* -------------------------------------------------------------------------- */
{
	unsigned len;

	assert(stm);
	assert(data);

	port_sys_lock();

	len = priv_stm_get(stm, data, size);

	port_sys_unlock();

	return len;
}

/* -------------------------------------------------------------------------- */
static
unsigned priv_stm_wait( stm_t *stm, char *data, unsigned size, cnt_t time, unsigned(*wait)(void*,cnt_t) )
/* -------------------------------------------------------------------------- */
{
	unsigned len;

	assert(!port_isr_inside());
	assert(stm);
	assert(data);

	port_sys_lock();

	len = priv_stm_get(stm, data, size);

	if (len < size)
	{
		priv_stm_owner(stm, System.cur);
		System.cur->tmp.idata = data + len;
		System.cur->evt.size = size - len;
		wait(stm, time);
		len = size - System.cur->evt.size;
	}

	port_sys_unlock();

	return len;
}

/* -------------------------------------------------------------------------- */
unsigned stm_waitUntil( stm_t *stm, void *data, unsigned size, cnt_t time )
/* -------------------------------------------------------------------------- */
{
	return priv_stm_wait(stm, data, size, time, core_tsk_waitUntil);
}

/* -------------------------------------------------------------------------- */
unsigned stm_waitFor( stm_t *stm, void *data, unsigned size, cnt_t delay )
/* -------------------------------------------------------------------------- */
{
	return priv_stm_wait(stm, data, size, delay, core_tsk_waitFor);
}

/* -------------------------------------------------------------------------- */
unsigned stm_give( stm_t *stm, const void *data, unsigned size )
/* -------------------------------------------------------------------------- */
{
	unsigned len;

	assert(stm);
	assert(data);

	port_sys_lock();

	len = priv_stm_put(stm, data, size);

	port_sys_unlock();

	return len;
}

/* -------------------------------------------------------------------------- */
static
unsigned priv_stm_send( stm_t *stm, const char *data, unsigned size, cnt_t time, unsigned(*wait)(void*,cnt_t) )
/* -------------------------------------------------------------------------- */
{
	unsigned len;

	assert(!port_isr_inside());
	assert(stm);
	assert(data);

	port_sys_lock();

	len = priv_stm_put(stm, data, size);

	if (len < size)
	{
		priv_stm_owner(stm, System.cur);
		System.cur->tmp.odata = data + len;
		System.cur->evt.size = size - len;
		wait(stm, time);
		len = size - System.cur->evt.size;
	}

	port_sys_unlock();

	return len;
}

/* -------------------------------------------------------------------------- */
unsigned stm_sendUntil( stm_t *stm, const void *data, unsigned size, cnt_t time )
/* -------------------------------------------------------------------------- */
{
	return priv_stm_send(stm, data, size, time, core_tsk_waitUntil);
}

/* -------------------------------------------------------------------------- */
unsigned stm_sendFor( stm_t *stm, const void *data, unsigned size, cnt_t delay )
/* -------------------------------------------------------------------------- */
{
	return priv_stm_send(stm, data, size, delay, core_tsk_waitFor);
}

/* -------------------------------------------------------------------------- */
