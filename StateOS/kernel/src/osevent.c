/******************************************************************************

    @file    StateOS: osevent.c
    @author  Rajmund Szymanski
    @date    18.05.2020
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

#include "inc/osevent.h"
#include "inc/ostask.h"
#include "inc/oscriticalsection.h"
#include "osalloc.h"

/* -------------------------------------------------------------------------- */
static
void priv_evt_init( evt_t *evt, void *res )
/* -------------------------------------------------------------------------- */
{
	memset(evt, 0, sizeof(evt_t));

	core_obj_init(&evt->obj, res);
}

/* -------------------------------------------------------------------------- */
void evt_init( evt_t *evt )
/* -------------------------------------------------------------------------- */
{
	assert_tsk_context();
	assert(evt);

	sys_lock();
	{
		priv_evt_init(evt, NULL);
	}
	sys_unlock();
}

/* -------------------------------------------------------------------------- */
evt_t *evt_create( void )
/* -------------------------------------------------------------------------- */
{
	evt_t *evt;

	assert_tsk_context();

	sys_lock();
	{
		evt = sys_alloc(sizeof(evt_t));
		if (evt)
			priv_evt_init(evt, evt);
	}
	sys_unlock();

	return evt;
}

/* -------------------------------------------------------------------------- */
static
void priv_evt_reset( evt_t *evt, unsigned event )
/* -------------------------------------------------------------------------- */
{
	core_all_wakeup(evt->obj.queue, event);
}

/* -------------------------------------------------------------------------- */
void evt_reset( evt_t *evt )
/* -------------------------------------------------------------------------- */
{
	assert_tsk_context();
	assert(evt);
	assert(evt->obj.res!=RELEASED);

	sys_lock();
	{
		priv_evt_reset(evt, E_STOPPED);
	}
	sys_unlock();
}

/* -------------------------------------------------------------------------- */
void evt_destroy( evt_t *evt )
/* -------------------------------------------------------------------------- */
{
	assert_tsk_context();
	assert(evt);
	assert(evt->obj.res!=RELEASED);

	sys_lock();
	{
		priv_evt_reset(evt, evt->obj.res ? E_DELETED : E_STOPPED);
		core_res_free(&evt->obj);
	}
	sys_unlock();
}

/* -------------------------------------------------------------------------- */
unsigned evt_waitFor( evt_t *evt, unsigned *data, cnt_t delay )
/* -------------------------------------------------------------------------- */
{
	unsigned event;

	assert_tsk_context();
	assert(evt);
	assert(evt->obj.res!=RELEASED);
	assert(data);

	sys_lock();
	{
		System.cur->tmp.evt.data = data;
		event = core_tsk_waitFor(&evt->obj.queue, delay);
	}
	sys_unlock();

	return event;
}

/* -------------------------------------------------------------------------- */
unsigned evt_waitUntil( evt_t *evt, unsigned *data, cnt_t time )
/* -------------------------------------------------------------------------- */
{
	unsigned event;

	assert_tsk_context();
	assert(evt);
	assert(evt->obj.res!=RELEASED);
	assert(data);

	sys_lock();
	{
		System.cur->tmp.evt.data = data;
		event = core_tsk_waitUntil(&evt->obj.queue, time);
	}
	sys_unlock();

	return event;
}

/* -------------------------------------------------------------------------- */
void evt_give( evt_t *evt, unsigned data )
/* -------------------------------------------------------------------------- */
{
	tsk_t *tsk;

	assert(evt);
	assert(evt->obj.res!=RELEASED);

	sys_lock();
	{
		while ((tsk = evt->obj.queue) != 0)
		{
			*tsk->tmp.evt.data = data;
			core_tsk_wakeup(tsk, E_SUCCESS);
		}
	}
	sys_unlock();
}

/* -------------------------------------------------------------------------- */
