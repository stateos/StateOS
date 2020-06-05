/******************************************************************************

    @file    StateOS: osalloc.c
    @author  Rajmund Szymanski
    @date    05.06.2020
    @brief   This file provides set of variables and functions for StateOS.

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

#include "osalloc.h"
#include "inc/oscriticalsection.h"

#define SEG_ALIGN( base, alignment ) \
 (seg_t *)ALIGNED((uintptr_t)(base), alignment )

/* -------------------------------------------------------------------------- */
// INTERNAL ALLOC/FREE SERVICES
/* -------------------------------------------------------------------------- */

#if OS_HEAP_SIZE

static
seg_t Heap[SEG_SIZE(OS_HEAP_SIZE)+1] =
  { { Heap+SEG_SIZE(OS_HEAP_SIZE), Heap } };

#endif

/* -------------------------------------------------------------------------- */

#if OS_HEAP_SIZE

static
void *priv_mem_alloc( size_t alignment, size_t size )
{
	seg_t *mem;
	seg_t *nxt;

	size = SEG_SIZE(size + sizeof(seg_t));

	for (mem = Heap; mem; mem = mem->next)
	{
		if (mem->owner == NULL)
	//	memory segment has already been allocated
			continue;

		while (nxt = mem->next, nxt->owner != NULL)
	//	it is possible to merge adjacent free memory segments
			mem->next = nxt->next;

		nxt = SEG_ALIGN(mem, alignment);

		if (nxt + size > mem->next)
	//	memory segment is too small
			continue;

		if (nxt > mem)
		{
	//	memory segment must be aligned
			nxt->next  = mem->next;
			nxt->owner = nxt;
			mem->next  = nxt;
			mem = nxt;
		}

		if (mem + size < mem->next)
		{
	//	memory segment is larger than required
			nxt = mem + size;
			nxt->next  = mem->next;
			nxt->owner = nxt;
			mem->next  = nxt;
		}

	//	memory segment can be allocated
		mem->owner = NULL;
		mem = mem + 1;
		break;
	}

	return mem;
}

#endif

/* -------------------------------------------------------------------------- */

#if OS_HEAP_SIZE

static
size_t priv_mem_resize( void *ptr, size_t size )
{
	seg_t *mem;
	seg_t *nxt;

	size = SEG_SIZE(size + sizeof(seg_t));
	mem  = (seg_t *)ptr - 1;

	while (nxt = mem->next, nxt->owner != NULL)
	//	it is possible to attach adjacent free memory segment
		mem->next = nxt->next;

	if (mem + size < mem->next)
	{
	//	it is possible to reduce the size of the memory segment
		nxt = mem + size;
		nxt->next  = mem->next;
		nxt->owner = nxt;
		mem->next  = nxt;
	}

	return (mem->next - mem - 1) * sizeof(seg_t);
}

#endif

/* -------------------------------------------------------------------------- */

#if OS_HEAP_SIZE

static
void priv_mem_free( void *ptr )
{
	seg_t *mem;
	seg_t *seg = (seg_t *)ptr - 1;

	for (mem = Heap; mem; mem = mem->next)
	{
		if (mem != seg)
	//	this is not the memory segment we are looking for
			continue;

	//	memory segment can be released
		mem->owner = mem;
		break;
	}
}

#endif

/* -------------------------------------------------------------------------- */
// STANDARD ALLOC/FREE SERVICES
/* -------------------------------------------------------------------------- */

#if OS_HEAP_SIZE

size_t malloc_usable_size( void *ptr )
{
	seg_t *mem;
	size_t size;

	if (ptr == NULL)
		return 0;

	sys_lock();
	{
		mem  = (seg_t *)ptr - 1;
		size = (mem->next - mem - 1) * sizeof(seg_t);
	}
	sys_unlock();

	return size;
}

#endif

/* -------------------------------------------------------------------------- */

#if OS_HEAP_SIZE

void *memalign( size_t alignment, size_t size )
{
	seg_t *mem;

	assert(alignment>0&&alignment==(alignment&~(alignment-1)));
	assert(size>0&&size<OS_HEAP_SIZE);

	sys_lock();
	{
		mem = priv_mem_alloc(alignment, size);
	}
	sys_unlock();

	assert(mem);

	return mem;
}

#endif

/* -------------------------------------------------------------------------- */

#if OS_HEAP_SIZE

void *malloc( size_t size )
{
	seg_t *mem;

	assert(size>0&&size<OS_HEAP_SIZE);

	sys_lock();
	{
		mem = priv_mem_alloc(1, size);
	}
	sys_unlock();

	assert(mem);

	return mem;
}

#endif

/* -------------------------------------------------------------------------- */

#if OS_HEAP_SIZE

void *calloc( size_t num, size_t size )
{
	void *mem;

	mem = malloc(size *= num);

	if (mem != NULL)
		memset(mem, 0, size);

	return mem;
}

#endif

/* -------------------------------------------------------------------------- */

#if OS_HEAP_SIZE

void *realloc( void *ptr, size_t size )
{
	size_t len;
	void * mem;

	assert(ptr==NULL||(ptr>(void*)Heap&&ptr<(void*)(Heap+SEG_SIZE(OS_HEAP_SIZE))));
	assert(size<OS_HEAP_SIZE);

	if (ptr == NULL)
		return malloc(size);

	if (size == 0)
	{
		free(ptr);
		return NULL;
	}

	sys_lock();
	{
		len = priv_mem_resize(ptr, size);
	}
	sys_unlock();

	if (len >= size)
		return ptr;

	mem = malloc(size);
	if (mem != NULL)
	{
		memcpy(mem, ptr, len);
		free(ptr);
	}

	assert(mem);

	return mem;
}

#endif

/* -------------------------------------------------------------------------- */

#if OS_HEAP_SIZE

void free( void *ptr )
{
	assert(ptr>(void*)Heap&&ptr<(void*)(Heap+SEG_SIZE(OS_HEAP_SIZE)));

	sys_lock();
	{
		priv_mem_free(ptr);
	}
	sys_unlock();
}

#endif

/* -------------------------------------------------------------------------- */
// SYSTEM ALLOC/FREE SERVICES
/* -------------------------------------------------------------------------- */

void *sys_alloc( size_t size )
{
	void *mem;

	assert_tsk_context();
	assert(size);

	sys_lock();
	{
		core_tsk_deleter();
	}
	sys_unlock();

	mem = malloc(size);

	assert(mem);

	return mem;
}

/* -------------------------------------------------------------------------- */

void *sys_realloc( void *ptr, size_t size )
{
	void *mem;

	assert_tsk_context();

	sys_lock();
	{
		core_tsk_deleter();
	}
	sys_unlock();

	mem = realloc(ptr, size);

	assert(mem);

	return mem;
}

/* -------------------------------------------------------------------------- */

void sys_free( void *ptr )
{
	assert_tsk_context();

	sys_lock();
	{
		core_tsk_deleter();
	}
	sys_unlock();

	free(ptr);
}

/* -------------------------------------------------------------------------- */

size_t sys_heapSize( void )
{
	seg_t *mem;
	seg_t *nxt;
	size_t size = 0;

	assert_tsk_context(); 

	sys_lock();
	{
		//	call garbage collection procedure
		core_tsk_deleter();
#if OS_HEAP_SIZE
		for (mem = Heap; mem; mem = mem->next)
		{
			if (mem->owner == NULL)
		//	memory segment has already been allocated
				continue;

			while (nxt = mem->next, nxt->owner != NULL)
		//	it is possible to merge adjacent free memory segments
				mem->next = nxt->next;

			size += nxt - mem - 1;
		}
#else
		// to prevent warnings
		(void) mem;
		(void) nxt;
#endif
	}
	sys_unlock();

	return size * sizeof(seg_t);
}

/* -------------------------------------------------------------------------- */
