#include "test.h"

static void proc9()
{
	         tsk_stop();
}

static void proc8()
{
	int result;
	tsk_t *tsk9 = tsk_create(9, proc9);

	         tsk_start(tsk9);                     ASSERT_dead(tsk9);
	result = tsk_join(tsk9);                      ASSERT_success(result);
	         tsk_stop();
}

static void proc7()
{
	int result;
	tsk_t *tsk8 = wrk_create(8, proc8, 512, false, true);

	         tsk_start(tsk8);                     ASSERT_dead(tsk8);
	result = tsk_join(tsk8);                      ASSERT_success(result);
	         tsk_stop();
}

static void proc6()
{
	int result;
	tsk_t *tsk7 = wrk_create(7, proc7, 256, false, true);

	         tsk_start(tsk7);                     ASSERT_dead(tsk7);
	result = tsk_join(tsk7);                      ASSERT_success(result);
	         tsk_stop();
}

static void test()
{
	int result;
	tsk_t *tsk6 = tsk_create(6, proc6);

	         tsk_start(tsk6);                     ASSERT_dead(tsk6);
	result = tsk_join(tsk6);                      ASSERT_success(result);
}

void test_task_create_3()
{
	TEST_Notify();
	TEST_Call();
}
