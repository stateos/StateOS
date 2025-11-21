#include <stm32f4_discovery.h>
#include <os.h>

using namespace device;
using namespace stateos;

auto tsk = Task::Start([]
{
	auto led = Led();
	auto one = OnceFlag();

	for (;;)
	{
		one.call([&]{ led = 0; });
		tsk_delay(SEC);
		led.tick();
	}
});

int main()
{
	thisTask::stop();
}
