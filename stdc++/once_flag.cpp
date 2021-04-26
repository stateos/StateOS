#include "stm32f4_discovery.h"
#include <thread>
#include <mutex>
#include <chrono>

void test()
{
	std::once_flag flg;
	std::jthread jt([&](std::stop_token st)
	{
		do std::call_once(flg, []{});
		while (!st.stop_requested());
	});
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
