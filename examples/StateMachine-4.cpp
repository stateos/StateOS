#include <stm32f4_discovery.h>
#include <os.h>
#include <vector>
#include <chrono>

enum
{
	EventALL    = hsmALL,
	EventStop   = hsmStop,
	EventExit   = hsmExit,
	EventEntry  = hsmEntry,
	EventInit   = hsmInit,
	EventSwitch = hsmUser,
	EventTick,
};

auto led        = device::Led();
auto dispatcher = stateos::Task(0, nullptr);
auto StateOff   = stateos::State();
auto StateOn    = stateos::State();
auto blinker    = stateos::StateMachineT<10>
{{
	{ StateOff, EventSwitch, StateOn },
	{ StateOn,  EventSwitch, StateOff },
	{ StateOn,  EventTick,   []( hsm_t *, unsigned ){ led.tick(); } },
}};

int main()
{
	blinker.start(dispatcher, StateOff);
	blinker.send(EventSwitch);
	for (;;)
	{
		stateos::thisTask::delay(std::chrono::seconds(1));
		blinker.send(EventTick);
	}
}
