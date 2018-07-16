/******************************************************************************

    @file    StateOS: osfastmutex.c
    @author  Rajmund Szymanski
    @date    16.07.2018
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

#include "inc/osfastmutex.h"
#include "inc/oscriticalsection.h"

/* -------------------------------------------------------------------------- */
void mut_init( mut_t *mut )
/* -------------------------------------------------------------------------- */
{
	assert(!port_isr_inside());
	assert(mut);

	sys_lock();
	{
		memset(mut, 0, sizeof(mut_t));
	}
	sys_unlock();
}

/* -------------------------------------------------------------------------- */
mut_t *mut_create( void )
/* -------------------------------------------------------------------------- */
{
	mut_t *mut;

	assert(!port_isr_inside());

	sys_lock();
	{
		mut = core_sys_alloc(sizeof(mut_t));
		mut_init(mut);
		mut->res = mut;
	}
	sys_unlock();

	return mut;
}

/* -------------------------------------------------------------------------- */
void mut_kill( mut_t *mut )
/* -------------------------------------------------------------------------- */
{
	assert(!port_isr_inside());
	assert(mut);

	sys_lock();
	{
		core_all_wakeup(mut, E_STOPPED);
	}
	sys_unlock();
}

/* -------------------------------------------------------------------------- */
void mut_delete( mut_t *mut )
/* -------------------------------------------------------------------------- */
{
	sys_lock();
	{
		mut_kill(mut);
		core_sys_free(mut->res);
	}
	sys_unlock();
}

/* -------------------------------------------------------------------------- */
static
unsigned priv_mut_wait( mut_t *mut, cnt_t time, unsigned(*wait)(void*,cnt_t) )
/* -------------------------------------------------------------------------- */
{
	unsigned event = E_TIMEOUT;

	assert(!port_isr_inside());
	assert(mut);

	sys_lock();
	{
		if (mut->owner == 0)
		{
			mut->owner = System.cur;
			event = E_SUCCESS;
		}
		else
		if (mut->owner != System.cur)
		{
			event = wait(mut, time);
		}
	}
	sys_unlock();

	return event;
}

/* -------------------------------------------------------------------------- */
unsigned mut_waitUntil( mut_t *mut, cnt_t time )
/* -------------------------------------------------------------------------- */
{
	return priv_mut_wait(mut, time, core_tsk_waitUntil);
}

/* -------------------------------------------------------------------------- */
unsigned mut_waitFor( mut_t *mut, cnt_t delay )
/* -------------------------------------------------------------------------- */
{
	return priv_mut_wait(mut, delay, core_tsk_waitFor);
}

/* -------------------------------------------------------------------------- */
unsigned mut_give( mut_t *mut )
/* -------------------------------------------------------------------------- */
{
	unsigned event = E_TIMEOUT;

	assert(!port_isr_inside());
	assert(mut);

	sys_lock();
	{
		if (mut->owner == System.cur)
		{
			mut->owner = core_one_wakeup(mut, E_SUCCESS);
			event = E_SUCCESS;
		}
	}
	sys_unlock();

	return event;
}

/* -------------------------------------------------------------------------- */
