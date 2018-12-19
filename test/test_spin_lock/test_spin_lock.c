#include "test.h"

void test_spin_lock()
{
	UNIT_Notify();
//	TEST_Add(test_spin_lock_1);
#ifndef __CSMC__
//	TEST_Add(test_spin_lock_2);
//	TEST_Add(test_spin_lock_3);
#endif
}
