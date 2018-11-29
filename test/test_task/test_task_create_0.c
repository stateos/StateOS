#include "test.h"

static void proc()
{
	        tsk_stop();
}

static void test()
{
	unsigned event;

	tsk_t tsk6 = TSK_INIT(6, proc);
	tsk_t tsk7 = TSK_INIT(7, proc);
	tsk_t tsk8 = TSK_INIT(8, proc);
	tsk_t tsk9 = TSK_INIT(9, proc);

	        tsk_start(&tsk6);
	event = tsk_join(&tsk6);                     assert_success(event);
	        tsk_start(&tsk7);
	event = tsk_join(&tsk7);                     assert_success(event);
	        tsk_start(&tsk8);
	event = tsk_join(&tsk8);                     assert_success(event);
	        tsk_start(&tsk9);
	event = tsk_join(&tsk9);                     assert_success(event);
}

void test_task_create_0()
{
	int i;
	TEST_Notify();
	for (i = 0; i < PASS; i++)
		test();
}
