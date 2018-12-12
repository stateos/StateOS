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

 	event = Evq3.wait(&received);                assert_success(event);
 	                                             assert(received == sent);
	event = Evq2.give(received);                 assert_success(event);
	        ThisTask::stop();
}

static void proc2()
{
	unsigned received;
	unsigned event;
		                                         assert(!Tsk3);
	        Tsk3.startFrom(proc3);               assert(!!Tsk3);
 	event = Evq2.wait(&received);                assert_success(event);
 	                                             assert(received == sent);
	event = Evq3.give(received);                 assert_success(event);
 	event = Evq2.wait(&received);                assert_success(event);
 	                                             assert(received == sent);
	event = Evq1.give(received);                 assert_success(event);
	event = Tsk3.join();                         assert_success(event);
	        ThisTask::stop();
}

static void proc1()
{
	unsigned received;
	unsigned event;
		                                         assert(!Tsk2);
	        Tsk2.startFrom(proc2);               assert(!!Tsk2);
 	event = Evq1.wait(&received);                assert_success(event);
 	                                             assert(received == sent);
	event = Evq2.give(received);                 assert_success(event);
 	event = Evq1.wait(&received);                assert_success(event);
 	                                             assert(received == sent);
	event = Evq0.give(received);                 assert_success(event);
	event = Tsk2.join();                         assert_success(event);
	        ThisTask::stop();
}

static void proc0()
{
	unsigned received;
	unsigned event;
		                                         assert(!Tsk1);
	        Tsk1.startFrom(proc1);               assert(!!Tsk1);
 	event = Evq0.wait(&received);                assert_success(event);
 	                                             assert(received == sent);
	event = Evq1.give(received);                 assert_success(event);
 	event = Evq0.wait(&received);                assert_success(event);
 	                                             assert(received == sent);
	event = Tsk1.join();                         assert_success(event);
	        ThisTask::stop();
}

static void test()
{
	unsigned event;
		                                         assert(!Tsk0);
	        Tsk0.startFrom(proc0);               assert(!!Tsk0);
	event = Evq0.give(sent = rand());            assert_success(event);
	event = Tsk0.join();                         assert_success(event);
}

extern "C"
void test_event_queue_3()
{
	TEST_Notify();
	TEST_Call();
}
