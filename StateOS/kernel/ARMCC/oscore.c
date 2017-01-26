/******************************************************************************

    @file    StateOS: oscore.c
    @author  Rajmund Szymanski
    @date    25.01.2017
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

#if defined(__CC_ARM)

#include <stddef.h>
#include <oskernel.h>

/* -------------------------------------------------------------------------- */

__asm void PendSV_Handler( void )
{
	PRESERVE8
	IMPORT core_tsk_handler

#if __CORTEX_M < 3

	mrs   r0,    PSP
	mov   r3,    lr
	lsls  r3,  # 29
	bmi   priv_ctx_enter
	mov   r0,    sp
	sub   sp,  # 36
priv_ctx_enter
	subs  r0,  # 36
	stm   r0!, { r4  - r7 }
	mov   r3,    r8
	mov   r4,    r9
	mov   r5,    r10
	mov   r6,    r11
	mov   r7,    lr
	stm   r0!, { r3  - r7 }
	subs  r0,  # 36

#else //__CORTEX_M

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

#endif//__CORTEX_M

	bl    core_tsk_handler

#if __CORTEX_M < 3

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
	lsls  r3,  # 29
	bmi   priv_ctx_exit
	mov   sp,    r0
	bx    lr
priv_ctx_exit
	msr   PSP,   r0
	bx    lr

#else //__CORTEX_M

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

#endif//__CORTEX_M

	ALIGN
}

/* -------------------------------------------------------------------------- */

__asm void core_tsk_flip( void *sp )
{
	PRESERVE8
	IMPORT core_ctx_switch
	IMPORT core_tsk_start

	mov   sp,    r0
	bl    core_ctx_switch
	bl    core_tsk_start
	
	ALIGN
}

/* -------------------------------------------------------------------------- */

#endif // __CC_ARM
