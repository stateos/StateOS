#include "test.h"

void test_mutex()
{
	UNIT_Notify();
	TEST_Add(test_mutex_1);
	TEST_Add(test_mutex_2);
	TEST_Add(test_mutex_3);
#ifndef __CSMC__
	TEST_Add(test_mutex_4);
	TEST_Add(test_mutex_5);
#endif
}
