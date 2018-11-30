#include "test.h"

void test_memory_pool()
{
	TEST_Notify();
	TEST_Add(test_memory_pool_0);
	TEST_Add(test_memory_pool_1);
	TEST_Add(test_memory_pool_2);
	TEST_Add(test_memory_pool_3);
}
