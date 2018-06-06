/******************************************************************************

    @file    StateOS: osspinlock.c
    @author  Rajmund Szymanski
    @date    05.06.2018
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

#include "inc/osspinlock.h"

/* -------------------------------------------------------------------------- */
void spn_init( spn_t *spn )
/* -------------------------------------------------------------------------- */
{
	assert(!port_isr_inside());
	assert(spn);

	port_sys_lock();

	memset(spn, 0, sizeof(spn_t));

	port_sys_unlock();
}

/* -------------------------------------------------------------------------- */
spn_t *spn_create( void )
/* -------------------------------------------------------------------------- */
{
	spn_t *spn;

	assert(!port_isr_inside());

	port_sys_lock();

	spn = core_sys_alloc(sizeof(spn_t));
	spn_init(spn);
	spn->res = spn;

	port_sys_unlock();

	return spn;
}

/* -------------------------------------------------------------------------- */
void spn_kill( spn_t *spn )
/* -------------------------------------------------------------------------- */
{
	assert(!port_isr_inside());
	assert(spn);

	port_sys_lock();

	spn->flag = 0;

	core_all_wakeup(spn, E_STOPPED);

	port_sys_unlock();
}

/* -------------------------------------------------------------------------- */
void spn_delete( spn_t *spn )
/* -------------------------------------------------------------------------- */
{
	port_sys_lock();

	spn_kill(spn);
	core_sys_free(spn->res);

	port_sys_unlock();
}

/* -------------------------------------------------------------------------- */
unsigned spn_take( spn_t *spn )
/* -------------------------------------------------------------------------- */
{
	unsigned event = E_TIMEOUT;

	assert(spn);

	port_sys_lock();

	if (spn->flag == 0)
	{
		spn->flag = 1;
		event = E_SUCCESS;
	}

	port_sys_unlock();

	return event;
}

/* -------------------------------------------------------------------------- */
static
unsigned priv_spn_wait( spn_t *spn, cnt_t time, unsigned(*wait)(void*,cnt_t) )
/* -------------------------------------------------------------------------- */
{
	unsigned event;

	assert(!port_isr_inside());
	assert(spn);

	port_sys_lock();

	if (spn->flag == 0)
	{
		spn->flag = 1;
		event = E_SUCCESS;
	}
	else
	{
		event = wait(spn, time);
	}

	port_sys_unlock();

	return event;
}

/* -------------------------------------------------------------------------- */
unsigned spn_waitUntil( spn_t *spn, cnt_t time )
/* -------------------------------------------------------------------------- */
{
	return priv_spn_wait(spn, time, core_tsk_waitUntil);
}

/* -------------------------------------------------------------------------- */
unsigned spn_waitFor( spn_t *spn, cnt_t delay )
/* -------------------------------------------------------------------------- */
{
	return priv_spn_wait(spn, delay, core_tsk_waitFor);
}

/* -------------------------------------------------------------------------- */
void spn_give( spn_t *spn )
/* -------------------------------------------------------------------------- */
{
	assert(spn);

	port_sys_lock();

	if (core_one_wakeup(spn, E_SUCCESS) == 0)
		spn->flag = 0;

	port_sys_unlock();
}

/* -------------------------------------------------------------------------- */
