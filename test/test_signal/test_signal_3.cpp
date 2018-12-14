#include "test.h"

auto Sig0 = Signal();
auto Sig1 = Signal();
auto Sig2 = Signal();
auto Sig3 = Signal();

static unsigned sent;

static void proc3()
{
	unsigned event;

	event = Sig3.wait(sigAll);                   assert(event == sent);
 	        Sig2.give(event);
	        ThisTask::stop();
}

static void proc2()
{
	unsigned event;
		                                         assert(!Tsk3);
	        Tsk3.startFrom(proc3);               assert(!!Tsk3);
	event = Sig2.wait(sigAll);                   assert(event == sent);
 	        Sig3.give(event);
	event = Sig2.wait(sigAll);                   assert(event == sent);
 	        Sig1.give(event);
	event = Tsk3.join();                         assert_success(event);
	        ThisTask::stop();
}

static void proc1()
{
	unsigned event;
		                                         assert(!Tsk2);
	        Tsk2.startFrom(proc2);               assert(!!Tsk2);
	event = Sig1.wait(sigAll);                   assert(event == sent);
 	        Sig2.give(event);
	event = Sig1.wait(sigAll);                   assert(event == sent);
 	        Sig0.give(event);
	event = Tsk2.join();                         assert_success(event);
	        ThisTask::stop();
}

static void proc0()
{
	unsigned event;
		                                         assert(!Tsk1);
	        Tsk1.startFrom(proc1);               assert(!!Tsk1);
	event = Sig0.wait(sigAll);                   assert(event == sent);
 	        Sig1.give(event);
	event = Sig0.wait(sigAll);                   assert(event == sent);
	event = Tsk1.join();                         assert_success(event);
	        ThisTask::stop();
}

static void test()
{
	unsigned event;
		                                         assert(!Tsk0);
	        Tsk0.startFrom(proc0);               assert(!!Tsk0);
	        ThisTask::yield();
	        ThisTask::yield();
 	        Sig0.give(sent = rand() % SIG_LIMIT);
	event = Tsk0.join();                         assert_success(event);
}

extern "C"
void test_signal_3()
{
	TEST_Notify();
	TEST_Call();
}
