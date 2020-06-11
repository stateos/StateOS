/******************************************************************************

    @file    StateOS: osalloc.c
    @author  Rajmund Szymanski
    @date    11.06.2020
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

/* -------------------------------------------------------------------------- */

#define  SEG_SIZE( size ) \
     ALIGNED_SIZE( size, sizeof( seg_t ))

#define SEG_ALIGN( base, alignment ) \
(seg_t *)(ALIGNED((uintptr_t)( base ) + sizeof( seg_t ), alignment ) - sizeof(seg_t))

/* -------------------------------------------------------------------------- */
// INTERNAL ALLOC/FREE SERVICES
/* -------------------------------------------------------------------------- */

#if OS_HEAP_SIZE

static
seg_t            Heap[SEG_SIZE(OS_HEAP_SIZE)+1] __ALIGNED(sizeof(stk_t));
#define HeapEnd (Heap+SEG_SIZE(OS_HEAP_SIZE))

#endif

/* -------------------------------------------------------------------------- */

#if OS_HEAP_SIZE

static
seg_t *priv_init( void )
{
	if (Heap[0].next == NULL)
	{
	//	system heap must be initialized
		Heap[0].next  = HeapEnd;
		Heap[0].owner = Heap;
	}

	return Heap;
}

#endif

/* -------------------------------------------------------------------------- */

#if OS_HEAP_SIZE

static
void *priv_alloc( size_t alignment, size_t size )
{
	seg_t *mem;
	seg_t *nxt;

	size = SEG_SIZE(size + sizeof(seg_t));

	for (mem = priv_init(); mem != NULL; mem = mem->next)
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
	//		nxt->owner = nxt; // updated below (mem->owner = NULL)
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
void priv_free( void *ptr )
{
	seg_t *mem;
	seg_t *seg = (seg_t *)ptr - 1;

	for (mem = priv_init(); mem != NULL; mem = mem->next)
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

#if OS_HEAP_SIZE

static
void *priv_realloc( void *ptr, size_t size )
{
	seg_t *mem;
	seg_t *nxt;
	seg_t *seg = (seg_t *)ptr - 1;
	size_t len = SEG_SIZE(size + sizeof(seg_t));

	for (mem = priv_init(); mem != NULL; mem = mem->next)
	{
		if (mem != seg)
	//	this is not the memory segment we are looking for
			continue;

		if (mem->owner != NULL)
	//	memory segment is not allocated
			return NULL;

		while (nxt = mem->next, nxt->owner != NULL)
	//	it is possible to attach adjacent free memory segment
			mem->next = nxt->next;

		if (mem + len < mem->next)
		{
	//	it is possible to reduce the size of the memory segment
			nxt = mem + len;
			nxt->next  = mem->next;
			nxt->owner = nxt;
			mem->next  = nxt;
		}

		len = (mem->next - mem) * sizeof(seg_t) - sizeof(seg_t);
		if (len >= size)
	//	memory segment has been successfully resized
			return ptr;

		mem = priv_alloc(sizeof(stk_t), size);

		if (mem != NULL)
		{
	//	new memory segment has been successfully allocated
			memcpy(mem, ptr, len);
			priv_free(ptr);
		}

		break;
	}

	return mem;
}

#endif

/* -------------------------------------------------------------------------- */

#if OS_HEAP_SIZE

static
size_t priv_size( void )
{
	seg_t *mem;
	seg_t *nxt;
	size_t size = 0;

	for (mem = priv_init(); mem != NULL; mem = mem->next)
	{
		if (mem->owner == NULL)
	//	memory segment has already been allocated
			continue;

		while (nxt = mem->next, nxt->owner != NULL)
	//	it is possible to merge adjacent free memory segments
			mem->next = nxt->next;

		size += (mem->next - mem) * sizeof(seg_t) - sizeof(seg_t);
	}

	return size;
}

#endif

/* -------------------------------------------------------------------------- */
// STANDARD ALLOC/FREE SERVICES
/* -------------------------------------------------------------------------- */

#if OS_HEAP_SIZE

void *memalign( size_t alignment, size_t size )
{
	seg_t *mem;

	assert_tsk_context();
	assert(alignment>0&&alignment==(alignment&-alignment));
	assert(size>0&&size<(OS_HEAP_SIZE));

	sys_lock();
	{
		mem = priv_alloc(alignment, size);
	}
	sys_unlock();

	assert(mem);

	return mem;
}

#endif

/* -------------------------------------------------------------------------- */

#if OS_HEAP_SIZE

void *aligned_alloc( size_t alignment, size_t size )
{
	seg_t *mem;

	assert_tsk_context();
	assert(alignment>0&&alignment==(alignment&-alignment));
	assert(size>0&&size<(OS_HEAP_SIZE)&&(size%alignment)==0);

	sys_lock();
	{
		mem = priv_alloc(alignment, size);
	}
	sys_unlock();

	assert(mem);

	return mem;
}

#endif

/* -------------------------------------------------------------------------- */

#if OS_HEAP_SIZE

int posix_memalign( void **ptr, size_t alignment, size_t size )
{
	assert_tsk_context();
	assert(ptr!=NULL);
	assert(alignment>=sizeof(void*)&&alignment==(alignment&-alignment));
	assert(size>0&&size<(OS_HEAP_SIZE));

	sys_lock();
	{
		*ptr = priv_alloc(alignment, size);
	}
	sys_unlock();

	assert(*ptr);

	return *ptr ? 0 /*OK*/ : 12 /*ENOMEM*/;
}

#endif

/* -------------------------------------------------------------------------- */

#if OS_HEAP_SIZE

void *malloc( size_t size )
{
	seg_t *mem;

	assert_tsk_context();
	assert(size>0&&size<(OS_HEAP_SIZE));

	sys_lock();
	{
		mem = priv_alloc(sizeof(stk_t), size);
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

void free( void *ptr )
{
	assert_tsk_context();
	assert(ptr==NULL||(ptr>(void*)Heap&&ptr<(void*)HeapEnd));

	if (ptr == NULL)
	// nothing to release
		return;

	sys_lock();
	{
		priv_free(ptr);
	}
	sys_unlock();
}

#endif

/* -------------------------------------------------------------------------- */

#if OS_HEAP_SIZE

void *realloc( void *ptr, size_t size )
{
	void * mem;

	assert_tsk_context();
	assert(ptr==NULL||(ptr>(void*)Heap&&ptr<(void*)HeapEnd));
	assert(size<(OS_HEAP_SIZE));

	if (ptr == NULL)
	// we want to allocate memory segment
		return malloc(size);

	if (size == 0)
	{
	// we want to free memory segment
		free(ptr);
		return NULL;
	}

	sys_lock();
	{
		mem = priv_realloc(ptr, size);
	}
	sys_unlock();

	assert(mem);

	return mem;
}

#endif

/* -------------------------------------------------------------------------- */
// SYSTEM HEAP SERVICES
/* -------------------------------------------------------------------------- */

size_t sys_heapSize( void )
{
	size_t size;

	assert_tsk_context(); 

	sys_lock();
	{
		core_tsk_deleter();
#if OS_HEAP_SIZE
		size = priv_size();
#else
		size = 0;
#endif
	}
	sys_unlock();

	return size;
}

/* -------------------------------------------------------------------------- */

size_t sys_segSize( void *ptr )
{
	size_t size;

	assert_tsk_context(); 

	sys_lock();
	{
#if OS_HEAP_SIZE
		seg_t *seg = (seg_t *)ptr - 1;
		size = (seg->next - seg) * sizeof(seg_t) - sizeof(seg_t);
#else
		(void) ptr;
		size = 0;
#endif
	}
	sys_unlock();

	return size;
}

/* -------------------------------------------------------------------------- */
