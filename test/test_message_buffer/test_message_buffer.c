#include "test.h"

void test_message_buffer()
{
	TEST_Notify();
	TEST_Add(test_message_buffer_1);
#ifndef __CSMC__
	TEST_Add(test_message_buffer_2);
	TEST_Add(test_message_buffer_3);
#endif
}
