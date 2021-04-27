#include "test.h"

using namespace stateos;

static unsigned sent = 0;
static unsigned received = 0;

static void action( unsigned signo )
{
	         received = signo;
}

static void proc2()
{
	         sent = (unsigned)rand() % SIGLIM;
	         Tsk1.signal(sent);
	         thisTask::stop();
}

static void proc1()
{
	         thisTask::action(action);
	         thisTask::suspend();                 ASSERT(sent == received);
	         thisTask::action(nullptr);
	         thisTask::stop();
}

static void test()
{
	int result;
	                                              ASSERT(!Tsk1);
	         Tsk1.startFrom(proc1);               ASSERT(!!Tsk1);
	                                              ASSERT(!Tsk2);
	         Tsk2.startFrom(proc2);               ASSERT(!Tsk2);
	result = Tsk2.join();                         ASSERT_success(result);
	result = Tsk1.resume();                       ASSERT_success(result);
	result = Tsk1.join();                         ASSERT_success(result);
}

extern "C"
void test_task_signal_3()
{
	TEST_Notify();
	TEST_Call();
}
