/******************************************************************************

    @file    StateOS: osnotification.c
    @author  Rajmund Szymanski
    @date    07.09.2018
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

#include "inc/osnotification.h"
#include "inc/oscriticalsection.h"
#include "osalloc.h"

/* -------------------------------------------------------------------------- */
void nfo_init( nfo_t *nfo )
/* -------------------------------------------------------------------------- */
{
	assert(!port_isr_context());
	assert(nfo);

	sys_lock();
	{
		memset(nfo, 0, sizeof(nfo_t));
	}
	sys_unlock();
}

/* -------------------------------------------------------------------------- */
nfo_t *nfo_create( void )
/* -------------------------------------------------------------------------- */
{
	nfo_t *nfo;

	assert(!port_isr_context());

	sys_lock();
	{
		nfo = sys_alloc(sizeof(nfo_t));
		nfo_init(nfo);
		nfo->obj.res = nfo;
	}
	sys_unlock();

	return nfo;
}

/* -------------------------------------------------------------------------- */
void nfo_kill( nfo_t *nfo )
/* -------------------------------------------------------------------------- */
{
	assert(!port_isr_context());
	assert(nfo);

	sys_lock();
	{
		core_all_wakeup(&nfo->obj.queue, E_STOPPED);
	}
	sys_unlock();
}

/* -------------------------------------------------------------------------- */
void nfo_delete( nfo_t *nfo )
/* -------------------------------------------------------------------------- */
{
	sys_lock();
	{
		nfo_kill(nfo);
		sys_free(nfo->obj.res);
	}
	sys_unlock();
}

/* -------------------------------------------------------------------------- */
static
unsigned priv_nfo_wait( nfo_t *nfo, cnt_t time, unsigned(*wait)(tsk_t**,cnt_t) )
/* -------------------------------------------------------------------------- */
{
	assert(!port_isr_context());
	assert(nfo);

	return wait(&nfo->obj.queue, time);
}

/* -------------------------------------------------------------------------- */
unsigned nfo_waitFor( nfo_t *nfo, cnt_t delay )
/* -------------------------------------------------------------------------- */
{
	unsigned event;

	sys_lock();
	{
		event = priv_nfo_wait(nfo, delay, core_tsk_waitFor);
	}
	sys_unlock();

	return event;
}

/* -------------------------------------------------------------------------- */
unsigned nfo_waitUntil( nfo_t *nfo, cnt_t time )
/* -------------------------------------------------------------------------- */
{
	unsigned event;

	sys_lock();
	{
		event = priv_nfo_wait(nfo, time, core_tsk_waitUntil);
	}
	sys_unlock();

	return event;
}

/* -------------------------------------------------------------------------- */
void nfo_give( nfo_t *nfo, unsigned event )
/* -------------------------------------------------------------------------- */
{
	assert(nfo);

	sys_lock();
	{
		core_all_wakeup(&nfo->obj.queue, event);
	}
	sys_unlock();
}

/* -------------------------------------------------------------------------- */
