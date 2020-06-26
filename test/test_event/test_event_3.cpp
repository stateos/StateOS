#include "test.h"

static auto Evt0 = Event();
static auto Evt1 = Event();
static auto Evt2 = Event();
static auto Evt3 = Event();

static unsigned sent;

static void proc3()
{
	unsigned received;
	int result;

	result = Evt3.wait(&received);                ASSERT_success(result);
	                                              ASSERT(received == sent);
	         ThisTask::stop();
}

static void proc2()
{
	unsigned received;
	int result;
	                                              ASSERT(!Tsk3);
	         Tsk3.startFrom(proc3);               ASSERT(!!Tsk3);
	result = Evt2.wait(&received);                ASSERT_success(result);
	                                              ASSERT(received == sent);
	         Evt3.give(received);
	result = Tsk3.join();                         ASSERT_success(result);
	         ThisTask::stop();
}

static void proc1()
{
	unsigned received;
	int result;
	                                              ASSERT(!Tsk2);
	         Tsk2.startFrom(proc2);               ASSERT(!!Tsk2);
	result = Evt1.wait(&received);                ASSERT_success(result);
	                                              ASSERT(received == sent);
	         Evt2.give(received);
	result = Tsk2.join();                         ASSERT_success(result);
	         ThisTask::stop();
}

static void proc0()
{
	unsigned received;
	int result;
	                                              ASSERT(!Tsk1);
	         Tsk1.startFrom(proc1);               ASSERT(!!Tsk1);
	result = Evt0.wait(&received);                ASSERT_success(result);
	                                              ASSERT(received == sent);
	         Evt1.give(received);
	result = Tsk1.join();                         ASSERT_success(result);
	         ThisTask::stop();
}

static void test()
{
	int result;
	                                              ASSERT(!Tsk0);
	         Tsk0.startFrom(proc0);               ASSERT(!!Tsk0);
	         ThisTask::yield();
	         ThisTask::yield();
	         Evt0.give(sent = rand());
	result = Tsk0.join();                         ASSERT_success(result);
}

extern "C"
void test_event_3()
{
	TEST_Notify();
	TEST_Call();
}
