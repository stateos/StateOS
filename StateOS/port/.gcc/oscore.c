/******************************************************************************

    @file    State Machine OS: oscore.c
    @author  Rajmund Szymanski
    @date    08.12.2015
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

#include <oskernel.h>
#include <stddef.h>

#ifdef  __GNUC__

/* -------------------------------------------------------------------------- */

__attribute__(( naked ))
void PendSV_Handler( void )
{
	__asm volatile
	(
"	ldr   r0,   =System            \n"
"	ldr   r0,  [ r0, %[cur] ]      \n"
"	mrs   r1,    PSP               \n"
#if __FPU_USED
"	tst   lr,   #16                \n"
"	it    eq                       \n"
"vstmdbeq r1!, { s16 - s31 }       \n"
#endif
#if __CORTEX_M < 3
"	sub   r1,   #36                \n"
"	stm   r1!, { r4  - r7 }        \n"
"	mov   r3,    r8                \n"
"	mov   r4,    r9                \n"
"	mov   r5,    r10               \n"
"	mov   r6,    r11               \n"
"	mov   r7,    lr                \n"
"	stm   r1!, { r3  - r7 }        \n"
"	sub   r1,   #36                \n"
#else
"	stmdb r1!, { r4  - r11, lr }   \n"
#endif
"	str   r1,  [ r0, %[sp]  ]      \n"
"	bl    core_tsk_handler         \n"
"	ldr   r1,  [ r0, %[sp]  ]      \n"
#if __CORTEX_M < 3
"	add   r1,   #16                \n"
"	ldm   r1!, { r3  - r7 }        \n"
"	mov   r8,    r3                \n"
"	mov   r9,    r4                \n"
"	mov   r10,   r5                \n"
"	mov   r11,   r6                \n"
"	mov   lr,    r7                \n"
"	sub   r1,   #36                \n"
"	ldm   r1!, { r4  - r7 }        \n"
"	add   r1,   #20                \n"
#else
"	ldmia r1!, { r4  - r11, lr }   \n"
#endif
#if __FPU_USED
"	tst   lr,   #16                \n"
"	it    eq                       \n"
"vldmiaeq r1!, { s16 - s31 }       \n"
#endif
"	msr   PSP,   r1                \n"
"	bx    lr                       \n"

::	[cur] "n" (offsetof(sys_t, cur)),
	[sp]  "n" (offsetof(tsk_t, sp))

	);
}

/* -------------------------------------------------------------------------- */

__noreturn
void port_tsk_break( void )
{
	__asm volatile
	(
"	ldr   r0,   =System            \n"
"	ldr   r4,  [ r0, %[cur]   ]    \n"
"	ldr   r1,  [ r4, %[top]   ]    \n"
"	mov   sp,    r1                \n"

"priv_tsk_break:                   \n"

#if OS_LOCK_LEVEL
"	movs  r3,   #0                 \n"
"	msr   BASEPRI, r3              \n"
#else
"	cpsie i                        \n"
#endif
"	bl    core_ctx_switch          \n"
"	ldr   r3,  [ r4, %[state] ]    \n"
"	blx   r3                       \n"
"	b     priv_tsk_break           \n"

::	[cur]   "n" (offsetof(sys_t, cur)),
	[top]   "n" (offsetof(tsk_t, top)),
	[state] "n" (offsetof(tsk_t, state))

	);
}

/* -------------------------------------------------------------------------- */

#endif // __GNUC__
