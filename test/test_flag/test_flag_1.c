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
		flag  = bitfield & -bitfield;
		bitfield -= flag;
		flg_give(flg, flag, &flags);              ASSERT(flags == check);
	}
}

static void proc3()
{
	unsigned flags;
	int result;

	result = flg_wait(flg3, FLAG3, flgAll+flgProtect+flgIgnore, NULL);
	                                              ASSERT_success(result);
	         flg_give(flg2, FLAG2, &flags);       ASSERT(flags == FLAG2);
	         tsk_stop();
}

static void proc2()
{
	unsigned flags;
	int result;
	                                              ASSERT_dead(tsk3);
	         tsk_startFrom(tsk3, proc3);          ASSERT_ready(tsk3);
	result = flg_wait(flg2, FLAG2, flgAll, NULL); ASSERT_success(result);
	         give(flg3, FLAG3, FLAG3);
	result = flg_wait(flg2, FLAG2, flgAll, NULL); ASSERT_success(result);
	         flg_give(flg1, FLAG1, &flags);       ASSERT(flags == FLAG1);
	result = tsk_join(tsk3);                      ASSERT_success(result);
	         tsk_stop();
}

static void proc1()
{
	unsigned flags;
	int result;
	                                              ASSERT_dead(tsk2);
	         tsk_startFrom(tsk2, proc2);          ASSERT_ready(tsk2);
	result = flg_wait(flg1, FLAG1, flgAll, NULL); ASSERT_success(result);
	         give(flg2, FLAG2, 0);
	result = flg_wait(flg1, FLAG1, flgAll, NULL); ASSERT_success(result);
	         flg_give(&flg0, FLAG0, &flags);      ASSERT(flags == FLAG0);
	result = tsk_join(tsk2);                      ASSERT_success(result);
	         tsk_stop();
}

static void proc0()
{
	int result;
	                                              ASSERT_dead(tsk1);
	         tsk_startFrom(tsk1, proc1);          ASSERT_ready(tsk1);
	result = flg_wait(&flg0, FLAG0, flgAll, NULL);ASSERT_success(result);
	         give(flg1, FLAG1, 0);
	result = flg_wait(&flg0, FLAG0, flgAll, NULL);ASSERT_success(result);
	result = tsk_join(tsk1);                      ASSERT_success(result);
	         tsk_stop();
}

static void test()
{
	int result;
	                                              ASSERT_dead(&tsk0);
	         tsk_startFrom(&tsk0, proc0);         ASSERT_ready(&tsk0);
	         tsk_yield();
	         tsk_yield();
	         give(&flg0, FLAG0, 0);
	result = tsk_join(&tsk0);                     ASSERT_success(result);
}

void test_flag_1()
{
	TEST_Notify();
	TEST_Call();
}
