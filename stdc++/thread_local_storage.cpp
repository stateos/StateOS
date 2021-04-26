#include "stm32f4_discovery.h"
#include <thread>

thread_local int i = 2;

void work(int val)
{
	i = ++val;
	i += 10;
}

void test()
{
	i = 11;
	std::thread(work, 23).join();
	std::thread(work, 17).join();
	std::thread(work, 19).join();
	if (i != 11) abort();
}

int main()
{
	using namespace std::chrono_literals;
	device::Led led;
	for (;;)
	{
		i = 0;
		test();
		std::this_thread::sleep_for(100ms);
		led.tick();
	}
}
