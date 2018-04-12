/******************************************************************************

    @file    StateOS: os_stm.c
    @author  Rajmund Szymanski
    @date    12.04.2018
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
void priv_stm_get( stm_t *stm, tsk_t *tsk )
/* -------------------------------------------------------------------------- */
{
	while (tsk->evt.size > 0 && stm->count > 0)
	{
		*tsk->tmp.buff++ = stm->data[stm->first++];
		if (stm->first >= stm->limit)
			stm->first = 0;
		tsk->evt.size--;
		stm->count--;
	}
}

/* -------------------------------------------------------------------------- */
static
void priv_stm_put( stm_t *stm, tsk_t *tsk )
/* -------------------------------------------------------------------------- */
{
	while (tsk->evt.size > 0 && stm->count < stm->limit)
	{
		stm->data[stm->next++] = *tsk->tmp.buff++;
		if (stm->next >= stm->limit)
			stm->next = 0;
		tsk->evt.size--;
		stm->count++;
	}
}

/* -------------------------------------------------------------------------- */
static
unsigned priv_stm_wait( stm_t *stm, void *data, unsigned size, cnt_t time, unsigned(*wait)(void*,cnt_t) )
/* -------------------------------------------------------------------------- */
{
	tsk_t *cur = System.cur;
	tsk_t *tsk;

	assert(stm);
	assert(data);

	port_sys_lock();

	cur->tmp.buff = data;
	cur->evt.size = size;

	while (cur->evt.size > 0 && stm->count > 0)
	{
		priv_stm_get(stm, cur);

		while ((tsk = stm->queue) != 0 && stm->count < stm->limit)
		{
			priv_stm_put(stm, tsk);
			if (tsk->evt.size == 0)
				core_one_wakeup(stm, E_SUCCESS);
		}
	}

	if (cur->evt.size > 0)
		wait(stm, time);

	size = cur->tmp.buff - (char *)data;
	
	port_sys_unlock();

	return size;
}

/* -------------------------------------------------------------------------- */
unsigned stm_waitUntil( stm_t *stm, void *data, unsigned size, cnt_t time )
/* -------------------------------------------------------------------------- */
{
	assert(!port_isr_inside());

	return priv_stm_wait(stm, data, size, time, core_tsk_waitUntil);
}

/* -------------------------------------------------------------------------- */
unsigned stm_waitFor( stm_t *stm, void *data, unsigned size, cnt_t delay )
/* -------------------------------------------------------------------------- */
{
	assert(!port_isr_inside() || !delay);

	return priv_stm_wait(stm, data, size, delay, core_tsk_waitFor);
}

/* -------------------------------------------------------------------------- */
static
unsigned priv_stm_send( stm_t *stm, void *data, unsigned size, cnt_t time, unsigned(*wait)(void*,cnt_t) )
/* -------------------------------------------------------------------------- */
{
	tsk_t *cur = System.cur;
	tsk_t *tsk;

	assert(stm);
	assert(data);

	port_sys_lock();

	cur->tmp.buff = data;
	cur->evt.size = size;

	while (cur->evt.size > 0 && stm->count < stm->limit)
	{
		priv_stm_put(stm, cur);

		while ((tsk = stm->queue) != 0 && stm->count > 0)
		{
			priv_stm_get(stm, tsk);
			if (tsk->evt.size == 0)
				core_one_wakeup(stm, E_SUCCESS);
		}
	}

	if (cur->evt.size > 0)
		wait(stm, time);

	size = cur->tmp.buff - (char *)data;
	
	port_sys_unlock();

	return size;
}

/* -------------------------------------------------------------------------- */
unsigned stm_sendUntil( stm_t *stm, void *data, unsigned size, cnt_t time )
/* -------------------------------------------------------------------------- */
{
	assert(!port_isr_inside());

	return priv_stm_send(stm, data, size, time, core_tsk_waitUntil);
}

/* -------------------------------------------------------------------------- */
unsigned stm_sendFor( stm_t *stm, void *data, unsigned size, cnt_t delay )
/* -------------------------------------------------------------------------- */
{
	assert(!port_isr_inside() || !delay);

	return priv_stm_send(stm, data, size, delay, core_tsk_waitFor);
}

/* -------------------------------------------------------------------------- */
