#include "test.h"

void test_job_queue()
{
	TEST_Notify();
	TEST_Add(test_job_queue_1);
#ifndef __CSMC__
	TEST_Add(test_job_queue_2);
	TEST_Add(test_job_queue_3);
#endif
}
