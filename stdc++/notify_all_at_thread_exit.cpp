#include "stm32f4_discovery.h"
#include <thread>
#include <mutex>
#include <condition_variable>
#include <future>
#include <chrono>
 
std::mutex m;
std::condition_variable cv;
 
bool ready = true;
int result = 123;
 
void work()
{
	thread_local int thread_local_data = 123;
	std::unique_lock<std::mutex> lk(m);
	result = thread_local_data;
	ready = true;
	std::notify_all_at_thread_exit(cv, std::move(lk));
}
 
void test()
{
	std::thread(work).detach();
	std::unique_lock<std::mutex> lk(m);
	cv.wait(lk, []{ return ready; });
	if (result != 123) abort();
}

int main()
{
	using namespace std::chrono_literals;
	device::Led led;
	for (;;)
	{
		ready = false;
		result = 0;
		test();
		std::this_thread::sleep_for(100ms);
		led.tick();
	}
}
