/******************************************************************************
 * @file    stm32f4_discovery_btns.h
 * @author  Rajmund Szymanski
 * @date    04.10.2016
 * @brief   This file contains definitions for STM32F4-Discovery Kit.
 ******************************************************************************/

#ifndef __STM32F4_DISCOVERY_BTNS_H
#define __STM32F4_DISCOVERY_BTNS_H

#include <stm32f4_io.h>

#ifdef  __cplusplus
extern "C" {
#endif//__cplusplus

/* -------------------------------------------------------------------------- */

#define BTN BITBAND(GPIOA->IDR)[0] // user button

/* -------------------------------------------------------------------------- */

// config user button (PA0)

__STATIC_INLINE
void BTN_Config( void )
{
	GPIO_Init(GPIOA, GPIO_Pin_0, GPIO_Input_PullDown);
}

/* -------------------------------------------------------------------------- */

#ifdef  __cplusplus
}
#endif//__cplusplus

/* -------------------------------------------------------------------------- */

#ifdef  __cplusplus

/* -------------------------------------------------------------------------- */

class Button
{
public:
	Button( void ) { BTN_Config(); }
	
	unsigned operator ()( void ) { return BTN; }
};

/* -------------------------------------------------------------------------- */

#endif//__cplusplus

#endif//__STM32F4_DISCOVERY_BTNS_H
