#include <stm32f4_discovery.h>
#include <os.h>

using namespace device;
using namespace stateos;

int main()
{
	auto led = Led();

	for (;;)
	{
		ThisTask::delay(SEC);
		led.tick();
	}
}
