#include "test.h"

static unsigned sent = 0;
static unsigned received = 0;

static void action( unsigned signo )
{
	        received = signo;
}

static void proc2()
{
	        Tsk1.signal(sent = rand() % SIG_LIMIT);
	        ThisTask::stop();
}

static void proc1()
{
	        ThisTask::action(action);
	        ThisTask::suspend();                 assert(sent == received);
	        ThisTask::action(0);
	        ThisTask::stop();
}

static void test()
{
	unsigned event;
		                                         assert(!Tsk1);
	        Tsk1.startFrom(proc1);               assert(!!Tsk1);
		                                         assert(!Tsk2);
	        Tsk2.startFrom(proc2);               assert(!Tsk2);
	event = Tsk2.join();                         assert_success(event);
	event = Tsk1.resume();                       assert_success(event);
	event = Tsk1.join();                         assert_success(event);
}

extern "C"
void test_task_signal_3()
{
	TEST_Notify();
	TEST_Call();
}
