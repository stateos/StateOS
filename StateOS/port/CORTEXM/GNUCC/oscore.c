/******************************************************************************

    @file    StateOS: oscore.c
    @author  Rajmund Szymanski
    @date    14.11.2017
    @brief   StateOS port file for ARM Cotrex-M uC.

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

#if defined(__GNUC__) && !defined(__ARMCC_VERSION)

#include <oskernel.h>

/* -------------------------------------------------------------------------- */

#if __CORTEX_M < 3

__attribute__((naked))
void PendSV_Handler( void )
{
	__ASM volatile
	(
"	.syntax	unified                \n"

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

"	bl  %[core_tsk_handler]        \n"

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
"	.syntax	unified                \n"

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

"	bl  %[core_tsk_handler]        \n"

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
"	.syntax	unified                \n"

"	mov   sp,    %0                \n"
"	bl  %[core_tsk_loop]           \n"

::	"r" (sp),
	[core_tsk_loop] "i" (core_tsk_loop)
:	"memory"
	);
}
	
/* -------------------------------------------------------------------------- */

#endif // __GNUC__ && !__ARMCC_VERSION
