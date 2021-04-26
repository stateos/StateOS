// example from cppreference.com
// modified by Rajmund Szymanski

#include "stm32f4_discovery.h"
#include <thread>
#include <future>
#include <vector>
#include <numeric>
#include <chrono>
 
void work1(std::vector<int>& v, std::promise<int> p)
{
	auto sum = std::accumulate(v.begin(), v.end(), 0);
	p.set_value(sum);
}
 
void work2(std::promise<void> barrier)
{
	using namespace std::chrono_literals;
	std::this_thread::sleep_for(100ms);
	barrier.set_value();
}
 
void test()
{
	std::vector<int> numbers = { 1, 2, 3 };
	std::promise<int> accumulate_promise;
	std::future<int> accumulate_future = accumulate_promise.get_future();
	std::thread t1(work1, std::ref(numbers), std::move(accumulate_promise));
	if (accumulate_future.get() != 6) abort();
	t1.join();
 
	std::promise<void> barrier;
	std::future<void> barrier_future = barrier.get_future();
	std::thread t2(work2, std::move(barrier));
	barrier_future.wait();
	t2.join();
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
