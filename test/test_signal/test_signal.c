#include "test.h"

void test_signal()
{
	UNIT_Notify();
	TEST_Add(test_signal_1);
#ifndef __CSMC__
	TEST_Add(test_signal_2);
#endif
}
