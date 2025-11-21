#include <stm32f4_discovery.h>
#include <os.h>

using namespace device;
using namespace stateos;

auto led = Led();
auto tsk = Task([]{});

int main()
{
	thisTask::flip([]
	{
		thisTask::sleepNext(SEC);
		tsk.startFrom([]
		{
			thisTask::flip([]
			{
				led.tick();
				thisTask::stop();
			});
		});
		tsk.join();
	});
}
