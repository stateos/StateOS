/******************************************************************************
 * @file    stm32f4_io.h
 * @author  Rajmund Szymanski
 * @date    18.10.2018
 * @brief   This file contains macro definitions for the STM32F4XX GPIO ports.
 ******************************************************************************/

#ifndef __STM32F4_IO_H
#define __STM32F4_IO_H

#include <stm32f4xx.h>
#include <bitband.h>

#ifdef  __cplusplus
extern "C" {
#endif//__cplusplus

/* -------------------------------------------------------------------------- */
/*
struct __gpio_config
{
	unsigned out:    1;
	unsigned mode:   2;
	unsigned otype:  1;
	unsigned ospeed: 2;
	unsigned pupd:   2;
	unsigned af:     4;
};
*/
#define GPIO_OUT_Pos      (0U)
#define GPIO_OUT_Msk      (0x1UL << GPIO_OUT_Pos)
#define GPIO_OUT(cfg)     _FLD2VAL(GPIO_OUT, cfg)

#define GPIO_Reset        _VAL2FLD(GPIO_OUT, 0) /* reset output pin */
#define GPIO_Set          _VAL2FLD(GPIO_OUT, 1)   /* set output pin */

#define GPIO_MODE_Pos     (1U)
#define GPIO_MODE_Msk     (0x3UL << GPIO_MODE_Pos)
#define GPIO_MODE(cfg)    _FLD2VAL(GPIO_MODE, cfg)

#define GPIO_Input        _VAL2FLD(GPIO_MODE, 0)
#define GPIO_Output       _VAL2FLD(GPIO_MODE, 1)
#define GPIO_Alternate    _VAL2FLD(GPIO_MODE, 2)
#define GPIO_Analog       _VAL2FLD(GPIO_MODE, 3)

#define GPIO_OTYPE_Pos    (3U)
#define GPIO_OTYPE_Msk    (0x1UL << GPIO_OTYPE_Pos)
#define GPIO_OTYPE(cfg)   _FLD2VAL(GPIO_OTYPE, cfg)

#define GPIO_PushPull     _VAL2FLD(GPIO_OTYPE, 0)
#define GPIO_OpenDrain    _VAL2FLD(GPIO_OTYPE, 1)

#define GPIO_OSPEED_Pos   (4U)
#define GPIO_OSPEED_Msk   (0x3UL << GPIO_OSPEED_Pos)
#define GPIO_OSPEED(cfg)  _FLD2VAL(GPIO_OSPEED, cfg)

#define GPIO_2MHz         _VAL2FLD(GPIO_OSPEED, 0)
#define GPIO_25MHz        _VAL2FLD(GPIO_OSPEED, 1)
#define GPIO_50MHz        _VAL2FLD(GPIO_OSPEED, 2)
#define GPIO_100MHz       _VAL2FLD(GPIO_OSPEED, 3)

#define GPIO_PUPD_Pos     (6U)
#define GPIO_PUPD_Msk     (0x3UL << GPIO_PUPD_Pos)
#define GPIO_PUPD(cfg)    _FLD2VAL(GPIO_PUPD, cfg)

#define GPIO_NoPull       _VAL2FLD(GPIO_PUPD, 0)
#define GPIO_PullUp       _VAL2FLD(GPIO_PUPD, 1)
#define GPIO_PullDown     _VAL2FLD(GPIO_PUPD, 2)

#define GPIO_AF_Pos       (8U)
#define GPIO_AF_Msk       (0xFUL << GPIO_AF_Pos)
#define GPIO_AF(cfg)      _FLD2VAL(GPIO_AF, cfg)

#define GPIO_AF_RTC_50Hz  _VAL2FLD(GPIO_AF, 0)  /* RTC_50Hz Alternate Function mapping */
#define GPIO_AF_MCO       _VAL2FLD(GPIO_AF, 0)  /* MCO (MCO1 and MCO2) Alternate Function mapping */
#define GPIO_AF_TAMPER    _VAL2FLD(GPIO_AF, 0)  /* TAMPER (TAMPER_1 and TAMPER_2) Alternate Function mapping */
#define GPIO_AF_SWJ       _VAL2FLD(GPIO_AF, 0)  /* SWJ (SWD and JTAG) Alternate Function mapping */
#define GPIO_AF_TRACE     _VAL2FLD(GPIO_AF, 0)  /* TRACE Alternate Function mapping */
#define GPIO_AF_TIM1      _VAL2FLD(GPIO_AF, 1)  /* TIM1 Alternate Function mapping */
#define GPIO_AF_TIM2      _VAL2FLD(GPIO_AF, 1)  /* TIM2 Alternate Function mapping */
#define GPIO_AF_TIM3      _VAL2FLD(GPIO_AF, 2)  /* TIM3 Alternate Function mapping */
#define GPIO_AF_TIM4      _VAL2FLD(GPIO_AF, 2)  /* TIM4 Alternate Function mapping */
#define GPIO_AF_TIM5      _VAL2FLD(GPIO_AF, 2)  /* TIM5 Alternate Function mapping */
#define GPIO_AF_TIM8      _VAL2FLD(GPIO_AF, 3)  /* TIM8 Alternate Function mapping */
#define GPIO_AF_TIM9      _VAL2FLD(GPIO_AF, 3)  /* TIM9 Alternate Function mapping */
#define GPIO_AF_TIM10     _VAL2FLD(GPIO_AF, 3)  /* TIM10 Alternate Function mapping */
#define GPIO_AF_TIM11     _VAL2FLD(GPIO_AF, 3)  /* TIM11 Alternate Function mapping */
#define GPIO_AF_I2C1      _VAL2FLD(GPIO_AF, 4)  /* I2C1 Alternate Function mapping */
#define GPIO_AF_I2C2      _VAL2FLD(GPIO_AF, 4)  /* I2C2 Alternate Function mapping */
#define GPIO_AF_I2C3      _VAL2FLD(GPIO_AF, 4)  /* I2C3 Alternate Function mapping */
#define GPIO_AF_SPI1      _VAL2FLD(GPIO_AF, 5)  /* SPI1 Alternate Function mapping */
#define GPIO_AF_SPI2      _VAL2FLD(GPIO_AF, 5)  /* SPI2/I2S2 Alternate Function mapping */
#define GPIO_AF_SPI4      _VAL2FLD(GPIO_AF, 5)  /* SPI4 Alternate Function mapping */
#define GPIO_AF_SPI5      _VAL2FLD(GPIO_AF, 5)  /* SPI5 Alternate Function mapping */
#define GPIO_AF_SPI6      _VAL2FLD(GPIO_AF, 5)  /* SPI6 Alternate Function mapping */
#define GPIO_AF_SPI3      _VAL2FLD(GPIO_AF, 6)  /* SPI3/I2S3 Alternate Function mapping */
#define GPIO_AF_SAI1      _VAL2FLD(GPIO_AF, 6)  /* SAI1 Alternate Function mapping */
#define GPIO_AF_USART1    _VAL2FLD(GPIO_AF, 7)  /* USART1 Alternate Function mapping */
#define GPIO_AF_USART2    _VAL2FLD(GPIO_AF, 7)  /* USART2 Alternate Function mapping */
#define GPIO_AF_USART3    _VAL2FLD(GPIO_AF, 7)  /* USART3 Alternate Function mapping */
#define GPIO_AF_I2S3ext   _VAL2FLD(GPIO_AF, 7)  /* I2S3ext Alternate Function mapping */
#define GPIO_AF_UART4     _VAL2FLD(GPIO_AF, 8)  /* UART4 Alternate Function mapping */
#define GPIO_AF_UART5     _VAL2FLD(GPIO_AF, 8)  /* UART5 Alternate Function mapping */
#define GPIO_AF_USART6    _VAL2FLD(GPIO_AF, 8)  /* USART6 Alternate Function mapping */
#define GPIO_AF_UART7     _VAL2FLD(GPIO_AF, 8)  /* UART7 Alternate Function mapping */
#define GPIO_AF_UART8     _VAL2FLD(GPIO_AF, 8)  /* UART8 Alternate Function mapping */
#define GPIO_AF_CAN1      _VAL2FLD(GPIO_AF, 9)  /* CAN1 Alternate Function mapping */
#define GPIO_AF_CAN2      _VAL2FLD(GPIO_AF, 9)  /* CAN2 Alternate Function mapping */
#define GPIO_AF_TIM12     _VAL2FLD(GPIO_AF, 9)  /* TIM12 Alternate Function mapping */
#define GPIO_AF_TIM13     _VAL2FLD(GPIO_AF, 9)  /* TIM13 Alternate Function mapping */
#define GPIO_AF_TIM14     _VAL2FLD(GPIO_AF, 9)  /* TIM14 Alternate Function mapping */
#define GPIO_AF9_I2C2     _VAL2FLD(GPIO_AF, 9)  /* I2C2 Alternate Function mapping */
#define GPIO_AF9_I2C3     _VAL2FLD(GPIO_AF, 9)  /* I2C3 Alternate Function mapping */
#define GPIO_AF_OTG_FS    _VAL2FLD(GPIO_AF,10)  /* OTG_FS Alternate Function mapping */
#define GPIO_AF_OTG_HS    _VAL2FLD(GPIO_AF,10)  /* OTG_HS Alternate Function mapping */
#define GPIO_AF_ETH       _VAL2FLD(GPIO_AF,11)  /* ETHERNET Alternate Function mapping */
#define GPIO_AF_FSMC      _VAL2FLD(GPIO_AF,12)  /* FSMC Alternate Function mapping */
#define GPIO_AF_FMC       _VAL2FLD(GPIO_AF,12)  /* FMC Alternate Function mapping */
#define GPIO_AF_OTG_HS_FS _VAL2FLD(GPIO_AF,12)  /* OTG HS configured in FS, Alternate Function mapping */
#define GPIO_AF_SDIO      _VAL2FLD(GPIO_AF,12)  /* SDIO Alternate Function mapping */
#define GPIO_AF_DCMI      _VAL2FLD(GPIO_AF,13)  /* DCMI Alternate Function mapping */
#define GPIO_AF_LTDC      _VAL2FLD(GPIO_AF,14)  /* LCD-TFT Alternate Function mapping */
#define GPIO_AF_EVENTOUT  _VAL2FLD(GPIO_AF,15)  /* EVENTOUT Alternate Function mapping */

#define GPIO_AF_OTG1_FS    GPIO_AF_OTG_FS
#define GPIO_AF_OTG2_HS    GPIO_AF_OTG_HS
#define GPIO_AF_OTG2_FS    GPIO_AF_OTG_HS_FS

#define GPIO_Pin_0        (1UL<< 0U)
#define GPIO_Pin_1        (1UL<< 1U)
#define GPIO_Pin_2        (1UL<< 2U)
#define GPIO_Pin_3        (1UL<< 3U)
#define GPIO_Pin_4        (1UL<< 4U)
#define GPIO_Pin_5        (1UL<< 5U)
#define GPIO_Pin_6        (1UL<< 6U)
#define GPIO_Pin_7        (1UL<< 7U)
#define GPIO_Pin_8        (1UL<< 8U)
#define GPIO_Pin_9        (1UL<< 9U)
#define GPIO_Pin_10       (1UL<<10U)
#define GPIO_Pin_11       (1UL<<11U)
#define GPIO_Pin_12       (1UL<<12U)
#define GPIO_Pin_13       (1UL<<13U)
#define GPIO_Pin_14       (1UL<<14U)
#define GPIO_Pin_15       (1UL<<15U)
#define GPIO_Pin_All     ((1UL<<16U)-1UL)

#define GPIO_Input_NoPull                (GPIO_Input | GPIO_NoPull)
#define GPIO_Input_PullUp                (GPIO_Input | GPIO_PullUp)
#define GPIO_Input_PullDown              (GPIO_Input | GPIO_PullDown)

#define GPIO_Output_2MHz                 (GPIO_Output | GPIO_2MHz)
#define GPIO_Output_25MHz                (GPIO_Output | GPIO_25MHz)
#define GPIO_Output_50MHz                (GPIO_Output | GPIO_50MHz)
#define GPIO_Output_100MHz               (GPIO_Output | GPIO_100MHz)

#define GPIO_Output_PushPull             (GPIO_Output | GPIO_PushPull)
#define GPIO_Output_PushPull_2MHz        (GPIO_Output | GPIO_PushPull | GPIO_2MHz)
#define GPIO_Output_PushPull_25MHz       (GPIO_Output | GPIO_PushPull | GPIO_25MHz)
#define GPIO_Output_PushPull_50MHz       (GPIO_Output | GPIO_PushPull | GPIO_50MHz)
#define GPIO_Output_PushPull_100MHz      (GPIO_Output | GPIO_PushPull | GPIO_100MHz)

#define GPIO_Output_OpenDrain            (GPIO_Output | GPIO_OpenDrain)
#define GPIO_Output_OpenDrain_2MHz       (GPIO_Output | GPIO_OpenDrain | GPIO_2MHz)
#define GPIO_Output_OpenDrain_25MHz      (GPIO_Output | GPIO_OpenDrain | GPIO_25MHz)
#define GPIO_Output_OpenDrain_50MHz      (GPIO_Output | GPIO_OpenDrain | GPIO_50MHz)
#define GPIO_Output_OpenDrain_100MHz     (GPIO_Output | GPIO_OpenDrain | GPIO_100MHz)

#define GPIO_Alternate_2MHz              (GPIO_Alternate | GPIO_2MHz)
#define GPIO_Alternate_25MHz             (GPIO_Alternate | GPIO_25MHz)
#define GPIO_Alternate_50MHz             (GPIO_Alternate | GPIO_50MHz)
#define GPIO_Alternate_100MHz            (GPIO_Alternate | GPIO_100MHz)

#define GPIO_Alternate_PushPull          (GPIO_Alternate | GPIO_PushPull)
#define GPIO_Alternate_PushPull_2MHz     (GPIO_Alternate | GPIO_PushPull | GPIO_2MHz)
#define GPIO_Alternate_PushPull_25MHz    (GPIO_Alternate | GPIO_PushPull | GPIO_25MHz)
#define GPIO_Alternate_PushPull_50MHz    (GPIO_Alternate | GPIO_PushPull | GPIO_50MHz)
#define GPIO_Alternate_PushPull_100MHz   (GPIO_Alternate | GPIO_PushPull | GPIO_100MHz)

#define GPIO_Alternate_OpenDrain         (GPIO_Alternate | GPIO_OpenDrain)
#define GPIO_Alternate_OpenDrain_2MHz    (GPIO_Alternate | GPIO_OpenDrain | GPIO_2MHz)
#define GPIO_Alternate_OpenDrain_25MHz   (GPIO_Alternate | GPIO_OpenDrain | GPIO_25MHz)
#define GPIO_Alternate_OpenDrain_50MHz   (GPIO_Alternate | GPIO_OpenDrain | GPIO_50MHz)
#define GPIO_Alternate_OpenDrain_100MHz  (GPIO_Alternate | GPIO_OpenDrain | GPIO_100MHz)

/* -------------------------------------------------------------------------- */

#define GPIO_PORT_Pos     (10U)
#define GPIO_PORT_Msk     (0xFUL << GPIO_PORT_Pos)
#define GPIO_PORT(gpio)   _FLD2VAL(GPIO_PORT, gpio)

/* -------------------------------------------------------------------------- */

__STATIC_FORCEINLINE
uint32_t __stretch( uint32_t pins )
{
	return 
	((pins & 0x0001UL) * 0x0001UL) |
	((pins & 0x0002UL) * 0x0002UL) |
	((pins & 0x0004UL) * 0x0004UL) |
	((pins & 0x0008UL) * 0x0008UL) |
	((pins & 0x0010UL) * 0x0010UL) |
	((pins & 0x0020UL) * 0x0020UL) |
	((pins & 0x0040UL) * 0x0040UL) |
	((pins & 0x0080UL) * 0x0080UL) |
	((pins & 0x0100UL) * 0x0100UL) |
	((pins & 0x0200UL) * 0x0200UL) |
	((pins & 0x0400UL) * 0x0400UL) |
	((pins & 0x0800UL) * 0x0800UL) |
	((pins & 0x1000UL) * 0x1000UL) |
	((pins & 0x2000UL) * 0x2000UL) |
	((pins & 0x4000UL) * 0x4000UL) |
	((pins & 0x8000UL) * 0x8000UL) ;
}

/* -------------------------------------------------------------------------- */

__STATIC_FORCEINLINE
void __pinini( GPIO_TypeDef *gpio, uint32_t pins, uint32_t cfg )
{
	uint32_t high;

	if (0UL != pins*GPIO_OUT(cfg))    gpio->BSRR    = (pins&GPIO_Pin_All)*GPIO_OUT(cfg);
	if (0UL != pins*GPIO_OTYPE(cfg))  gpio->OTYPER  |= pins*GPIO_OTYPE(cfg);

	pins = __stretch(pins);

	if (0UL != pins*GPIO_MODE(cfg))   gpio->MODER   |= pins*GPIO_MODE(cfg);
	if (0UL != pins*GPIO_OSPEED(cfg)) gpio->OSPEEDR |= pins*GPIO_OSPEED(cfg);
	if (0UL != pins*GPIO_PUPD(cfg))   gpio->PUPDR   |= pins*GPIO_PUPD(cfg);

	high = __stretch(pins >> 16);
	pins = __stretch(pins);

	if (0UL != pins*GPIO_AF(cfg))     gpio->AFR[0]  |= pins*GPIO_AF(cfg);
	if (0UL != high*GPIO_AF(cfg))     gpio->AFR[1]  |= high*GPIO_AF(cfg);
}

/* -------------------------------------------------------------------------- */

__STATIC_FORCEINLINE
void __pincfg( GPIO_TypeDef *gpio, uint32_t pins, uint32_t cfg )
{
	uint32_t high;

	gpio->BSRR    = (pins & GPIO_Pin_All) << (16 * GPIO_OUT(~cfg));

	gpio->OTYPER  = (gpio->OTYPER  & ~(pins * 0x1)) | (pins * GPIO_OTYPE(cfg));

	pins = __stretch(pins);

	gpio->MODER   = (gpio->MODER   & ~(pins * 0x3)) | (pins * GPIO_MODE(cfg));
	gpio->OSPEEDR = (gpio->OSPEEDR & ~(pins * 0x3)) | (pins * GPIO_OSPEED(cfg));
	gpio->PUPDR   = (gpio->PUPDR   & ~(pins * 0x3)) | (pins * GPIO_PUPD(cfg));

	high = __stretch(pins>>16);
	pins = __stretch(pins);

	if (pins)
	gpio->AFR[0]  = (gpio->AFR[0]  & ~(pins * 0xF)) | (pins * GPIO_AF(cfg));
	if (high)
	gpio->AFR[1]  = (gpio->AFR[1]  & ~(high * 0xF)) | (high * GPIO_AF(cfg));
}

/* -------------------------------------------------------------------------- */

__STATIC_FORCEINLINE
void __pinlck( GPIO_TypeDef *gpio, uint32_t pins )
{
	gpio->LCKR    = pins | 0x00010000;
	gpio->LCKR    = pins;
	gpio->LCKR    = pins | 0x00010000;
	(void)
	gpio->LCKR;
}

/* -------------------------------------------------------------------------- */

__STATIC_FORCEINLINE
void GPIO_Init( GPIO_TypeDef *gpio, uint32_t pins, uint32_t cfg )
{
	RCC->AHB1ENR |= 1U << GPIO_PORT(gpio); RCC->AHB1ENR;

	__pinini(gpio, pins, cfg);
}

/* -------------------------------------------------------------------------- */

static inline
void GPIO_Config( GPIO_TypeDef *gpio, uint32_t pins, uint32_t cfg )
{
	RCC->AHB1ENR |= 1U << GPIO_PORT(gpio); RCC->AHB1ENR;

	__pincfg(gpio, pins, cfg);
}

/* -------------------------------------------------------------------------- */

__STATIC_FORCEINLINE
void GPIO_Lock( GPIO_TypeDef *gpio, uint32_t pins )
{
	__pinlck(gpio, pins);
}

/* -------------------------------------------------------------------------- */

#ifdef  __cplusplus
}
#endif//__cplusplus

/* -------------------------------------------------------------------------- */

#ifdef  __cplusplus

/* -------------------------------------------------------------------------- */

template<const unsigned gpio>
class PortT
{
public:
	PortT( void ) { RCC->AHB1ENR |= 1U << GPIO_PORT(gpio); RCC->AHB1ENR; }
	
	void init  ( const unsigned pins, const unsigned cfg ) { __pinini((GPIO_TypeDef *)gpio, pins, cfg); }
	void config( const unsigned pins, const unsigned cfg ) { __pincfg((GPIO_TypeDef *)gpio, pins, cfg); }
	void lock  ( const unsigned pins )                     { __pinlck((GPIO_TypeDef *)gpio, pins);      }

	unsigned   operator ()( const unsigned number ) { return BITBAND(((GPIO_TypeDef *)gpio)->IDR)[number]; }
	volatile
	unsigned & operator []( const unsigned number ) { return BITBAND(((GPIO_TypeDef *)gpio)->ODR)[number]; }
};

#ifdef  GPIOA_BASE
typedef PortT<GPIOA_BASE> PORTA;
#endif
#ifdef  GPIOB_BASE
typedef PortT<GPIOB_BASE> PORTB;
#endif
#ifdef  GPIOC_BASE
typedef PortT<GPIOC_BASE> PORTC;
#endif
#ifdef  GPIOD_BASE
typedef PortT<GPIOD_BASE> PORTD;
#endif
#ifdef  GPIOE_BASE
typedef PortT<GPIOE_BASE> PORTE;
#endif
#ifdef  GPIOF_BASE
typedef PortT<GPIOF_BASE> PORTF;
#endif
#ifdef  GPIOG_BASE
typedef PortT<GPIOG_BASE> PORTG;
#endif
#ifdef  GPIOH_BASE
typedef PortT<GPIOH_BASE> PORTH;
#endif
#ifdef  GPIOI_BASE
typedef PortT<GPIOI_BASE> PORTI;
#endif
#ifdef  GPIOK_BASE
typedef PortT<GPIOK_BASE> PORTK;
#endif

/* -------------------------------------------------------------------------- */

template<const unsigned gpio, const unsigned pin>
class PinT
{
public:
	PinT( void ) { RCC->AHB1ENR |= 1U << GPIO_PORT(gpio); RCC->AHB1ENR; }

	void init  ( const unsigned cfg ) { __pinini((GPIO_TypeDef *)gpio, 1 << pin, cfg); }
	void config( const unsigned cfg ) { __pincfg((GPIO_TypeDef *)gpio, 1 << pin, cfg); }
	void lock  ( void )               { __pinlck((GPIO_TypeDef *)gpio, 1 << pin);      }

	unsigned operator ()( void )                 { return              BITBAND(((GPIO_TypeDef *)gpio)->IDR)[pin]; }
	operator unsigned & ( void )                 { return (unsigned &)(BITBAND(((GPIO_TypeDef *)gpio)->ODR)[pin]); }
	unsigned operator = ( const unsigned value ) { return              BITBAND(((GPIO_TypeDef *)gpio)->ODR)[pin] = value; }
};

/* -------------------------------------------------------------------------- */

#endif//__cplusplus

#endif//__STM32F4_IO_H
