#include <stm32f4_discovery.h>
#include <thread>
#include <cassert>

device::Led led;

thread_local int i = 0;

void f(int newval)
{
	i = newval;
}

void g()
{
	++i;
}

void threadfunc(int id)
{
	f(id);
	g();
}

int main()
{
	i = 9;
	std::thread t1(threadfunc, 1);
	std::thread t2(threadfunc, 2);
	std::thread t3(threadfunc, 3);
	t1.join();
	t2.join();
	t3.join();
	assert(i == 9);
	led = 15;
}
