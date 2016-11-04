/******************************************************************************

    @file    StateOS: os_mem.c
    @author  Rajmund Szymanski
    @date    04.11.2016
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
	port_sys_lock();
	
	if (mem->next)
	{
		void  **_mem = mem->next;
		unsigned cnt = mem->limit;
		while (--cnt) _mem = (void**)(*_mem = _mem + mem->size);
		*_mem = 0;
	}		

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
		mem->next  = (limit && size) ? (void**)(mem + 1) : 0;
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

	port_sys_unlock();
}

/* -------------------------------------------------------------------------- */
static inline
void *priv_mem_wait( mem_id mem, unsigned time, unsigned(*wait)() )
/* -------------------------------------------------------------------------- */
{
	void **ptr;
	unsigned status;

	port_sys_lock();

	if (mem->next == 0)
	{
		status = wait(mem, time);
		ptr = (void**)(((status & ~0xFU) == ~0xFU) ? 0 : status);
	}
	else
	{
		ptr = mem->next;
		mem->next = *ptr;
	}

	if (ptr)
	{
		void **_mem = ptr;
		void **_end = ptr + mem->size;
		do *_mem++ = 0; while (_mem < _end);
	}
	
	port_sys_unlock();

	return ptr;
}

/* -------------------------------------------------------------------------- */
void *mem_waitUntil( mem_id mem, unsigned time )
/* -------------------------------------------------------------------------- */
{
	return priv_mem_wait(mem, time, core_tsk_waitUntil);
}

/* -------------------------------------------------------------------------- */
void *mem_waitFor( mem_id mem, unsigned delay )
/* -------------------------------------------------------------------------- */
{
	return priv_mem_wait(mem, delay, core_tsk_waitFor);
}

/* -------------------------------------------------------------------------- */
void mem_give( mem_id mem, void *data )
/* -------------------------------------------------------------------------- */
{
	port_sys_lock();

	if (core_one_wakeup(mem, (unsigned)data) == 0)
	{
		*(void**)data = mem->next;
		mem->next = (void**)data;
	}

	port_sys_unlock();
}

/* -------------------------------------------------------------------------- */
