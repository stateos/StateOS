#include "test.h"

void test_once_flag()
{
	UNIT_Notify();
	TEST_Add(test_once_flag_1);
#ifndef __CSMC__
	TEST_Add(test_once_flag_2);
	TEST_Add(test_once_flag_3);
#endif
}
