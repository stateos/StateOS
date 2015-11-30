/******************************************************************************
 * @file    stm32f4_discovery_btns.h
 * @author  Rajmund Szymanski
 * @date    10.11.2015
 * @brief   This file contains definitions for STM32F4-Discovery Kit.
 ******************************************************************************/

#pragma once

/* -------------------------------------------------------------------------- */

#include <stm32f4_io.h>

#ifdef __cplusplus
extern "C" {
#endif

/* -------------------------------------------------------------------------- */

#define BTN BITBAND(GPIOA->IDR)[0] // user button

/* -------------------------------------------------------------------------- */

// config user button (PA0)

static inline
void BTN_Config( void )
{
	GPIO_Init(GPIOA, GPIO_Pin_0, GPIO_Input_PullDown);
}

/* -------------------------------------------------------------------------- */

#ifdef __cplusplus
}
#endif

/* -------------------------------------------------------------------------- */

#ifdef __cplusplus

/* -------------------------------------------------------------------------- */

class Button
{
public:
	Button( void ) { BTN_Config(); }
	
	unsigned operator ()( void ) { return BTN; }
};

/* -------------------------------------------------------------------------- */

#endif
