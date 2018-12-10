#include "test.h"

void test_stream_buffer()
{
	TEST_Notify();
	TEST_Add(test_stream_buffer_1);
#ifndef __CSMC__
	TEST_Add(test_stream_buffer_2);
#endif
}
