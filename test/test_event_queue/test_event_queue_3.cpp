#include "test.h"

using namespace stateos;

static auto Evq0 = EventQueueT<1>();
static auto Evq1 = EventQueueT<1>();
static auto Evq2 = EventQueueT<1>();
static auto Evq3 = EventQueueT<1>();

static unsigned sent;

static void proc3()
{
	unsigned received;
	int result;

	result = Evq3.wait(&received);                ASSERT_success(result);
	                                              ASSERT(received == sent);
	result = Evq2.give(received);                 ASSERT_success(result);
	         ThisTask::stop();
}

static void proc2()
{
	unsigned received;
	int result;
	                                              ASSERT(!Tsk3);
	         Tsk3.startFrom(proc3);               ASSERT(!!Tsk3);
	result = Evq2.wait(&received);                ASSERT_success(result);
	                                              ASSERT(received == sent);
	result = Evq3.give(received);                 ASSERT_success(result);
	result = Evq2.wait(&received);                ASSERT_success(result);
	                                              ASSERT(received == sent);
	result = Evq1.give(received);                 ASSERT_success(result);
	result = Tsk3.join();                         ASSERT_success(result);
	         ThisTask::stop();
}

static void proc1()
{
	unsigned received;
	int result;
	                                              ASSERT(!Tsk2);
	         Tsk2.startFrom(proc2);               ASSERT(!!Tsk2);
	result = Evq1.wait(&received);                ASSERT_success(result);
	                                              ASSERT(received == sent);
	result = Evq2.give(received);                 ASSERT_success(result);
	result = Evq1.wait(&received);                ASSERT_success(result);
	                                              ASSERT(received == sent);
	result = Evq0.give(received);                 ASSERT_success(result);
	result = Tsk2.join();                         ASSERT_success(result);
	         ThisTask::stop();
}

static void proc0()
{
	unsigned received;
	int result;
	                                              ASSERT(!Tsk1);
	         Tsk1.startFrom(proc1);               ASSERT(!!Tsk1);
	result = Evq0.wait(&received);                ASSERT_success(result);
	                                              ASSERT(received == sent);
	result = Evq1.give(received);                 ASSERT_success(result);
	result = Evq0.wait(&received);                ASSERT_success(result);
	                                              ASSERT(received == sent);
	result = Tsk1.join();                         ASSERT_success(result);
	         ThisTask::stop();
}

static void test()
{
	int result;
	                                              ASSERT(!Tsk0);
	         Tsk0.startFrom(proc0);               ASSERT(!!Tsk0);
	         sent = (unsigned)rand();
	result = Evq0.give(sent);                     ASSERT_success(result);
	result = Tsk0.join();                         ASSERT_success(result);
}

extern "C"
void test_event_queue_3()
{
	TEST_Notify();
	TEST_Call();
}
