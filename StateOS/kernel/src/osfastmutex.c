/******************************************************************************

    @file    StateOS: osfastmutex.c
    @author  Rajmund Szymanski
    @date    06.05.2020
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

#include "inc/osfastmutex.h"
#include "inc/oscriticalsection.h"
#include "osalloc.h"

/* -------------------------------------------------------------------------- */
static
void priv_mut_init( mut_t *mut, void *res )
/* -------------------------------------------------------------------------- */
{
	memset(mut, 0, sizeof(mut_t));

	core_obj_init(&mut->obj, res);
}

/* -------------------------------------------------------------------------- */
void mut_init( mut_t *mut )
/* -------------------------------------------------------------------------- */
{
	assert_tsk_context();
	assert(mut);

	sys_lock();
	{
		priv_mut_init(mut, NULL);
	}
	sys_unlock();
}

/* -------------------------------------------------------------------------- */
mut_t *mut_create( void )
/* -------------------------------------------------------------------------- */
{
	mut_t *mut;

	assert_tsk_context();

	sys_lock();
	{
		mut = sys_alloc(sizeof(mut_t));
		priv_mut_init(mut, mut);
	}
	sys_unlock();

	return mut;
}

/* -------------------------------------------------------------------------- */
static
void priv_mut_reset( mut_t *mut, unsigned event )
/* -------------------------------------------------------------------------- */
{
	core_all_wakeup(mut->obj.queue, event);
}

/* -------------------------------------------------------------------------- */
void mut_reset( mut_t *mut )
/* -------------------------------------------------------------------------- */
{
	assert_tsk_context();
	assert(mut);
	assert(mut->obj.res!=RELEASED);

	sys_lock();
	{
		priv_mut_reset(mut, E_STOPPED);
	}
	sys_unlock();
}

/* -------------------------------------------------------------------------- */
void mut_destroy( mut_t *mut )
/* -------------------------------------------------------------------------- */
{
	assert_tsk_context();
	assert(mut);
	assert(mut->obj.res!=RELEASED);

	sys_lock();
	{
		priv_mut_reset(mut, mut->obj.res ? E_DELETED : E_STOPPED);
		core_res_free(&mut->obj);
	}
	sys_unlock();
}

/* -------------------------------------------------------------------------- */
static
unsigned priv_mut_take( mut_t *mut )
/* -------------------------------------------------------------------------- */
{
	if (mut->owner == 0)
	{
		mut->owner = System.cur;
		return E_SUCCESS;
	}

	if (mut->owner != System.cur)
		return E_TIMEOUT;

	return E_FAILURE;
}

/* -------------------------------------------------------------------------- */
unsigned mut_take( mut_t *mut )
/* -------------------------------------------------------------------------- */
{
	unsigned event;

	assert_tsk_context();
	assert(mut);
	assert(mut->obj.res!=RELEASED);

	sys_lock();
	{
		event = priv_mut_take(mut);
	}
	sys_unlock();

	return event;
}

/* -------------------------------------------------------------------------- */
unsigned mut_waitFor( mut_t *mut, cnt_t delay )
/* -------------------------------------------------------------------------- */
{
	unsigned event;

	assert_tsk_context();
	assert(mut);
	assert(mut->obj.res!=RELEASED);

	sys_lock();
	{
		event = priv_mut_take(mut);

		if (event == E_TIMEOUT)
			event = core_tsk_waitFor(&mut->obj.queue, delay);
	}
	sys_unlock();

	return event;
}

/* -------------------------------------------------------------------------- */
unsigned mut_waitUntil( mut_t *mut, cnt_t time )
/* -------------------------------------------------------------------------- */
{
	unsigned event;

	assert_tsk_context();
	assert(mut);
	assert(mut->obj.res!=RELEASED);

	sys_lock();
	{
		event = priv_mut_take(mut);

		if (event == E_TIMEOUT)
			event = core_tsk_waitUntil(&mut->obj.queue, time);
	}
	sys_unlock();

	return event;
}

/* -------------------------------------------------------------------------- */
static
unsigned priv_mut_give( mut_t *mut )
/* -------------------------------------------------------------------------- */
{
	if (mut->owner == System.cur)
	{
		mut->owner = core_one_wakeup(mut->obj.queue, E_SUCCESS);
		return E_SUCCESS;
	}

	return E_FAILURE;
}

/* -------------------------------------------------------------------------- */
unsigned mut_give( mut_t *mut )
/* -------------------------------------------------------------------------- */
{
	unsigned event;

	assert_tsk_context();
	assert(mut);
	assert(mut->obj.res!=RELEASED);

	sys_lock();
	{
		event = priv_mut_give(mut);
	}
	sys_unlock();

	return event;
}

/* -------------------------------------------------------------------------- */
