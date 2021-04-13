#include <stm32f4_discovery.h>
#include <thread>
#include <future>
#include <chrono>

int main()
{
	using namespace std::chrono_literals;

	device::Led led;

	std::promise<int> p;
	std::future<int> f = p.get_future();
	std::thread([&p]
	{
		std::this_thread::sleep_for(10ms);
		p.set_value_at_thread_exit(10);
	}).detach();
 
	f.wait();
	if (f.get() == 10)
		led = 15;
}
