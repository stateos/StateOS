#include "test.h"

using namespace stateos;

auto Box0 = MailBoxQueueTT<1, unsigned>();
auto Box1 = MailBoxQueueTT<1, unsigned>();
auto Box2 = MailBoxQueueTT<1, unsigned>();
auto Box3 = MailBoxQueueTT<1, unsigned>();

static unsigned sent;

static void proc3()
{
	unsigned received;
	int result;

	result = Box3.wait(&received);                ASSERT_success(result);
	                                              ASSERT(received == sent);
	result = Box2.give(&received);                ASSERT_success(result);
	         ThisTask::stop();
}

static void proc2()
{
	unsigned received;
	int result;
	                                              ASSERT(!Tsk3);
	         Tsk3.startFrom(proc3);               ASSERT(!!Tsk3);
	result = Box2.wait(&received);                ASSERT_success(result);
	                                              ASSERT(received == sent);
	result = Box3.give(&received);                ASSERT_success(result);
	result = Box2.wait(&received);                ASSERT_success(result);
	                                              ASSERT(received == sent);
	result = Box1.give(&received);                ASSERT_success(result);
	result = Tsk3.join();                         ASSERT_success(result);
	         ThisTask::stop();
}

static void proc1()
{
	unsigned received;
	int result;
	                                              ASSERT(!Tsk2);
	         Tsk2.startFrom(proc2);               ASSERT(!!Tsk2);
	result = Box1.wait(&received);                ASSERT_success(result);
	                                              ASSERT(received == sent);
	result = Box2.give(&received);                ASSERT_success(result);
	result = Box1.wait(&received);                ASSERT_success(result);
	                                              ASSERT(received == sent);
	result = Box0.give(&received);                ASSERT_success(result);
	result = Tsk2.join();                         ASSERT_success(result);
	         ThisTask::stop();
}

static void proc0()
{
	unsigned received;
	int result;
	                                              ASSERT(!Tsk1);
	         Tsk1.startFrom(proc1);               ASSERT(!!Tsk1);
	result = Box0.wait(&received);                ASSERT_success(result);
	                                              ASSERT(received == sent);
	result = Box1.give(&received);                ASSERT_success(result);
	result = Box0.wait(&received);                ASSERT_success(result);
	                                              ASSERT(received == sent);
	result = Tsk1.join();                         ASSERT_success(result);
	         ThisTask::stop();
}

static void test()
{
	int result;
	                                              ASSERT(!Tsk0);
	         Tsk0.startFrom(proc0);               ASSERT(!!Tsk0);
	         tsk_yield();
	         tsk_yield();
	         sent = (unsigned)rand();
	result = Box0.give(&sent);                    ASSERT_success(result);
	result = Tsk0.join();                         ASSERT_success(result);
}

extern "C"
void test_mailbox_queue_3()
{
	TEST_Notify();
	TEST_Call();
}
