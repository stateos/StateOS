/******************************************************************************

    @file    State Machine OS: bitband.h
    @author  Rajmund Szymanski
    @date    21.12.2015
    @brief   This file contains macro definitions for the Cortex-M devices.

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

#pragma once

/* -------------------------------------------------------------------------- */

#ifdef __cplusplus
extern "C" {
#endif

/* Exported macros ---------------------------------------------------------- */

#define BB_BASE(var)  ((((unsigned)&(var))&0xF0000000U)+0x02000000U)
#define BB_OFFS(var)  ((((unsigned)&(var))&0x000FFFFFU)*32)

#define BITBAND(var)  ((volatile unsigned *)(BB_BASE(var)+BB_OFFS(var)))

// bit-banding example:
// #define green_led BITBAND(GPIOA->ODR)[9]
// green_led = 1;
/* -------------------------------------------------------------------------- */

#ifdef __CC_ARM

static inline __attribute__((always_inline, const))
int __builtin_ctz( unsigned mask )
{
	int result = 0;

	if ((mask & 0x0000FFFF) == 0) { result += 16; mask >>= 16; }
	if ((mask & 0x000000FF) == 0) { result +=  8; mask >>=  8; }
	if ((mask & 0x0000000F) == 0) { result +=  4; mask >>=  4; }
	if ((mask & 0x00000003) == 0) { result +=  2; mask >>=  2; }
	if ((mask & 0x00000001) == 0) { result +=  1; mask >>=  1; }
	if ((mask & 0x00000001) == 0) { result +=  1; }

	return result;
}

#endif

#define BB(var, msk)  BITBAND(var)[__builtin_ctz(msk)]

// bit-banding with bit mask example:
// #define GPIOA_enable BB(RCC->AHB1ENR, RCC_AHB1ENR_GPIOAEN)
// GPIOA_enable = 1;
/* -------------------------------------------------------------------------- */

#ifndef __cplusplus

#define BF(var, msk)  (((struct { unsigned:  __builtin_ctz(msk); \
                         volatile unsigned f:__builtin_popcount(msk); \
                                  unsigned:  __builtin_clz(msk); } *) &(var))->f)
#endif

// bit field with bit mask example:
// #define LEDs BF(GPIOD->ODR, 0xF000)
// LEDs = 15;
/* -------------------------------------------------------------------------- */

#ifdef __cplusplus
}
#endif
