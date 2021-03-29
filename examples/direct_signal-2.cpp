#include <stm32f4_discovery.h>
#include <os.h>

using namespace device;
using namespace stateos;

int main()
{
	auto led = Led();

	auto cons = Task::Start(0, [&]
	{
		thisTask::action([&](unsigned signo)
		{
			thisTask::flip([&]
			{
				led = SIGSET(signo);
				thisTask::suspend();
			});
		});
		thisTask::suspend();
	});

	auto prod = Task::Start(0, [&]
	{
		unsigned x = 0;

		for (;;)
		{
			thisTask::sleepNext(SEC);
			cons.signal(x);
			x = (x + 1) % 4;
		}
	});

	thisTask::suspend();
}
