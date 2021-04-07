#include <stm32f4_discovery.h>
#include <condition_variable>
#include <thread>
#include <chrono>

auto led = device::Led();
auto mtx = std::mutex();
auto cnd = std::condition_variable();

void consumer()
{
	std::unique_lock<std::mutex> lock(mtx);
	for (;;)
	{
		cnd.wait(lock);
		led.tick();
	}
}

void producer()
{
	for (;;)
	{
		std::unique_lock<std::mutex> lock(mtx);
		std::this_thread::sleep_for(std::chrono::seconds{1});
		cnd.notify_all();
		std::this_thread::yield();
	}
}

auto cons = std::thread(consumer);
auto prod = std::thread(producer);

int main()
{
	cons.join();
	prod.join();
}
