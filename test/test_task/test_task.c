#include "test.h"

void test_task()
{
	TEST_Notify();
	TEST_Add(test_task_create_0);
	TEST_Add(test_task_create_1);
	TEST_Add(test_task_create_2);
	TEST_Add(test_task_signal_1);
}
