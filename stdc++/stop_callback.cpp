#include "stm32f4_discovery.h"
#include <thread>
#include <mutex>
#include <condition_variable>
#include <chrono>

device::Led led;

void test()
{
	std::jthread worker([] (std::stop_token stoken) {
		led.toggle(0);
		std::mutex mutex;
		std::unique_lock lock(mutex);
		std::condition_variable_any().wait(lock, stoken,
			[&stoken] { return stoken.stop_requested(); });
	});

	std::stop_callback callback(worker.get_stop_token(), [] {
		led.toggle(1);
	});

	{
		std::stop_callback scoped_callback(worker.get_stop_token(), [] {
			led.toggle(1);
		});
	}

	auto stopper_func = [&worker] {
		if(worker.request_stop())
			led.toggle(1);
		led.toggle(2);
	};

	std::jthread stopper1(stopper_func);
	std::jthread stopper2(stopper_func);
	stopper1.join();
	stopper2.join();

	led.toggle(3);
	std::stop_callback callback_after_stop(worker.get_stop_token(), [] {
		led.toggle(3);
	});
	led.toggle(0);
}

int main()
{
	for (;;)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds{100});
		test();
		led.tick();
	}
}
