/*******************************************************************************
@file     startup.c
@author   Rajmund Szymanski
@date     18.05.2016
@brief    STM32F4xx startup file.
          After reset the Cortex-M4 processor is in thread mode,
          priority is privileged, and the stack is set to main.
*******************************************************************************/

#if defined(__GNUC__) && !defined(__ARMCC_VERSION)

#include <stm32f4xx.h>

/*******************************************************************************
 Configuration of stacks
*******************************************************************************/

#ifndef main_stack_size
#define main_stack_size 1024 // <- default size of main stack
#endif
#define main_stack (((main_stack_size)+7)&(~7))

#if     main_stack_size > 0
char  __main_stack[main_stack] __attribute__ ((used, section(".main_stack")));
#endif

#ifndef proc_stack_size
#define proc_stack_size 1024 // <- default size of process stack
#endif
#define proc_stack (((proc_stack_size)+7)&(~7))

#if     proc_stack_size > 0
char  __proc_stack[proc_stack] __attribute__ ((used, section(".proc_stack")));
#endif

/*******************************************************************************
 Symbols defined in linker script
*******************************************************************************/

extern unsigned  __data_init_start[];
extern unsigned       __data_start[];
extern unsigned       __data_end  [];
extern unsigned       __data_size [];
extern unsigned        __bss_start[];
extern unsigned        __bss_end  [];
extern unsigned        __bss_size [];

extern void(*__preinit_array_start[])();
extern void(*__preinit_array_end  [])();
extern void(*   __init_array_start[])();
extern void(*   __init_array_end  [])();
extern void(*   __fini_array_start[])();
extern void(*   __fini_array_end  [])();

/*******************************************************************************
 Default reset procedures
*******************************************************************************/

static inline
void MemCpy( unsigned *dst_, unsigned *end_, unsigned *src_ )
{
	while (dst_ < end_) *dst_++ = *src_++;
}

static inline
void MemSet( unsigned *dst_, unsigned *end_, unsigned val_ )
{
	while (dst_ < end_) *dst_++ = val_;
}

static inline
void DataInit( void )
{
	/* Initialize the data segment */
	MemCpy(__data_start, __data_end, __data_init_start);
	/* Zero fill the bss segment */
	MemSet(__bss_start, __bss_end, 0);
}

static inline
void CallArray( void(**dst_)(), void(**end_)() )
{
	while (dst_ < end_)(*dst_++)();
}

#ifndef __NOSTARTFILES

void __libc_init_array( void );
void __libc_fini_array( void );

#else //__NOSTARTFILES

static inline
void __libc_init_array( void )
{
#ifndef __NOSTARTFILES
	CallArray(__preinit_array_start, __preinit_array_end);
	_init();
#endif
	CallArray(__init_array_start, __init_array_end);
}

static inline
void __libc_fini_array( void )
{
	CallArray(__fini_array_start, __fini_array_end);
#ifndef __NOSTARTFILES
	_fini();
#endif
}

#endif//__NOSTARTFILES

/*******************************************************************************
 Initial process stack pointer
*******************************************************************************/

extern char __initial_psp[];

/*******************************************************************************
 Prototypes of external functions
*******************************************************************************/

int  main( void );

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
	/* Initialize data segments */
	DataInit();
	/* Call global & static constructors */
	__libc_init_array();
	/* Call the application's entry point */
	main();
	/* Call global & static destructors */
	__libc_fini_array();
	/* Go into an infinite loop */
	for (;;);
}

/******************************************************************************/

#endif // __GNUC__ && !__ARMCC_VERSION
