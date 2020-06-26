#include "test.h"

static_TSK_START(tsk9, 9)
{
	         tsk_stop();
}

static_TSK_START(tsk8, 8, 256)
{
	int result;

	         tsk_start(tsk9);                     ASSERT_dead(tsk9);
	result = tsk_join(tsk9);                      ASSERT_success(result);
	         tsk_stop();
}

static_TSK_START(tsk7, 7, 512)
{
	int result;

	         tsk_start(tsk8);                     ASSERT_dead(tsk8);
	result = tsk_join(tsk8);                      ASSERT_success(result);
	         tsk_stop();
}

static_TSK_START(tsk6, 6)
{
	int result;

	         tsk_start(tsk7);                     ASSERT_dead(tsk7);
	result = tsk_join(tsk7);                      ASSERT_success(result);
	         tsk_stop();
}

static void test()
{
	int result;

	         tsk_start(tsk6);                     ASSERT_dead(tsk6);
	result = tsk_join(tsk6);                      ASSERT_success(result);
}

void test_task_create_2()
{
	TEST_Notify();
	TEST_Call();
}
