#include "test.h"

void test_once_flag()
{
	TEST_Notify();
	TEST_Add(test_once_flag_0);
	TEST_Add(test_once_flag_1);
	TEST_Add(test_once_flag_2);
	TEST_Add(test_once_flag_3);
}
