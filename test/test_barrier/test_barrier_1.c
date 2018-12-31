#include "test.h"

static_BAR(bar3, 2);

static void proc3()
{
	unsigned event;

	event = bar_wait(bar3);                      ASSERT_success(event);
	event = bar_wait(bar2);                      ASSERT_success(event);
	        tsk_stop();
}

static void proc2()
{
	unsigned event;
		                                         ASSERT_dead(tsk3);
	        tsk_startFrom(tsk3, proc3);          ASSERT_ready(tsk3);
	event = bar_wait(bar2);                      ASSERT_success(event);
	event = bar_wait(bar3);                      ASSERT_success(event);
	event = bar_wait(bar2);                      ASSERT_success(event);
	event = bar_wait(bar1);                      ASSERT_success(event);
	event = tsk_join(tsk3);                      ASSERT_success(event);
	        tsk_stop();
}

static void proc1()
{
	unsigned event;
		                                         ASSERT_dead(tsk2);
	        tsk_startFrom(tsk2, proc2);          ASSERT_ready(tsk2);
	event = bar_wait(bar1);                      ASSERT_success(event);
	event = bar_wait(bar2);                      ASSERT_success(event);
	event = bar_wait(bar1);                      ASSERT_success(event);
	event = bar_wait(&bar0);                     ASSERT_success(event);
	event = tsk_join(tsk2);                      ASSERT_success(event);
	        tsk_stop();
}

static void proc0()
{
	unsigned event;
		                                         ASSERT_dead(tsk1);
	        tsk_startFrom(tsk1, proc1);          ASSERT_ready(tsk1);
	event = bar_wait(&bar0);                     ASSERT_success(event);
	event = bar_wait(bar1);                      ASSERT_success(event);
	event = bar_wait(&bar0);                     ASSERT_success(event);
	event = tsk_join(tsk1);                      ASSERT_success(event);
	        tsk_stop();
}

static void test()
{
	unsigned event;
		                                         ASSERT_dead(&tsk0);
	        tsk_startFrom(&tsk0, proc0);         ASSERT_ready(&tsk0);
	event = bar_wait(&bar0);                     ASSERT_success(event);
	event = tsk_join(&tsk0);                     ASSERT_success(event);
}

void test_barrier_1()
{
	TEST_Notify();
	bar_init(&bar0, 2);
	bar_init(bar1, 2);
	bar_init(bar2, 2);
	TEST_Call();
}
