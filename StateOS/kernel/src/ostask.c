/******************************************************************************

    @file    StateOS: ostask.c
    @author  Rajmund Szymanski
    @date    26.06.2020
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

#include "inc/ostask.h"
#include "inc/ossignal.h"
#include "inc/oscriticalsection.h"

/* -------------------------------------------------------------------------- */
static
void priv_wrk_init( tsk_t *tsk, unsigned prio, fun_t *state, stk_t *stack, size_t size, void *res, bool detached )
/* -------------------------------------------------------------------------- */
{
	memset(tsk, 0, sizeof(tsk_t));

	core_hdr_init(&tsk->hdr, res);

	tsk->prio  = prio;
	tsk->basic = prio;
	tsk->state = state;
	tsk->stack = stack;
	tsk->size  = size;
	tsk->owner = detached ? tsk : NULL;
}

/* -------------------------------------------------------------------------- */
static
tsk_t *priv_wrk_create( unsigned prio, fun_t *state, size_t size, bool detached )
/* -------------------------------------------------------------------------- */
{
	struct tsk_T { tsk_t tsk; stk_t buf[]; } *tmp;
	tsk_t *tsk = NULL;
	size_t bufsize;

	bufsize = STK_OVER(size);
	tmp = malloc(sizeof(struct tsk_T) + bufsize);
	if (tmp)
		priv_wrk_init(tsk = &tmp->tsk, prio, state, tmp->buf, bufsize, tmp, detached);

	return tsk;
}

/* -------------------------------------------------------------------------- */
void wrk_init( tsk_t *tsk, unsigned prio, fun_t *state, stk_t *stack, size_t size )
/* -------------------------------------------------------------------------- */
{
	assert_tsk_context();
	assert(tsk);
	assert(state);
	assert(stack);
	assert(size>sizeof(ctx_t));

	sys_lock();
	{
		priv_wrk_init(tsk, prio, state, stack, size, NULL, false);
	}
	sys_unlock();
}

/* -------------------------------------------------------------------------- */
void tsk_init( tsk_t *tsk, unsigned prio, fun_t *state, stk_t *stack, size_t size )
/* -------------------------------------------------------------------------- */
{
	assert_tsk_context();
	assert(tsk);
	assert(state);
	assert(stack);
	assert(size>sizeof(ctx_t));

	sys_lock();
	{
		priv_wrk_init(tsk, prio, state, stack, size, NULL, false);
		core_ctx_init(tsk);
		core_tsk_insert(tsk);
	}
	sys_unlock();
}

/* -------------------------------------------------------------------------- */
tsk_t *wrk_create( unsigned prio, fun_t *state, size_t size )
/* -------------------------------------------------------------------------- */
{
	tsk_t *tsk;

	assert_tsk_context();
	assert(state);
	assert(size>sizeof(ctx_t));

	sys_lock();
	{
		tsk = priv_wrk_create(prio, state, size, false);
		if (tsk)
		{
			core_ctx_init(tsk);
			core_tsk_insert(tsk);
		}
	}
	sys_unlock();

	return tsk;
}

/* -------------------------------------------------------------------------- */
tsk_t *wrk_detached( unsigned prio, fun_t *state, size_t size )
/* -------------------------------------------------------------------------- */
{
	tsk_t *tsk;

	assert_tsk_context();
	assert(state);
	assert(size>sizeof(ctx_t));

	sys_lock();
	{
		tsk = priv_wrk_create(prio, state, size, true);
		if (tsk)
		{
			core_ctx_init(tsk);
			core_tsk_insert(tsk);
		}
	}
	sys_unlock();

	return tsk;
}


/* -------------------------------------------------------------------------- */
tsk_t *thd_create( unsigned prio, fun_t *state, size_t size, bool detached )
/* -------------------------------------------------------------------------- */
{
	tsk_t *tsk;

	assert_tsk_context();
	assert(state);
	assert(size>sizeof(ctx_t));

	sys_lock();
	{
		tsk = priv_wrk_create(prio, state, size, detached);
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

			core_ctx_init(tsk);
			core_tsk_insert(tsk);
		}
	}
	sys_unlock();
}

/* -------------------------------------------------------------------------- */
int tsk_detach( tsk_t *tsk )
/* -------------------------------------------------------------------------- */
{
	int result;

	assert_tsk_context();
	assert(tsk);
	assert(tsk->hdr.obj.res!=RELEASED);

	sys_lock();
	{
		if (tsk->owner == tsk ||                    // task has already been detached
		    tsk->hdr.obj.res == NULL)               // task is undetachable
			result = E_FAILURE;
		else
		if (tsk->hdr.id == ID_STOPPED)              // task is already inactive
		{
			core_res_free(&tsk->hdr.obj);           // release resources
			result = E_SUCCESS;
		}
		else                                        // task is active and can be detached
		{
			core_tsk_wakeup(tsk->owner, E_FAILURE); // notify waiting task
			tsk->owner = tsk;                       // mark as detached
			result = E_SUCCESS;
		}
	}
	sys_unlock();

	return result;
}

/* -------------------------------------------------------------------------- */
int tsk_join( tsk_t *tsk )
/* -------------------------------------------------------------------------- */
{
	int result;

	assert_tsk_context();
	assert(tsk);
	assert(tsk->hdr.obj.res!=RELEASED);

	sys_lock();
	{
		if (tsk->owner != NULL ||                             // task is unjoinable
		    tsk == System.cur)                                // deadlock detected
			result = E_FAILURE;
		else
		if (tsk->hdr.id == ID_STOPPED)                        // task is already inactive
			result = E_SUCCESS;
		else                                                  // task is active
			result = core_tsk_waitFor(&tsk->owner, INFINITE); // wait for termination

		if (result != E_FAILURE &&                            // task has not been detached
		    result != E_DELETED &&                            // task has not been deleted
		    tsk->hdr.id == ID_STOPPED)                        // task is still inactive
			core_res_free(&tsk->hdr.obj);                     // release resources
	}
	sys_unlock();

	return result;
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
		if ((mtx->mode & mtxRobust) == 0)
			core_mtx_reset(mtx, E_STOPPED);
		else
		if (core_mtx_transferLock(mtx, OWNERDEAD) == 0)
			mtx->mode |= mtxInconsistent;
	}
}

/* -------------------------------------------------------------------------- */
static
void priv_tsk_stop( tsk_t *tsk )
/* -------------------------------------------------------------------------- */
{
	if (tsk->guard != 0)                 // blocked task
	{
		core_tsk_unlink(tsk, 0);         // remove task from blocked queue; ignored event value
		core_tmr_remove((tmr_t *)tsk);   // remove task from timers queue
	}
	else
//	if (tsk->hdr.id == ID_READY)         // ready task
		core_tsk_remove(tsk);            // remove task from ready queue
}

/* -------------------------------------------------------------------------- */
static
void priv_tsk_idle( void )
/* -------------------------------------------------------------------------- */
{
	sys_lock();
	{
		core_tsk_deleter();              // call garbage collection procedure
		IDLE.state = core_tsk_idle;      // restore default idle procedure
	}
	sys_unlock();
}

/* -------------------------------------------------------------------------- */
static
void priv_tsk_destroy( void )
/* -------------------------------------------------------------------------- */
{
	core_tsk_deleter();                  // call garbage collection procedure
	IDLE.state = priv_tsk_idle;          // set task deleter as idle procedure
	core_tsk_waitFor(&IDLE.hdr.obj.queue, INFINITE); // wait for removal

	assert(!"system cannot return here");
}

/* -------------------------------------------------------------------------- */
static
void priv_sig_reset( tsk_t *tsk )
/* -------------------------------------------------------------------------- */
{
	tsk->sig.sigset = 0;
	tsk->sig.backup.sp = 0;
}

/* -------------------------------------------------------------------------- */
void tsk_stop( void )
/* -------------------------------------------------------------------------- */
{
	assert_tsk_context();
	assert(System.cur->mtx.list == 0);

	port_set_lock();

	priv_sig_reset(System.cur);                    // reset signal variables of current task
//	priv_mtx_remove(tsk);                          // release all owned robust mutexes

	if (System.cur->owner == System.cur)           // current task is detached
		priv_tsk_destroy();                        // wait for destruction

	core_tsk_wakeup(System.cur->owner, E_SUCCESS); // notify waiting task
	core_tsk_remove(System.cur);                   // remove current task from ready queue

	assert(!"system cannot return here");
	for (;;);                                      // disable unnecessary warning
}

/* -------------------------------------------------------------------------- */
int tsk_reset( tsk_t *tsk )
/* -------------------------------------------------------------------------- */
{
	int result;

	assert_tsk_context();
	assert(tsk);
	assert(tsk->hdr.obj.res!=RELEASED);

	sys_lock();
	{
		if (tsk->owner == tsk)                          // detached task cannot be reseted
			result = E_FAILURE;
		else
		{
			priv_sig_reset(tsk);                        // reset task signal variables

			if (tsk->hdr.id != ID_STOPPED)              // inactive task cannot be removed
			{
				priv_mtx_remove(tsk);                   // release all owned robust mutexes
				core_tsk_wakeup(tsk->owner, E_STOPPED); // notify waiting task
				priv_tsk_stop(tsk);                     // remove task from all queues
			}

			result = E_SUCCESS;
		}
	}
	sys_unlock();

	return result;
}

/* -------------------------------------------------------------------------- */
int tsk_destroy( tsk_t *tsk )
/* -------------------------------------------------------------------------- */
{
	int result;

	assert_tsk_context();
	assert(tsk);
	assert(tsk->hdr.obj.res!=RELEASED);

	sys_lock();
	{
		if (tsk->owner == tsk)                          // detached task cannot be deleted
			result = E_FAILURE;
		else
		{
			priv_sig_reset(tsk);                        // reset task signal variables

			if (tsk->hdr.id != ID_STOPPED)              // only active task can be removed
			{
				priv_mtx_remove(tsk);                   // release all owned robust mutexes
				core_tsk_wakeup(tsk->owner, E_DELETED); // notify waiting task

				if (tsk == System.cur)                  // current task will be destroyed by destructor
					priv_tsk_destroy();                 // wait for destruction

				priv_tsk_stop(tsk);                     // remove task from all queues
			}

			core_res_free(&tsk->hdr.obj);               // release resources
			result = E_SUCCESS;
		}
	}
	sys_unlock();

	return result;
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

	priv_sig_reset(System.cur); // reset signal variables of current task
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
void tsk_sleepFor( cnt_t delay )
/* -------------------------------------------------------------------------- */
{
	sys_lock();
	{
		core_tsk_waitFor(&WAIT.hdr.obj.queue, delay);
	}
	sys_unlock();
}

/* -------------------------------------------------------------------------- */
void tsk_sleepNext( cnt_t delay )
/* -------------------------------------------------------------------------- */
{
	sys_lock();
	{
		core_tsk_waitNext(&WAIT.hdr.obj.queue, delay);
	}
	sys_unlock();
}

/* -------------------------------------------------------------------------- */
void tsk_sleepUntil( cnt_t time )
/* -------------------------------------------------------------------------- */
{
	sys_lock();
	{
		core_tsk_waitUntil(&WAIT.hdr.obj.queue, time);
	}
	sys_unlock();
}

/* -------------------------------------------------------------------------- */
int tsk_suspend( tsk_t *tsk )
/* -------------------------------------------------------------------------- */
{
	int result;

	assert(tsk);
	assert(tsk->hdr.obj.res!=RELEASED);

	sys_lock();
	{
		if (tsk->hdr.id == ID_READY && tsk->guard == 0)
		{
			core_tsk_suspend(tsk);
			result = E_SUCCESS;
		}
		else
			result = E_FAILURE;
	}
	sys_unlock();

	return result;
}

/* -------------------------------------------------------------------------- */
int tsk_resume( tsk_t *tsk )
/* -------------------------------------------------------------------------- */
{
	int result;

	assert(tsk);
	assert(tsk->hdr.obj.res!=RELEASED);

	sys_lock();
	{
		if (tsk->guard == &WAIT.hdr.obj.queue && tsk->delay == INFINITE)
		{
			core_tsk_wakeup(tsk, 0); // ignored event value
			result = E_SUCCESS;
		}
		else
			result = E_FAILURE;
	}
	sys_unlock();

	return result;
}

/* -------------------------------------------------------------------------- */
static
void priv_sig_handler( tsk_t *tsk )
/* -------------------------------------------------------------------------- */
{
	unsigned signo;
	unsigned sigset;
	act_t  * action = tsk->sig.action;

	while (tsk->sig.sigset)
	{
		sigset = tsk->sig.sigset;
		sigset &= -sigset;
		tsk->sig.sigset &= ~sigset;

		port_clr_lock();
		{
			if (action)
			{
				for (signo = 0; sigset >>= 1; signo++);
				action(signo);
			}
		}
		port_set_lock();
	}
}

/* -------------------------------------------------------------------------- */
static
void priv_sig_deliver( void )
/* -------------------------------------------------------------------------- */
{
	tsk_t *cur = System.cur;

	port_set_lock();

	priv_sig_handler(cur);

	cur->sp = cur->sig.backup.sp;
	cur->sig.backup.sp = 0;

	core_tsk_wait(cur, cur->sig.backup.guard);

	assert(!"system cannot return here");
}

/* -------------------------------------------------------------------------- */
static
void priv_sig_dispatch( tsk_t *tsk )
/* -------------------------------------------------------------------------- */
{
	if (tsk->sig.backup.sp)
		return;

	if (tsk == System.cur)
	{
		priv_sig_handler(tsk);
		return;
	}

	tsk->sig.backup.sp = tsk->sp;
	tsk->sig.backup.guard = tsk->guard;

	tsk->sp = (ctx_t *)tsk->sp - 1;
	assert_ctx_integrity(tsk, tsk->sp);
	port_ctx_init(tsk->sp, priv_sig_deliver);

	if (tsk->guard)
		core_tsk_wakeup(tsk, 0);
}

/* -------------------------------------------------------------------------- */
void tsk_give( tsk_t *tsk, unsigned signo )
/* -------------------------------------------------------------------------- */
{
	unsigned sigset = SIGSET(signo);

	assert_tsk_context();
	assert(tsk);
	assert(tsk->hdr.obj.res!=RELEASED);
	assert(sigset);

	sys_lock();
	{
		if (tsk->hdr.id == ID_READY)
		{
			tsk->sig.sigset |= sigset;
			if (tsk->sig.sigset && tsk->sig.action)
				priv_sig_dispatch(tsk);
		}
	}
	sys_unlock();
}

/* -------------------------------------------------------------------------- */
void tsk_action( tsk_t *tsk, act_t *action )
/* -------------------------------------------------------------------------- */
{
	assert_tsk_context();
	assert(tsk);
	assert(tsk->hdr.obj.res!=RELEASED);

	sys_lock();
	{
		if (tsk->hdr.id == ID_READY)
		{
			tsk->sig.action = action;
			if (tsk->sig.action && tsk->sig.sigset)
				priv_sig_dispatch(tsk);
		}
	}
	sys_unlock();
}

/* -------------------------------------------------------------------------- */
