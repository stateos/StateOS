#include <stm32f4_discovery.h>
#include <os.h>

using namespace device;
using namespace stateos;

auto led = Led();

auto cons = Task::Start(0, []
{
	thisTask::action([](unsigned signo){ led = SIGSET(signo); });
	thisTask::suspend();
});

auto prod = Task::Start(0, []
{
	unsigned x = 0;

	for (;;)
	{
		thisTask::sleepFor(SEC);
		cons.signal(x);
		x = (x + 1) % 4;
	}
});

int main()
{
	thisTask::stop();
}
