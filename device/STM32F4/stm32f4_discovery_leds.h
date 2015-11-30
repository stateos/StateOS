/******************************************************************************
 * @file    stm32f4_discovery_leds.h
 * @author  Rajmund Szymanski
 * @date    10.11.2015
 * @brief   This file contains definitions for STM32F4-Discovery Kit.
 ******************************************************************************/

#pragma once

/* -------------------------------------------------------------------------- */

#include <stm32f4xx_io.h>

#ifdef __cplusplus
extern "C" {
#endif

/* -------------------------------------------------------------------------- */

#define GRN  BITBAND(GPIOA->ODR)[ 9] // usb green led

#define LED (BITBAND(GPIOD->ODR)+12) // leds array
#define LEDG BITBAND(GPIOD->ODR)[12] // green led
#define LEDO BITBAND(GPIOD->ODR)[13] // orange led
#define LEDR BITBAND(GPIOD->ODR)[14] // red led
#define LEDB BITBAND(GPIOD->ODR)[15] // blue led

struct  __LEDs { uint16_t: 12; volatile uint16_t f: 4; uint16_t: 0; };

#define   LEDs (((struct __LEDs *)&(GPIOD->ODR))->f)

/* -------------------------------------------------------------------------- */

// config usb green led (PA9)

static inline
void GRN_Config( void )
{
	GPIO_Init(GPIOA, GPIO_Pin_9, GPIO_Output_PushPull);
}

/* -------------------------------------------------------------------------- */

// config leds (PD12..PD15) as pushpull output

static inline
void LED_Config( void )
{
	GPIO_Init(GPIOD, GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15, GPIO_Output_PushPull);
}

/* -------------------------------------------------------------------------- */

// rotate leds

static inline
void LED_Tick( void )
{
	static unsigned value = 0x11111111;

	GPIOD->BSRR = ( value & 0xF000F000 ) ^ 0xF0000000;

	value = __ROR ( value , 31 );
}

/* -------------------------------------------------------------------------- */

#ifdef __cplusplus
}
#endif

/* -------------------------------------------------------------------------- */

#ifdef __cplusplus

/* -------------------------------------------------------------------------- */

class GRN_Led
{
public:
	GRN_Led( void ) { GRN_Config(); }

	unsigned   operator = ( const unsigned status ) { return   GRN=status; }
	unsigned   operator ++( void ) /* ++grn */      { return ++GRN;        }
	unsigned   operator ++( int  ) /* grn++ */      { return   GRN++;      }
};

/* -------------------------------------------------------------------------- */

class Led
{
	unsigned get( void )            { return (GPIOD->ODR >> 12); }
	void     set( unsigned status ) { GPIOD->BSRR = ((status << 12) & 0xFFFF) | (~status << 28); }

public:
	Led( void ) { LED_Config(); }

	volatile
	unsigned & operator []( const unsigned number ) { return LED[number]; }
	unsigned   operator = ( const unsigned status ) {                            set(status);   return status & 0xF; }
	unsigned   operator ++( void ) /* ++led */      { unsigned status = get()+1; set(status);   return status & 0xF; }
	unsigned   operator ++( int  ) /* led++ */      { unsigned status = get();   set(status+1); return status & 0xF; }

	void tick( void ) { LED_Tick(); }
};

/* -------------------------------------------------------------------------- */

#endif
