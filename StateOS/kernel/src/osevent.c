/******************************************************************************

    @file    StateOS: osevent.c
    @author  Rajmund Szymanski
    @date    08.09.2018
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

#include "inc/osevent.h"
#include "inc/oscriticalsection.h"
#include "osalloc.h"

/* -------------------------------------------------------------------------- */
void evt_init( evt_t *evt )
/* -------------------------------------------------------------------------- */
{
	assert(!port_isr_context());
	assert(evt);

	sys_lock();
	{
		memset(evt, 0, sizeof(evt_t));
	}
	sys_unlock();
}

/* -------------------------------------------------------------------------- */
evt_t *evt_create( void )
/* -------------------------------------------------------------------------- */
{
	evt_t *evt;

	assert(!port_isr_context());

	sys_lock();
	{
		evt = sys_alloc(sizeof(evt_t));
		evt_init(evt);
		evt->obj.res = evt;
	}
	sys_unlock();

	return evt;
}

/* -------------------------------------------------------------------------- */
void evt_kill( evt_t *evt )
/* -------------------------------------------------------------------------- */
{
	assert(!port_isr_context());
	assert(evt);

	sys_lock();
	{
		core_all_wakeup(&evt->obj.queue, E_STOPPED);
	}
	sys_unlock();
}

/* -------------------------------------------------------------------------- */
void evt_delete( evt_t *evt )
/* -------------------------------------------------------------------------- */
{
	sys_lock();
	{
		evt_kill(evt);
		sys_free(evt->obj.res);
	}
	sys_unlock();
}

/* -------------------------------------------------------------------------- */
static
unsigned priv_evt_wait( evt_t *evt, cnt_t time, unsigned(*wait)(tsk_t**,cnt_t) )
/* -------------------------------------------------------------------------- */
{
	assert(!port_isr_context());
	assert(evt);

	return wait(&evt->obj.queue, time);
}

/* -------------------------------------------------------------------------- */
unsigned evt_waitFor( evt_t *evt, cnt_t delay )
/* -------------------------------------------------------------------------- */
{
	unsigned event;

	sys_lock();
	{
		event = priv_evt_wait(evt, delay, core_tsk_waitFor);
	}
	sys_unlock();

	return event;
}

/* -------------------------------------------------------------------------- */
unsigned evt_waitUntil( evt_t *evt, cnt_t time )
/* -------------------------------------------------------------------------- */
{
	unsigned event;

	sys_lock();
	{
		event = priv_evt_wait(evt, time, core_tsk_waitUntil);
	}
	sys_unlock();

	return event;
}

/* -------------------------------------------------------------------------- */
void evt_give( evt_t *evt, unsigned event )
/* -------------------------------------------------------------------------- */
{
	assert(evt);

	sys_lock();
	{
		core_all_wakeup(&evt->obj.queue, event);
	}
	sys_unlock();
}

/* -------------------------------------------------------------------------- */
