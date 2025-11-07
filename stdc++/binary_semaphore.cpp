#include "stm32f4_discovery.h"
#include <thread>
#include <chrono>
#include <semaphore>

void consumer(std::binary_semaphore &sem)
{
	auto led = device::Led();

	for (;;)
	{
		sem.acquire();
		led.tick();
	}
}

void producer()
{
	auto sem = std::binary_semaphore(0);
	std::thread(consumer, std::ref(sem)).detach();

	for (;;)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds{1000});
		sem.release();
	}
}

int main()
{
	std::thread(producer).join();
}
