#include "test.h"

static_FLG(flg3, ~0U);

static void proc1()
{
	unsigned event;

	event = flg_wait(flg3, 7, flgAll+flgProtect+flgIgnore); assert_success(event);
	        tsk_stop();
}

static void proc2()
{
	unsigned flags;

	flags = flg_give(flg3, 2);                   assert(flags == ~0U);
	flags = flg_give(flg3, 4);                   assert(flags == ~0U);
	flags = flg_give(flg3, 1);                   assert(flags == ~0U);
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

void test_flag_3()
{
	int i;
	TEST_Notify();
	for (i = 0; i < PASS; i++)
		test();
}
