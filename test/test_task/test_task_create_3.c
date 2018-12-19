#include "test.h"

static void proc9()
{
	        tsk_stop();
}

static void proc8()
{
	unsigned event;
	tsk_t *tsk9 = tsk_create(9, proc9);

	        tsk_start(tsk9);                     assert_dead(tsk9);
	event = tsk_join(tsk9);                      assert_success(event);
	        tsk_stop();
}

static void proc7()
{
	unsigned event;
	tsk_t *tsk8 = wrk_create(8, proc8, 512);

	        tsk_start(tsk8);                     assert_dead(tsk8);
	event = tsk_join(tsk8);                      assert_success(event);
	        tsk_stop();
}

static void proc6()
{
	unsigned event;
	tsk_t *tsk7 = wrk_create(7, proc7, 256);

	        tsk_start(tsk7);                     assert_dead(tsk7);
	event = tsk_join(tsk7);                      assert_success(event);
	        tsk_stop();
}

static void test()
{
	unsigned event;
	tsk_t *tsk6 = tsk_create(6, proc6);

	        tsk_start(tsk6);                     assert_dead(tsk6);
	event = tsk_join(tsk6);                      assert_success(event);
}

void test_task_create_3()
{
	TEST_Notify();
	TEST_Call();
}
