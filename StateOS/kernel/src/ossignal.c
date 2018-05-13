/******************************************************************************

    @file    StateOS: ossignal.c
    @author  Rajmund Szymanski
    @date    13.05.2018
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

#include "inc/ossignal.h"

/* -------------------------------------------------------------------------- */
void sig_init( sig_t *sig, unsigned type )
/* -------------------------------------------------------------------------- */
{
	assert(!port_isr_inside());
	assert(sig);

	port_sys_lock();

	memset(sig, 0, sizeof(sig_t));

	sig->type = type & sigMASK;

	port_sys_unlock();
}

/* -------------------------------------------------------------------------- */
sig_t *sig_create( unsigned type )
/* -------------------------------------------------------------------------- */
{
	sig_t *sig;

	assert(!port_isr_inside());

	port_sys_lock();

	sig = core_sys_alloc(sizeof(sig_t));
	sig_init(sig, type);
	sig->res = sig;

	port_sys_unlock();

	return sig;
}

/* -------------------------------------------------------------------------- */
void sig_kill( sig_t *sig )
/* -------------------------------------------------------------------------- */
{
	assert(!port_isr_inside());
	assert(sig);

	port_sys_lock();

	sig->flag = 0;
	
	core_all_wakeup(sig, E_STOPPED);

	port_sys_unlock();
}

/* -------------------------------------------------------------------------- */
void sig_delete( sig_t *sig )
/* -------------------------------------------------------------------------- */
{
	port_sys_lock();

	sig_kill(sig);
	core_sys_free(sig->res);

	port_sys_unlock();
}

/* -------------------------------------------------------------------------- */
unsigned sig_take( sig_t *sig )
/* -------------------------------------------------------------------------- */
{
	unsigned event = E_TIMEOUT;

	assert(sig);
	assert((sig->type & ~sigMASK) == 0U);

	port_sys_lock();

	if (sig->flag)
	{
		sig->flag = sig->type;
		event = E_SUCCESS;
	}

	port_sys_unlock();

	return event;
}

/* -------------------------------------------------------------------------- */
static
unsigned priv_sig_wait( sig_t *sig, cnt_t time, unsigned(*wait)(void*,cnt_t) )
/* -------------------------------------------------------------------------- */
{
	unsigned event;

	assert(!port_isr_inside());
	assert(sig);
	assert((sig->type & ~sigMASK) == 0U);

	port_sys_lock();

	if (sig->flag)
	{
		sig->flag = sig->type;
		event = E_SUCCESS;
	}
	else
	{
		event = wait(sig, time);
	}

	port_sys_unlock();

	return event;
}

/* -------------------------------------------------------------------------- */
unsigned sig_waitUntil( sig_t *sig, cnt_t time )
/* -------------------------------------------------------------------------- */
{
	return priv_sig_wait(sig, time, core_tsk_waitUntil);
}

/* -------------------------------------------------------------------------- */
unsigned sig_waitFor( sig_t *sig, cnt_t delay )
/* -------------------------------------------------------------------------- */
{
	return priv_sig_wait(sig, delay, core_tsk_waitFor);
}

/* -------------------------------------------------------------------------- */
void sig_give( sig_t *sig )
/* -------------------------------------------------------------------------- */
{
	assert(sig);
	assert((sig->type & ~sigMASK) == 0U);

	port_sys_lock();

	sig->flag = 1;

	if (sig->type == sigClear)
	{
		if (core_one_wakeup(sig, E_SUCCESS))
		sig->flag = 0;
	}
	else
	{
		core_all_wakeup(sig, E_SUCCESS);
	}

	port_sys_unlock();
}

/* -------------------------------------------------------------------------- */
void sig_clear( sig_t *sig )
/* -------------------------------------------------------------------------- */
{
	assert(sig);

	sig->flag = 0;
}

/* -------------------------------------------------------------------------- */
