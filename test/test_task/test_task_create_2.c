#include "test.h"

static_TSK_START(tsk6, 6)      { tsk_stop(); }
static_TSK_START(tsk7, 7, 512) { tsk_stop(); }
static_TSK_START(tsk8, 8, 256) { tsk_stop(); }
static_TSK_START(tsk9, 9)      { tsk_stop(); }

static void test()
{
	                                             assert_dead(tsk6);
	        tsk_start(tsk6);                     assert_dead(tsk6);
	                                             assert_dead(tsk7);
	        tsk_start(tsk7);                     assert_dead(tsk7);
	                                             assert_dead(tsk8);
	        tsk_start(tsk8);                     assert_dead(tsk8);
	                                             assert_dead(tsk9);
	        tsk_start(tsk9);                     assert_dead(tsk9);
}

void test_task_create_2()
{
	int i;
	TEST_Notify();
	for (i = 0; i < PASS; i++)
		test();
}
