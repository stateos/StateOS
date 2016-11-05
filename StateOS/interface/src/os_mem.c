/******************************************************************************

    @file    StateOS: os_mem.c
    @author  Rajmund Szymanski
    @date    05.11.2016
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

#include <os.h>

/* -------------------------------------------------------------------------- */
void mem_init( mem_id mem )
/* -------------------------------------------------------------------------- */
{
	void   **ptr;
	unsigned cnt;

	port_sys_lock();
	
	ptr = mem->data;
	cnt = mem->limit;

	mem->next = 0;
	while (cnt--) { mem_give(mem, ptr); ptr += mem->size; }

	port_sys_unlock();
}

/* -------------------------------------------------------------------------- */
mem_id mem_create( unsigned limit, unsigned size )
/* -------------------------------------------------------------------------- */
{
	mem_id mem;

	port_sys_lock();

	size = MSIZE(size);

	mem = core_sys_alloc(sizeof(mem_t) + limit * size * sizeof(void*));

	if (mem)
	{
		mem->limit = limit;
		mem->size  = size;
		mem->data  = mem + 1;
		mem_init(mem);
	}

	port_sys_unlock();

	return mem;
}

/* -------------------------------------------------------------------------- */
void mem_kill( mem_id mem )
/* -------------------------------------------------------------------------- */
{
	port_sys_lock();

	core_all_wakeup(mem, E_STOPPED);
	mem_init(mem);

	port_sys_unlock();
}

/* -------------------------------------------------------------------------- */
static inline
unsigned priv_mem_wait( mem_id mem, void **data, unsigned time, unsigned(*wait)() )
/* -------------------------------------------------------------------------- */
{
	unsigned event = E_SUCCESS;

	port_sys_lock();

	if (mem->next)
	{
		*data = mem->next;
		mem->next = *mem->next;
	}
	else
	{
		Current->data = data;
		event = wait(mem, time);
	}
	
	if (event == E_SUCCESS)
	{
		void   **ptr = *data;
		unsigned cnt = mem->size;
		while (cnt--) *ptr++ = 0;
	}
	
	port_sys_unlock();

	return event;
}

/* -------------------------------------------------------------------------- */
unsigned mem_waitUntil( mem_id mem, void **data, unsigned time )
/* -------------------------------------------------------------------------- */
{
	return priv_mem_wait(mem, data, time, core_tsk_waitUntil);
}

/* -------------------------------------------------------------------------- */
unsigned mem_waitFor( mem_id mem, void **data, unsigned delay )
/* -------------------------------------------------------------------------- */
{
	return priv_mem_wait(mem, data, delay, core_tsk_waitFor);
}

/* -------------------------------------------------------------------------- */
void mem_give( mem_id mem, void *data )
/* -------------------------------------------------------------------------- */
{
	port_sys_lock();

	tsk_id tsk = core_one_wakeup(mem, E_SUCCESS);

	if (tsk)
	{
		tsk->data = data;
	}
	else
	{
		*(void**)data = mem->next;
		mem->next = data;
	}

	port_sys_unlock();
}

/* -------------------------------------------------------------------------- */
