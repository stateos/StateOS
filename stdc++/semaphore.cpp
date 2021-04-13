#include "stm32f4_discovery.h"
#include <thread>
#include <chrono>

auto led  = device::Led();
auto sem  = std::binary_semaphore();
auto cons = std::thread([]
{
	for (;;)
	{
		sem.acquire();
		led.tick();
	}
});
auto prod = std::thread([]
{
	for (;;)
	{
		std::this_thread::sleep_for(std::chrono::seconds{1});
		sem.release();
	}
});

int main()
{
	cons.join();
	prod.join();
}
