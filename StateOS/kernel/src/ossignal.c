/******************************************************************************

    @file    StateOS: ossignal.c
    @author  Rajmund Szymanski
    @date    28.09.2018
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
#include "inc/ostask.h"
#include "inc/oscriticalsection.h"
#include "osalloc.h"

/* -------------------------------------------------------------------------- */
void sig_init( sig_t *sig, unsigned mask )
/* -------------------------------------------------------------------------- */
{
	assert_tsk_context();
	assert(sig);

	sys_lock();
	{
		memset(sig, 0, sizeof(sig_t));
		core_obj_init(&sig->obj);
		sig->mask = mask;
	}
	sys_unlock();
}

/* -------------------------------------------------------------------------- */
sig_t *sig_create( unsigned mask )
/* -------------------------------------------------------------------------- */
{
	sig_t *sig;

	assert_tsk_context();

	sys_lock();
	{
		sig = sys_alloc(sizeof(sig_t));
		sig_init(sig, mask);
		sig->obj.res = sig;
	}
	sys_unlock();

	return sig;
}

/* -------------------------------------------------------------------------- */
void sig_kill( sig_t *sig )
/* -------------------------------------------------------------------------- */
{
	assert_tsk_context();
	assert(sig);

	sys_lock();
	{
		core_all_wakeup(sig->obj.queue, E_STOPPED);

		sig->flags = 0;
	}
	sys_unlock();
}

/* -------------------------------------------------------------------------- */
void sig_delete( sig_t *sig )
/* -------------------------------------------------------------------------- */
{
	sys_lock();
	{
		sig_kill(sig);
		sys_free(sig->obj.res);
	}
	sys_unlock();
}

/* -------------------------------------------------------------------------- */
static
unsigned priv_sig_take( sig_t *sig, unsigned num )
/* -------------------------------------------------------------------------- */
{
	unsigned flag = 1U << num;

	assert(sig);

	flag &= sig->flags;
	sig->flags &= (~flag | sig->mask);
		
	return (flag != 0) ? E_SUCCESS : E_TIMEOUT;
}

/* -------------------------------------------------------------------------- */
unsigned sig_take( sig_t *sig, unsigned num )
/* -------------------------------------------------------------------------- */
{
	unsigned event;

	sys_lock();
	{
		event = priv_sig_take(sig, num);
	}
	sys_unlock();

	return event;
}

/* -------------------------------------------------------------------------- */
unsigned sig_waitFor( sig_t *sig, unsigned num, cnt_t delay )
/* -------------------------------------------------------------------------- */
{
	unsigned event;

	assert_tsk_context();

	sys_lock();
	{
		event = priv_sig_take(sig, num);

		if (event == E_TIMEOUT)
		{
			System.cur->tmp.sig.num = num;
			event = core_tsk_waitFor(&sig->obj.queue, delay);
		}
	}
	sys_unlock();

	return event;
}

/* -------------------------------------------------------------------------- */
unsigned sig_waitUntil( sig_t *sig, unsigned num, cnt_t time )
/* -------------------------------------------------------------------------- */
{
	unsigned event;

	assert_tsk_context();

	sys_lock();
	{
		event = priv_sig_take(sig, num);

		if (event == E_TIMEOUT)
		{
			System.cur->tmp.sig.num = num;
			event = core_tsk_waitUntil(&sig->obj.queue, time);
		}
	}
	sys_unlock();

	return event;
}

/* -------------------------------------------------------------------------- */
void sig_give( sig_t *sig, unsigned num )
/* -------------------------------------------------------------------------- */
{
	unsigned flag = 1U << num;
	obj_t  * obj;
	tsk_t  * tsk;

	assert(sig);

	sys_lock();
	{
		sig->flags |= flag;

		obj = &sig->obj;
		while (obj->queue)
		{
			tsk = obj->queue;
			if (tsk->tmp.sig.num == num)
			{
				sig->flags &= (~flag | sig->mask);
				core_tsk_wakeup(tsk, E_SUCCESS);
				continue;
			}
			obj = &tsk->hdr.obj;
		}
	}
	sys_unlock();
}

/* -------------------------------------------------------------------------- */
void sig_clear( sig_t *sig, unsigned num )
/* -------------------------------------------------------------------------- */
{
	unsigned flag = 1U << num;

	assert(sig);
	assert(flag);

	sys_lock();
	{
		sig->flags &= ~flag;
	}
	sys_unlock();
}

/* -------------------------------------------------------------------------- */
bool sig_get( sig_t *sig, unsigned num )
/* -------------------------------------------------------------------------- */
{
	unsigned flag = 1U << num;

	assert(sig);
	assert(flag);

	sys_lock();
	{
		flag &= sig->flags;
	}
	sys_unlock();

	return flag != 0;
}

/* -------------------------------------------------------------------------- */
