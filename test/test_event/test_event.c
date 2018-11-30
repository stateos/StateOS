#include "test.h"

void test_event()
{
	TEST_Notify();
	TEST_Add(test_event_0);
	TEST_Add(test_event_1);
	TEST_Add(test_event_2);
	TEST_Add(test_event_3);
}
