#include "test.h"

static_BAR(bar3, 3);

static void proc1()
{
	unsigned event;

	event = bar_wait(bar3);                      assert_success(event);
	        tsk_stop();
}

static void proc2()
{
	unsigned event;
	                                             assert_dead(tsk1);
	        tsk_startFrom(tsk1, proc1);
	event = bar_wait(bar3);                      assert_success(event);
	event = tsk_join(tsk1);                      assert_success(event);
	        tsk_stop();
}

static void test()
{
	unsigned event;
	                                             assert_dead(tsk2);
	        tsk_startFrom(tsk2, proc2);
	event = bar_wait(bar3);                      assert_success(event);
	event = tsk_join(tsk2);                      assert_success(event);
}

void test_barrier_3()
{
	int i;
	TEST_Notify();
	for (i = 0; i < PASS; i++)
		test();
}
