/******************************************************************************

    @file    StateOS: oskernel.c
    @author  Rajmund Szymanski
    @date    31.07.2018
    @brief   This file provides set of variables and functions for StateOS.

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

#include "oskernel.h"
#include "inc/ostimer.h"
#include "inc/ostask.h"

/* -------------------------------------------------------------------------- */
// SYSTEM INTERNAL SERVICES
/* -------------------------------------------------------------------------- */

static
void priv_tsk_idle( void )
{
	__WFI();
}

/* -------------------------------------------------------------------------- */

static
void priv_ctx_switchNow( void )
{
	port_ctx_switch();
	port_clr_lock(); port_set_barrier();
	port_set_lock();
}

/* -------------------------------------------------------------------------- */

static
void priv_rdy_insert( obj_t *obj, obj_t *nxt )
{
	obj_t *prv = nxt->prev;

	obj->prev = prv;
	obj->next = nxt;
	nxt->prev = obj;
	prv->next = obj;
}

/* -------------------------------------------------------------------------- */

static
void priv_rdy_remove( obj_t *obj )
{
	obj_t *nxt = obj->next;
	obj_t *prv = obj->prev;

	nxt->prev = prv;
	prv->next = nxt;
}

/* -------------------------------------------------------------------------- */
// SYSTEM TIMER SERVICES
/* -------------------------------------------------------------------------- */

tmr_t WAIT = { .obj={ .prev=&WAIT, .next=&WAIT }, .id=ID_TIMER, .delay=INFINITE }; // timers queue

/* -------------------------------------------------------------------------- */

static
void priv_tmr_insert( tmr_t *tmr, unsigned id )
{
	tmr_t *nxt = &WAIT;
	tmr->id = id;

	if (tmr->delay != INFINITE)
		do nxt = nxt->obj.next;
		while (nxt->delay < (cnt_t)(tmr->start + tmr->delay - nxt->start));

	priv_rdy_insert(&tmr->obj, &nxt->obj);
}

/* -------------------------------------------------------------------------- */

static
void priv_tmr_remove( tmr_t *tmr )
{
	priv_rdy_remove(&tmr->obj);
}

/* -------------------------------------------------------------------------- */

void core_tmr_insert( tmr_t *tmr, unsigned id )
{
	priv_tmr_insert(tmr, id);
	port_tmr_force();
}

/* -------------------------------------------------------------------------- */

void core_tmr_remove( tmr_t *tmr )
{
	tmr->id = ID_STOPPED;
	priv_tmr_remove(tmr);
}

/* -------------------------------------------------------------------------- */

#if HW_TIMER_SIZE

static
bool priv_tmr_expired( tmr_t *tmr )
{
	port_tmr_stop();

	if (tmr->delay == INFINITE)
	return false; // return if timer counting indefinitely

	if (tmr->delay <= (cnt_t)(core_sys_time() - tmr->start))
	return true;  // return if timer finished counting

	port_tmr_start((cnt_t)(tmr->start + tmr->delay));

	if (tmr->delay >  (cnt_t)(core_sys_time() - tmr->start))
	return false; // return if timer still counts

	port_tmr_stop();

	return true;  // however timer finished counting
}

/* -------------------------------------------------------------------------- */

#else

static
bool priv_tmr_expired( tmr_t *tmr )
{
	if (tmr->delay >= (cnt_t)(core_sys_time() - tmr->start + 1))
	return false; // return if timer still counts or counting indefinitely

	return true;  // timer finished counting
}

#endif

/* -------------------------------------------------------------------------- */

static
void priv_tmr_wakeup( tmr_t *tmr, unsigned event )
{
	if (tmr->state)
		tmr->state();

	core_tmr_remove(tmr);
	if (tmr->delay >= (cnt_t)(core_sys_time() - tmr->start + 1))
		priv_tmr_insert(tmr, ID_TIMER);

	core_all_wakeup(tmr, event);
}

/* -------------------------------------------------------------------------- */

void core_tmr_handler( void )
{
	tmr_t *tmr;

	core_stk_assert();

	port_set_lock();
	{
		while (priv_tmr_expired(tmr = WAIT.obj.next))
		{
			tmr->start += tmr->delay;

			if (tmr->id == ID_TIMER)
			{
				tmr->delay = tmr->period;

				priv_tmr_wakeup((tmr_t *)tmr, E_SUCCESS);
			}
			else  /* id == ID_DELAYED */
				core_tsk_wakeup((tsk_t *)tmr, E_TIMEOUT);
		}
	}
	port_clr_lock();
}

/* -------------------------------------------------------------------------- */
// SYSTEM TASK SERVICES
/* -------------------------------------------------------------------------- */

#ifndef MAIN_TOP
static  stk_t     MAIN_STK[SSIZE(OS_STACK_SIZE)];
#define MAIN_TOP (MAIN_STK+SSIZE(OS_STACK_SIZE))
#endif

static  union  { stk_t STK[SSIZE(OS_IDLE_STACK)];
        struct { char  stk[ABOVE(OS_IDLE_STACK)-sizeof(ctx_t)]; ctx_t ctx; } CTX; }
        IDLE_STACK = { .CTX = { .ctx = _CTX_INIT(core_tsk_loop) } };
#define IDLE_STK (void *)(&IDLE_STACK)
//      IDLE_TOP (stk_t*)(&IDLE_STACK+1) // because of the SDCC
#define IDLE_TOP (stk_t*)(&IDLE_STACK)+SSIZE(OS_IDLE_STACK)
#define IDLE_SP  (void *)(&IDLE_STACK.CTX.ctx)

tsk_t MAIN = { .obj={ .prev=&IDLE, .next=&IDLE }, .id=ID_READY, .top=MAIN_TOP, .basic=OS_MAIN_PRIO, .prio=OS_MAIN_PRIO }; // main task
tsk_t IDLE = { .obj={ .prev=&MAIN, .next=&MAIN }, .id=ID_IDLE, .state=priv_tsk_idle, .sp=IDLE_SP, .top=IDLE_TOP, .stack=IDLE_STK }; // idle task and tasks queue
sys_t System = { .cur=&MAIN };

/* -------------------------------------------------------------------------- */

static
void priv_tsk_insert( tsk_t *tsk )
{
	tsk_t *nxt = &IDLE;
#if OS_ROBIN && HW_TIMER_SIZE == 0
	tsk->slice = 0;
#endif
	if (tsk->prio)
		do nxt = nxt->obj.next;
		while (tsk->prio <= nxt->prio);

	priv_rdy_insert(&tsk->obj, &nxt->obj);
}

/* -------------------------------------------------------------------------- */

static
void priv_tsk_remove( tsk_t *tsk )
{
	priv_rdy_remove(&tsk->obj);
}

/* -------------------------------------------------------------------------- */

void core_tsk_insert( tsk_t *tsk )
{
	tsk->id = ID_READY;
	priv_tsk_insert(tsk);
	if (tsk == IDLE.obj.next)
		port_ctx_switch();
}

/* -------------------------------------------------------------------------- */

void core_tsk_remove( tsk_t *tsk )
{
	tsk->id = ID_STOPPED;
	priv_tsk_remove(tsk);
	if (tsk == System.cur)
		priv_ctx_switchNow();
}

/* -------------------------------------------------------------------------- */

void core_ctx_init( tsk_t *tsk )
{
#ifdef DEBUG
	memset(tsk->stack, 0xFF, (size_t)tsk->top - (size_t)tsk->stack);
#endif
	tsk->sp = (ctx_t *)tsk->top - 1;
	port_ctx_init(tsk->sp, core_tsk_loop);
}

/* -------------------------------------------------------------------------- */

void core_ctx_switch( void )
{
	tsk_t *cur = IDLE.obj.next;
	tsk_t *nxt = cur->obj.next;
	if (nxt->prio == cur->prio)
		port_ctx_switch();
}

/* -------------------------------------------------------------------------- */

void core_tsk_loop( void )
{
	for (;;)
	{
		port_clr_lock();
		System.cur->state();
		port_set_lock();
		core_ctx_switch();
	}
}

/* -------------------------------------------------------------------------- */

void core_tsk_append( tsk_t *tsk, void *obj )
{
	tsk_t *prv;
	tsk_t *nxt = obj;
	tsk->guard = obj;

	do prv = nxt, nxt = nxt->obj.queue;
	while (nxt && tsk->prio <= nxt->prio);

	if (nxt)
	nxt->back = tsk;
	tsk->back = prv;
	tsk->obj.queue = nxt;
	prv->obj.queue = tsk;
}

/* -------------------------------------------------------------------------- */

void core_tsk_unlink( tsk_t *tsk, unsigned event )
{
	tsk_t *prv = tsk->back;
	tsk_t *nxt = tsk->obj.queue;
	tsk->event = event;

	if (nxt)
	nxt->back = prv;
	prv->obj.queue = nxt;
	tsk->obj.queue = 0; // necessary because of tsk_wait[Until|For] functions
	tsk->guard = 0;
}

/* -------------------------------------------------------------------------- */

void core_tsk_transfer( tsk_t *tsk, void *obj )
{
	core_tsk_unlink(tsk, tsk->event);
	core_tsk_append(tsk, obj);
}

/* -------------------------------------------------------------------------- */

static
void priv_tsk_wait( tsk_t *tsk, void *obj )
{
	assert(!port_isr_inside());

	core_tsk_append((tsk_t *)tsk, obj);
	priv_tsk_remove((tsk_t *)tsk);
	core_tmr_insert((tmr_t *)tsk, ID_DELAYED);
}

/* -------------------------------------------------------------------------- */

unsigned core_tsk_waitUntil( void *obj, cnt_t time )
{
	tsk_t *cur = System.cur;

	cur->start = core_sys_time();
	cur->delay = time - cur->start;

	if (cur->delay > ((CNT_MAX)>>1))
		return E_TIMEOUT;

	priv_tsk_wait(cur, obj);
	priv_ctx_switchNow();

	return cur->event;
}

/* -------------------------------------------------------------------------- */

unsigned core_tsk_waitNext( void *obj, cnt_t delay )
{
	tsk_t *cur = System.cur;

	cur->delay = delay;

	if (cur->delay == IMMEDIATE)
		return E_TIMEOUT;

	priv_tsk_wait(cur, obj);
	priv_ctx_switchNow();

	return cur->event;
}

/* -------------------------------------------------------------------------- */

unsigned core_tsk_waitFor( void *obj, cnt_t delay )
{
	tsk_t *cur = System.cur;

	cur->start = core_sys_time();
	cur->delay = delay;

	if (cur->delay == IMMEDIATE)
		return E_TIMEOUT;

	priv_tsk_wait(cur, obj);
	priv_ctx_switchNow();

	return cur->event;
}

/* -------------------------------------------------------------------------- */

void core_tsk_suspend( tsk_t *tsk )
{
	tsk->delay = INFINITE;

	priv_tsk_wait(tsk, &WAIT);
	if (tsk == System.cur)
		priv_ctx_switchNow();
}

/* -------------------------------------------------------------------------- */

tsk_t *core_tsk_wakeup( tsk_t *tsk, unsigned event )
{
	if (tsk)
	{
		core_tsk_unlink((tsk_t *)tsk, event);
		core_tmr_remove((tmr_t *)tsk);
		core_tsk_insert((tsk_t *)tsk);
	}

	return tsk;
}

/* -------------------------------------------------------------------------- */

tsk_t *core_one_wakeup( void *obj, unsigned event )
{
	obj_t *lst = obj;

	return core_tsk_wakeup(lst->queue, event);
}

/* -------------------------------------------------------------------------- */

void core_all_wakeup( void *obj, unsigned event )
{
	obj_t *lst = obj;

	while (core_tsk_wakeup(lst->queue, event));
}

/* -------------------------------------------------------------------------- */

void core_tsk_prio( tsk_t *tsk, unsigned prio )
{
	mtx_t *mtx;

	if (prio < tsk->basic)
		prio = tsk->basic;

	for (mtx = tsk->mtx.list; mtx; mtx = mtx->list)
		if (mtx->queue)
			if (prio < mtx->queue->prio)
				prio = mtx->queue->prio;

	if (tsk->prio != prio)
	{
		tsk->prio = prio;

		if (tsk == System.cur)
		{
			tsk = tsk->obj.next;
			if (tsk->prio > prio)
				port_ctx_switch();
		}
		else
		if (tsk->id == ID_READY)
		{
			priv_tsk_remove(tsk);
			core_tsk_insert(tsk);
		}
		else
		if (tsk->id == ID_DELAYED)
		{
			core_tsk_transfer(tsk, tsk->guard);
			if (tsk->mtx.tree)
				core_tsk_prio(tsk->mtx.tree, prio);
		}
	}
}

/* -------------------------------------------------------------------------- */

void core_cur_prio( unsigned prio )
{
	mtx_t *mtx;
	tsk_t *tsk = System.cur;

	if (prio < tsk->basic)
		prio = tsk->basic;

	for (mtx = tsk->mtx.list; mtx; mtx = mtx->list)
		if (mtx->queue)
			if (prio < mtx->queue->prio)
				prio = mtx->queue->prio;

	if (tsk->prio != prio)
	{
		tsk->prio = prio;
		tsk = tsk->obj.next;
		if (tsk->prio > prio)
			port_ctx_switch();
	}
}

/* -------------------------------------------------------------------------- */

void *core_tsk_handler( void *sp )
{
	tsk_t *cur, *nxt;

	core_stk_assert();

	port_set_lock();
	{
		core_ctx_reset();

		cur = System.cur;
		cur->sp = sp;

		nxt = IDLE.obj.next;

#if OS_ROBIN && HW_TIMER_SIZE == 0
		if (cur == nxt || (nxt->slice >= (OS_FREQUENCY)/(OS_ROBIN) && (nxt->slice = 0) == 0))
#else
		if (cur == nxt)
#endif
		{
			priv_tsk_remove(nxt);
			priv_tsk_insert(nxt);
			nxt = IDLE.obj.next;
		}

		System.cur = nxt;
		sp = nxt->sp;
	}
	port_clr_lock();

	return sp;
}

/* -------------------------------------------------------------------------- */

#if HW_TIMER_SIZE == 0

void core_sys_tick( void )
{
	System.cnt++;
	core_tmr_handler();
	#if OS_ROBIN
	if (++System.cur->slice >= (OS_FREQUENCY)/(OS_ROBIN))
		core_ctx_switch();
	#endif
}

#endif

/* -------------------------------------------------------------------------- */
