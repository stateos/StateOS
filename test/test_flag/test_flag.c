#include "test.h"

void test_flag()
{
	UNIT_Notify();
	TEST_Add(test_flag_1);
#ifndef __CSMC__
	TEST_Add(test_flag_2);
	TEST_Add(test_flag_3);
#endif
}
