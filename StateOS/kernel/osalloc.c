/******************************************************************************

    @file    StateOS: osalloc.c
    @author  Rajmund Szymanski
    @date    27.11.2017
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

/* -------------------------------------------------------------------------- */
// SYSTEM ALLOC/FREE SERVICES
/* -------------------------------------------------------------------------- */

#if OS_HEAP_SIZE

static  stk_t    Heap[ASIZE(OS_HEAP_SIZE)] = { 0 };
#define HeapEnd (Heap+ASIZE(OS_HEAP_SIZE))

void *core_sys_alloc( size_t size )
{
	static
	stk_t *heap = Heap;
	stk_t *base;
	stk_t *next;

	assert(size);
	assert(ASIZE(size));

	port_sys_lock();

	base = 0;
	next = heap + ASIZE(size);
	if (next <= HeapEnd)
	{
		base = heap; // memset(heap, 0, size);
		heap = next;
	}

	port_sys_unlock();

	assert(base);

	return base;
}

/* -------------------------------------------------------------------------- */

void core_sys_free( void *base )
{
	assert(base == 0);

	(void) base;
}

/* -------------------------------------------------------------------------- */

#else

void *core_sys_alloc( size_t size )
{
	void *base;

	assert(size);

	base = malloc(size);

	assert(base);

	if (base)
		base = memset(base, 0, size);

	return base;
}

/* -------------------------------------------------------------------------- */

void core_sys_free( void *base )
{
	free(base);
}

#endif

/* -------------------------------------------------------------------------- */
