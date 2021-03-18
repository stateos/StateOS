#include <stm32f4_discovery.h>
#include <os.h>

using namespace device;
using namespace stateos;

auto led = Led();
auto sem = Semaphore(0);
auto cons = Task::Start(0, []{ sem.wait(); led.tick(); });
auto prod = Task::Start(0, []{ this_task::sleepFor(SEC); sem.give(); });

int main()
{
	this_task::stop();
}
