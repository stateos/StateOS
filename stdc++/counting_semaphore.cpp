#include "stm32f4_discovery.h"
#include <thread>
#include <chrono>
#include <semaphore>

void test()
{
	std::counting_semaphore sem(0);
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
