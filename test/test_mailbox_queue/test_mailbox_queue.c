#include "test.h"

void test_mailbox_queue()
{
	UNIT_Notify();
	TEST_Add(test_mailbox_queue_1);
#ifndef __CSMC__
	TEST_Add(test_mailbox_queue_2);
	TEST_Add(test_mailbox_queue_3);
#endif
}
