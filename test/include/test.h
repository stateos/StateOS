#include <stm32f4_discovery.h>
#include <os.h>
#include <stdio.h>
#include "test_resources.h"

#pragma once

#define PASS                   1000

#ifndef DEBUG
#define ASSERT(result)          (void)(result)
#else
#define ASSERT(result)          assert(result)
#endif

#define ASSERT_success(result)  ASSERT((result) == E_SUCCESS)
#define ASSERT_failure(result)  ASSERT((result) == E_FAILURE)
#define ASSERT_stopped(result)  ASSERT((result) == E_STOPPED)
#define ASSERT_deleted(result)  ASSERT((result) == E_DELETED)
#define ASSERT_timeout(result)  ASSERT((result) == E_TIMEOUT)
#define ASSERT_owndead(result)  ASSERT((result) == OWNERDEAD)

#define ASSERT_dead(tsk)        ASSERT((tsk)->hdr.id == ID_STOPPED)
#define ASSERT_ready(tsk)       ASSERT((tsk)->hdr.id == ID_READY)

#ifdef  __cplusplus
extern "C" {
#endif

void test_add (fun_t *fun);
void test_call(fun_t *fun);

#ifdef  __cplusplus
}
#endif

#define TEST_Add(fun)          do { void fun (void); test_add(fun); } while (0)
#define TEST_AddUnit(unit)     do { void unit(void); unit();        } while (0)
#define TEST_Call()            do { test_call(test);                } while (0)

#ifdef  DEBUG
#ifdef  __CSMC__
#define UNIT_Notify()          do { puts(__FILE__); } while (0)
#define TEST_Notify()          do { puts(__FILE__); } while (0)
#else//!__CSMC__
#define UNIT_Notify()          do { puts(__func__); } while (0)
#define TEST_Notify()          do { puts(__func__); } while (0)
#endif//__CSMC__
#else//!DEBUG
#define UNIT_Notify()          do { LED_Tick(); } while (0)
#define TEST_Notify()          do { LED_Tick(); } while (0)
#endif//DEBUG
