#include "test.h"

static_TMR_DEF(tmr3) {}

static_TMR_START(tmr4, 4, 0) {}
static_TMR_START(tmr5, 5, 0) {}
static_TMR_START(tmr6, 6, 0) {}
static_TMR_START(tmr7, 7, 0) {}

static int counter;

static void proc()
{
	sys_lock();
	{
		counter++;
	}
	sys_unlock();
}

static void test()
{
	int result;

	         counter = 0;
	result = tmr_wait(tmr4);                      ASSERT_success(result);
	result = tmr_wait(tmr5);                      ASSERT_success(result);
	result = tmr_wait(tmr6);                      ASSERT_success(result);
	result = tmr_wait(tmr7);                      ASSERT_success(result);
	         tmr_startFrom(tmr3, 3, 0, proc);
	         tmr_startFrom(tmr2, 2, 0, proc);
	         tmr_startFrom(tmr1, 1, 0, proc);
	         tmr_startFrom(&tmr0, 0, 0, proc);
	result = tmr_wait(&tmr0);                     ASSERT_success(result);
	result = tmr_wait(tmr1);                      ASSERT_success(result);
	result = tmr_wait(tmr2);                      ASSERT_success(result);
	result = tmr_wait(tmr3);                      ASSERT_success(result);
	                                              ASSERT(counter == 4);
}

void test_timer_1()
{
	TEST_Notify();
	TEST_Call();
}
