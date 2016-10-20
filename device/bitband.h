/******************************************************************************
 * @file    bitband.h
 * @author  Rajmund Szymanski
 * @date    04.10.2016
 * @brief   This file contains macro definitions for the Cortex-M devices.
 ******************************************************************************/

#ifndef __BITBAND_H
#define __BITBAND_H

#ifdef  __cplusplus
extern "C" {
#endif//__cplusplus

/* Exported macros ---------------------------------------------------------- */

#define BB_BASE(var)  ((((unsigned)&(var))&0xFFF00000U)+0x02000000U)
#define BB_OFFS(var)  ((((unsigned)&(var))&0x000FFFFFU)*32)

#define BITBAND(var)  ((volatile unsigned *)(((BB_BASE(var)==0x22000000U)||(BB_BASE(var)==0x42000000U))?(BB_BASE(var)+BB_OFFS(var)):0U))

/* -------------------------------------------------------------------------- */

#ifndef __GNUC__

#define __bit_ctz01( mask ) ((((mask) << 31) == 0) ? 1 : 0)
#define __bit_ctz02( mask ) ((((mask) << 31) == 0) ? ( 1 + __bit_ctz01((mask) >>  1)) : __bit_ctz01(mask))
#define __bit_ctz04( mask ) ((((mask) << 30) == 0) ? ( 2 + __bit_ctz02((mask) >>  2)) : __bit_ctz02(mask))
#define __bit_ctz08( mask ) ((((mask) << 28) == 0) ? ( 4 + __bit_ctz04((mask) >>  4)) : __bit_ctz04(mask))
#define __bit_ctz16( mask ) ((((mask) << 24) == 0) ? ( 8 + __bit_ctz08((mask) >>  8)) : __bit_ctz08(mask))
#define __bit_ctz32( mask ) ((((mask) << 16) == 0) ? (16 + __bit_ctz16((mask) >> 16)) : __bit_ctz16(mask))

#define __bit_clz01( mask ) ((((mask) >> 31) == 0) ? 1 : 0)
#define __bit_clz02( mask ) ((((mask) >> 31) == 0) ? ( 1 + __bit_clz01((mask) <<  1)) : __bit_clz01(mask))
#define __bit_clz04( mask ) ((((mask) >> 30) == 0) ? ( 2 + __bit_clz02((mask) <<  2)) : __bit_clz02(mask))
#define __bit_clz08( mask ) ((((mask) >> 28) == 0) ? ( 4 + __bit_clz04((mask) <<  4)) : __bit_clz04(mask))
#define __bit_clz16( mask ) ((((mask) >> 24) == 0) ? ( 8 + __bit_clz08((mask) <<  8)) : __bit_clz08(mask))
#define __bit_clz32( mask ) ((((mask) >> 16) == 0) ? (16 + __bit_clz16((mask) << 16)) : __bit_clz16(mask))

#define __bit_cnt01( mask ) ((mask) & 1)
#define __bit_cnt02( mask ) (__bit_cnt01((mask) >>  1) + __bit_cnt01(mask))
#define __bit_cnt04( mask ) (__bit_cnt02((mask) >>  2) + __bit_cnt02(mask))
#define __bit_cnt08( mask ) (__bit_cnt04((mask) >>  4) + __bit_cnt04(mask))
#define __bit_cnt16( mask ) (__bit_cnt08((mask) >>  8) + __bit_cnt08(mask))
#define __bit_cnt32( mask ) (__bit_cnt16((mask) >> 16) + __bit_cnt16(mask))

#define __builtin_ctz( mask )      __bit_ctz32((unsigned)(mask))
#define __builtin_clz( mask )      __bit_clz32((unsigned)(mask))
#define __builtin_popcount( mask ) __bit_cnt32((unsigned)(mask))

#endif//__GNUC__

/* -------------------------------------------------------------------------- */

// bit-banding example:
// #define green_led BITBAND(GPIOA->ODR)[9]
// green_led = 1;
/* -------------------------------------------------------------------------- */

#define BB(var, msk)  BITBAND(var)[__builtin_ctz(msk)]

// bit-banding with bit mask example:
// #define GPIOA_enable BB(RCC->AHB1ENR, RCC_AHB1ENR_GPIOAEN)
// GPIOA_enable = 1;
/* -------------------------------------------------------------------------- */

#ifndef __cplusplus

#define BF(var, msk)  (((struct { unsigned:  __builtin_ctz(msk); \
                         volatile unsigned f:__builtin_popcount(msk); \
                                  unsigned:  __builtin_clz(msk); } *) &(var))->f)
#endif//__cplusplus

// bit field with bit mask example:
// #define LEDs BF(GPIOD->ODR, 0xF000)
// LEDs = 15;
/* -------------------------------------------------------------------------- */

#ifdef  __cplusplus
}
#endif//__cplusplus

#endif//__BITBAND_H
