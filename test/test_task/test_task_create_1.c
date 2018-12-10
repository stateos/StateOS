#include "test.h"

static void proc()
{
	        tsk_stop();
}

static void test()
{
	unsigned event;

	tsk_t *tsk6 = TSK_CREATE(6, proc, 256);
	tsk_id tsk7 = TSK_CREATE(7, proc);
	tsk_t *tsk8 = TSK_CREATE(8, proc, 512);
	tsk_id tsk9 = TSK_CREATE(9, proc);

	        tsk_start(tsk6);
	event = tsk_join(tsk6);                      assert_success(event);
	        tsk_start(tsk7);
	event = tsk_join(tsk7);                      assert_success(event);
	        tsk_start(tsk8);
	event = tsk_join(tsk8);                      assert_success(event);
	        tsk_start(tsk9);
	event = tsk_join(tsk9);                      assert_success(event);
}

void test_task_create_1()
{
	TEST_Notify();
	TEST_Call();
}
