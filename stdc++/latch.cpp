// example from cppreference.com
// modified by Rajmund Szymanski

#include "stm32f4_discovery.h"
#include <thread>
#include <chrono>

void test()
{
	struct job
	{
		int num;
		std::thread action{};
	} jobs[] = {{1}, {2}, {3}, {4}};

	std::latch work_done{std::size(jobs)};
	std::latch start_clean_up{1};

	auto work = [&](job& my_job)
	{
		my_job.num = 5;
		work_done.count_down();
		start_clean_up.wait();
		my_job.num = 0;
	};

	for (auto& job : jobs)
	{
		job.action = std::thread{work, std::ref(job)};
	}

	work_done.wait();
	for (auto const& job : jobs)
	{
		if (job.num != 5)
			abort();
	}
	start_clean_up.count_down();
	for (auto& job : jobs)
	{
		job.action.join();
	}
	for (auto const& job : jobs)
	{
		if (job.num != 0)
			abort();
	}
}

int main()
{
	device::Led led;
	for (;;)
	{
		test();
		std::this_thread::sleep_for(std::chrono::milliseconds{100});
		led.tick();
	}
}
