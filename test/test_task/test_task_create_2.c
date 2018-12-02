#include "test.h"

static_BAR(bar3, 5);

static_TSK_START(tsk6, 6)      { bar_wait(bar3); tsk_stop(); }
static_TSK_START(tsk7, 7, 512) { bar_wait(bar3); tsk_stop(); }
static_TSK_START(tsk8, 8, 256) { bar_wait(bar3); tsk_stop(); }
static_TSK_START(tsk9, 9)      { bar_wait(bar3); tsk_stop(); }

static void test()
{
	unsigned event;

	event = bar_wait(bar3);                      assert_success(event);
	                                             assert_dead(tsk6);
	        tsk_start(tsk6);
	                                             assert_dead(tsk7);
	        tsk_start(tsk7);
	                                             assert_dead(tsk8);
	        tsk_start(tsk8);
	                                             assert_dead(tsk9);
	        tsk_start(tsk9);
}

void test_task_create_2()
{
	int i;
	TEST_Notify();
	for (i = 0; i < PASS; i++)
		test();
}
