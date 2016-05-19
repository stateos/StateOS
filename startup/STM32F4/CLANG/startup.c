/*******************************************************************************
@file     startup.c
@author   Rajmund Szymanski
@date     19.05.2016
@brief    STM32F4xx startup file.
          After reset the Cortex-M4 processor is in thread mode,
          priority is privileged, and the stack is set to main.
*******************************************************************************/

#if defined(__ARMCOMPILER_VERSION)

#include <stm32f4xx.h>

/*******************************************************************************
 Specific definitions for the chip
*******************************************************************************/

#define __ccm_start 0x10000000
#define __ccm_end   0x10010000
#define __ram_start 0x20000000
#define __ram_end   0x20020000

/*******************************************************************************
 Configuration of stacks
*******************************************************************************/

#ifndef main_stack_size
#define main_stack_size 1024 // <- default size of main stack
#endif

#ifndef proc_stack_size
#define proc_stack_size 1024 // <- default size of process stack
#endif

/*******************************************************************************
 Configuration of stacks and heap
*******************************************************************************/

#define NUM(n) #n
#define STR(n) NUM(n) "\n"

void __user_stackheap_config( void )
{
	__asm volatile
	(
"		.pushsection .stack, \"aw\", %nobits \n"

		#if main_stack_size > 0
"		.space  " STR(main_stack_size)
"		.align  3               \n"
"__initial_msp  = .             \n"
		#else
"__initial_msp  = " STR(__ram_end)
		#endif

		#if proc_stack_size > 0
		#ifndef __MICROLIB
"		.global __use_two_region_memory \n"
		#endif
"		.space  " STR(proc_stack_size)
"		.align  3               \n"
"__initial_psp  = .             \n"
"__initial_sp   = __initial_psp \n"
		#else
"__initial_psp  = .             \n"
"__initial_sp   = __initial_msp \n"
		#endif

"		.popsection             \n"

"		.global __initial_msp   \n"
"		.global __initial_psp   \n"
"		.global __initial_sp    \n"

"		.pushsection .heap, \"aw\", %nobits \n"

"__heap_base    = .             \n"
"__heap_limit   = " STR(__ram_end)

"		.popsection             \n"

"		.global __heap_base     \n"
"		.global __heap_limit    \n"
	);
}

/*******************************************************************************
 Initial process stack pointer
*******************************************************************************/

extern char __initial_psp[];

/*******************************************************************************
 Prototypes of external functions
*******************************************************************************/

void __main( void ) __attribute__ ((noreturn));

/*******************************************************************************
 Default reset handler
*******************************************************************************/

void Reset_Handler( void )
{
#if proc_stack_size > 0
	/* Initialize the process stack pointer */
	__set_PSP((unsigned)__initial_psp);
	__set_CONTROL(CONTROL_SPSEL_Msk);
#endif
#if __FPU_USED
	/* Set CP10 and CP11 Full Access */
	SCB->CPACR = 0x00F00000U;
#endif
#ifndef __NO_SYSTEM_INIT
	/* Call the system clock intitialization function */
	SystemInit();
#endif
	/* Call the application's entry point */
	__main();
}

/******************************************************************************/

#endif // __ARMCOMPILER_VERSION
