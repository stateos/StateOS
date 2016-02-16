/*******************************************************************************
@file     startup.h
@author   Rajmund Szymanski
@date     16.02.2016
@brief    Definitions for STM32F4xx startup file.
*******************************************************************************/

#pragma once

/*******************************************************************************
 Configuration of stacks
*******************************************************************************/

#ifndef   proc_stack_size
#define   proc_stack_size 1024
#endif
#define   proc_stack  (((proc_stack_size)+7)&(~7))

#ifndef   main_stack_size
#define   main_stack_size 1024
#endif
#define   main_stack  (((main_stack_size)+7)&(~7))

