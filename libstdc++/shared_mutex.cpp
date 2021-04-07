#include "stm32f4_discovery.h"
#include <shared_mutex>
#include <thread>
#include <chrono>

std::shared_mutex mtx;

void consumer()
{
	device::Led led;
	for (;;)
	{
		std::this_thread::yield();
		std::shared_lock<std::shared_mutex> lock(mtx);
		led.tick();
	}
}

void producer()
{
	for (;;)
	{
		std::this_thread::yield();
		std::unique_lock<std::shared_mutex> lock(mtx);
		std::this_thread::sleep_for(std::chrono::seconds{1});
	}
}

int main()
{
	auto cons = std::jthread(consumer);
	auto prod = std::jthread(producer);
}
