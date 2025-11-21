#include <stm32f4_discovery.h>
#include <os.h>

using namespace device;
using namespace stateos;

auto led = Led();
auto sem = Semaphore();
auto cons = Task([]{ sem.wait(); led.tick(); });
auto prod = Task([]{ thisTask::sleepFor(SEC); sem.give(); });

int main()
{
	cons.start();
	prod.start();

	thisTask::stop();
}
