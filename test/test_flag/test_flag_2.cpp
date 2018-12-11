#include "test.h"

static_FLG(flg3, ~0U);

static void proc3()
{
	unsigned event;

	event = flg_wait(flg3, 7, flgAll+flgProtect+flgIgnore); assert_success(event);
	        tsk_stop();
}

static void proc2()
{
	unsigned flags;
	unsigned event;
	                                             assert_dead(tsk3);
	        tsk_startFrom(tsk3, proc3);          assert_ready(tsk3);
	event = flg_wait(flg2, 7, flgAll);           assert_success(event);
	flags = flg_give(flg3, 2);                   assert(flags == ~0U);
	flags = flg_give(flg3, 4);                   assert(flags == ~0U);
	flags = flg_give(flg3, 1);                   assert(flags == ~0U);
	event = tsk_join(tsk3);                      assert_success(event);
	        tsk_stop();
}

static void proc1()
{
	unsigned flags;
	unsigned event;
	                                             assert_dead(tsk2);
	        tsk_startFrom(tsk2, proc2);          assert_ready(tsk2);
	event = flg_wait(flg1, 7, flgAll);           assert_success(event);
	flags = flg_give(flg2, 1);                   assert(flags == 0);
	flags = flg_give(flg2, 4);                   assert(flags == 0);
	flags = flg_give(flg2, 2);                   assert(flags == 0);
	event = tsk_join(tsk2);                      assert_success(event);
	        tsk_stop();
}

static void proc0()
{
	unsigned flags;
	unsigned event;
	                                             assert_dead(tsk1);
	        tsk_startFrom(tsk1, proc1);          assert_ready(tsk1);
	event = flg_wait(&flg0, 7, flgAll);          assert_success(event);
	flags = flg_give(flg1, 1);                   assert(flags == 0);
	flags = flg_give(flg1, 4);                   assert(flags == 0);
	flags = flg_give(flg1, 2);                   assert(flags == 0);
	event = tsk_join(tsk1);                      assert_success(event);
	        tsk_stop();
}

static void test()
{
	unsigned flags;
	unsigned event;
	                                             assert_dead(&tsk0);
	        tsk_startFrom(&tsk0, proc0);         assert_ready(&tsk0);
	flags = flg_give(&flg0, 1);                  assert(flags == 0 || flags == 1);
	flags = flg_give(&flg0, 4);                  assert(flags == 0 || flags == 4 || flags == 5);
	flags = flg_give(&flg0, 2);                  assert(flags == 0 || flags == 2 || flags == 6 || flags == 7);
	event = tsk_join(&tsk0);                     assert_success(event);
}

extern "C"
void test_flag_2()
{
	TEST_Notify();
	TEST_Call();
}
