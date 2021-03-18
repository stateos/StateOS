#include <stm32f4_discovery.h>
#include <os.h>

using namespace device;
using namespace stateos;

auto led = Led();
auto tsk = Task(0, []{});

int main()
{
	this_task::flip([]
	{
		this_task::sleepNext(SEC);
		tsk.startFrom([]
		{
			led.tick();
			this_task::stop();
		});
		tsk.join();
	});
}
