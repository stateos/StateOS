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
	        cur_suspend();	                     ASSERT(sent == received);
	        cur_action(nullptr);
	        tsk_stop();
}

static void test()
{
	unsigned event;
		                                         ASSERT_dead(tsk1);
	        tsk_startFrom(tsk1, proc1);          ASSERT_ready(tsk1);
		                                         ASSERT_dead(tsk2);
	        tsk_startFrom(tsk2, proc2);          ASSERT_dead(tsk2);
	event = tsk_join(tsk2);                      ASSERT_success(event);
	event = tsk_resume(tsk1);                    ASSERT_success(event);
	event = tsk_join(tsk1);                      ASSERT_success(event);
}

extern "C"
void test_task_signal_2()
{
	TEST_Notify();
	TEST_Call();
}
