/*******************************************************************************
@file     startup.c
@author   Rajmund Szymanski
@date     14.04.2016
@brief    STM32F4xx startup file.
          After reset the Cortex-M4 processor is in thread mode,
          priority is privileged, and the stack is set to main.
*******************************************************************************/

#ifdef  __GNUC__

#include <stm32f4xx.h>

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

extern  char  __initial_msp[];
extern  char  __initial_psp[];

/*******************************************************************************
 Default fault handler
*******************************************************************************/

__attribute__ ((weak, noreturn, naked)) void Fault_Handler( void )
{
	/* Go into an infinite loop */
	for (;;);
}

/*******************************************************************************
 Default exit handler
*******************************************************************************/

void _exit( int ) __attribute__ ((weak, alias("Fault_Handler")));

/*******************************************************************************
 Prototypes of external functions
*******************************************************************************/

int   main( void );

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
 Default reset handler
*******************************************************************************/

__attribute__ ((weak, noreturn, naked)) void Reset_Handler( void )
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

/*******************************************************************************
 Declaration of exception handlers
*******************************************************************************/

/* Core exceptions */
void NMI_Handler                  (void) __attribute__ ((weak, alias("Fault_Handler")));
void HardFault_Handler            (void) __attribute__ ((weak, alias("Fault_Handler")));
void MemManage_Handler            (void) __attribute__ ((weak, alias("Fault_Handler")));
void BusFault_Handler             (void) __attribute__ ((weak, alias("Fault_Handler")));
void UsageFault_Handler           (void) __attribute__ ((weak, alias("Fault_Handler")));
void SVC_Handler                  (void) __attribute__ ((weak, alias("Fault_Handler")));
void DebugMon_Handler             (void) __attribute__ ((weak, alias("Fault_Handler")));
void PendSV_Handler               (void) __attribute__ ((weak, alias("Fault_Handler")));
void SysTick_Handler              (void) __attribute__ ((weak, alias("Fault_Handler")));

/* External interrupts */
void WWDG_IRQHandler              (void) __attribute__ ((weak, alias("Fault_Handler")));
void PVD_IRQHandler               (void) __attribute__ ((weak, alias("Fault_Handler")));
void TAMP_STAMP_IRQHandler        (void) __attribute__ ((weak, alias("Fault_Handler")));
void RTC_WKUP_IRQHandler          (void) __attribute__ ((weak, alias("Fault_Handler")));
void FLASH_IRQHandler             (void) __attribute__ ((weak, alias("Fault_Handler")));
void RCC_IRQHandler               (void) __attribute__ ((weak, alias("Fault_Handler")));
void EXTI0_IRQHandler             (void) __attribute__ ((weak, alias("Fault_Handler")));
void EXTI1_IRQHandler             (void) __attribute__ ((weak, alias("Fault_Handler")));
void EXTI2_IRQHandler             (void) __attribute__ ((weak, alias("Fault_Handler")));
void EXTI3_IRQHandler             (void) __attribute__ ((weak, alias("Fault_Handler")));
void EXTI4_IRQHandler             (void) __attribute__ ((weak, alias("Fault_Handler")));
void DMA1_Stream0_IRQHandler      (void) __attribute__ ((weak, alias("Fault_Handler")));
void DMA1_Stream1_IRQHandler      (void) __attribute__ ((weak, alias("Fault_Handler")));
void DMA1_Stream2_IRQHandler      (void) __attribute__ ((weak, alias("Fault_Handler")));
void DMA1_Stream3_IRQHandler      (void) __attribute__ ((weak, alias("Fault_Handler")));
void DMA1_Stream4_IRQHandler      (void) __attribute__ ((weak, alias("Fault_Handler")));
void DMA1_Stream5_IRQHandler      (void) __attribute__ ((weak, alias("Fault_Handler")));
void DMA1_Stream6_IRQHandler      (void) __attribute__ ((weak, alias("Fault_Handler")));
void ADC_IRQHandler               (void) __attribute__ ((weak, alias("Fault_Handler")));
void CAN1_TX_IRQHandler           (void) __attribute__ ((weak, alias("Fault_Handler")));
void CAN1_RX0_IRQHandler          (void) __attribute__ ((weak, alias("Fault_Handler")));
void CAN1_RX1_IRQHandler          (void) __attribute__ ((weak, alias("Fault_Handler")));
void CAN1_SCE_IRQHandler          (void) __attribute__ ((weak, alias("Fault_Handler")));
void EXTI9_5_IRQHandler           (void) __attribute__ ((weak, alias("Fault_Handler")));
void TIM1_BRK_TIM9_IRQHandler     (void) __attribute__ ((weak, alias("Fault_Handler")));
void TIM1_UP_TIM10_IRQHandler     (void) __attribute__ ((weak, alias("Fault_Handler")));
void TIM1_TRG_COM_TIM11_IRQHandler(void) __attribute__ ((weak, alias("Fault_Handler")));
void TIM1_CC_IRQHandler           (void) __attribute__ ((weak, alias("Fault_Handler")));
void TIM2_IRQHandler              (void) __attribute__ ((weak, alias("Fault_Handler")));
void TIM3_IRQHandler              (void) __attribute__ ((weak, alias("Fault_Handler")));
void TIM4_IRQHandler              (void) __attribute__ ((weak, alias("Fault_Handler")));
void I2C1_EV_IRQHandler           (void) __attribute__ ((weak, alias("Fault_Handler")));
void I2C1_ER_IRQHandler           (void) __attribute__ ((weak, alias("Fault_Handler")));
void I2C2_EV_IRQHandler           (void) __attribute__ ((weak, alias("Fault_Handler")));
void I2C2_ER_IRQHandler           (void) __attribute__ ((weak, alias("Fault_Handler")));
void SPI1_IRQHandler              (void) __attribute__ ((weak, alias("Fault_Handler")));
void SPI2_IRQHandler              (void) __attribute__ ((weak, alias("Fault_Handler")));
void USART1_IRQHandler            (void) __attribute__ ((weak, alias("Fault_Handler")));
void USART2_IRQHandler            (void) __attribute__ ((weak, alias("Fault_Handler")));
void USART3_IRQHandler            (void) __attribute__ ((weak, alias("Fault_Handler")));
void EXTI15_10_IRQHandler         (void) __attribute__ ((weak, alias("Fault_Handler")));
void RTC_Alarm_IRQHandler         (void) __attribute__ ((weak, alias("Fault_Handler")));
void OTG_FS_WKUP_IRQHandler       (void) __attribute__ ((weak, alias("Fault_Handler")));
void TIM8_BRK_TIM12_IRQHandler    (void) __attribute__ ((weak, alias("Fault_Handler")));
void TIM8_UP_TIM13_IRQHandler     (void) __attribute__ ((weak, alias("Fault_Handler")));
void TIM8_TRG_COM_TIM14_IRQHandler(void) __attribute__ ((weak, alias("Fault_Handler")));
void TIM8_CC_IRQHandler           (void) __attribute__ ((weak, alias("Fault_Handler")));
void DMA1_Stream7_IRQHandler      (void) __attribute__ ((weak, alias("Fault_Handler")));
void FSMC_IRQHandler              (void) __attribute__ ((weak, alias("Fault_Handler")));
void SDIO_IRQHandler              (void) __attribute__ ((weak, alias("Fault_Handler")));
void TIM5_IRQHandler              (void) __attribute__ ((weak, alias("Fault_Handler")));
void SPI3_IRQHandler              (void) __attribute__ ((weak, alias("Fault_Handler")));
void UART4_IRQHandler             (void) __attribute__ ((weak, alias("Fault_Handler")));
void UART5_IRQHandler             (void) __attribute__ ((weak, alias("Fault_Handler")));
void TIM6_DAC_IRQHandler          (void) __attribute__ ((weak, alias("Fault_Handler")));
void TIM7_IRQHandler              (void) __attribute__ ((weak, alias("Fault_Handler")));
void DMA2_Stream0_IRQHandler      (void) __attribute__ ((weak, alias("Fault_Handler")));
void DMA2_Stream1_IRQHandler      (void) __attribute__ ((weak, alias("Fault_Handler")));
void DMA2_Stream2_IRQHandler      (void) __attribute__ ((weak, alias("Fault_Handler")));
void DMA2_Stream3_IRQHandler      (void) __attribute__ ((weak, alias("Fault_Handler")));
void DMA2_Stream4_IRQHandler      (void) __attribute__ ((weak, alias("Fault_Handler")));
void ETH_IRQHandler               (void) __attribute__ ((weak, alias("Fault_Handler")));
void ETH_WKUP_IRQHandler          (void) __attribute__ ((weak, alias("Fault_Handler")));
void CAN2_TX_IRQHandler           (void) __attribute__ ((weak, alias("Fault_Handler")));
void CAN2_RX0_IRQHandler          (void) __attribute__ ((weak, alias("Fault_Handler")));
void CAN2_RX1_IRQHandler          (void) __attribute__ ((weak, alias("Fault_Handler")));
void CAN2_SCE_IRQHandler          (void) __attribute__ ((weak, alias("Fault_Handler")));
void OTG_FS_IRQHandler            (void) __attribute__ ((weak, alias("Fault_Handler")));
void DMA2_Stream5_IRQHandler      (void) __attribute__ ((weak, alias("Fault_Handler")));
void DMA2_Stream6_IRQHandler      (void) __attribute__ ((weak, alias("Fault_Handler")));
void DMA2_Stream7_IRQHandler      (void) __attribute__ ((weak, alias("Fault_Handler")));
void USART6_IRQHandler            (void) __attribute__ ((weak, alias("Fault_Handler")));
void I2C3_EV_IRQHandler           (void) __attribute__ ((weak, alias("Fault_Handler")));
void I2C3_ER_IRQHandler           (void) __attribute__ ((weak, alias("Fault_Handler")));
void OTG_HS_EP1_OUT_IRQHandler    (void) __attribute__ ((weak, alias("Fault_Handler")));
void OTG_HS_EP1_IN_IRQHandler     (void) __attribute__ ((weak, alias("Fault_Handler")));
void OTG_HS_WKUP_IRQHandler       (void) __attribute__ ((weak, alias("Fault_Handler")));
void OTG_HS_IRQHandler            (void) __attribute__ ((weak, alias("Fault_Handler")));
void DCMI_IRQHandler              (void) __attribute__ ((weak, alias("Fault_Handler")));
void CRYP_IRQHandler              (void) __attribute__ ((weak, alias("Fault_Handler")));
void HASH_RNG_IRQHandler          (void) __attribute__ ((weak, alias("Fault_Handler")));
void FPU_IRQHandler               (void) __attribute__ ((weak, alias("Fault_Handler")));
void UART7_IRQHandler             (void) __attribute__ ((weak, alias("Fault_Handler")));
void UART8_IRQHandler             (void) __attribute__ ((weak, alias("Fault_Handler")));
void SPI4_IRQHandler              (void) __attribute__ ((weak, alias("Fault_Handler")));
void SPI5_IRQHandler              (void) __attribute__ ((weak, alias("Fault_Handler")));
void SPI6_IRQHandler              (void) __attribute__ ((weak, alias("Fault_Handler")));
void SAI1_IRQHandler              (void) __attribute__ ((weak, alias("Fault_Handler")));
void LTDC_IRQHandler              (void) __attribute__ ((weak, alias("Fault_Handler")));
void LTDC_ER_IRQHandler           (void) __attribute__ ((weak, alias("Fault_Handler")));
void DMA2D_IRQHandler             (void) __attribute__ ((weak, alias("Fault_Handler")));
void SAI2_IRQHandler              (void) __attribute__ ((weak, alias("Fault_Handler")));
void QUADSPI_IRQHandler           (void) __attribute__ ((weak, alias("Fault_Handler")));
void CEC_IRQHandler               (void) __attribute__ ((weak, alias("Fault_Handler")));
void SPDIF_RX_IRQHandler          (void) __attribute__ ((weak, alias("Fault_Handler")));
void FMPI2C1_EV_IRQHandler        (void) __attribute__ ((weak, alias("Fault_Handler")));
void FMPI2C1_ER_IRQHandler        (void) __attribute__ ((weak, alias("Fault_Handler")));

/*******************************************************************************
 Vector table for STM32F4xx (Cortex-M4F)
*******************************************************************************/

void (* const vectors[])(void) __attribute__ ((used, section(".vectors"))) =
{
	/* Initial stack pointer */
	(void(*)(void))__initial_msp,

	/* Core exceptions */
	Reset_Handler,      /* Reset                                   */
	NMI_Handler,        /* Non-maskable interrupt                  */
	HardFault_Handler,  /* All classes of faults                   */
	MemManage_Handler,  /* Memory management                       */
	BusFault_Handler,   /* Pre-fetch fault, memory access fault    */
	UsageFault_Handler, /* Undefined instruction or illegal state  */
	0, 0, 0, 0,         /* Reserved                                */
	SVC_Handler,        /* System service call via SWI instruction */
	DebugMon_Handler,   /* Debug Monitor                           */
	0,                  /* Reserved                                */
	PendSV_Handler,     /* Pendable request for system service     */
	SysTick_Handler,    /* System tick timer                       */

#ifndef __NO_EXTERNAL_INTERRUPTS

	/* External interrupts */
	WWDG_IRQHandler,
	PVD_IRQHandler,
	TAMP_STAMP_IRQHandler,
	RTC_WKUP_IRQHandler,
	FLASH_IRQHandler,
	RCC_IRQHandler,
	EXTI0_IRQHandler,
	EXTI1_IRQHandler,
	EXTI2_IRQHandler,
	EXTI3_IRQHandler,
	EXTI4_IRQHandler,
	DMA1_Stream0_IRQHandler,
	DMA1_Stream1_IRQHandler,
	DMA1_Stream2_IRQHandler,
	DMA1_Stream3_IRQHandler,
	DMA1_Stream4_IRQHandler,
	DMA1_Stream5_IRQHandler,
	DMA1_Stream6_IRQHandler,
	ADC_IRQHandler,
	CAN1_TX_IRQHandler,
	CAN1_RX0_IRQHandler,
	CAN1_RX1_IRQHandler,
	CAN1_SCE_IRQHandler,
	EXTI9_5_IRQHandler,
	TIM1_BRK_TIM9_IRQHandler,
	TIM1_UP_TIM10_IRQHandler,
	TIM1_TRG_COM_TIM11_IRQHandler,
	TIM1_CC_IRQHandler,
	TIM2_IRQHandler,
	TIM3_IRQHandler,
	TIM4_IRQHandler,
	I2C1_EV_IRQHandler,
	I2C1_ER_IRQHandler,
	I2C2_EV_IRQHandler,
	I2C2_ER_IRQHandler,
	SPI1_IRQHandler,
	SPI2_IRQHandler,
	USART1_IRQHandler,
	USART2_IRQHandler,
	USART3_IRQHandler,
	EXTI15_10_IRQHandler,
	RTC_Alarm_IRQHandler,
	OTG_FS_WKUP_IRQHandler,
	TIM8_BRK_TIM12_IRQHandler,
	TIM8_UP_TIM13_IRQHandler,
	TIM8_TRG_COM_TIM14_IRQHandler,
	TIM8_CC_IRQHandler,
	DMA1_Stream7_IRQHandler,
	FSMC_IRQHandler,
	SDIO_IRQHandler,
	TIM5_IRQHandler,
	SPI3_IRQHandler,
	UART4_IRQHandler,
	UART5_IRQHandler,
	TIM6_DAC_IRQHandler,
	TIM7_IRQHandler,
	DMA2_Stream0_IRQHandler,
	DMA2_Stream1_IRQHandler,
	DMA2_Stream2_IRQHandler,
	DMA2_Stream3_IRQHandler,
	DMA2_Stream4_IRQHandler,
	ETH_IRQHandler,
	ETH_WKUP_IRQHandler,
	CAN2_TX_IRQHandler,
	CAN2_RX0_IRQHandler,
	CAN2_RX1_IRQHandler,
	CAN2_SCE_IRQHandler,
	OTG_FS_IRQHandler,
	DMA2_Stream5_IRQHandler,
	DMA2_Stream6_IRQHandler,
	DMA2_Stream7_IRQHandler,
	USART6_IRQHandler,
	I2C3_EV_IRQHandler,
	I2C3_ER_IRQHandler,
	OTG_HS_EP1_OUT_IRQHandler,
	OTG_HS_EP1_IN_IRQHandler,
	OTG_HS_WKUP_IRQHandler,
	OTG_HS_IRQHandler,
	DCMI_IRQHandler,
	CRYP_IRQHandler,
	HASH_RNG_IRQHandler,
	FPU_IRQHandler,
#if defined(FMPI2C1_ER_IRQn)||defined(DMA2D_IRQn)||defined(SPI5_IRQn)||defined(SPI4_IRQn)
	UART7_IRQHandler,
	UART8_IRQHandler,
	SPI4_IRQHandler,
#endif
#if defined(FMPI2C1_ER_IRQn)||defined(DMA2D_IRQn)||defined(SPI5_IRQn)
	SPI5_IRQHandler,
#endif
#if defined(FMPI2C1_ER_IRQn)||defined(DMA2D_IRQn)
	SPI6_IRQHandler,
	SAI1_IRQHandler,
	LTDC_IRQHandler,
	LTDC_ER_IRQHandler,
	DMA2D_IRQHandler,
#endif
#if defined(FMPI2C1_ER_IRQn)
	SAI2_IRQHandler,
	QUADSPI_IRQHandler,
	CEC_IRQHandler,
	SPDIF_RX_IRQHandler,
	FMPI2C1_EV_IRQHandler,
	FMPI2C1_ER_IRQHandler,
#endif

#endif//__NO_EXTERNAL_INTERRUPTS
};

/******************************************************************************/

#endif//__GNUC__
