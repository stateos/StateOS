/******************************************************************************

    @file    StateOS: osalloc.c
    @author  Rajmund Szymanski
    @date    29.11.2017
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

/* -------------------------------------------------------------------------- */

typedef struct __hdr hdr_t;

struct __hdr
{
	hdr_t  * next;
	size_t   size;
};

/* -------------------------------------------------------------------------- */

#define HSIZE( size ) \
 ALIGNED_SIZE( size, hdr_t )

/* -------------------------------------------------------------------------- */

static
hdr_t Heap[HSIZE(OS_HEAP_SIZE)] =
  { { Heap+HSIZE(OS_HEAP_SIZE)-1, HSIZE(OS_HEAP_SIZE)-1 } };

/* -------------------------------------------------------------------------- */

void *core_sys_alloc( size_t size )
{
	hdr_t *heap;
	hdr_t *next;

	assert(HSIZE(size));

	size = HSIZE(size) + 1;

	port_sys_lock();

	for (heap = Heap; heap; heap = heap->next)
	{
		if (heap->size == 0)				// memory segment has already been allocated
			continue;

		while ((next = heap->next)->size)	// it is possible to merge adjacent free memory segments
		{
			heap->next = next->next;
			heap->size += next->size;
		}

		if (heap->size < size)				// memory segment is too small
			continue;

		if (heap->size > size)				// memory segment is larger than necessary
		{
			next = heap + size;
			next->next = heap->next;
			next->size = heap->size - size;
		}

		heap = memset(heap, 0, size * sizeof(hdr_t));
		heap->next = next;
		heap = heap + 1;
		break;								// memory segment was successfully allocated
	}

	port_sys_unlock();

	assert(heap);

	return heap;
}

/* -------------------------------------------------------------------------- */

void core_sys_free( void *base )
{
	hdr_t *heap;

	base = (hdr_t *) base - 1;

	port_sys_lock();

	for (heap = Heap; heap; heap = heap->next)
	{
		if (heap != base)					// this is not the memory segment we are looking for
			continue;

		heap->size = heap->next - heap;
		break;								// memory segment was successfully released
	}

	port_sys_unlock();
}

/* -------------------------------------------------------------------------- */

#else

void *core_sys_alloc( size_t size )
{
	void *base;

	assert(size);

	base = malloc(size);

	if (base)
		base = memset(base, 0, size);

	assert(base);

	return base;
}

/* -------------------------------------------------------------------------- */

void core_sys_free( void *base )
{
	free(base);
}

#endif

/* -------------------------------------------------------------------------- */
