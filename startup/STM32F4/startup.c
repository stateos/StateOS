/*******************************************************************************
@file     startup.c
@author   Rajmund Szymanski
@date     04.11.2017
@brief    STM32F4xx startup file.
          After reset the Cortex-M4 processor is in thread mode,
          priority is privileged, and the stack is set to main.
*******************************************************************************/

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
#define main_stack_size  0 // <- default size of main stack
#endif

#ifndef proc_stack_size
#define proc_stack_size  0 // <- default size of process stack
#endif

#define main_stack (((main_stack_size)+7)&(~7))
#define proc_stack (((proc_stack_size)+7)&(~7))

/*******************************************************************************
 Initial stacks' pointers
*******************************************************************************/

extern char __initial_msp[];
extern char __initial_sp [];

/*******************************************************************************
 Cosmic compiler does not know the keyword __attribute__
*******************************************************************************/

#ifdef  __CSMC__
#define __attribute__(attribute)
#endif

/*******************************************************************************
 Default fault handler
*******************************************************************************/

static __attribute__ ((used, noreturn)) void Fault_Handler( void )
{
	/* Go into an infinite loop */
	for (;;);
}

/*******************************************************************************
 Default exit handlers
*******************************************************************************/

#if   defined(__MICROLIB)
void _microlib_exit( void ) __attribute__ ((weak, noreturn, alias("Fault_Handler")));
#elif defined(__ARMCC_VERSION)
void      _sys_exit( void ) __attribute__ ((weak, noreturn, alias("Fault_Handler")));
#elif defined(__GNUC__)
void          _exit( int  ) __attribute__ ((weak, noreturn, alias("Fault_Handler")));
#endif

/*******************************************************************************
 Declaration of exception handlers
*******************************************************************************/

/* Core exceptions */
void Reset_Handler                (void) __attribute__ ((weak, noreturn));
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
#ifdef  __CSMC__
#pragma section const { vector }
#endif
void (* const isr_vectors[])(void) __attribute__ ((used, section(".vectors"))) =
{
	/* Initial stack pointer */
	(void(*)(void)) __initial_msp,

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

/*******************************************************************************
 Specific definitions for the compiler
*******************************************************************************/

#if   defined(__CC_ARM)
#include "ARMCC/startup.h"
#elif defined(__ARMCOMPILER_VERSION)
#include "CLANG/startup.h"
#elif defined(__GNUC__)
#include "GNUCC/startup.h"
#elif defined(__CSMC__)
#include "CSMCC/startup.h"
#else
#error Unknown compiler!
#endif

/*******************************************************************************
 Default reset handler
*******************************************************************************/

void Reset_Handler( void )
{
#if proc_stack_size > 0
	/* Initialize the process stack pointer */
	__set_PSP((uint32_t) __initial_sp);
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
