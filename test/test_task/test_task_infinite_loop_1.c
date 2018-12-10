#include "test.h"

void proc2()
{
	unsigned event;

	event = tsk_kill(tsk1);                      assert_success(event);
	        tsk_stop();
}

void proc1()
{
	                                             assert_dead(tsk2);
	        tsk_startFrom(tsk2, proc2);
	        for (;;);
}

static void test()
{
	unsigned event;
	                                             assert_dead(tsk1);
	        tsk_startFrom(tsk1, proc1);
	event = tsk_join(tsk2);                      assert_success(event);
	event = tsk_join(tsk1);                      assert_success(event);
}

void test_task_infinite_loop_1()
{
	TEST_Notify();
	TEST_Call();
}
