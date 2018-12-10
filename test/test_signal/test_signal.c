#include "test.h"

void test_signal()
{
	TEST_Notify();
	TEST_Add(test_signal_1);
#ifndef __CSMC__
	TEST_Add(test_signal_2);
#endif
}
