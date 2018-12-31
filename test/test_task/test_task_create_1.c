#include "test.h"

static void proc()
{
	        tsk_stop();
}

static void test()
{
	unsigned event;

	tsk_id tsk9 = TSK_CREATE(9, proc);
	tsk_t *tsk8 = TSK_CREATE(8, proc, 512);
	tsk_id tsk7 = TSK_CREATE(7, proc);
	tsk_t *tsk6 = TSK_CREATE(6, proc, 256);

	        tsk_start(tsk9);                     ASSERT_dead(tsk9);
	event = tsk_join(tsk9);                      ASSERT_success(event);
	        tsk_start(tsk8);                     ASSERT_dead(tsk8);
	event = tsk_join(tsk8);                      ASSERT_success(event);
	        tsk_start(tsk7);                     ASSERT_dead(tsk7);
	event = tsk_join(tsk7);                      ASSERT_success(event);
	        tsk_start(tsk6);                     ASSERT_dead(tsk6);
	event = tsk_join(tsk6);                      ASSERT_success(event);
}

void test_task_create_1()
{
	TEST_Notify();
	TEST_Call();
}
