#include "test.h"

void test_event()
{
	TEST_Notify();
	TEST_Add(test_event_1);
#ifndef __CSMC__
	TEST_Add(test_event_2);
	TEST_Add(test_event_3);
#endif
}
