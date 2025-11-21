#include <stm32f4_discovery.h>
#include <os.h>

using namespace device;
using namespace stateos;

int main()
{
	auto sem = Semaphore::Binary();

	auto cons = Task::Start([&]
	{
		Led led;

		for (;;)
		{
			sem.wait();
			led.tick();
		}
	});

	auto prod = Task::Start([&]
	{
		for (;;)
		{
			thisTask::sleepFor(SEC);
			sem.give();
		}
	});

	thisTask::sleep();
}
