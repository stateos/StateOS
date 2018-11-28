/******************************************************************************

    @file    StateOS: ostimer.c
    @author  Rajmund Szymanski
    @date    28.11.2018
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
static
void priv_tmr_init( tmr_t *tmr, fun_t *state )
/* -------------------------------------------------------------------------- */
{
	core_hdr_init(&tmr->hdr);

	tmr->state = state;
}

/* -------------------------------------------------------------------------- */
void tmr_init( tmr_t *tmr, fun_t *state )
/* -------------------------------------------------------------------------- */
{
	assert_tsk_context();
	assert(tmr);

	sys_lock();
	{
		memset(tmr, 0, sizeof(tmr_t));
		priv_tmr_init(tmr, state);
	}
	sys_unlock();
}

/* -------------------------------------------------------------------------- */
tmr_t *tmr_create( fun_t *state )
/* -------------------------------------------------------------------------- */
{
	tmr_t *tmr;

	assert_tsk_context();

	sys_lock();
	{
		tmr = sys_alloc(sizeof(tmr_t));
		priv_tmr_init(tmr, state);
		tmr->hdr.obj.res = tmr;
	}
	sys_unlock();

	return tmr;
}

/* -------------------------------------------------------------------------- */
static
void priv_tmr_reset( tmr_t *tmr, unsigned event )
/* -------------------------------------------------------------------------- */
{
	if (tmr->hdr.id == ID_TIMER)
	{
		core_all_wakeup(tmr->hdr.obj.queue, event);
		core_tmr_remove(tmr);
	}
}

/* -------------------------------------------------------------------------- */
void tmr_reset( tmr_t *tmr )
/* -------------------------------------------------------------------------- */
{
	assert_tsk_context();
	assert(tmr);
	assert(tmr->hdr.obj.res!=RELEASED);

	sys_lock();
	{
		priv_tmr_reset(tmr, E_STOPPED);
	}
	sys_unlock();
}

/* -------------------------------------------------------------------------- */
void tmr_destroy( tmr_t *tmr )
/* -------------------------------------------------------------------------- */
{
	assert_tsk_context();
	assert(tmr);
	assert(tmr->hdr.obj.res!=RELEASED);

	sys_lock();
	{
		priv_tmr_reset(tmr, tmr->hdr.obj.res ? E_DELETED : E_STOPPED);
		core_res_free(&tmr->hdr.obj.res);
	}
	sys_unlock();
}

/* -------------------------------------------------------------------------- */
static
void priv_tmr_start( tmr_t *tmr )
/* -------------------------------------------------------------------------- */
{
	if (tmr->hdr.id == ID_TIMER)
		core_tmr_remove(tmr);
	core_tmr_insert(tmr);
}

/* -------------------------------------------------------------------------- */
void tmr_start( tmr_t *tmr, cnt_t delay, cnt_t period )
/* -------------------------------------------------------------------------- */
{
	assert_tsk_context();
	assert(tmr);
	assert(tmr->hdr.obj.res!=RELEASED);

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
	assert_tsk_context();
	assert(tmr);
	assert(tmr->hdr.obj.res!=RELEASED);

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
	assert_tsk_context();
	assert(tmr);
	assert(tmr->hdr.obj.res!=RELEASED);

	sys_lock();
	{
		tmr->delay = delay;

		priv_tmr_start(tmr);
	}
	sys_unlock();
}

/* -------------------------------------------------------------------------- */
void tmr_startUntil( tmr_t *tmr, cnt_t time )
/* -------------------------------------------------------------------------- */
{
	assert_tsk_context();
	assert(tmr);
	assert(tmr->hdr.obj.res!=RELEASED);

	sys_lock();
	{
		tmr->start = core_sys_time();
		tmr->delay = time - tmr->start;
		if (tmr->delay > ((CNT_MAX)>>1))
			tmr->delay = 0;
		tmr->period = 0;

		priv_tmr_start(tmr);
	}
	sys_unlock();
}

/* -------------------------------------------------------------------------- */
static
unsigned priv_tmr_take( tmr_t *tmr )
/* -------------------------------------------------------------------------- */
{
	if (tmr->hdr.next == 0)
		return E_FAILURE; // timer has not yet been started

	if (tmr->hdr.id == ID_STOPPED)
		return E_SUCCESS;

	return E_TIMEOUT;
}

/* -------------------------------------------------------------------------- */
unsigned tmr_take( tmr_t *tmr )
/* -------------------------------------------------------------------------- */
{
	unsigned event;

	assert(tmr);
	assert(tmr->hdr.obj.res!=RELEASED);

	sys_lock();
	{
		event = priv_tmr_take(tmr);
	}
	sys_unlock();

	return event;
}

/* -------------------------------------------------------------------------- */
unsigned tmr_waitFor( tmr_t *tmr, cnt_t delay )
/* -------------------------------------------------------------------------- */
{
	unsigned event;

	assert_tsk_context();
	assert(tmr);
	assert(tmr->hdr.obj.res!=RELEASED);

	sys_lock();
	{
		event = priv_tmr_take(tmr);

		if (event == E_TIMEOUT)
			event = core_tsk_waitFor(&tmr->hdr.obj.queue, delay);
	}
	sys_unlock();

	return event;
}

/* -------------------------------------------------------------------------- */
unsigned tmr_waitNext( tmr_t *tmr, cnt_t delay )
/* -------------------------------------------------------------------------- */
{
	unsigned event;

	assert_tsk_context();
	assert(tmr);
	assert(tmr->hdr.obj.res!=RELEASED);

	sys_lock();
	{
		event = priv_tmr_take(tmr);

		if (event == E_TIMEOUT)
			event = core_tsk_waitNext(&tmr->hdr.obj.queue, delay);
	}
	sys_unlock();

	return event;
}

/* -------------------------------------------------------------------------- */
unsigned tmr_waitUntil( tmr_t *tmr, cnt_t time )
/* -------------------------------------------------------------------------- */
{
	unsigned event;

	assert_tsk_context();
	assert(tmr);
	assert(tmr->hdr.obj.res!=RELEASED);

	sys_lock();
	{
		event = priv_tmr_take(tmr);

		if (event == E_TIMEOUT)
			event = core_tsk_waitUntil(&tmr->hdr.obj.queue, time);
	}
	sys_unlock();

	return event;
}

/* -------------------------------------------------------------------------- */
