#include "test.h"

using namespace stateos;

auto Sig0 = Signal();
auto Sig1 = Signal();
auto Sig2 = Signal();
auto Sig3 = Signal();

static unsigned sent;

static void proc3()
{
	unsigned value;
	int result;

	result = Sig3.wait(sigAll, &value);           ASSERT_success(result);
	                                              ASSERT(value == sent);
	         Sig2.give(value);
	         thisTask::stop();
}

static void proc2()
{
	unsigned value;
	int result;
	                                              ASSERT(!Tsk3);
	         Tsk3.startFrom(proc3);               ASSERT(!!Tsk3);
	result = Sig2.wait(sigAll, &value);           ASSERT_success(result);
	                                              ASSERT(value == sent);
	         Sig3.give(value);
	result = Sig2.wait(sigAll, &value);           ASSERT_success(result);
	                                              ASSERT(value == sent);
	         Sig1.give(value);
	result = Tsk3.join();                         ASSERT_success(result);
	         thisTask::stop();
}

static void proc1()
{
	unsigned value;
	int result;
	                                              ASSERT(!Tsk2);
	         Tsk2.startFrom(proc2);               ASSERT(!!Tsk2);
	result = Sig1.wait(sigAll, &value);           ASSERT_success(result);
	                                              ASSERT(value == sent);
	         Sig2.give(value);
	result = Sig1.wait(sigAll, &value);           ASSERT_success(result);
	                                              ASSERT(value == sent);
	         Sig0.give(value);
	result = Tsk2.join();                         ASSERT_success(result);
	         thisTask::stop();
}

static void proc0()
{
	unsigned value;
	int result;
	                                              ASSERT(!Tsk1);
	         Tsk1.startFrom(proc1);               ASSERT(!!Tsk1);
	result = Sig0.wait(sigAll, &value);           ASSERT_success(result);
	                                              ASSERT(value == sent);
	         Sig1.give(value);
	result = Sig0.wait(sigAll, &value);           ASSERT_success(result);
	                                              ASSERT(value == sent);
	result = Tsk1.join();                         ASSERT_success(result);
	         thisTask::stop();
}

static void test()
{
	int result;
	                                              ASSERT(!Tsk0);
	         Tsk0.startFrom(proc0);               ASSERT(!!Tsk0);
	         thisTask::yield();
	         thisTask::yield();
	         sent = (unsigned)rand() % SIGLIM;
	         Sig0.give(sent);
	result = Tsk0.join();                         ASSERT_success(result);
}

extern "C"
void test_signal_3()
{
	TEST_Notify();
	TEST_Call();
}
