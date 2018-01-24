/******************************************************************************

    @file    StateOS: oscore.c
    @author  Rajmund Szymanski
    @date    24.01.2018
    @brief   StateOS port file for ARM Cotrex-M uC.

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

#if defined(__ICCARM__)

#include <oskernel.h>

/* -------------------------------------------------------------------------- */

#if __CORTEX_M < 3

__attribute__((naked))
void PendSV_Handler( void )
{
	__ASM volatile
	(
"	mrs   r0,    PSP               \n"
"	mov   r3,    lr                \n"
"	lsrs  r3,    r3, # 3           \n"
"	bcs   1f                       \n"
"	mov   r0,    sp                \n"
"	sub   sp,  # 36                \n"
"1:	subs  r0,  # 36                \n"
"	stm   r0!, { r4  - r7 }        \n"
"	mov   r3,    r8                \n"
"	mov   r4,    r9                \n"
"	mov   r5,    r10               \n"
"	mov   r6,    r11               \n"
"	mov   r7,    lr                \n"
"	stm   r0!, { r3  - r7 }        \n"
"	subs  r0,  # 36                \n"

"	bl %c[core_tsk_handler]        \n"

"	adds  r0,  # 16                \n"
"	ldm   r0!, { r3  - r7 }        \n"
"	mov   r8,    r3                \n"
"	mov   r9,    r4                \n"
"	mov   r10,   r5                \n"
"	mov   r11,   r6                \n"
"	mov   lr,    r7                \n"
"	subs  r0,  # 36                \n"
"	ldm   r0!, { r4  - r7 }        \n"
"	adds  r0,  # 20                \n"
"	mov   r3,    lr                \n"
"	lsrs  r3,    r3, # 3           \n"
"	bcs   2f                       \n"
"	mov   sp,    r0                \n"
"	bx    lr                       \n"
"2:	msr   PSP,   r0                \n"
"	bx    lr                       \n"

::	[core_tsk_handler] "i" (core_tsk_handler)
:	"memory"
	);
}

#endif//__CORTEX_M

/* -------------------------------------------------------------------------- */

#if __CORTEX_M >= 3

__attribute__((naked))
void PendSV_Handler( void )
{
	__ASM volatile
	(
"	tst   lr,  # 4                 \n"
"	itee  ne                       \n"
"	mrsne r0,    PSP               \n"
"	moveq r0,    sp                \n"
#if __FPU_USED
"	subeq sp,  # 100               \n"
"	tst   lr,  # 16                \n"
"	it    eq                       \n"
"vstmdbeq r0!, { s16 - s31 }       \n"
#else
"	subeq sp,  # 36                \n"
#endif
"	stmdb r0!, { r4  - r11, lr }   \n"

"	bl %c[core_tsk_handler]        \n"

"	ldmia r0!, { r4  - r11, lr }   \n"
#if __FPU_USED
"	tst   lr,  # 16                \n"
"	it    eq                       \n"
"vldmiaeq r0!, { s16 - s31 }       \n"
#endif
"	tst   lr,  # 4                 \n"
"	ite   ne                       \n"
"	msrne PSP,   r0                \n"
"	moveq sp,    r0                \n"
"	bx    lr                       \n"

::	[core_tsk_handler] "i" (core_tsk_handler)
:	"memory"
	);
}

#endif//__CORTEX_M

/* -------------------------------------------------------------------------- */

__attribute__((naked))
void core_tsk_flip( void *sp )
{
	__ASM volatile
	(
"	mov   sp,  %[sp]               \n"
"	b  %c[core_tsk_loop]           \n"

::	[sp] "r" (sp),
	[core_tsk_loop] "i" (core_tsk_loop)
:	"memory"
	);

	for(;;);
}
	
/* -------------------------------------------------------------------------- */

#endif // __ICCARM__
