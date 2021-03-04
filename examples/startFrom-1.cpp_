#include <stm32f4_discovery.h>
#include <os.h>

using namespace device;
using namespace stateos;

auto led = Led();
auto tsk = Task(0, []{});

int main()
{
	ThisTask::flip([]
	{
		ThisTask::sleepNext(SEC);
		tsk.startFrom([]
		{
			led.tick();
			ThisTask::stop();
		});
		tsk.join();
	});
}
