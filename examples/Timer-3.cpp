#include <stm32f4_discovery.h>
#include <os.h>

using namespace device;
using namespace stateos;

int main()
{
	auto led = Led();
	auto tmr = Timer::StartPeriodic(std::chrono::seconds{1}, [&]{ led.tick(); });
	this_task::sleep();
}
