#include <stm32f4_discovery.h>
#include <os.h>
#include <vector>

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

OS_TSK(dispatcher, 0, NULL);
OS_HSM_STATE(StateOff, NULL);
OS_HSM_STATE(StateOn,  NULL);
OS_HSM(blinker, 1);

void StateOffHandler(hsm_t *, unsigned event)
{
	switch (event)
	{
	case EventInit:
		LEDs = 0;
		break;
	default:
		assert(false); // system shouldn't get here
	}
}

void StateOnHandler(hsm_t *, unsigned event)
{
	switch (event)
	{
	case EventTick:
		LED_Tick();
		break;
	default:
		assert(false); // system shouldn't get here
	}
}

std::vector<hsm_action_t> tab =
{
	_HSM_ACTION_INIT(StateOff, EventInit,   NULL,     StateOffHandler),
	_HSM_ACTION_INIT(StateOff, EventSwitch, StateOn,  NULL),
	_HSM_ACTION_INIT(StateOn,  EventSwitch, StateOff, NULL),
	_HSM_ACTION_INIT(StateOn,  EventTick,   NULL,     StateOnHandler),
};

int main()
{
	LED_Init();

	for (auto& a: tab) hsm_link(blinker, &a);

	hsm_start(blinker, dispatcher, StateOff);
	hsm_send(blinker, EventSwitch);
	for (;;)
	{
		tsk_delay(SEC);
		hsm_send(blinker, EventTick);
	}
}
