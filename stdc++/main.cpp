// example from cppreference.com
// modified by Rajmund Szymanski

#include "stm32f4_discovery.h"
#include <thread>
#include <shared_mutex>
#include <chrono>

class ThreadSafeCounter
{
	public:
	ThreadSafeCounter() = default;
 
	unsigned int get() const
	{
		std::shared_lock lock(mutex_);
		return value_;
	}
 
	void increment()
	{
		std::unique_lock lock(mutex_);
		value_++;
	}
 
	void reset()
	{
		std::unique_lock lock(mutex_);
		value_ = 0;
	}
 
	private:
	mutable std::shared_mutex mutex_;
	unsigned int value_ = 0;
};
 
void test()
{
	using namespace std::chrono_literals;
	ThreadSafeCounter counter;
	auto increment = [&counter]()
	{
		for (int i = 0; i < 5; i++)
		{
			std::this_thread::sleep_for(1ms);
			counter.increment();
		}
	};
	std::thread t1(increment);
	std::thread t2(increment);
	t1.join();
	t2.join();
	if (counter.get() != 10) abort();
}

int main()
{
	using namespace std::chrono_literals;
	device::Led led;
	for (;;)
	{
		test();
		std::this_thread::sleep_for(100ms);
		led.tick();
	}
}
