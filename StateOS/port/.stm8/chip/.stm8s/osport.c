/******************************************************************************

    @file    StateOS: osport.c
    @author  Rajmund Szymanski
    @date    29.10.2019
    @brief   StateOS port file for STM8S uC.

 ******************************************************************************

   Copyright (c) 2018 Rajmund Szymanski. All rights reserved.

   Permission is hereby granted, free of charge, to any person obtaining a copy
   of this software and associated documentation files (the "Software"), to
   deal in the Software without restriction, including without limitation the
   rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
   sell copies of the Software, and to permit persons to whom the Software is
   furnished to do so, subject to the following conditions:

   The above copyright notice and this permission notice shall be included
   in all copies or substantial portions of the Software.

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
   OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
   THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
   FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
   IN THE SOFTWARE.

 ******************************************************************************/

#include "oskernel.h"

/* -------------------------------------------------------------------------- */

void port_sys_init( void )
{
	CLK->CKDIVR = 0;
	CLK->ECKR  |= CLK_ECKR_HSEEN; while ((CLK->ECKR & CLK_ECKR_HSERDY) == 0);
	CLK->SWCR  |= CLK_SWCR_SWEN;
	CLK->SWR    = 0xB4; /* HSE */ while ((CLK->SWCR & CLK_SWCR_SWBSY)  == 1);

#if HW_TIMER_SIZE == 0

	#define  CNT_(X)   ((X)>>0?(X)>>1?(X)>>2?(X)>>3?(X)>>4?(X)>>5?(X)>>6?(X)>>7?(X)>>8?(X)>>9?-1:9:8:7:6:5:4:3:2:1:0)
	#define  PSC_ CNT_ (((CPU_FREQUENCY)/(OS_FREQUENCY)-1)>>16)
	#if      PSC_ < 0
	#error Incorrect CPU_FREQUENCY / OS_FREQUENCY value!
	#endif
	#define  ARR_     ((((CPU_FREQUENCY)/(OS_FREQUENCY))>>PSC_)-1)

/******************************************************************************
 Non-tick-less mode: configuration of system timer
 It must generate interrupts with frequency OS_FREQUENCY
*******************************************************************************/

	TIM3->PSCR  = (uint8_t)(PSC_);
	TIM3->ARRH  = (uint8_t)(ARR_ >> 8);
	TIM3->ARRL  = (uint8_t)(ARR_);
	TIM3->CCR1H = 0xFF;
	TIM3->CCR1L = 0xFF;
	TIM3->IER   = TIM3_IER_CC1IE | TIM3_IER_UIE;
	TIM3->CR1   = TIM3_CR1_CEN;

/******************************************************************************
 End of configuration
*******************************************************************************/

#else //HW_TIMER_SIZE

	#if (OS_FREQUENCY)/(OS_ROBIN)-1 > UINT16_MAX
	#error Incorrect OS_ROBIN frequency!
	#endif

	#define  CNT_(X)   ((X)>>0?(X)>>1?(X)>>2?(X)>>3?(X)>>4?(X)>>5?(X)>>6?(X)>>7?(X)>>8?(X)>>9?-1:9:8:7:6:5:4:3:2:1:0)
	#define  PSC_ CNT_  ((CPU_FREQUENCY)/(OS_FREQUENCY)-1)
	#if      PSC_ < 0
	#error Incorrect CPU_FREQUENCY / OS_FREQUENCY value!
	#endif
	#define  CCR_       ((OS_FREQUENCY)/(OS_ROBIN))

/******************************************************************************
 Tick-less mode: configuration of system timer
 It must be rescaled to frequency OS_FREQUENCY
*******************************************************************************/

	TIM3->PSCR  = (uint8_t)(PSC_);
	TIM3->CCR1H = (uint8_t)(CCR_ >> 8);
	TIM3->CCR1L = (uint8_t)(CCR_);
	#if HW_TIMER_SIZE < OS_TIMER_SIZE
	TIM3->IER   = TIM3_IER_CC1IE | TIM3_IER_UIE;
	#else
	TIM3->IER   = TIM3_IER_CC1IE;
	#endif
	TIM3->CR1   = TIM3_CR1_CEN;

/******************************************************************************
 End of configuration
*******************************************************************************/

#endif//HW_TIMER_SIZE

	port_clr_lock();
}

/* -------------------------------------------------------------------------- */

#if HW_TIMER_SIZE == 0

/******************************************************************************
 Non-tick-less mode: interrupt handler of system timer
*******************************************************************************/

#if defined(__CSMC__)
@svlreg
#endif
INTERRUPT_HANDLER(TIM3_UPD_OVF_BRK_IRQHandler, 15)
{
//	if (TIM3->SR1 & TIM3_SR1_UIF)
	{
		TIM3->SR1 = (uint8_t) ~TIM3_SR1_UIF;
		core_sys_tick();
	}
}

/******************************************************************************
 End of the handler
*******************************************************************************/

/******************************************************************************
 Interrupt handler for context switch
*******************************************************************************/

#if defined(__CSMC__)
@svlreg
#endif
INTERRUPT_HANDLER(TIM3_CAP_COM_IRQHandler, 16)
{
//	if (TIM3->SR1 & TIM3_SR1_CC1IF)
	{
		TIM3->SR1 = (uint8_t) ~TIM3_SR1_CC1IF;
		_set_SP(core_tsk_handler(_get_SP()));
	}
}

/******************************************************************************
 End of the handler
*******************************************************************************/

#else //HW_TIMER_SIZE

/******************************************************************************
 Tick-less mode: interrupt handler of system timer
*******************************************************************************/

#if HW_TIMER_SIZE < OS_TIMER_SIZE

#if defined(__CSMC__)
@svlreg
#endif
INTERRUPT_HANDLER(TIM3_UPD_OVF_BRK_IRQHandler, 15)
{
//	if (TIM3->SR1 & TIM3_SR1_UIF)
	{
		TIM3->SR1 = (uint8_t) ~TIM3_SR1_UIF;
		core_sys_tick();
	}
}

#endif

/******************************************************************************
 End of the handler
*******************************************************************************/

/******************************************************************************
 Interrupt handler for context switch
*******************************************************************************/

#if defined(__CSMC__)
@svlreg
#endif
INTERRUPT_HANDLER(TIM3_CAP_COM_IRQHandler, 16)
{
	if (TIM3->SR1 & TIM3_SR1_CC2IF)
	{
		TIM3->SR1 = (uint8_t) ~TIM3_SR1_CC2IF;
		core_tmr_handler();
	}
	if (TIM3->SR1 & TIM3_SR1_CC1IF)
	{
		TIM3->SR1 = (uint8_t) ~TIM3_SR1_CC1IF;
		_set_SP(core_tsk_handler(_get_SP()));
	}
}

/******************************************************************************
 End of the handler
*******************************************************************************/

/******************************************************************************
 Tick-less mode: return current system time
*******************************************************************************/

#if HW_TIMER_SIZE < OS_TIMER_SIZE

cnt_t port_sys_time( void )
{
	cnt_t    cnt;
	uint16_t tck;

	cnt = System.cnt;
	tck = ((uint16_t)TIM3->CNTRH << 8) | TIM3->CNTRL;

	if (TIM3->SR1 & TIM3_SR1_UIF)
	{
		tck = ((uint16_t)TIM3->CNTRH << 8) | TIM3->CNTRL;
		cnt += (cnt_t)(1) << (HW_TIMER_SIZE);
	}

	return cnt + tck;
}

#endif

/******************************************************************************
 End of the function
*******************************************************************************/

#endif//HW_TIMER_SIZE

/* -------------------------------------------------------------------------- */
