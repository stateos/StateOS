#include "test.h"

void test_alloc()
{
	TEST_Notify();
	TEST_Add(test_alloc_1);
	TEST_Add(test_alloc_2);
}
