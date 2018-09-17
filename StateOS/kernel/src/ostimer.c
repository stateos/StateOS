/******************************************************************************

    @file    StateOS: ostimer.c
    @author  Rajmund Szymanski
    @date    16.09.2018
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

#include "inc/ostimer.h"
#include "inc/oscriticalsection.h"
#include "osalloc.h"

/* -------------------------------------------------------------------------- */
void tmr_init( tmr_t *tmr, fun_t *state )
/* -------------------------------------------------------------------------- */
{
	assert(!port_isr_context());
	assert(tmr);

	sys_lock();
	{
		memset(tmr, 0, sizeof(tmr_t));

		core_hdr_init(&tmr->hdr);

		tmr->state  = state;
	}
	sys_unlock();
}

/* -------------------------------------------------------------------------- */
tmr_t *tmr_create( fun_t *state )
/* -------------------------------------------------------------------------- */
{
	tmr_t *tmr;

	assert(!port_isr_context());

	sys_lock();
	{
		tmr = sys_alloc(sizeof(tmr_t));
		tmr_init(tmr, state);
		tmr->hdr.obj.res = tmr;
	}
	sys_unlock();

	return tmr;
}

/* -------------------------------------------------------------------------- */
void tmr_kill( tmr_t *tmr )
/* -------------------------------------------------------------------------- */
{
	assert(!port_isr_context());
	assert(tmr);

	sys_lock();
	{
		if (tmr->hdr.id != ID_STOPPED)
		{
			core_all_wakeup(&tmr->hdr.obj.queue, E_STOPPED);
			core_tmr_remove(tmr);
		}
	}
	sys_unlock();
}

/* -------------------------------------------------------------------------- */
void tmr_delete( tmr_t *tmr )
/* -------------------------------------------------------------------------- */
{
	sys_lock();
	{
		tmr_kill(tmr);
		sys_free(tmr->hdr.obj.res);
	}
	sys_unlock();
}

/* -------------------------------------------------------------------------- */
static
void priv_tmr_start( tmr_t *tmr )
/* -------------------------------------------------------------------------- */
{
	assert(!port_isr_context());

	if (tmr->hdr.id != ID_STOPPED)
		core_tmr_remove(tmr);
	core_tmr_insert(tmr, ID_TIMER);
}

/* -------------------------------------------------------------------------- */
void tmr_start( tmr_t *tmr, cnt_t delay, cnt_t period )
/* -------------------------------------------------------------------------- */
{
	assert(tmr);

	sys_lock();
	{
		tmr->start  = core_sys_time();
		tmr->delay  = delay;
		tmr->period = period;

		priv_tmr_start(tmr);
	}
	sys_unlock();
}

/* -------------------------------------------------------------------------- */
void tmr_startFrom( tmr_t *tmr, cnt_t delay, cnt_t period, fun_t *proc )
/* -------------------------------------------------------------------------- */
{
	assert(tmr);

	sys_lock();
	{
		tmr->state  = proc;
		tmr->start  = core_sys_time();
		tmr->delay  = delay;
		tmr->period = period;

		priv_tmr_start(tmr);
	}
	sys_unlock();
}

/* -------------------------------------------------------------------------- */
void tmr_startNext( tmr_t *tmr, cnt_t delay )
/* -------------------------------------------------------------------------- */
{
	assert(tmr);

	sys_lock();
	{
		tmr->delay  = delay;

		priv_tmr_start(tmr);
	}
	sys_unlock();
}

/* -------------------------------------------------------------------------- */
void tmr_startUntil( tmr_t *tmr, cnt_t time )
/* -------------------------------------------------------------------------- */
{
	assert(tmr);

	sys_lock();
	{
		tmr->start  = core_sys_time();
		tmr->delay  = time - tmr->start;
		if (tmr->delay > ((CNT_MAX)>>1))
			tmr->delay = 0;
		tmr->period = 0;

		priv_tmr_start(tmr);
	}
	sys_unlock();
}

/* -------------------------------------------------------------------------- */
unsigned tmr_take( tmr_t *tmr )
/* -------------------------------------------------------------------------- */
{
	assert(tmr);

	if (tmr->hdr.id == ID_STOPPED)
		return E_SUCCESS;

	return E_TIMEOUT;
}

/* -------------------------------------------------------------------------- */
unsigned tmr_waitFor( tmr_t *tmr, cnt_t delay )
/* -------------------------------------------------------------------------- */
{
	unsigned event;

	assert(!port_isr_context());

	sys_lock();
	{
		event = tmr_take(tmr);

		if (event != E_SUCCESS)
			core_tsk_waitFor(&tmr->hdr.obj.queue, delay);
	}
	sys_unlock();

	return event;
}

/* -------------------------------------------------------------------------- */
unsigned tmr_waitNext( tmr_t *tmr, cnt_t delay )
/* -------------------------------------------------------------------------- */
{
	unsigned event;

	assert(!port_isr_context());

	sys_lock();
	{
		event = tmr_take(tmr);

		if (event != E_SUCCESS)
			core_tsk_waitNext(&tmr->hdr.obj.queue, delay);
	}
	sys_unlock();

	return event;
}

/* -------------------------------------------------------------------------- */
unsigned tmr_waitUntil( tmr_t *tmr, cnt_t time )
/* -------------------------------------------------------------------------- */
{
	unsigned event;

	assert(!port_isr_context());

	sys_lock();
	{
		event = tmr_take(tmr);

		if (event != E_SUCCESS)
			core_tsk_waitUntil(&tmr->hdr.obj.queue, time);
	}
	sys_unlock();

	return event;
}

/* -------------------------------------------------------------------------- */
