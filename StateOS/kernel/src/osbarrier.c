/******************************************************************************

    @file    StateOS: osbarrier.c
    @author  Rajmund Szymanski
    @date    02.07.2020
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

#include "inc/osbarrier.h"
#include "inc/oscriticalsection.h"

/* -------------------------------------------------------------------------- */
static
void priv_bar_init( bar_t *bar, unsigned limit, void *res )
/* -------------------------------------------------------------------------- */
{
	memset(bar, 0, sizeof(bar_t));

	core_obj_init(&bar->obj, res);

	bar->limit = limit;
}

/* -------------------------------------------------------------------------- */
void bar_init( bar_t *bar, unsigned limit )
/* -------------------------------------------------------------------------- */
{
	assert_tsk_context();
	assert(bar);
	assert(limit);

	sys_lock();
	{
		priv_bar_init(bar, limit, NULL);
	}
	sys_unlock();
}

/* -------------------------------------------------------------------------- */
bar_t *bar_create( unsigned limit )
/* -------------------------------------------------------------------------- */
{
	bar_t *bar;

	assert_tsk_context();
	assert(limit);

	sys_lock();
	{
		bar = malloc(sizeof(bar_t));
		if (bar)
			priv_bar_init(bar, limit, bar);
	}
	sys_unlock();

	return bar;
}

/* -------------------------------------------------------------------------- */
static
void priv_bar_reset( bar_t *bar, int event )
/* -------------------------------------------------------------------------- */
{
	core_all_wakeup(bar->obj.queue, event);
}

/* -------------------------------------------------------------------------- */
void bar_reset( bar_t *bar )
/* -------------------------------------------------------------------------- */
{
	assert_tsk_context();
	assert(bar);
	assert(bar->obj.res!=RELEASED);

	sys_lock();
	{
		priv_bar_reset(bar, E_STOPPED);
	}
	sys_unlock();
}

/* -------------------------------------------------------------------------- */
void bar_destroy( bar_t *bar )
/* -------------------------------------------------------------------------- */
{
	assert_tsk_context();
	assert(bar);
	assert(bar->obj.res!=RELEASED);

	sys_lock();
	{
		priv_bar_reset(bar, bar->obj.res ? E_DELETED : E_STOPPED);
		core_res_free(&bar->obj);
	}
	sys_unlock();
}

/* -------------------------------------------------------------------------- */
static
int priv_bar_take( bar_t *bar )
/* -------------------------------------------------------------------------- */
{
	if (core_tsk_count(bar->obj.queue) + 1 == bar->limit)
	{
		core_all_wakeup(bar->obj.queue, E_SUCCESS);
		return E_SUCCESS;
	}

	return E_TIMEOUT;
}

/* -------------------------------------------------------------------------- */
int bar_waitFor( bar_t *bar, cnt_t delay )
/* -------------------------------------------------------------------------- */
{
	int result;

	assert_tsk_context();
	assert(bar);
	assert(bar->obj.res!=RELEASED);
	assert(bar->limit);

	sys_lock();
	{
		result = priv_bar_take(bar);
		if (result == E_TIMEOUT)
			result = core_tsk_waitFor(&bar->obj.queue, delay);
	}
	sys_unlock();

	return result;
}

/* -------------------------------------------------------------------------- */
int bar_waitUntil( bar_t *bar, cnt_t time )
/* -------------------------------------------------------------------------- */
{
	int result;

	assert_tsk_context();
	assert(bar);
	assert(bar->obj.res!=RELEASED);
	assert(bar->limit);

	sys_lock();
	{
		result = priv_bar_take(bar);
		if (result == E_TIMEOUT)
			result = core_tsk_waitUntil(&bar->obj.queue, time);
	}
	sys_unlock();

	return result;
}

/* -------------------------------------------------------------------------- */
