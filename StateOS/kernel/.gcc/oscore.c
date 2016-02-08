/******************************************************************************

    @file    StateOS: oscore.c
    @author  Rajmund Szymanski
    @date    07.02.2016
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

#ifdef  __GNUC__

#include <oskernel.h>
#include <stddef.h>

/* -------------------------------------------------------------------------- */

__attribute__((naked))
void PendSV_Handler( void )
{
	__asm volatile
	(
"	ldr   r0,   =System            \n"
"	ldr   r0,  [ r0, %[cur] ]      \n"
"	mrs   r1,    PSP               \n"
"	str   r1,  [ r0, %[sp]  ]      \n"
#if __CORTEX_M < 3
"	sub   r1,   #36                \n"
"	stm   r1!, { r4  - r7 }        \n"
"	mov   r3,    r8                \n"
"	mov   r4,    r9                \n"
"	mov   r5,    r10               \n"
"	mov   r6,    r11               \n"
"	mov   r7,    lr                \n"
"	stm   r1!, { r3  - r7 }        \n"
#else
"	stmdb r1!, { r4  - r11, lr }   \n"
#if __FPU_USED
"	tst   lr,   #16                \n"
"	it    eq                       \n"
"vstmdbeq r1!, { s16 - s31 }       \n"
#endif
#endif
"	bl    core_tsk_handler         \n"
"	ldr   r1,  [ r0, %[sp]  ]      \n"
"	msr   PSP,   r1                \n"
#if __CORTEX_M < 3
"	sub   r1,   #20                \n"
"	ldm   r1!, { r3  - r7 }        \n"
"	mov   r8,    r3                \n"
"	mov   r9,    r4                \n"
"	mov   r10,   r5                \n"
"	mov   r11,   r6                \n"
"	mov   lr,    r7                \n"
"	sub   r1,   #36                \n"
"	ldm   r1!, { r4  - r7 }        \n"
#else
"	ldmdb r1!, { r4  - r11, lr }   \n"
#if __FPU_USED
"	tst   lr,   #16                \n"
"	it    eq                       \n"
"vldmdbeq r1!, { s16 - s31 }       \n"
#endif
#endif
"	bx    lr                       \n"

::	[cur] "n" (offsetof(sys_t, cur)),
	[sp]  "n" (offsetof(tsk_t, sp))

	);
}

/* -------------------------------------------------------------------------- */

#endif // __GNUC__
