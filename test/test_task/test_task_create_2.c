#include "test.h"

static_TSK_START(tsk9, 9)
{
	        tsk_stop();
}

static_TSK_START(tsk8, 8, 256)
{
	unsigned event;

	        tsk_start(tsk9);                     assert_dead(tsk9);
	event = tsk_join(tsk9);                      assert_success(event);
	        tsk_stop();
}

static_TSK_START(tsk7, 7, 512)
{
	unsigned event;

	        tsk_start(tsk8);                     assert_dead(tsk8);
	event = tsk_join(tsk8);                      assert_success(event);
	        tsk_stop();
}

static_TSK_START(tsk6, 6)
{
	unsigned event;

	        tsk_start(tsk7);                     assert_dead(tsk7);
	event = tsk_join(tsk7);                      assert_success(event);
	        tsk_stop();
}

static void test()
{
	unsigned event;

	        tsk_start(tsk6);                     assert_dead(tsk6);
	event = tsk_join(tsk6);                      assert_success(event);
}

void test_task_create_2()
{
	TEST_Notify();
	TEST_Call();
}
