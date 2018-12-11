#include "test.h"

void test_task()
{
	TEST_Notify();
	TEST_Add(test_task_create_0);
	TEST_Add(test_task_create_1);
	TEST_Add(test_task_create_2);
	TEST_Add(test_task_create_3);
	TEST_Add(test_task_infinite_loop_1);
	TEST_Add(test_task_signal_1);
#ifndef __CSMC__
	TEST_Add(test_task_create_4);
#endif
}
