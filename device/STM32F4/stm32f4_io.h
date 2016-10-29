/******************************************************************************
 * @file    stm32f4_io.h
 * @author  Rajmund Szymanski
 * @date    24.10.2016
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
#define GPIO_OUT_Pos      (0)
#define GPIO_MODE_Pos     (1)
#define GPIO_OTYPE_Pos    (3)
#define GPIO_OSPEED_Pos   (4)
#define GPIO_PUPD_Pos     (6)
#define GPIO_AF_Pos       (8)

#define GPIO_OUT(cfg)     (0x1u&((cfg)>>GPIO_OUT_Pos))

#define GPIO_Reset        (0x0u<<GPIO_OUT_Pos)  /* reset output pin */
#define GPIO_Set          (0x1u<<GPIO_OUT_Pos)  /* set output pin */

#define GPIO_MODE(cfg)    (0x3u&((cfg)>>GPIO_MODE_Pos))

#define GPIO_Input        (0x0u<<GPIO_MODE_Pos)
#define GPIO_Output       (0x1u<<GPIO_MODE_Pos)
#define GPIO_Alternate    (0x2u<<GPIO_MODE_Pos)
#define GPIO_Analog       (0x3u<<GPIO_MODE_Pos)

#define GPIO_OTYPE(cfg)   (0x1u&((cfg)>>GPIO_OTYPE_Pos))

#define GPIO_PushPull     (0x0u<<GPIO_OTYPE_Pos)
#define GPIO_OpenDrain    (0x1u<<GPIO_OTYPE_Pos)

#define GPIO_OSPEED(cfg)  (0x3u&((cfg)>>GPIO_OSPEED_Pos))

#define GPIO_2MHz         (0x0u<<GPIO_OSPEED_Pos)
#define GPIO_25MHz        (0x1u<<GPIO_OSPEED_Pos)
#define GPIO_50MHz        (0x2u<<GPIO_OSPEED_Pos)
#define GPIO_100MHz       (0x3u<<GPIO_OSPEED_Pos)

#define GPIO_PUPD(cfg)    (0x3u&((cfg)>>GPIO_PUPD_Pos))

#define GPIO_NoPull       (0x0u<<GPIO_PUPD_Pos)
#define GPIO_PullUp       (0x1u<<GPIO_PUPD_Pos)
#define GPIO_PullDown     (0x2u<<GPIO_PUPD_Pos)

#define GPIO_AF(cfg)      (0xFu&((cfg)>>GPIO_AF_Pos))

#define GPIO_AF_RTC_50Hz  (0x0u<<GPIO_AF_Pos)  /* RTC_50Hz Alternate Function mapping */
#define GPIO_AF_MCO       (0x0u<<GPIO_AF_Pos)  /* MCO (MCO1 and MCO2) Alternate Function mapping */
#define GPIO_AF_TAMPER    (0x0u<<GPIO_AF_Pos)  /* TAMPER (TAMPER_1 and TAMPER_2) Alternate Function mapping */
#define GPIO_AF_SWJ       (0x0u<<GPIO_AF_Pos)  /* SWJ (SWD and JTAG) Alternate Function mapping */
#define GPIO_AF_TRACE     (0x0u<<GPIO_AF_Pos)  /* TRACE Alternate Function mapping */
#define GPIO_AF_TIM1      (0x1u<<GPIO_AF_Pos)  /* TIM1 Alternate Function mapping */
#define GPIO_AF_TIM2      (0x1u<<GPIO_AF_Pos)  /* TIM2 Alternate Function mapping */
#define GPIO_AF_TIM3      (0x2u<<GPIO_AF_Pos)  /* TIM3 Alternate Function mapping */
#define GPIO_AF_TIM4      (0x2u<<GPIO_AF_Pos)  /* TIM4 Alternate Function mapping */
#define GPIO_AF_TIM5      (0x2u<<GPIO_AF_Pos)  /* TIM5 Alternate Function mapping */
#define GPIO_AF_TIM8      (0x3u<<GPIO_AF_Pos)  /* TIM8 Alternate Function mapping */
#define GPIO_AF_TIM9      (0x3u<<GPIO_AF_Pos)  /* TIM9 Alternate Function mapping */
#define GPIO_AF_TIM10     (0x3u<<GPIO_AF_Pos)  /* TIM10 Alternate Function mapping */
#define GPIO_AF_TIM11     (0x3u<<GPIO_AF_Pos)  /* TIM11 Alternate Function mapping */
#define GPIO_AF_I2C1      (0x4u<<GPIO_AF_Pos)  /* I2C1 Alternate Function mapping */
#define GPIO_AF_I2C2      (0x4u<<GPIO_AF_Pos)  /* I2C2 Alternate Function mapping */
#define GPIO_AF_I2C3      (0x4u<<GPIO_AF_Pos)  /* I2C3 Alternate Function mapping */
#define GPIO_AF_SPI1      (0x5u<<GPIO_AF_Pos)  /* SPI1 Alternate Function mapping */
#define GPIO_AF_SPI2      (0x5u<<GPIO_AF_Pos)  /* SPI2/I2S2 Alternate Function mapping */
#define GPIO_AF_SPI4      (0x5u<<GPIO_AF_Pos)  /* SPI4 Alternate Function mapping */
#define GPIO_AF_SPI5      (0x5u<<GPIO_AF_Pos)  /* SPI5 Alternate Function mapping */
#define GPIO_AF_SPI6      (0x5u<<GPIO_AF_Pos)  /* SPI6 Alternate Function mapping */
#define GPIO_AF_SPI3      (0x6u<<GPIO_AF_Pos)  /* SPI3/I2S3 Alternate Function mapping */
#define GPIO_AF_SAI1      (0x6u<<GPIO_AF_Pos)  /* SAI1 Alternate Function mapping */
#define GPIO_AF_USART1    (0x7u<<GPIO_AF_Pos)  /* USART1 Alternate Function mapping */
#define GPIO_AF_USART2    (0x7u<<GPIO_AF_Pos)  /* USART2 Alternate Function mapping */
#define GPIO_AF_USART3    (0x7u<<GPIO_AF_Pos)  /* USART3 Alternate Function mapping */
#define GPIO_AF_I2S3ext   (0x7u<<GPIO_AF_Pos)  /* I2S3ext Alternate Function mapping */
#define GPIO_AF_UART4     (0x8u<<GPIO_AF_Pos)  /* UART4 Alternate Function mapping */
#define GPIO_AF_UART5     (0x8u<<GPIO_AF_Pos)  /* UART5 Alternate Function mapping */
#define GPIO_AF_USART6    (0x8u<<GPIO_AF_Pos)  /* USART6 Alternate Function mapping */
#define GPIO_AF_UART7     (0x8u<<GPIO_AF_Pos)  /* UART7 Alternate Function mapping */
#define GPIO_AF_UART8     (0x8u<<GPIO_AF_Pos)  /* UART8 Alternate Function mapping */
#define GPIO_AF_CAN1      (0x9u<<GPIO_AF_Pos)  /* CAN1 Alternate Function mapping */
#define GPIO_AF_CAN2      (0x9u<<GPIO_AF_Pos)  /* CAN2 Alternate Function mapping */
#define GPIO_AF_TIM12     (0x9u<<GPIO_AF_Pos)  /* TIM12 Alternate Function mapping */
#define GPIO_AF_TIM13     (0x9u<<GPIO_AF_Pos)  /* TIM13 Alternate Function mapping */
#define GPIO_AF_TIM14     (0x9u<<GPIO_AF_Pos)  /* TIM14 Alternate Function mapping */
#define GPIO_AF9_I2C2     (0x9u<<GPIO_AF_Pos)  /* I2C2 Alternate Function mapping (Only for STM32F401xx Devices) */
#define GPIO_AF9_I2C3     (0x9u<<GPIO_AF_Pos)  /* I2C3 Alternate Function mapping (Only for STM32F401xx Devices) */
#define GPIO_AF_OTG_FS    (0xAu<<GPIO_AF_Pos)  /* OTG_FS Alternate Function mapping */
#define GPIO_AF_OTG_HS    (0xAu<<GPIO_AF_Pos)  /* OTG_HS Alternate Function mapping */
#define GPIO_AF_ETH       (0xBu<<GPIO_AF_Pos)  /* ETHERNET Alternate Function mapping */
#define GPIO_AF_FSMC      (0xCu<<GPIO_AF_Pos)  /* FSMC Alternate Function mapping */
#define GPIO_AF_FMC       (0xCu<<GPIO_AF_Pos)  /* FMC Alternate Function mapping */
#define GPIO_AF_OTG_HS_FS (0xCu<<GPIO_AF_Pos)  /* OTG HS configured in FS, Alternate Function mapping */
#define GPIO_AF_SDIO      (0xCu<<GPIO_AF_Pos)  /* SDIO Alternate Function mapping */
#define GPIO_AF_DCMI      (0xDu<<GPIO_AF_Pos)  /* DCMI Alternate Function mapping */
#define GPIO_AF_LTDC      (0xEu<<GPIO_AF_Pos)  /* LCD-TFT Alternate Function mapping */
#define GPIO_AF_EVENTOUT  (0xFu<<GPIO_AF_Pos)  /* EVENTOUT Alternate Function mapping */

#define GPIO_AF_OTG1_FS    GPIO_AF_OTG_FS
#define GPIO_AF_OTG2_HS    GPIO_AF_OTG_HS
#define GPIO_AF_OTG2_FS    GPIO_AF_OTG_HS_FS

#define GPIO_Pin_0        (0x1u<< 0)
#define GPIO_Pin_1        (0x1u<< 1)
#define GPIO_Pin_2        (0x1u<< 2)
#define GPIO_Pin_3        (0x1u<< 3)
#define GPIO_Pin_4        (0x1u<< 4)
#define GPIO_Pin_5        (0x1u<< 5)
#define GPIO_Pin_6        (0x1u<< 6)
#define GPIO_Pin_7        (0x1u<< 7)
#define GPIO_Pin_8        (0x1u<< 8)
#define GPIO_Pin_9        (0x1u<< 9)
#define GPIO_Pin_10       (0x1u<<10)
#define GPIO_Pin_11       (0x1u<<11)
#define GPIO_Pin_12       (0x1u<<12)
#define GPIO_Pin_13       (0x1u<<13)
#define GPIO_Pin_14       (0x1u<<14)
#define GPIO_Pin_15       (0x1u<<15)
#define GPIO_Pin_All     ((0x1u<<16)-1)

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

#define GPIO_PORT(gpio)  ((((uint32_t)(gpio))>>10)&0xF)

/* -------------------------------------------------------------------------- */

static inline
uint32_t __stretch( uint32_t pins )
{
	return 
	((pins & 0x0001u) * 0x0001u) |
	((pins & 0x0002u) * 0x0002u) |
	((pins & 0x0004u) * 0x0004u) |
	((pins & 0x0008u) * 0x0008u) |
	((pins & 0x0010u) * 0x0010u) |
	((pins & 0x0020u) * 0x0020u) |
	((pins & 0x0040u) * 0x0040u) |
	((pins & 0x0080u) * 0x0080u) |
	((pins & 0x0100u) * 0x0100u) |
	((pins & 0x0200u) * 0x0200u) |
	((pins & 0x0400u) * 0x0400u) |
	((pins & 0x0800u) * 0x0800u) |
	((pins & 0x1000u) * 0x1000u) |
	((pins & 0x2000u) * 0x2000u) |
	((pins & 0x4000u) * 0x4000u) |
	((pins & 0x8000u) * 0x8000u) ;
}

/* -------------------------------------------------------------------------- */

static inline
void __pinini( GPIO_TypeDef *gpio, uint32_t pins, uint32_t cfg )
{
	uint32_t high;

	if (pins*GPIO_OUT(cfg))    gpio->BSRR    = (pins&GPIO_Pin_All)*GPIO_OUT(cfg);
	if (pins*GPIO_OTYPE(cfg))  gpio->OTYPER  |= pins*GPIO_OTYPE(cfg);

	pins = __stretch(pins);

	if (pins*GPIO_MODE(cfg))   gpio->MODER   |= pins*GPIO_MODE(cfg);
	if (pins*GPIO_OSPEED(cfg)) gpio->OSPEEDR |= pins*GPIO_OSPEED(cfg);
	if (pins*GPIO_PUPD(cfg))   gpio->PUPDR   |= pins*GPIO_PUPD(cfg);

	high = __stretch(pins >> 16);
	pins = __stretch(pins);

	if (pins*GPIO_AF(cfg))     gpio->AFR[0]  |= pins*GPIO_AF(cfg);
	if (high*GPIO_AF(cfg))     gpio->AFR[1]  |= high*GPIO_AF(cfg);
}

/* -------------------------------------------------------------------------- */

static inline
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

static inline
void __pinlck( GPIO_TypeDef *gpio, uint32_t pins )
{
	gpio->LCKR    = pins | 0x00010000;
	gpio->LCKR    = pins;
	gpio->LCKR    = pins | 0x00010000;
	(void)
	gpio->LCKR;
}

/* -------------------------------------------------------------------------- */

static inline
void GPIO_Init( GPIO_TypeDef *gpio, uint32_t pins, uint32_t cfg )
{
	BITBAND(RCC->AHB1ENR)[GPIO_PORT(gpio)] = 1; __DSB();

	__pinini(gpio, pins, cfg);
}

/* -------------------------------------------------------------------------- */

static inline
void GPIO_Config( GPIO_TypeDef *gpio, uint32_t pins, uint32_t cfg )
{
	BITBAND(RCC->AHB1ENR)[GPIO_PORT(gpio)] = 1; __DSB();

	__pincfg(gpio, pins, cfg);
}

/* -------------------------------------------------------------------------- */

static inline
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
	PortT( void ) { BITBAND(RCC->AHB1ENR)[GPIO_PORT(gpio)] = 1; }
	
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
	PinT( void ) { BITBAND(RCC->AHB1ENR)[GPIO_PORT(gpio)] = 1; }

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
