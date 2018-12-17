#include "test.h"

static unsigned sent = 0;
static unsigned received = 0;

static void action( unsigned signo )
{
	        received = signo;
}

static void proc2()
{
	        tsk_signal(tsk1, sent = rand() % SIG_LIMIT);
	        tsk_stop();
}

static void proc1()
{
	        cur_action(action);
	        cur_suspend();	                     assert(sent == received);
	        cur_action(0);
	        tsk_stop();
}

static void test()
{
	unsigned event;
		                                         assert_dead(tsk1);
	        tsk_startFrom(tsk1, proc1);          assert_ready(tsk1);
		                                         assert_dead(tsk2);
	        tsk_startFrom(tsk2, proc2);          assert_dead(tsk2);
	event = tsk_join(tsk2);                      assert_success(event);
	event = tsk_resume(tsk1);                    assert_success(event);
	event = tsk_join(tsk1);                      assert_success(event);
}

void test_task_signal_1()
{
	TEST_Notify();
	TEST_Call();
}
