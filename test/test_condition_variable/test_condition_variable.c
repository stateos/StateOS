#include "test.h"

void test_condition_variable()
{
	TEST_Notify();
	TEST_Add(test_condition_variable_0);
	TEST_Add(test_condition_variable_1);
	TEST_Add(test_condition_variable_2);
	TEST_Add(test_condition_variable_3);
#ifndef __CSMC__
	TEST_Add(test_condition_variable_4);
#endif
}
