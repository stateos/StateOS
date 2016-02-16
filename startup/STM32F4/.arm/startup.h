/*******************************************************************************
@file     startup.h
@author   Rajmund Szymanski
@date     16.02.2016
@brief    Definitions for STM32F4xx startup file.
*******************************************************************************/

#pragma once

/*******************************************************************************
 Specific definitions for the chip
*******************************************************************************/

#define __rom_start 0x08000000
#define __rom_size  0x00100000
#define __rom_end  (__rom_start+__rom_size)
#define __ccm_start 0x10000000
#define __ccm_size  0x00010000
#define __ccm_end  (__ccm_start+__ccm_size)
#define __ram_start 0x20000000
#define __ram_size  0x00020000
#define __ram_end  (__ram_start+__ram_size)

/*******************************************************************************
 Configuration of stacks
*******************************************************************************/

#ifndef   proc_stack_size
#define   proc_stack_size 1024
#endif
#define   proc_stack ((((proc_stack_size)+7)/8)*8)

#ifndef   main_stack_size
#define   main_stack_size 1024
#endif
#define   main_stack ((((main_stack_size)+7)/8)*8)
