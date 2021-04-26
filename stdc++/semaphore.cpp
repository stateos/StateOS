#include "stm32f4_discovery.h"
#include <thread>
#include <chrono>

void test()
{
	std::binary_semaphore sem;
	std::thread([&] {
		std::this_thread::sleep_for(std::chrono::milliseconds{100});
		sem.release();
	}).detach();
	sem.acquire();
}

int main()
{
	device::Led led;
	for (;;)
	{
		test();
		led.tick();
	}
}
