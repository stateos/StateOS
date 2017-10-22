/******************************************************************************

    @file    StateOS: os_mem.c
    @author  Rajmund Szymanski
    @date    22.10.2017
    @brief   This file provides set of functions for StateOS.

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

#include "inc/os_mem.h"
#include "inc/os_tsk.h"

/* -------------------------------------------------------------------------- */
void mem_bind( mem_t *mem )
/* -------------------------------------------------------------------------- */
{
	void   **ptr;
	unsigned cnt;

	assert(!port_isr_inside());
	assert(mem);
	assert(mem->limit);
	assert(mem->size);
	assert(mem->data);

	port_sys_lock();
	
	ptr = mem->data;
	cnt = mem->limit;

	mem->next = 0;
	while (cnt--) { mem_give(mem, ++ptr); ptr += mem->size; }

	port_sys_unlock();
}

/* -------------------------------------------------------------------------- */
void mem_init( mem_t *mem, unsigned limit, unsigned size, void *data )
/* -------------------------------------------------------------------------- */
{
	assert(!port_isr_inside());
	assert(mem);
	assert(limit);
	assert(size);
	assert(data);

	port_sys_lock();

	memset(mem, 0, sizeof(mem_t));

	mem->limit = limit;
	mem->size  = size;
	mem->data  = data;

	mem_bind(mem);

	port_sys_unlock();
}

/* -------------------------------------------------------------------------- */
mem_t *mem_create( unsigned limit, unsigned size )
/* -------------------------------------------------------------------------- */
{
	mem_t *mem;

	assert(!port_isr_inside());
	assert(limit);
	assert(size);

	size = MSIZE(size);

	port_sys_lock();

	mem = core_sys_alloc(ABOVE(sizeof(mem_t)) + limit * (1 + size) * sizeof(void*));
	mem_init(mem, limit, size, (void *)ABOVE(mem + 1));
	mem->res = mem;

	port_sys_unlock();

	return mem;
}

/* -------------------------------------------------------------------------- */
void mem_kill( mem_t *mem )
/* -------------------------------------------------------------------------- */
{
	assert(!port_isr_inside());
	assert(mem);

	port_sys_lock();

	core_all_wakeup(mem, E_STOPPED);

	port_sys_unlock();
}

/* -------------------------------------------------------------------------- */
void mem_delete( mem_t *mem )
/* -------------------------------------------------------------------------- */
{
	port_sys_lock();

	mem_kill(mem);
	core_sys_free(mem->res);

	port_sys_unlock();
}

/* -------------------------------------------------------------------------- */
static
unsigned priv_mem_wait( mem_t *mem, void **data, uint32_t time, unsigned(*wait)(void*,uint32_t) )
/* -------------------------------------------------------------------------- */
{
	void   **ptr;
	unsigned cnt;
	unsigned event = E_SUCCESS;

	assert(mem);
	assert(data);

	port_sys_lock();

	if (mem->next)
	{
		*data = mem->next + 1;
		mem->next = mem->next->next;
	}
	else
	{
		Current->tmp.data = data;
		event = wait(mem, time);
	}
	
	if (event == E_SUCCESS)
	{
		ptr = *data;
		cnt = mem->size;
		while (cnt--) *ptr++ = 0;
	}

	port_sys_unlock();

	return event;
}

/* -------------------------------------------------------------------------- */
unsigned mem_waitUntil( mem_t *mem, void **data, uint32_t time )
/* -------------------------------------------------------------------------- */
{
	assert(!port_isr_inside());

	return priv_mem_wait(mem, data, time, core_tsk_waitUntil);
}

/* -------------------------------------------------------------------------- */
unsigned mem_waitFor( mem_t *mem, void **data, uint32_t delay )
/* -------------------------------------------------------------------------- */
{
	assert(!port_isr_inside() || !delay);

	return priv_mem_wait(mem, data, delay, core_tsk_waitFor);
}

/* -------------------------------------------------------------------------- */
void mem_give( mem_t *mem, void *data )
/* -------------------------------------------------------------------------- */
{
	tsk_t *tsk;
	que_t *ptr;

	assert(mem);
	assert(data);

	port_sys_lock();

	tsk = core_one_wakeup(mem, E_SUCCESS);

	if (tsk)
	{
		*(void**)tsk->tmp.data = data;
	}
	else
	{
		ptr = (que_t *)&(mem->next);
		while (ptr->next) ptr = ptr->next;
		ptr->next = (que_t *)data - 1;
		ptr->next->next = 0;
	}

	port_sys_unlock();
}

/* -------------------------------------------------------------------------- */
