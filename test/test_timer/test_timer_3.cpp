#include "test.h"

static auto Tmr0 = Timer(nullptr);
static auto Tmr1 = Timer(nullptr);
static auto Tmr2 = Timer(nullptr);
static auto Tmr3 = Timer(nullptr);

static auto Tmr4 = Timer::Start(4U, 0U, []{});
static auto Tmr5 = Timer::Start(5U, 0U, []{});
static auto Tmr6 = Timer::Start(6U, 0U, []{});
static auto Tmr7 = Timer::Start(7U, 0U, []{});

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
	         Tmr3.startFrom(3U, 0U, proc);
	         Tmr2.startFrom(2U, 0U, proc);
	         Tmr1.startFrom(1U, 0U, proc);
	         Tmr0.startFrom(0U, 0U, proc);
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
