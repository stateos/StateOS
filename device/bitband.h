/******************************************************************************
 * @file    bitband.h
 * @author  Rajmund Szymanski
   @date    01.03.2016
 * @brief   This file contains macro definitions for the Cortex-M devices.
 ******************************************************************************/

#pragma once

/* -------------------------------------------------------------------------- */

#ifdef __cplusplus
extern "C" {
#endif

/* Exported macros ---------------------------------------------------------- */

#define BB_BASE(var)  ((((unsigned)&(var))&0xFFF00000U)+0x02000000U)
#define BB_OFFS(var)  ((((unsigned)&(var))&0x000FFFFFU)*32)

#define BITBAND(var)  ((volatile unsigned *)(((BB_BASE(var)==0x22000000U)||(BB_BASE(var)==0x42000000U))?(BB_BASE(var)+BB_OFFS(var)):0U))

// bit-banding example:
// #define green_led BITBAND(GPIOA->ODR)[9]
// green_led = 1;
/* -------------------------------------------------------------------------- */

#ifdef __CC_ARM

static inline __attribute__(( always_inline, const ))
int __builtin_ctz( unsigned mask )
{
	return (mask == 0) ? 32 : (mask & 1U) ? 0 : 1 + __builtin_ctz(mask >> 1);
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
