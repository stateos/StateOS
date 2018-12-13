#include "test.h"

auto Lst0 = ListTT<unsigned>();
auto Lst1 = ListTT<unsigned>();
auto Lst2 = ListTT<unsigned>();
auto Lst3 = ListTT<unsigned>();
auto Mem0 = MemoryPoolTT<1, unsigned>();
auto Mem1 = MemoryPoolTT<1, unsigned>();
auto Mem2 = MemoryPoolTT<1, unsigned>();
auto Mem3 = MemoryPoolTT<1, unsigned>();

static unsigned sent;

static void proc3()
{
	unsigned *p;
	unsigned received;
	unsigned event;

 	event = Lst3.wait(&p);                       assert_success(event);
	        received = *p;                       assert(sent == received);
            Mem3.give(p);
 	event = Mem2.wait(&p);                       assert_success(event);
	        *p = received;
	        Lst2.give(p);
	        ThisTask::stop();
}

static void proc2()
{
	unsigned *p;
	unsigned received;
	unsigned event;
		                                         assert(!Tsk3);
	        Tsk3.startFrom(proc3);               assert(!!Tsk3);
 	event = Lst2.wait(&p);                       assert_success(event);
	        received = *p;                       assert(sent == received);
            Mem2.give(p);
 	event = Mem3.wait(&p);                       assert_success(event);
	        *p = received;
	        Lst3.give(p);
 	event = Lst2.wait(&p);                       assert_success(event);
	        received = *p;                       assert(sent == received);
            Mem2.give(p);
 	event = Mem1.wait(&p);                       assert_success(event);
	        *p = received;
	        Lst1.give(p);
	event = Tsk3.join();                         assert_success(event);
	        ThisTask::stop();
}

static void proc1()
{
	unsigned *p;
	unsigned received;
	unsigned event;
		                                         assert(!Tsk2);
	        Tsk2.startFrom(proc2);               assert(!!Tsk2);
 	event = Lst1.wait(&p);                       assert_success(event);
	        received = *p;                       assert(sent == received);
            Mem1.give(p);
 	event = Mem2.wait(&p);                       assert_success(event);
	        *p = received;
	        Lst2.give(p);
 	event = Lst1.wait(&p);                       assert_success(event);
	        received = *p;                       assert(sent == received);
            Mem1.give(p);
 	event = Mem0.wait(&p);                       assert_success(event);
	        *p = received;
	        Lst0.give(p);
	event = Tsk2.join();                         assert_success(event);
	        ThisTask::stop();
}

static void proc0()
{
	unsigned *p;
	unsigned received;
	unsigned event;
		                                         assert(!Tsk1);
	        Tsk1.startFrom(proc1);               assert(!!Tsk1);
 	event = Lst0.wait(&p);                       assert_success(event);
	        received = *p;                       assert(sent == received);
            Mem0.give(p);
 	event = Mem1.wait(&p);                       assert_success(event);
	        *p = received;
	        Lst1.give(p);
 	event = Lst0.wait(&p);                       assert_success(event);
	        received = *p;                       assert(sent == received);
            Mem0.give(p);
	event = Tsk1.join();                         assert_success(event);
	        ThisTask::stop();
}

static void test()
{
	unsigned *p;
	unsigned event;
		                                         assert(!Tsk0);
	        Tsk0.startFrom(proc0);               assert(!!Tsk0);
	        tsk_yield();
	        tsk_yield();
 	event = Mem0.wait(&p);                       assert_success(event);
	        *p = sent = rand();
	        Lst0.give(p);
	event = Tsk0.join();                         assert_success(event);
}

extern "C"
void test_memory_pool_3()
{
	TEST_Notify();
	TEST_Call();
}
