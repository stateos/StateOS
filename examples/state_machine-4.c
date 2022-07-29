#include <stm32f4_discovery.h>
#include <os.h>

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

hsm_state_t StateOff;
hsm_state_t StateOn;
hsm_t       blinker;
unsigned    blinker_queue[100];
tsk_t       dispatcher;
stk_t       dispatcher_stack[100];

void StateOffHandler(hsm_t *hsm, unsigned event)
{
	(void) hsm;

	switch (event)
	{
	case EventInit:
		LEDs = 0;
		break;
	default:
		assert(false); // system shouldn't get here
	}
}

void StateOnHandler(hsm_t *hsm, unsigned event)
{
	(void) hsm;

	switch (event)
	{
	case EventTick:
		LED_Tick();
		break;
	default:
		assert(false); // system shouldn't get here
	}
}

hsm_action_t tab[] =
{
	HSM_ACTION_INIT(&StateOff, EventInit,   NULL,      StateOffHandler),
	HSM_ACTION_INIT(&StateOff, EventSwitch, &StateOn,  NULL),
	HSM_ACTION_INIT(&StateOn,  EventSwitch, &StateOff, NULL),
	HSM_ACTION_INIT(&StateOn,  EventTick,   NULL,      StateOnHandler),
};
#define tabsize (int)(sizeof(tab)/sizeof(tab[0]))

int main()
{
	LED_Init();

	hsm_initState(&StateOff, NULL);
	hsm_initState(&StateOn,  NULL);
	hsm_init(&blinker, blinker_queue, sizeof(blinker_queue));
	wrk_init(&dispatcher, 0, NULL, dispatcher_stack, sizeof(dispatcher_stack));

	for (int i = 0; i < tabsize; i++) hsm_link(&tab[i]);

	hsm_start(&blinker, &dispatcher, &StateOff);
	hsm_send(&blinker, EventSwitch);
	for (;;)
	{
		tsk_delay(SEC);
		hsm_send(&blinker, EventTick);
	}
}
