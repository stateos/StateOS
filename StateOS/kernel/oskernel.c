/******************************************************************************

    @file    StateOS: oskernel.c
    @author  Rajmund Szymanski
    @date    01.12.2017
    @brief   This file provides set of variables and functions for StateOS.

 ******************************************************************************

    StateOS - Copyright (C) 2013 Rajmund Szymanski.

    This file is part of StateOS distribution.

    StateOS is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published
    by the Free Software Foundation; either version 3 of the License,
    or (at your option) any later version.

    StateOS is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program. If not, see <http://www.gnu.org/licenses/>.

 ******************************************************************************/

#include "oskernel.h"
#include "inc/os_tmr.h"
#include "inc/os_tsk.h"

/* -------------------------------------------------------------------------- */
// SYSTEM INTERNAL SERVICES
/* -------------------------------------------------------------------------- */

static
void priv_tsk_idle( void )
{
#if OS_ROBIN || OS_TICKLESS == 0
	__WFI();
#endif
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

tmr_t WAIT = { .obj={ .id=ID_TIMER, .prev=&WAIT, .next=&WAIT }, .delay=INFINITE }; // timers queue

/* -------------------------------------------------------------------------- */

static
void priv_tmr_insert( tmr_t *tmr, unsigned id )
{
	tmr_t *nxt = &WAIT;
	tmr->obj.id = id;

	if    (tmr->delay != INFINITE)
	do     nxt = nxt->obj.next;
	while (nxt->delay != INFINITE &&
	       nxt->delay <= tmr->start + tmr->delay - nxt->start);

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
	tmr->obj.id = ID_STOPPED;
	priv_tmr_remove(tmr);
}

/* -------------------------------------------------------------------------- */

#if OS_ROBIN && OS_TICKLESS

static
bool priv_tmr_expired( tmr_t *tmr )
{
	port_tmr_stop();

	if (tmr->delay == INFINITE)
	return false; // return if timer counting indefinitely

	if (tmr->delay <= Counter - tmr->start)
	return true;  // return if timer finished counting

	port_tmr_start(tmr->start + tmr->delay);

	if (tmr->delay >  Counter - tmr->start)
	return false; // return if timer still counts

	port_tmr_stop();

	return true;  // however timer finished counting
}

/* -------------------------------------------------------------------------- */

#else

static
bool priv_tmr_expired( tmr_t *tmr )
{
	if (tmr->delay >= Counter - tmr->start + 1)
	return false; // return if timer still counts or counting indefinitely

	return true;  // timer finished counting
}

#endif

/* -------------------------------------------------------------------------- */

static
void priv_tmr_wakeup( tmr_t *tmr, unsigned event )
{
	tmr->start += tmr->delay;
	tmr->delay  = tmr->period;

	if (tmr->state)
		tmr->state();

	core_tmr_remove(tmr);
	if (tmr->delay)
		priv_tmr_insert(tmr, ID_TIMER);

	core_all_wakeup(tmr, event);
}

/* -------------------------------------------------------------------------- */

void core_tmr_handler( void )
{
	tmr_t *tmr;

	core_stk_assert();

	port_isr_lock();

	while (priv_tmr_expired(tmr = WAIT.obj.next))
	{
		if (tmr->obj.id == ID_TIMER)
			priv_tmr_wakeup((tmr_t *)tmr, E_SUCCESS);

		else      /* id == ID_DELAYED */
			core_tsk_wakeup((tsk_t *)tmr, E_TIMEOUT);
	}

	port_isr_unlock();
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

tsk_t MAIN = { .obj={ .id=ID_READY, .prev=&IDLE, .next=&IDLE }, .top=MAIN_TOP, .basic=OS_MAIN_PRIO, .prio=OS_MAIN_PRIO }; // main task
tsk_t IDLE = { .obj={ .id=ID_IDLE,  .prev=&MAIN, .next=&MAIN }, .state=priv_tsk_idle, .sp=IDLE_SP, .top=IDLE_TOP, .stack=IDLE_STK }; // idle task and tasks queue
sys_t System = { .cur=&MAIN };

/* -------------------------------------------------------------------------- */

static
void priv_tsk_insert( tsk_t *tsk )
{
	tsk_t *nxt = &IDLE;
#if OS_ROBIN && OS_TICKLESS == 0
	tsk->slice = 0;
#endif
	if    (tsk->prio)
	do     nxt = nxt->obj.next;
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
	tsk->obj.id = ID_READY;
	priv_tsk_insert(tsk);
	if (tsk == IDLE.obj.next)
		port_ctx_switch();
}

/* -------------------------------------------------------------------------- */

void core_tsk_remove( tsk_t *tsk )
{
	tsk->obj.id = ID_STOPPED;
	priv_tsk_remove(tsk);
	if (tsk == Current)
		port_ctx_switchNow();
}

/* -------------------------------------------------------------------------- */

void core_ctx_init( tsk_t *tsk )
{
#if OS_ASSERT
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
		Current->state();
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
	tsk->evt.event = event;

	if (nxt)
	nxt->back = prv;
	prv->obj.queue = nxt;
	tsk->obj.queue = 0; // necessary because of tsk_wait[Until|For] functions
}

/* -------------------------------------------------------------------------- */

static
void priv_tsk_wait( tsk_t *tsk, void *obj )
{
	core_tsk_append((tsk_t *)tsk, obj);
	priv_tsk_remove((tsk_t *)tsk);
	core_tmr_insert((tmr_t *)tsk, ID_DELAYED);
}

/* -------------------------------------------------------------------------- */

unsigned core_tsk_waitUntil( void *obj, uint32_t time )
{
	tsk_t *cur = Current;

	cur->start = Counter;
	cur->delay = time - cur->start;

	if (cur->delay == IMMEDIATE)
	return E_TIMEOUT;

	priv_tsk_wait(cur, obj);
	port_ctx_switchLock();

	return cur->evt.event;
}

/* -------------------------------------------------------------------------- */

unsigned core_tsk_waitFor( void *obj, uint32_t delay )
{
	tsk_t *cur = Current;

	cur->start = Counter;
	cur->delay = delay;

	if (cur->delay == IMMEDIATE)
	return E_TIMEOUT;

	priv_tsk_wait(cur, obj);
	port_ctx_switchLock();

	return cur->evt.event;
}

/* -------------------------------------------------------------------------- */

void core_tsk_suspend( tsk_t *tsk )
{
	tsk->delay = INFINITE;

	priv_tsk_wait(tsk, &WAIT);
	if (tsk == Current)
		port_ctx_switchLock();
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

	for (mtx = tsk->mlist; mtx; mtx = mtx->list)
		if (mtx->queue)
			if (prio < mtx->queue->prio)
				prio = mtx->queue->prio;

	if (tsk->prio != prio)
	{
		tsk->prio = prio;

		if (tsk == Current)
		{
			tsk = tsk->obj.next;
			if (tsk->prio > prio)
				port_ctx_switch();
		}
		else
		if (tsk->obj.id == ID_READY)
		{
			priv_tsk_remove(tsk);
			core_tsk_insert(tsk);
		}
		else
		if (tsk->obj.id == ID_DELAYED)
		{
			core_tsk_unlink(tsk, 0);
			core_tsk_append(tsk, tsk->guard);
			if (tsk->mtree)
				core_tsk_prio(tsk->mtree, prio);
		}
	}
}

/* -------------------------------------------------------------------------- */

void core_cur_prio( unsigned prio )
{
	mtx_t *mtx;
	tsk_t *tsk = Current;
	
	if (prio < tsk->basic)
		prio = tsk->basic;

	for (mtx = tsk->mlist; mtx; mtx = mtx->list)
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

#if OS_ROBIN == 0
	core_tmr_handler();
#else
	core_stk_assert();
#endif
	port_isr_lock();
	core_ctx_reset();

	cur = Current;
	cur->sp = sp;

	nxt = IDLE.obj.next;

#if OS_ROBIN && OS_TICKLESS == 0
	if (cur == nxt || (nxt->slice >= OS_FREQUENCY/OS_ROBIN && (nxt->slice = 0) == 0))
#else
	if (cur == nxt)
#endif
	{
		priv_tsk_remove(nxt);
		priv_tsk_insert(nxt);
		nxt = IDLE.obj.next;
	}

	Current = nxt;
	sp = nxt->sp;

	port_isr_unlock();

	return sp;
}

/* -------------------------------------------------------------------------- */

#if OS_TICKLESS == 0

void core_sys_tick( void )
{
	System.cnt++;
	#if OS_ROBIN
	core_tmr_handler();
	if (++System.cur->slice >= OS_FREQUENCY/OS_ROBIN)
		core_ctx_switch();
	#endif
}

#endif

/* -------------------------------------------------------------------------- */
