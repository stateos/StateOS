#include "test.h"

void test_condition_variable()
{
	UNIT_Notify();
	TEST_Add(test_condition_variable_1);
#ifndef __CSMC__
	TEST_Add(test_condition_variable_2);
	TEST_Add(test_condition_variable_3);
#endif
}
