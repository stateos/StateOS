#include "test.h"

static_BAR(bar3, 2);

static void proc3()
{
	int result;

	result = bar_wait(bar3);                      ASSERT_success(result);
	result = bar_wait(bar2);                      ASSERT_success(result);
	         tsk_stop();
}

static void proc2()
{
	int result;
	                                              ASSERT_dead(tsk3);
	         tsk_startFrom(tsk3, proc3);          ASSERT_ready(tsk3);
	result = bar_wait(bar2);                      ASSERT_success(result);
	result = bar_wait(bar3);                      ASSERT_success(result);
	result = bar_wait(bar2);                      ASSERT_success(result);
	result = bar_wait(bar1);                      ASSERT_success(result);
	result = tsk_join(tsk3);                      ASSERT_success(result);
	         tsk_stop();
}

static void proc1()
{
	int result;
	                                              ASSERT_dead(tsk2);
	         tsk_startFrom(tsk2, proc2);          ASSERT_ready(tsk2);
	result = bar_wait(bar1);                      ASSERT_success(result);
	result = bar_wait(bar2);                      ASSERT_success(result);
	result = bar_wait(bar1);                      ASSERT_success(result);
	result = bar_wait(&bar0);                     ASSERT_success(result);
	result = tsk_join(tsk2);                      ASSERT_success(result);
	         tsk_stop();
}

static void proc0()
{
	int result;
	                                              ASSERT_dead(tsk1);
	         tsk_startFrom(tsk1, proc1);          ASSERT_ready(tsk1);
	result = bar_wait(&bar0);                     ASSERT_success(result);
	result = bar_wait(bar1);                      ASSERT_success(result);
	result = bar_wait(&bar0);                     ASSERT_success(result);
	result = tsk_join(tsk1);                      ASSERT_success(result);
	         tsk_stop();
}

static void test()
{
	int result;
	                                              ASSERT_dead(&tsk0);
	         tsk_startFrom(&tsk0, proc0);         ASSERT_ready(&tsk0);
	result = bar_wait(&bar0);                     ASSERT_success(result);
	result = tsk_join(&tsk0);                     ASSERT_success(result);
}

extern "C"
void test_barrier_2()
{
	TEST_Notify();
	bar_init(&bar0, 2);
	bar_init(bar1, 2);
	bar_init(bar2, 2);
	TEST_Call();
}
