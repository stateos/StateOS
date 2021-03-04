#include <stm32f4_discovery.h>
#include <os.h>

using namespace device;
using namespace stateos;

int main()
{
	auto sem = Semaphore::Binary();

	auto cons = Task::Start(0, [&]
	{
		Led led;

		for (;;)
		{
			sem.wait();
			led.tick();
		}
	});

	auto prod = Task::Start(0, [&]
	{
		for (;;)
		{
			ThisTask::sleepFor(SEC);
			sem.give();
		}
	});

	ThisTask::sleep();
}
