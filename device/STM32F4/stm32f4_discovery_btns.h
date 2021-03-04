/******************************************************************************
 * @file    stm32f4_discovery_btns.h
 * @author  Rajmund Szymanski
 * @date    04.03.2021
 * @brief   This file contains definitions for STM32F4-Discovery Kit.
 ******************************************************************************/

#ifndef __STM32F4_DISCOVERY_BTNS_H
#define __STM32F4_DISCOVERY_BTNS_H

#include <stdbool.h>
#include "stm32f4_io.h"

#ifdef  __cplusplus
extern "C" {
#endif//__cplusplus

/* -------------------------------------------------------------------------- */

#define BTN BITBAND(GPIOA->IDR)[0] // user button

/* -------------------------------------------------------------------------- */

// init user button (PA0)

static inline
void BTN_Init( void )
{
	GPIO_Init(GPIOA, GPIO_Pins(0), GPIO_Input_PullDown);
}

/* -------------------------------------------------------------------------- */

// get user button state (PA0)

static inline
bool BTN_Get( void )
{
	return BTN;
}

/* -------------------------------------------------------------------------- */

#ifdef  __cplusplus
}
#endif//__cplusplus

/* -------------------------------------------------------------------------- */

#ifdef  __cplusplus
namespace device {

/* -------------------------------------------------------------------------- */

struct Button
{
	Button( void ) { BTN_Init(); }
	
	bool operator ()( void ) { return BTN_Get(); }
};

/* -------------------------------------------------------------------------- */

}     //  namespace
#endif//__cplusplus

#endif//__STM32F4_DISCOVERY_BTNS_H
