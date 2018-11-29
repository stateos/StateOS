#include "test.h"

void test_event_queue()
{
	TEST_Notify();
	TEST_Add(test_event_queue_0);
	TEST_Add(test_event_queue_1);
	TEST_Add(test_event_queue_2);
}
