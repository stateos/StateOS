#include "test.h"

static auto Evt1 = Event();
static auto Evt2 = Event();
static auto Evt3 = Event();

static unsigned sent;

static void proc3()
{
	unsigned received;
	unsigned event;

 	event = Evt3.wait(&received);                assert_success(event);
 	                                             assert(received == sent);
	        ThisTask::stop();
}

static void proc2()
{
	unsigned received;
	unsigned event;
		                                         assert(!Tsk3);
	        Tsk3.startFrom(proc3);               assert(!!Tsk3);
 	event = Evt2.wait(&received);                assert_success(event);
 	                                             assert(received == sent);
	        Evt3.give(received);
	event = Tsk3.join();                         assert_success(event);
	        ThisTask::stop();
}

static void proc1()
{
	unsigned received;
	unsigned event;
		                                         assert(!Tsk2);
	        Tsk2.startFrom(proc2);               assert(!!Tsk2);
 	event = Evt1.wait(&received);                assert_success(event);
 	                                             assert(received == sent);
	        Evt2.give(received);
	event = Tsk2.join();                         assert_success(event);
	        ThisTask::stop();
}

static void proc0()
{
	unsigned event;
		                                         assert(!Tsk1);
	        Tsk1.startFrom(proc1);               assert(!!Tsk1);
	        sent = rand();
	        Evt1.give(sent);
	event = Tsk1.join();                         assert_success(event);
	        ThisTask::stop();
}

static void test()
{
	unsigned event;
		                                         assert(!Tsk0);
	        Tsk0.startFrom(proc0);
	event = Tsk0.join();                         assert_success(event);
}

extern "C"
void test_event_3()
{
	TEST_Notify();
	TEST_Call();
}
