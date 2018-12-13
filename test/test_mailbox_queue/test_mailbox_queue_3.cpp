#include "test.h"

auto Box0 = MailBoxQueueTT<1, unsigned>();
auto Box1 = MailBoxQueueTT<1, unsigned>();
auto Box2 = MailBoxQueueTT<1, unsigned>();
auto Box3 = MailBoxQueueTT<1, unsigned>();

static unsigned sent;

static void proc3()
{
	unsigned received;
	unsigned event;

 	event = Box3.wait(&received);                assert_success(event);
 	                                             assert(received == sent);
	event = Box2.give(&received);                assert_success(event);
	        ThisTask::stop();
}

static void proc2()
{
	unsigned received;
	unsigned event;
		                                         assert(!Tsk3);
	        Tsk3.startFrom(proc3);               assert(!!Tsk3);
 	event = Box2.wait(&received);                assert_success(event);
 	                                             assert(received == sent);
	event = Box3.give(&received);                assert_success(event);
 	event = Box2.wait(&received);                assert_success(event);
 	                                             assert(received == sent);
	event = Box1.give(&received);                assert_success(event);
	event = Tsk3.join();                         assert_success(event);
	        ThisTask::stop();
}

static void proc1()
{
	unsigned received;
	unsigned event;
		                                         assert(!Tsk2);
	        Tsk2.startFrom(proc2);               assert(!!Tsk2);
 	event = Box1.wait(&received);                assert_success(event);
 	                                             assert(received == sent);
	event = Box2.give(&received);                assert_success(event);
 	event = Box1.wait(&received);                assert_success(event);
 	                                             assert(received == sent);
	event = Box0.give(&received);                assert_success(event);
	event = Tsk2.join();                         assert_success(event);
	        ThisTask::stop();
}

static void proc0()
{
	unsigned received;
	unsigned event;
		                                         assert(!Tsk1);
	        Tsk1.startFrom(proc1);               assert(!!Tsk1);
 	event = Box0.wait(&received);                assert_success(event);
 	                                             assert(received == sent);
	event = Box1.give(&received);                assert_success(event);
 	event = Box0.wait(&received);                assert_success(event);
 	                                             assert(received == sent);
	event = Tsk1.join();                         assert_success(event);
	        ThisTask::stop();
}

static void test()
{
	unsigned event;
		                                         assert(!Tsk0);
	        Tsk0.startFrom(proc0);               assert(!!Tsk0);
	        tsk_yield();
	        tsk_yield();
	        sent = rand();
	event = Box0.give(&sent);                    assert_success(event);
	event = Tsk0.join();                         assert_success(event);
}

extern "C"
void test_mailbox_queue_3()
{
	TEST_Notify();
	TEST_Call();
}
