/******************************************************************************

    @file    StateOS: oscore.c
    @author  Rajmund Szymanski
    @date    14.12.2020
    @brief   StateOS port file for ARM Cotrex-M uC.

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

#include "oskernel.h"

/* -------------------------------------------------------------------------- */

#if __CORTEX_M < 3

void PendSV_Handler( void )
{
	#asm
	xref _core_tsk_handler

	mrs   r0,    PSP
	mov   r3,    lr
	lsrs  r3,    r3, # 3
	bcs   _1
	mov   r0,    sp
	sub   sp,  # 36
_1:	subs  r0,  # 36
	stm   r0!, { r4  - r7 }
	mov   r3,    r8
	mov   r4,    r9
	mov   r5,    r10
	mov   r6,    r11
	mov   r7,    lr
	stm   r0!, { r3  - r7 }
	subs  r0,  # 36

	bl   _core_tsk_handler

	adds  r0,  # 16
	ldm   r0!, { r3  - r7 }
	mov   r8,    r3
	mov   r9,    r4
	mov   r10,   r5
	mov   r11,   r6
	mov   lr,    r7
	subs  r0,  # 36
	ldm   r0!, { r4  - r7 }
	adds  r0,  # 20
	mov   r3,    lr
	lsrs  r3,    r3, # 3
	bcs   _2
	mov   sp,    r0
	bx    lr
_2:	msr   PSP,   r0
	bx    lr

	#endasm
}

#endif//__CORTEX_M

/* -------------------------------------------------------------------------- */

#if __CORTEX_M >= 3

void PendSV_Handler( void )
{
	#asm
	xref _core_tsk_handler

	tst   lr,  # 4                      ; process stack used?
	itee  ne
	mrsne r0,    PSP
	moveq r0,    sp
#if __FPU_USED
	subeq sp,  # 100
	tst   lr,  # 16                     ; fpu used?
	it    eq
 vstmdbeq r0!, { s16 - s31 }
#else
	subeq sp,  # 36
#endif
	stmdb r0!, { r4  - r11, lr }

	bl   _core_tsk_handler

	ldmia r0!, { r4  - r11, lr }
#if __FPU_USED
	tst   lr,  # 16                     ; fpu used?
	it    eq
 vldmiaeq r0!, { s16 - s31 }
#endif
	tst   lr,  # 4                      ; process stack used?
	ite   ne
	msrne PSP,   r0
	moveq sp,    r0
	bx    lr

	#endasm
}

#endif//__CORTEX_M

/* -------------------------------------------------------------------------- */

void core_tsk_flip( void *sp )
{
	#asm
	xref _core_tsk_loop
	xref _core_tsk_exec

	mov   sp,    r0
#if OS_TASK_EXIT == 0
	bl   _core_tsk_loop
#else
	bl   _core_tsk_exec
#endif

	#endasm
}

/* -------------------------------------------------------------------------- */
