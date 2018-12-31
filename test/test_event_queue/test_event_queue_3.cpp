#include "test.h"

static auto Evq0 = EventQueueT<1>();
static auto Evq1 = EventQueueT<1>();
static auto Evq2 = EventQueueT<1>();
static auto Evq3 = EventQueueT<1>();

static unsigned sent;

static void proc3()
{
	unsigned received;
	unsigned event;

 	event = Evq3.wait(&received);                ASSERT_success(event);
 	                                             ASSERT(received == sent);
	event = Evq2.give(received);                 ASSERT_success(event);
	        ThisTask::stop();
}

static void proc2()
{
	unsigned received;
	unsigned event;
		                                         ASSERT(!Tsk3);
	        Tsk3.startFrom(proc3);               ASSERT(!!Tsk3);
 	event = Evq2.wait(&received);                ASSERT_success(event);
 	                                             ASSERT(received == sent);
	event = Evq3.give(received);                 ASSERT_success(event);
 	event = Evq2.wait(&received);                ASSERT_success(event);
 	                                             ASSERT(received == sent);
	event = Evq1.give(received);                 ASSERT_success(event);
	event = Tsk3.join();                         ASSERT_success(event);
	        ThisTask::stop();
}

static void proc1()
{
	unsigned received;
	unsigned event;
		                                         ASSERT(!Tsk2);
	        Tsk2.startFrom(proc2);               ASSERT(!!Tsk2);
 	event = Evq1.wait(&received);                ASSERT_success(event);
 	                                             ASSERT(received == sent);
	event = Evq2.give(received);                 ASSERT_success(event);
 	event = Evq1.wait(&received);                ASSERT_success(event);
 	                                             ASSERT(received == sent);
	event = Evq0.give(received);                 ASSERT_success(event);
	event = Tsk2.join();                         ASSERT_success(event);
	        ThisTask::stop();
}

static void proc0()
{
	unsigned received;
	unsigned event;
		                                         ASSERT(!Tsk1);
	        Tsk1.startFrom(proc1);               ASSERT(!!Tsk1);
 	event = Evq0.wait(&received);                ASSERT_success(event);
 	                                             ASSERT(received == sent);
	event = Evq1.give(received);                 ASSERT_success(event);
 	event = Evq0.wait(&received);                ASSERT_success(event);
 	                                             ASSERT(received == sent);
	event = Tsk1.join();                         ASSERT_success(event);
	        ThisTask::stop();
}

static void test()
{
	unsigned event;
		                                         ASSERT(!Tsk0);
	        Tsk0.startFrom(proc0);               ASSERT(!!Tsk0);
	event = Evq0.give(sent = rand());            ASSERT_success(event);
	event = Tsk0.join();                         ASSERT_success(event);
}

extern "C"
void test_event_queue_3()
{
	TEST_Notify();
	TEST_Call();
}
