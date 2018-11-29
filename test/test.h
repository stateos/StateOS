#include <stm32f4_discovery.h>
#include <os.h>
#include <stdio.h>
#include "test_resources.h"

#pragma once

#define PASS                   1000

#define assert_success(event)  do { (void)(event); assert((event) == E_SUCCESS); } while (0)
#define assert_failure(event)  do { (void)(event); assert((event) == E_FAILURE); } while (0)

#define assert_stopped(tsk)    do { (void)(tsk);   assert((tsk)->hdr.id == ID_STOPPED); } while (0)

#ifdef  __cplusplus
extern "C" {
#endif

void test_add(fun_t *fun);

#ifdef  __cplusplus
}
#endif

#define TEST_Add(fun)          do { void fun (void); test_add(fun); } while (0)
#define TEST_AddUnit(unit)     do { void unit(void); unit();        } while (0)

#ifdef  DEBUG
#ifdef  __CSMC__
#define TEST_Notify()          puts(__FILE__)
#else//!__CSMC__
#define TEST_Notify()          puts(__FUNCTION__)
#endif//__CSMC__
#else//!DEBUG
#define TEST_Notify()          LED_Tick()
#endif//DEBUG
