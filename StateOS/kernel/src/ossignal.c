/******************************************************************************

    @file    StateOS: ossignal.c
    @author  Rajmund Szymanski
    @date    17.11.2018
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
static
void priv_sig_reset( sig_t *sig, unsigned event )
/* -------------------------------------------------------------------------- */
{
	sig->sigset = 0;

	core_all_wakeup(sig->obj.queue, event);
}

/* -------------------------------------------------------------------------- */
void sig_reset( sig_t *sig )
/* -------------------------------------------------------------------------- */
{
	assert_tsk_context();
	assert(sig);
	assert(sig->obj.res!=RELEASED);

	sys_lock();
	{
		priv_sig_reset(sig, E_STOPPED);
	}
	sys_unlock();
}

/* -------------------------------------------------------------------------- */
void sig_destroy( sig_t *sig )
/* -------------------------------------------------------------------------- */
{
	assert_tsk_context();
	assert(sig);
	assert(sig->obj.res!=RELEASED);

	sys_lock();
	{
		priv_sig_reset(sig, sig->obj.res ? E_DELETED : E_STOPPED);
		core_res_free(&sig->obj.res);
	}
	sys_unlock();
}

/* -------------------------------------------------------------------------- */
static
unsigned priv_sig_take( sig_t *sig, unsigned sigset )
/* -------------------------------------------------------------------------- */
{
	unsigned signo = E_TIMEOUT;

	sigset &= sig->sigset;
	sigset &= -sigset;
	sig->sigset &= ~sigset | sig->mask;
	if (sigset)
		for (signo = 0; sigset >>= 1; signo++);

	return signo;
}

/* -------------------------------------------------------------------------- */
unsigned sig_take( sig_t *sig, unsigned sigset )
/* -------------------------------------------------------------------------- */
{
	unsigned signo;

	assert(sig);
	assert(sig->obj.res!=RELEASED);

	sys_lock();
	{
		signo = priv_sig_take(sig, sigset);
	}
	sys_unlock();

	return signo;
}

/* -------------------------------------------------------------------------- */
unsigned sig_waitFor( sig_t *sig, unsigned sigset, cnt_t delay )
/* -------------------------------------------------------------------------- */
{
	unsigned signo;

	assert_tsk_context();
	assert(sig);
	assert(sig->obj.res!=RELEASED);

	sys_lock();
	{
		signo = priv_sig_take(sig, sigset);

		if (signo == E_TIMEOUT)
		{
			System.cur->tmp.sig.sigset = sigset;
			signo = core_tsk_waitFor(&sig->obj.queue, delay);
		}
	}
	sys_unlock();

	return signo;
}

/* -------------------------------------------------------------------------- */
unsigned sig_waitUntil( sig_t *sig, unsigned sigset, cnt_t time )
/* -------------------------------------------------------------------------- */
{
	unsigned signo;

	assert_tsk_context();
	assert(sig);
	assert(sig->obj.res!=RELEASED);

	sys_lock();
	{
		signo = priv_sig_take(sig, sigset);

		if (signo == E_TIMEOUT)
		{
			System.cur->tmp.sig.sigset = sigset;
			signo = core_tsk_waitUntil(&sig->obj.queue, time);
		}
	}
	sys_unlock();

	return signo;
}

/* -------------------------------------------------------------------------- */
void sig_give( sig_t *sig, unsigned signo )
/* -------------------------------------------------------------------------- */
{
	unsigned sigset = SIGSET(signo);
	obj_t  * obj;
	tsk_t  * tsk;

	assert(sig);
	assert(sig->obj.res!=RELEASED);

	sys_lock();
	{
		sig->sigset |= sigset;

		obj = &sig->obj;
		while (obj->queue)
		{
			tsk = obj->queue;
			if ((tsk->tmp.sig.sigset & sigset) != 0 || tsk->tmp.sig.sigset == 0)
			{
				sig->sigset &= ~sigset | sig->mask;
				core_tsk_wakeup(tsk, signo);
				continue;
			}
			obj = &tsk->hdr.obj;
		}
	}
	sys_unlock();
}

/* -------------------------------------------------------------------------- */
void sig_clear( sig_t *sig, unsigned signo )
/* -------------------------------------------------------------------------- */
{
	unsigned sigset = SIGSET(signo);

	assert(sig);
	assert(sig->obj.res!=RELEASED);
	assert(sigset);

	sys_lock();
	{
		sig->sigset &= ~sigset;
	}
	sys_unlock();
}

/* -------------------------------------------------------------------------- */
