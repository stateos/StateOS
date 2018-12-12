#include "test.h"

void test_event_queue()
{
	TEST_Notify();
	TEST_Add(test_event_queue_1);
#ifndef __CSMC__
	TEST_Add(test_event_queue_2);
	TEST_Add(test_event_queue_3);
#endif
}
