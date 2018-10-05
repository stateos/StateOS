/******************************************************************************

    @file    StateOS: osconditionvariable.c
    @author  Rajmund Szymanski
    @date    05.10.2018
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

#include "inc/osconditionvariable.h"
#include "inc/oscriticalsection.h"
#include "osalloc.h"

/* -------------------------------------------------------------------------- */
void cnd_init( cnd_t *cnd )
/* -------------------------------------------------------------------------- */
{
	assert_tsk_context();
	assert(cnd);

	sys_lock();
	{
		memset(cnd, 0, sizeof(cnd_t));

		core_obj_init(&cnd->obj);
	}
	sys_unlock();
}

/* -------------------------------------------------------------------------- */
cnd_t *cnd_create( void )
/* -------------------------------------------------------------------------- */
{
	cnd_t *cnd;

	assert_tsk_context();

	sys_lock();
	{
		cnd = sys_alloc(sizeof(cnd_t));
		cnd_init(cnd);
		cnd->obj.res = cnd;
	}
	sys_unlock();

	return cnd;
}

/* -------------------------------------------------------------------------- */
void cnd_kill( cnd_t *cnd )
/* -------------------------------------------------------------------------- */
{
	assert_tsk_context();
	assert(cnd);

	sys_lock();
	{
		core_all_wakeup(cnd->obj.queue, E_STOPPED);
	}
	sys_unlock();
}

/* -------------------------------------------------------------------------- */
void cnd_delete( cnd_t *cnd )
/* -------------------------------------------------------------------------- */
{
	sys_lock();
	{
		cnd_kill(cnd);
		core_res_free(&cnd->obj.res);
	}
	sys_unlock();
}

/* -------------------------------------------------------------------------- */
unsigned cnd_waitFor( cnd_t *cnd, mtx_t *mtx, cnt_t delay )
/* -------------------------------------------------------------------------- */
{
	unsigned event;
	unsigned wait_event;

	assert_tsk_context();
	assert(cnd);
	assert(mtx);

	sys_lock();
	{
		event = mtx_give(mtx);
		if (event == E_SUCCESS)
		{
			wait_event = core_tsk_waitFor(&cnd->obj.queue, delay);
			event = mtx_wait(mtx);
			if (event == E_SUCCESS)
				event = wait_event;
		}
	}
	sys_unlock();

	return event;
}

/* -------------------------------------------------------------------------- */
unsigned cnd_waitUntil( cnd_t *cnd, mtx_t *mtx, cnt_t time )
/* -------------------------------------------------------------------------- */
{
	unsigned event;
	unsigned wait_event;

	assert_tsk_context();
	assert(cnd);
	assert(mtx);

	sys_lock();
	{
		event = mtx_give(mtx);
		if (event == E_SUCCESS)
		{
			wait_event = core_tsk_waitUntil(&cnd->obj.queue, time);
			event = mtx_wait(mtx);
			if (event == E_SUCCESS)
				event = wait_event;
		}
	}
	sys_unlock();

	return event;
}

/* -------------------------------------------------------------------------- */
void cnd_give( cnd_t *cnd, bool all )
/* -------------------------------------------------------------------------- */
{
	assert(cnd);

	sys_lock();
	{
		while (core_one_wakeup(cnd->obj.queue, E_SUCCESS) && all);
	}
	sys_unlock();
}

/* -------------------------------------------------------------------------- */
