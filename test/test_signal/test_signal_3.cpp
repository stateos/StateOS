#include "test.h"

auto Sig0 = Signal();
auto Sig1 = Signal();
auto Sig2 = Signal();
auto Sig3 = Signal();

static unsigned sent;

static void proc3()
{
	unsigned value;
	unsigned event;

	event = Sig3.wait(sigAll, &value);           ASSERT_success(event);
	                                             ASSERT(value == sent);
 	        Sig2.give(value);
	        ThisTask::stop();
}

static void proc2()
{
	unsigned value;
	unsigned event;
		                                         ASSERT(!Tsk3);
	        Tsk3.startFrom(proc3);               ASSERT(!!Tsk3);
	event = Sig2.wait(sigAll, &value);           ASSERT_success(event);
	                                             ASSERT(value == sent);
 	        Sig3.give(value);
	event = Sig2.wait(sigAll, &value);           ASSERT_success(event);
	                                             ASSERT(value == sent);
 	        Sig1.give(value);
	event = Tsk3.join();                         ASSERT_success(event);
	        ThisTask::stop();
}

static void proc1()
{
	unsigned value;
	unsigned event;
		                                         ASSERT(!Tsk2);
	        Tsk2.startFrom(proc2);               ASSERT(!!Tsk2);
	event = Sig1.wait(sigAll, &value);           ASSERT_success(event);
	                                             ASSERT(value == sent);
 	        Sig2.give(value);
	event = Sig1.wait(sigAll, &value);           ASSERT_success(event);
	                                             ASSERT(value == sent);
 	        Sig0.give(value);
	event = Tsk2.join();                         ASSERT_success(event);
	        ThisTask::stop();
}

static void proc0()
{
	unsigned value;
	unsigned event;
		                                         ASSERT(!Tsk1);
	        Tsk1.startFrom(proc1);               ASSERT(!!Tsk1);
	event = Sig0.wait(sigAll, &value);           ASSERT_success(event);
	                                             ASSERT(value == sent);
 	        Sig1.give(value);
	event = Sig0.wait(sigAll, &value);           ASSERT_success(event);
	                                             ASSERT(value == sent);
	event = Tsk1.join();                         ASSERT_success(event);
	        ThisTask::stop();
}

static void test()
{
	unsigned event;
		                                         ASSERT(!Tsk0);
	        Tsk0.startFrom(proc0);               ASSERT(!!Tsk0);
	        ThisTask::yield();
	        ThisTask::yield();
 	        Sig0.give(sent = rand() % SIG_LIMIT);
	event = Tsk0.join();                         ASSERT_success(event);
}

extern "C"
void test_signal_3()
{
	TEST_Notify();
	TEST_Call();
}
