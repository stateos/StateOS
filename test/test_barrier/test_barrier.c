#include "test.h"

void test_barrier()
{
	TEST_Notify();
	TEST_Add(test_barrier_1);
#ifndef __CSMC__
	TEST_Add(test_barrier_2);
#endif
}
