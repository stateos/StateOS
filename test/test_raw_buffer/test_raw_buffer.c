#include "test.h"

void test_raw_buffer()
{
	UNIT_Notify();
	TEST_Add(test_raw_buffer_1);
#ifndef __CSMC__
	TEST_Add(test_raw_buffer_2);
	TEST_Add(test_raw_buffer_3);
#endif
}
