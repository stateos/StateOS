#include "test.h"

void test_barrier()
{
	TEST_Notify();
	TEST_Add(test_barrier_0);
	TEST_Add(test_barrier_1);
	TEST_Add(test_barrier_2);
}
