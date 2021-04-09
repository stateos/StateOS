#include <stm32f4_discovery.h>
#include <thread>
#include <future>
#include <chrono>
#include <functional>
#include <utility>
#include <cassert>
 
void worker(std::future<void>& output)
{
	std::packaged_task<void(bool&)> my_task{ [](bool& done) { done = true; } };
	auto result = my_task.get_future();
	bool done = false;
	my_task.make_ready_at_thread_exit(done);
	assert(done);
	auto status = result.wait_for(std::chrono::seconds(0));
	assert(status == std::future_status::timeout);
	output = std::move(result);
}
 
int main()
{
	device::Led led;
	std::future<void> result;
	std::thread{worker, std::ref(result)}.join();
	auto status = result.wait_for(std::chrono::seconds(0));
	assert(status == std::future_status::ready);
	led = 15;
}
