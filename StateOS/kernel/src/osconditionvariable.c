/******************************************************************************

    @file    StateOS: osconditionvariable.c
    @author  Rajmund Szymanski
    @date    31.08.2018
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

/* -------------------------------------------------------------------------- */
void cnd_init( cnd_t *cnd )
/* -------------------------------------------------------------------------- */
{
	assert(!port_isr_context());
	assert(cnd);

	sys_lock();
	{
		memset(cnd, 0, sizeof(cnd_t));
	}
	sys_unlock();
}

/* -------------------------------------------------------------------------- */
cnd_t *cnd_create( void )
/* -------------------------------------------------------------------------- */
{
	cnd_t *cnd;

	assert(!port_isr_context());

	sys_lock();
	{
		cnd = core_sys_alloc(sizeof(cnd_t));
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
	assert(!port_isr_context());
	assert(cnd);

	sys_lock();
	{
		core_all_wakeup(&cnd->obj.queue, E_STOPPED);
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
		core_sys_free(cnd->obj.res);
	}
	sys_unlock();
}

/* -------------------------------------------------------------------------- */
static
unsigned priv_cnd_wait( cnd_t *cnd, mtx_t *mtx, cnt_t time, unsigned(*wait)(tsk_t**,cnt_t) )
/* -------------------------------------------------------------------------- */
{
	unsigned event;

	assert(!port_isr_context());
	assert(cnd);
	assert(mtx);

	event = mtx_give(mtx);
	if (event != E_SUCCESS)
		return event;

	event = wait(&cnd->obj.queue, time);
	if (event != E_SUCCESS)
		return event;

	return mtx_wait(mtx);
}

/* -------------------------------------------------------------------------- */
unsigned cnd_waitFor( cnd_t *cnd, mtx_t *mtx, cnt_t delay )
/* -------------------------------------------------------------------------- */
{
	unsigned event;

	sys_lock();
	{
		event = priv_cnd_wait(cnd, mtx, delay, core_tsk_waitFor);
	}
	sys_unlock();

	return event;
}

/* -------------------------------------------------------------------------- */
unsigned cnd_waitUntil( cnd_t *cnd, mtx_t *mtx, cnt_t time )
/* -------------------------------------------------------------------------- */
{
	unsigned event;

	sys_lock();
	{
		event = priv_cnd_wait(cnd, mtx, time, core_tsk_waitUntil);
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
		if (all) core_all_wakeup(&cnd->obj.queue, E_SUCCESS);
		else     core_one_wakeup(&cnd->obj.queue, E_SUCCESS);
	}
	sys_unlock();
}

/* -------------------------------------------------------------------------- */
