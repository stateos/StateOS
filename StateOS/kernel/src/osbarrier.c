/******************************************************************************

    @file    StateOS: osbarrier.c
    @author  Rajmund Szymanski
    @date    31.07.2018
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

#include "inc/osbarrier.h"
#include "inc/oscriticalsection.h"

/* -------------------------------------------------------------------------- */
void bar_init( bar_t *bar, unsigned limit )
/* -------------------------------------------------------------------------- */
{
	assert(!port_isr_inside());
	assert(bar);
	assert(limit);

	sys_lock();
	{
		memset(bar, 0, sizeof(bar_t));

		bar->count = limit;
		bar->limit = limit;
	}
	sys_unlock();
}

/* -------------------------------------------------------------------------- */
bar_t *bar_create( unsigned limit )
/* -------------------------------------------------------------------------- */
{
	bar_t *bar;

	assert(!port_isr_inside());
	assert(limit);

	sys_lock();
	{
		bar = core_sys_alloc(sizeof(bar_t));
		bar_init(bar, limit);
		bar->res = bar;
	}
	sys_unlock();

	return bar;
}

/* -------------------------------------------------------------------------- */
void bar_kill( bar_t *bar )
/* -------------------------------------------------------------------------- */
{
	assert(!port_isr_inside());
	assert(bar);

	sys_lock();
	{
		bar->count = bar->limit;

		core_all_wakeup(bar, E_STOPPED);
	}
	sys_unlock();
}

/* -------------------------------------------------------------------------- */
void bar_delete( bar_t *bar )
/* -------------------------------------------------------------------------- */
{
	sys_lock();
	{
		bar_kill(bar);
		core_sys_free(bar->res);
	}
	sys_unlock();
}

/* -------------------------------------------------------------------------- */
static
unsigned priv_bar_wait( bar_t *bar, cnt_t time, unsigned(*wait)(void*,cnt_t) )
/* -------------------------------------------------------------------------- */
{
	unsigned event;

	assert(!port_isr_inside());
	assert(bar);
	assert(bar->count);

	sys_lock();
	{
		if (--bar->count == 0)
		{
			bar->count = bar->limit;
			core_all_wakeup(bar, E_SUCCESS);
			event = E_SUCCESS;
		}
		else
		{
			event = wait(bar, time);
		}
	}
	sys_unlock();

	return event;
}

/* -------------------------------------------------------------------------- */
unsigned bar_waitFor( bar_t *bar, cnt_t delay )
/* -------------------------------------------------------------------------- */
{
	return priv_bar_wait(bar, delay, core_tsk_waitFor);
}

/* -------------------------------------------------------------------------- */
unsigned bar_waitUntil( bar_t *bar, cnt_t time )
/* -------------------------------------------------------------------------- */
{
	return priv_bar_wait(bar, time, core_tsk_waitUntil);
}

/* -------------------------------------------------------------------------- */
