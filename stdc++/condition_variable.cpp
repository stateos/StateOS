// example from cppreference.com
// modified by Rajmund Szymanski

#include "stm32f4_discovery.h"
#include <thread>
#include <mutex>
#include <condition_variable>
#include <chrono>
 
std::mutex m;
std::condition_variable cv;
bool ready{ true };
bool processed{ true };
 
void work()
{
	std::unique_lock<std::mutex> lk(m);
	cv.wait(lk, []{ return ready; });
	processed = true;
	lk.unlock();
	cv.notify_one();
}
 
void test()
{
	std::thread t(work);
	{
		std::lock_guard<std::mutex> lk(m);
		ready = true;
	}
	cv.notify_one();
	{
		std::unique_lock<std::mutex> lk(m);
		cv.wait(lk, []{ return processed; });
	}
	t.join();
}

int main()
{
	using namespace std::chrono_literals;
	device::Led led;
	for (;;)
	{
		ready = false;
		processed = false;
		test();
		std::this_thread::sleep_for(100ms);
		led.tick();
	}
}
