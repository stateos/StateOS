#include "stm32f4_discovery.h"
#include <thread>
#include <mutex>
#include <chrono>

struct Tick : device::Led
{
	void operator()() { tick(); }
};

int main()
{
	Tick tick;

	for (;;)
	{
		std::this_thread::sleep_for(std::chrono::seconds{1});
		std::jthread jt([&](std::stop_token st)
		{
			std::once_flag flg;
			do std::call_once(flg, tick);
			while (!st.stop_requested());
		});
	}
}
