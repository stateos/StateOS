#include "stm32f4_discovery.h"
#include <thread>
#include <coroutine>

struct Generator
{
	struct promise_type
	{
		int current_value;

		Generator get_return_object() { return Generator{std::coroutine_handle<promise_type>::from_promise(*this)}; }

		std::suspend_always initial_suspend() { return {}; }
		std::suspend_always final_suspend() noexcept { return {}; }
		std::suspend_always yield_value(int value) { current_value = value; return {}; }

		void return_void() {}
		void unhandled_exception() { std::terminate(); }
	};

	std::coroutine_handle<promise_type> handle;

	~Generator() { if (handle) handle.destroy(); }

	bool next() { handle.resume(); return !handle.done(); }
	int value() { return handle.promise().current_value; }
};

Generator counter(int value)
{
	for (;;)
	{
		std::this_thread::sleep_for(std::chrono::seconds{1});
		co_yield value;
	}
}

int main()
{
	auto gen = counter(0);
	auto led = device::Led();
	
	while (gen.next())
		led.tick();
}
