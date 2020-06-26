#include "test.h"

static auto Tmr0 = Timer(nullptr);
static auto Tmr1 = Timer(nullptr);
static auto Tmr2 = Timer(nullptr);
static auto Tmr3 = Timer(nullptr);

static auto Tmr4 = Timer::Start(4, 0, []{});
static auto Tmr5 = Timer::Start(5, 0, []{});
static auto Tmr6 = Timer::Start(6, 0, []{});
static auto Tmr7 = Timer::Start(7, 0, []{});

static int counter;

static void proc()
{
	CriticalSection cs;
	counter++;
}

static void test()
{
	int result;

	         counter = 0;
	result = Tmr4.wait();                         ASSERT_success(result);
	result = Tmr5.wait();                         ASSERT_success(result);
	result = Tmr6.wait();                         ASSERT_success(result);
	result = Tmr7.wait();                         ASSERT_success(result);
	         Tmr3.startFrom(3, 0, proc);
	         Tmr2.startFrom(2, 0, proc);
	         Tmr1.startFrom(1, 0, proc);
	         Tmr0.startFrom(0, 0, proc);
	result = Tmr0.wait();                         ASSERT_success(result);
	result = Tmr1.wait();                         ASSERT_success(result);
	result = Tmr2.wait();                         ASSERT_success(result);
	result = Tmr3.wait();                         ASSERT_success(result);
	                                              ASSERT(counter == 4);
}

extern "C"
void test_timer_3()
{
	TEST_Notify();
	TEST_Call();
}
