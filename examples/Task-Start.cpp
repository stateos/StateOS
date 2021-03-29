#include <stm32f4_discovery.h>
#include <os.h>

using namespace device;
using namespace stateos;

int main()
{
	auto led = Led();
	auto sem = Semaphore::Binary();
	auto cons = Task::Make (0, [&]{ sem.wait(); led.tick(); });
	auto prod = Task::Start(0, [&]{ thisTask::sleepFor(SEC); sem.give(); });
	cons.start();
	cons.join();
}
