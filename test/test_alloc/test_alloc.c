#include "test.h"

void test_alloc()
{
	UNIT_Notify();
	TEST_Add(test_alloc_0);
	TEST_Add(test_alloc_1);
	TEST_Add(test_alloc_2);
#ifndef __CSMC__
	TEST_Add(test_alloc_3);
#endif
}
