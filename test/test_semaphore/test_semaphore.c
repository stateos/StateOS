#include "test.h"

void test_semaphore()
{
	UNIT_Notify();
	TEST_Add(test_semaphore_1);
#ifndef __CSMC__
	TEST_Add(test_semaphore_2);
	TEST_Add(test_semaphore_3);
#endif
}
