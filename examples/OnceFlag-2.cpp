#include <stm32f4_discovery.h>
#include <os.h>

using namespace device;
using namespace stateos;

int main()
{
	auto led = Led();
	auto one = OnceFlag();

	Task::Create(0, [&]
	{
		for (;;)
		{
			one.call([&]{ led = 0; });
			tsk_delay(SEC);
			led.tick();
		}
	});

	thisTask::stop();
}
