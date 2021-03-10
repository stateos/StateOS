#include "test.h"

void test_timer()
{
	UNIT_Notify();
	TEST_Add(test_timer_1);
#ifndef __CSMC__
	TEST_Add(test_timer_2);
	TEST_Add(test_timer_3);
#endif
}
