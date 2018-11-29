#include "test.h"

void test_fast_mutex()
{
	TEST_Notify();
	TEST_Add(test_fast_mutex_0);
	TEST_Add(test_fast_mutex_1);
	TEST_Add(test_fast_mutex_2);
}
