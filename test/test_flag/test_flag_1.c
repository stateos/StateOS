#include "test.h"

#define FLAG0 14U
#define FLAG1 13U
#define FLAG2 11U
#define FLAG3  7U

static_FLG(flg3, FLAG3);

static void give(flg_t *flg, unsigned bitfield, unsigned check)
{
	unsigned flag;
	unsigned flags;

	while (bitfield)
	{
		flag = bitfield & (~bitfield + 1);
		bitfield -= flag;
		flags = flg_give(flg, flag);             ASSERT(flags == check);
	}
}

static void proc3()
{
	unsigned flags;
	unsigned event;

	event = flg_wait(flg3, FLAG3, flgAll+flgProtect+flgIgnore); ASSERT_success(event);
	flags = flg_give(flg2, FLAG2);               ASSERT(flags == FLAG2);
	        tsk_stop();
}

static void proc2()
{
	unsigned flags;
	unsigned event;
	                                             ASSERT_dead(tsk3);
	        tsk_startFrom(tsk3, proc3);          ASSERT_ready(tsk3);
	event = flg_wait(flg2, FLAG2, flgAll);       ASSERT_success(event);
	        give(flg3, FLAG3, FLAG3);
	event = flg_wait(flg2, FLAG2, flgAll);       ASSERT_success(event);
	flags = flg_give(flg1, FLAG1);               ASSERT(flags == FLAG1);
	event = tsk_join(tsk3);                      ASSERT_success(event);
	        tsk_stop();
}

static void proc1()
{
	unsigned flags;
	unsigned event;
	                                             ASSERT_dead(tsk2);
	        tsk_startFrom(tsk2, proc2);          ASSERT_ready(tsk2);
	event = flg_wait(flg1, FLAG1, flgAll);       ASSERT_success(event);
	        give(flg2, FLAG2, 0);
	event = flg_wait(flg1, FLAG1, flgAll);       ASSERT_success(event);
	flags = flg_give(&flg0, FLAG0);              ASSERT(flags == FLAG0);
	event = tsk_join(tsk2);                      ASSERT_success(event);
	        tsk_stop();
}

static void proc0()
{
	unsigned event;
	                                             ASSERT_dead(tsk1);
	        tsk_startFrom(tsk1, proc1);          ASSERT_ready(tsk1);
	event = flg_wait(&flg0, FLAG0, flgAll);      ASSERT_success(event);
	        give(flg1, FLAG1, 0);
	event = flg_wait(&flg0, FLAG0, flgAll);      ASSERT_success(event);
	event = tsk_join(tsk1);                      ASSERT_success(event);
	        tsk_stop();
}

static void test()
{
	unsigned event;
	                                             ASSERT_dead(&tsk0);
	        tsk_startFrom(&tsk0, proc0);         ASSERT_ready(&tsk0);
	        tsk_yield();
	        tsk_yield();
	        give(&flg0, FLAG0, 0);
	event = tsk_join(&tsk0);                     ASSERT_success(event);
}

void test_flag_1()
{
	TEST_Notify();
	TEST_Call();
}
