/******************************************************************************

    @file    StateOS: ostask.c
    @author  Rajmund Szymanski
    @date    15.10.2018
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

#include "inc/ostask.h"
#include "inc/ossignal.h"
#include "inc/oscriticalsection.h"
#include "osalloc.h"

/* -------------------------------------------------------------------------- */
void tsk_init( tsk_t *tsk, unsigned prio, fun_t *state, stk_t *stack, unsigned size )
/* -------------------------------------------------------------------------- */
{
	assert_tsk_context();
	assert(tsk);
	assert(state);
	assert(stack);
	assert(size);

	sys_lock();
	{
		memset(tsk, 0, sizeof(tsk_t));

		core_hdr_init(&tsk->hdr);

		tsk->prio  = prio;
		tsk->basic = prio;
		tsk->state = state;
		tsk->stack = stack;
		tsk->size  = size;

		core_ctx_init(tsk);
		core_tsk_insert(tsk);
	}
	sys_unlock();
}

/* -------------------------------------------------------------------------- */
tsk_t *wrk_create( unsigned prio, fun_t *state, unsigned size )
/* -------------------------------------------------------------------------- */
{
	tsk_t *tsk;

	assert_tsk_context();
	assert(state);
	assert(size);

	sys_lock();
	{
		tsk = sys_alloc(SEG_OVER(sizeof(tsk_t)) + size);
		tsk_init(tsk, prio, state, (void *)((size_t)tsk + SEG_OVER(sizeof(tsk_t))), size);
		tsk->hdr.obj.res = tsk;
	}
	sys_unlock();

	return tsk;
}

/* -------------------------------------------------------------------------- */
tsk_t *wrk_detached( unsigned prio, fun_t *state, unsigned size )
/* -------------------------------------------------------------------------- */
{
	tsk_t *tsk;

	assert_tsk_context();
	assert(state);
	assert(size);

	sys_lock();
	{
		tsk = sys_alloc(SEG_OVER(sizeof(tsk_t)) + size);
		tsk_init(tsk, prio, state, (void *)((size_t)tsk + SEG_OVER(sizeof(tsk_t))), size);
		tsk->hdr.obj.res = tsk;
		tsk->join = DETACHED;
	}
	sys_unlock();

	return tsk;
}

/* -------------------------------------------------------------------------- */
void tsk_start( tsk_t *tsk )
/* -------------------------------------------------------------------------- */
{
	assert_tsk_context();
	assert(tsk);
	assert(tsk->hdr.obj.res!=RELEASED); // object with released resources cannot be used
	assert(tsk->state);

	sys_lock();
	{
		if (tsk->hdr.id == ID_STOPPED)  // active tasks cannot be started
		{
			tsk->sigset = 0;

			core_ctx_init(tsk);
			core_tsk_insert(tsk);
		}
	}
	sys_unlock();
}

/* -------------------------------------------------------------------------- */
void tsk_startFrom( tsk_t *tsk, fun_t *state )
/* -------------------------------------------------------------------------- */
{
	assert_tsk_context();
	assert(tsk);
	assert(tsk->hdr.obj.res!=RELEASED); // object with released resources cannot be used
	assert(state);

	sys_lock();
	{
		if (tsk->hdr.id == ID_STOPPED)  // active tasks cannot be started
		{
			tsk->state = state;
			tsk->sigset = 0;

			core_ctx_init(tsk);
			core_tsk_insert(tsk);
		}
	}
	sys_unlock();
}

/* -------------------------------------------------------------------------- */
unsigned tsk_detach( tsk_t *tsk )
/* -------------------------------------------------------------------------- */
{
	unsigned event;

	assert_tsk_context();
	assert(tsk);
	assert(tsk->hdr.obj.res!=RELEASED);

	sys_lock();
	{
		if (tsk->join == DETACHED ||               // task has already been detached
		    tsk->hdr.obj.res == 0)                 // task is undetachable
			event = E_FAILURE;
		else
		if (tsk->hdr.id == ID_STOPPED)             // task is already inactive
		{
			core_res_free(&tsk->hdr.obj.res);      // release resources
			event = E_SUCCESS;
		}
		else                                       // task is active and can be detached
		{
			core_tsk_wakeup(tsk->join, E_FAILURE); // notify waiting task
			tsk->join = DETACHED;                  // mark as detached
			event = E_SUCCESS;
		}
	}
	sys_unlock();

	return event;
}

/* -------------------------------------------------------------------------- */
unsigned tsk_join( tsk_t *tsk )
/* -------------------------------------------------------------------------- */
{
	unsigned event;

	assert_tsk_context();
	assert(tsk);
	assert(tsk->hdr.obj.res!=RELEASED);

	sys_lock();
	{
		if (tsk->join != JOINABLE ||                        // task is unjoinable
		    tsk == System.cur)                              // deadlock detected
			event = E_FAILURE;
		else
		if (tsk->hdr.id == ID_STOPPED)                      // task is already inactive
			event = E_SUCCESS;
		else                                                // task is active
			event = core_tsk_waitFor(&tsk->join, INFINITE); // wait for termination

		if (event != E_FAILURE &&                           // task has not been detached
		    tsk->hdr.id == ID_STOPPED)                      // task is still inactive
			core_res_free(&tsk->hdr.obj.res);               // release resources
	}
	sys_unlock();

	return event;
}

/* -------------------------------------------------------------------------- */
static
void priv_mtx_remove( tsk_t *tsk )
/* -------------------------------------------------------------------------- */
{
	mtx_t *mtx;
	mtx_t *nxt;

	tsk->mtx.tree = 0;

	for (mtx = tsk->mtx.list; mtx; mtx = nxt)
	{
		nxt = mtx->list;
		if ((mtx->mode & mtxRobust))
			if (core_mtx_transferLock(mtx, OWNERDEAD) == 0)
				mtx->mode |= mtxInconsistent;
	}
}

/* -------------------------------------------------------------------------- */
static
void priv_tsk_remove( tsk_t *tsk )
/* -------------------------------------------------------------------------- */
{
	if (tsk->hdr.id == ID_READY)         // ready task
		core_tsk_remove(tsk);            // remove task from ready queue
	else
	if (tsk->hdr.id == ID_BLOCKED)       // blocked task
	{
		core_tsk_unlink(tsk, E_STOPPED); // remove task from blocked queue
		core_tmr_remove((tmr_t *)tsk);   // remove task from timers queue
	}
}

/* -------------------------------------------------------------------------- */
void idle_tsk_default( void )
/* -------------------------------------------------------------------------- */
{
	__WFI();
}

/* -------------------------------------------------------------------------- */
void idle_tsk_destructor( void )
/* -------------------------------------------------------------------------- */
{
	tsk_t *tsk;

	sys_lock();
	{
		while (System.des)
		{
			tsk = System.des;                          // task waiting for destruction

			if (tsk->join != DETACHED)                 // task not detached
			{
				priv_mtx_remove(tsk);                  // release all owned robust mutexes
				core_tsk_wakeup(tsk->join, E_FAILURE); // notify waiting task
			}

			priv_tsk_remove(tsk);                      // remove task from all queues
			core_res_free(&tsk->hdr.obj.res);          // release resources
		}

		IDLE.state = idle_tsk_default;                 // use default handler for idle process
	}
	sys_unlock();
}

/* -------------------------------------------------------------------------- */
static
void priv_tsk_destroy( void )
/* -------------------------------------------------------------------------- */
{
	idle_tsk_destructor();                   // destroy all tasks waiting for destruction

	IDLE.state = idle_tsk_destructor;        // use destructor handler for idle process
	core_tsk_waitFor(&System.des, INFINITE); // wait for destruction

	assert(!"system cannot return here");
}

/* -------------------------------------------------------------------------- */
void tsk_stop( void )
/* -------------------------------------------------------------------------- */
{
	assert_tsk_context();
	assert(System.cur->mtx.list == 0);

	port_set_lock();

	if (System.cur->join == DETACHED)             // current task is detached
		priv_tsk_destroy();                       // wait for destruction

	core_tsk_wakeup(System.cur->join, E_SUCCESS); // notify waiting task
	core_tsk_remove(System.cur);                  // remove current task from ready queue

	assert(!"system cannot return here");
	for (;;);                                     // disable unnecessary warning
}

/* -------------------------------------------------------------------------- */
unsigned tsk_reset( tsk_t *tsk )
/* -------------------------------------------------------------------------- */
{
	unsigned event;

	assert_tsk_context();
	assert(tsk);
	assert(tsk->hdr.obj.res!=RELEASED);

	sys_lock();
	{
		if (tsk->join == DETACHED)                     // detached task cannot be reseted
			event = E_FAILURE;
		else
		{
			if (tsk->hdr.id != ID_STOPPED)             // inactive task cannot be removed
			{
				priv_mtx_remove(tsk);                  // release all owned robust mutexes
				core_tsk_wakeup(tsk->join, E_STOPPED); // notify waiting task
				priv_tsk_remove(tsk);                  // remove task from all queues
			}

			event = E_SUCCESS;
		}
	}
	sys_unlock();

	return event;
}

/* -------------------------------------------------------------------------- */
unsigned tsk_delete( tsk_t *tsk )
/* -------------------------------------------------------------------------- */
{
	unsigned event;

	assert_tsk_context();
	assert(tsk);
	assert(tsk->hdr.obj.res!=RELEASED);

	sys_lock();
	{
		if (tsk->join == DETACHED)                     // detached task cannot be deleted
			event = E_FAILURE;
		else
		{
			if (tsk == System.cur)                     // current task will be destroyed by destructor
				priv_tsk_destroy();                    // wait for destruction

			if (tsk->hdr.id != ID_STOPPED)             // only active task can be removed
			{
				priv_mtx_remove(tsk);                  // release all owned robust mutexes
				core_tsk_wakeup(tsk->join, E_FAILURE); // notify waiting task             
				priv_tsk_remove(tsk);                  // remove task from all queues     
			}

			core_res_free(&tsk->hdr.obj.res);          // release resources
			event = E_SUCCESS;
		}
	}
	sys_unlock();

	return event;
}

/* -------------------------------------------------------------------------- */
void tsk_yield( void )
/* -------------------------------------------------------------------------- */
{
	assert_tsk_context();

	sys_lock();
	{
		core_ctx_switchNow();
	}
	sys_unlock();
}

/* -------------------------------------------------------------------------- */
void tsk_flip( fun_t *state )
/* -------------------------------------------------------------------------- */
{
	assert_tsk_context();
	assert(state);

	port_set_lock();

	System.cur->state = state;

	core_ctx_switch();
	core_tsk_flip((void *)STK_CROP(System.cur->stack, System.cur->size));

	assert(!"system cannot return here");
}

/* -------------------------------------------------------------------------- */
void tsk_setPrio( unsigned prio )
/* -------------------------------------------------------------------------- */
{
	assert_tsk_context();

	sys_lock();
	{
		System.cur->basic = prio;
		core_cur_prio(prio);
	}
	sys_unlock();
}

/* -------------------------------------------------------------------------- */
unsigned tsk_getPrio( void )
/* -------------------------------------------------------------------------- */
{
	unsigned prio;

	assert_tsk_context();

	sys_lock();
	{
		prio = System.cur->basic;
	}
	sys_unlock();

	return prio;
}

/* -------------------------------------------------------------------------- */
static
unsigned priv_tsk_take( unsigned sigset )
/* -------------------------------------------------------------------------- */
{
	unsigned signo;

	sigset &= System.cur->sigset;
	sigset &= -sigset;
	System.cur->sigset &= ~sigset;
	for (signo = 0; sigset; sigset >>= 1, signo++);

	return signo;
}

/* -------------------------------------------------------------------------- */
unsigned tsk_take( unsigned sigset )
/* -------------------------------------------------------------------------- */
{
	unsigned signo;

	sys_lock();
	{
		signo = priv_tsk_take(sigset);
	}
	sys_unlock();

	return signo;
}

/* -------------------------------------------------------------------------- */
unsigned tsk_waitFor( unsigned sigset, cnt_t delay )
/* -------------------------------------------------------------------------- */
{
	unsigned signo;

	assert_tsk_context();

	sys_lock();
	{
		signo = priv_tsk_take(sigset);

		if (signo == 0)
		{
			System.cur->tmp.sig.sigset = sigset;
			signo = core_tsk_waitFor(&System.sig, delay);
		}
	}
	sys_unlock();

	return signo;
}

/* -------------------------------------------------------------------------- */
unsigned tsk_waitUntil( unsigned sigset, cnt_t time )
/* -------------------------------------------------------------------------- */
{
	unsigned signo;

	assert_tsk_context();

	sys_lock();
	{
		signo = priv_tsk_take(sigset);

		if (signo == 0)
		{
			System.cur->tmp.sig.sigset = sigset;
			signo = core_tsk_waitUntil(&System.sig, time);
		}
	}
	sys_unlock();

	return signo;
}

/* -------------------------------------------------------------------------- */
void tsk_give( tsk_t *tsk, unsigned signo )
/* -------------------------------------------------------------------------- */
{
	unsigned sigset = SIGSET(signo);

	assert(tsk);
	assert(tsk->hdr.obj.res!=RELEASED);

	sys_lock();
	{
		tsk->sigset |= sigset;

		if (tsk->guard == &System.sig)
		{
			if (tsk->tmp.sig.sigset & sigset || tsk->tmp.sig.sigset == 0)
			{
				tsk->sigset &= ~sigset;
				core_tsk_wakeup(tsk, signo);
			}
		}
	}
	sys_unlock();
}

/* -------------------------------------------------------------------------- */
void tsk_sleepFor( cnt_t delay )
/* -------------------------------------------------------------------------- */
{
	sys_lock();
	{
		core_tsk_waitFor(&System.dly, delay);
	}
	sys_unlock();
}

/* -------------------------------------------------------------------------- */
void tsk_sleepNext( cnt_t delay )
/* -------------------------------------------------------------------------- */
{
	sys_lock();
	{
		core_tsk_waitNext(&System.dly, delay);
	}
	sys_unlock();
}

/* -------------------------------------------------------------------------- */
void tsk_sleepUntil( cnt_t time )
/* -------------------------------------------------------------------------- */
{
	sys_lock();
	{
		core_tsk_waitUntil(&System.dly, time);
	}
	sys_unlock();
}

/* -------------------------------------------------------------------------- */
unsigned tsk_suspend( tsk_t *tsk )
/* -------------------------------------------------------------------------- */
{
	unsigned event;

	assert(tsk);
	assert(tsk->hdr.obj.res!=RELEASED);

	sys_lock();
	{
		if (tsk->hdr.id == ID_READY)
		{
			core_tsk_suspend(tsk);
			event = E_SUCCESS;
		}
		else
			event = E_FAILURE;
	}
	sys_unlock();

	return event;
}

/* -------------------------------------------------------------------------- */
unsigned tsk_resume( tsk_t *tsk )
/* -------------------------------------------------------------------------- */
{
	unsigned event;

	assert(tsk);
	assert(tsk->hdr.obj.res!=RELEASED);

	sys_lock();
	{
		if (tsk->guard == &System.dly && tsk->delay == INFINITE)
		{
			core_tsk_wakeup(tsk, 0); // unused event value
			event = E_SUCCESS;
		}
		else
			event = E_FAILURE;
	}
	sys_unlock();

	return event;
}

/* -------------------------------------------------------------------------- */
