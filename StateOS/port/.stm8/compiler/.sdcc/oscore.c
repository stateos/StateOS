/******************************************************************************

    @file    StateOS: oscore.c
    @author  Rajmund Szymanski
    @date    09.12.2019
    @brief   StateOS port file for STM8 uC.

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

/* -------------------------------------------------------------------------- */

void *_get_SP( void ) __naked
{
	__asm

#ifdef __SDCC_MODEL_LARGE
	pop    a
#endif
	popw   y
	ldw    x, sp
	pushw  y
#ifdef __SDCC_MODEL_LARGE
	push   a
	retf
#else
	ret
#endif

	__endasm;
}

/* -------------------------------------------------------------------------- */

void _set_SP( void *sp ) __naked
{
	(void) sp;

	__asm

#ifdef __SDCC_MODEL_LARGE
	pop    a
#endif
	popw   y
	popw   x
	ldw    sp, x
	pushw  x
	pushw  y
#ifdef __SDCC_MODEL_LARGE
	push   a
	retf
#else
	ret
#endif

	__endasm;
}

/* -------------------------------------------------------------------------- */

lck_t _get_CC( void ) __naked
{
	__asm

	push   cc
	pop    a
#ifdef __SDCC_MODEL_LARGE
	retf
#else
	ret
#endif

	__endasm;
}

/* -------------------------------------------------------------------------- */

void _set_CC( lck_t cc ) __naked
{
	(void) cc;

	__asm

#ifdef __SDCC_MODEL_LARGE
	ld     a, (4, sp)
#else
	ld     a, (3, sp)
#endif
	push   a
	pop    cc
#ifdef __SDCC_MODEL_LARGE
	retf
#else
	ret
#endif

	__endasm;
}

/* -------------------------------------------------------------------------- */

void core_tsk_flip( void *sp )
{
	_set_SP((char *)sp - 1);
	core_tsk_loop();
}

/* -------------------------------------------------------------------------- */
