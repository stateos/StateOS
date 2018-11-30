#include "test.h"

static void proc1()
{
	unsigned event;

	event = flg_wait(&flg0, 7, flgAll);          assert_success(event);
	        tsk_stop();
}

static void proc2()
{
	unsigned flags;

	flags = flg_give(&flg0, 1);                  assert(flags == 0);
	flags = flg_give(&flg0, 4);                  assert(flags == 0);
	flags = flg_give(&flg0, 2);                  assert(flags == 0);
	        tsk_stop();
}

static void test()
{
	unsigned event;
	                                             assert_dead(tsk1);
	        tsk_startFrom(tsk1, proc1);
	                                             assert_dead(tsk2);
	        tsk_startFrom(tsk2, proc2);
	event = tsk_join(tsk2);                      assert_success(event);
	event = tsk_join(tsk1);                      assert_success(event);
}

void test_flag_0()
{
	int i;
	TEST_Notify();
	for (i = 0; i < PASS; i++)
		test();
}
