#include "test.h"

static_ONE(one3);

static int counter = 0;

static void proc()
{
	sys_lock();
	{
		counter++;
	}
	sys_unlock();
}

static void proc3()
{
	         one_call(one3, proc);
	         tsk_stop();
}

static void proc2()
{
	int result;
	                                              ASSERT_dead(tsk3);
	         tsk_startFrom(tsk3, proc3);          ASSERT_dead(tsk3);
	         one_call(one2, proc);
	result = tsk_join(tsk3);                      ASSERT_success(result);
	         tsk_stop();
}

static void proc1()
{
	int result;
	                                              ASSERT_dead(tsk2);
	         tsk_startFrom(tsk2, proc2);          ASSERT_dead(tsk2);
	         one_call(one1, proc);
	result = tsk_join(tsk2);                      ASSERT_success(result);
	         tsk_stop();
}

static void proc0()
{
	int result;
	                                              ASSERT_dead(tsk1);
	         tsk_startFrom(tsk1, proc1);          ASSERT_dead(tsk1);
	         one_call(&one0, proc);               ASSERT(counter == 1 || counter == 4);
	result = tsk_join(tsk1);                      ASSERT_success(result);
	         tsk_stop();
}

static void test()
{
	int result;
	                                              ASSERT_dead(&tsk0);
	         tsk_startFrom(&tsk0, proc0);
	result = tsk_join(&tsk0);                     ASSERT_success(result);
}

extern "C"
void test_once_flag_2()
{
	TEST_Notify();
	TEST_Call();
}
