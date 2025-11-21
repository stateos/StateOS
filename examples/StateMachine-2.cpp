#include <stm32f4_discovery.h>
#include <os.h>
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
auto dispatcher = os::Task(nullptr);
auto StateOff   = os::State();
auto StateOn    = os::State();
auto blinker    = os::StateMachineT<10>
{{
	{ StateOff, EventInit,   []( hsm_t *, unsigned ){ led = 0; } },
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
		os::thisTask::delay(std::chrono::seconds(1));
		blinker.send(EventTick);
	}
}
