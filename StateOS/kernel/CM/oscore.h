/******************************************************************************

    @file    StateOS: oscore.h
    @author  Rajmund Szymanski
    @date    26.03.2017
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

#ifndef __STATEOSCORE_H
#define __STATEOSCORE_H

#include <osbase.h>

#ifdef __cplusplus
extern "C" {
#endif

/* -------------------------------------------------------------------------- */

// task context

typedef struct __ctx ctx_t;

struct __ctx
{
	// context saved by the software
	unsigned r4, r5, r6, r7, r8, r9, r10, r11;
	unsigned lr;  // EXC_RETURN
	// context saved by the hardware
	unsigned r0, r1, r2, r3;
	unsigned r12; // ip
	unsigned r14; // lr
	fun_t  * pc;
	unsigned psr;
};

#define _CTX_INIT( pc ) { 0, 0, 0, 0, 0, 0, 0, 0, 0xFFFFFFFD, 0, 0, 0, 0, 0, 0, pc, 0x01000000 }

/* -------------------------------------------------------------------------- */

__STATIC_INLINE
void port_ctx_init( ctx_t *ctx, fun_t *pc )
{
	ctx->lr  = 0xFFFFFFFD; // EXC_RETURN: return from psp
	ctx->pc  = pc;
	ctx->psr = 0x01000000;
}

/* -------------------------------------------------------------------------- */

// is procedure inside ISR?
__STATIC_INLINE
bool port_isr_inside( void )
{
	return (__get_IPSR() != 0U);
}

/* -------------------------------------------------------------------------- */

// are interrupts masked?
__STATIC_INLINE
bool port_isr_masked( void )
{
#if __CORTEX_M >= 3
	return (__get_PRIMASK() != 0U) || (__get_BASEPRI() != 0U);
#else
	return (__get_PRIMASK() != 0U);
#endif
}

/* -------------------------------------------------------------------------- */

#ifdef __cplusplus
}
#endif

#endif//__STATEOSCORE_H
