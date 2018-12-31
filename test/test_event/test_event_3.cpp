#include "test.h"

static auto Evt0 = Event();
static auto Evt1 = Event();
static auto Evt2 = Event();
static auto Evt3 = Event();

static unsigned sent;

static void proc3()
{
	unsigned received;
	unsigned event;

 	event = Evt3.wait(&received);                ASSERT_success(event);
 	                                             ASSERT(received == sent);
	        ThisTask::stop();
}

static void proc2()
{
	unsigned received;
	unsigned event;
		                                         ASSERT(!Tsk3);
	        Tsk3.startFrom(proc3);               ASSERT(!!Tsk3);
 	event = Evt2.wait(&received);                ASSERT_success(event);
 	                                             ASSERT(received == sent);
	        Evt3.give(received);
	event = Tsk3.join();                         ASSERT_success(event);
	        ThisTask::stop();
}

static void proc1()
{
	unsigned received;
	unsigned event;
		                                         ASSERT(!Tsk2);
	        Tsk2.startFrom(proc2);               ASSERT(!!Tsk2);
 	event = Evt1.wait(&received);                ASSERT_success(event);
 	                                             ASSERT(received == sent);
	        Evt2.give(received);
	event = Tsk2.join();                         ASSERT_success(event);
	        ThisTask::stop();
}

static void proc0()
{
	unsigned received;
	unsigned event;
		                                         ASSERT(!Tsk1);
	        Tsk1.startFrom(proc1);               ASSERT(!!Tsk1);
 	event = Evt0.wait(&received);                ASSERT_success(event);
 	                                             ASSERT(received == sent);
	        Evt1.give(received);
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
	        Evt0.give(sent = rand());
	event = Tsk0.join();                         ASSERT_success(event);
}

extern "C"
void test_event_3()
{
	TEST_Notify();
	TEST_Call();
}
