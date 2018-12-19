#include "test.h"

static auto Tmr0 = Timer(nullptr);
static auto Tmr1 = Timer(nullptr);
static auto Tmr2 = Timer(nullptr);
static auto Tmr3 = Timer(nullptr);

static auto Tmr4 = startTimer(4, 0, []{});
static auto Tmr5 = startTimer(5, 0, []{});
static auto Tmr6 = startTimer(6, 0, []{});
static auto Tmr7 = startTimer(7, 0, []{});

static int counter;

static void proc()
{
	CriticalSection cs;

	        counter++;
}

static void test()
{
	unsigned event;

	        counter = 0;
	event = Tmr4.wait();                         assert_success(event);
	event = Tmr5.wait();                         assert_success(event);
	event = Tmr6.wait();                         assert_success(event);
	event = Tmr7.wait();                         assert_success(event);
	        Tmr3.startFrom(3, 0, proc);
	        Tmr2.startFrom(2, 0, proc);
	        Tmr1.startFrom(1, 0, proc);
	        Tmr0.startFrom(0, 0, proc);
	event = Tmr0.wait();                         assert_success(event);
	event = Tmr1.wait();                         assert_success(event);
	event = Tmr2.wait();                         assert_success(event);
	event = Tmr3.wait();                         assert_success(event);
	                                             assert(counter == 4);
}

extern "C"
void test_timer_3()
{
	TEST_Notify();
	TEST_Call();
}
