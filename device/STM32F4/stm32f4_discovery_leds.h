/******************************************************************************
 * @file    stm32f4_discovery_leds.h
 * @author  Rajmund Szymanski
 * @date    23.03.2017
 * @brief   This file contains definitions for STM32F4-Discovery Kit.
 ******************************************************************************/

#ifndef __STM32F4_DISCOVERY_LEDS_H
#define __STM32F4_DISCOVERY_LEDS_H

#include <stm32f4_io.h>

#ifdef  __cplusplus
extern "C" {
#endif//__cplusplus

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

// init usb green led (PA9)

static inline
void GRN_Init( void )
{
	GPIO_Init(GPIOA, GPIO_Pin_9, GPIO_Output_PushPull);
}

/* -------------------------------------------------------------------------- */

// init leds (PD12..PD15) as pushpull output

static inline
void LED_Init( void )
{
	GPIO_Init(GPIOD, GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15, GPIO_Output_PushPull);
}

/* -------------------------------------------------------------------------- */

// rotate leds

static inline
void LED_Tick( void )
{
	unsigned leds = (GPIOD->ODR << 1) & 0xE000;
	GPIOD->BSRR = 0xF0000000;
	GPIOD->BSRR = leds ? leds : 0x1000;
}

/* -------------------------------------------------------------------------- */

#ifdef  __cplusplus
}
#endif//__cplusplus

/* -------------------------------------------------------------------------- */

#ifdef  __cplusplus

/* -------------------------------------------------------------------------- */

class GreenLed
{
public:
	GreenLed( void ) { GRN_Init(); }

	operator   unsigned & ( void )                  { return (unsigned &)GRN; }
	unsigned   operator = ( const unsigned status ) { return   GRN = status; }
	unsigned   operator ! ( void ) /* ++grn */      { return   GRN ^ 1U; }
	unsigned   operator ++( void ) /* ++grn */      { return ++GRN;   }
	unsigned   operator ++( int  ) /* grn++ */      { return   GRN++; }
};

/* -------------------------------------------------------------------------- */

class Led
{
	unsigned get( void )            { return GPIOD->ODR >> 12; }
	void     set( unsigned status ) { GPIOD->BSRR = (~status << 28) | (uint16_t)(status << 12); }

public:
	Led( void ) { LED_Init(); }

	unsigned & operator []( const unsigned number ) { return (unsigned &)LED[number]; }
	unsigned   operator = ( const unsigned status ) {                              set(status); return status & 0xF; }
	unsigned   operator ++( void ) /* ++led */      { unsigned status = get() + 1; set(status); return status & 0xF; }
	unsigned   operator ++( int  ) /* led++ */      { unsigned status = get(); set(status + 1); return status; }

	void tick( void ) { LED_Tick(); }
};

/* -------------------------------------------------------------------------- */

#endif//__cplusplus

#endif//__STM32F4_DISCOVERY_LEDS_H
