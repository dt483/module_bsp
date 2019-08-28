/*
 * module-wdog.c
 *
 *  Created on: 7 июн. 2019 г.
 *      Author: aleksey
 */


#include "module-wdog.h"

#define WDOG_ok 0
#define WDOG_err -1


static volatile module_WDOG_controller_t * const module_WDOG_instance = (module_WDOG_controller_t *) (WDOG_BASE);


static void dummyHandler(void)
{
	NOP();
}

module_WDOG_lockState_t module_WDOG_toggleLock(module_WDOG_lockState_t lck)
{
	if (lck)
		module_WDOG_instance->WdogLock = WDOG_UnLockWord;
	else
		module_WDOG_instance->WdogLock = 0xABABABAUL;

	return (module_WDOG_lockState_t) module_WDOG_instance->WdogLock;
}

uint32_t module_WDOG_getValue()
{
	return module_WDOG_instance->WdogValue;
}

void module_WDOG_startCounter(uint32_t value)
{
	module_WDOG_instance->WdogLoad = value;
}

void module_WDOG_enableInterrupt()
{
	module_WDOG_instance->WdogControl |= WdogControl_INTEN;
	if (!irqHandler)
	{
		_assert("WDOG: null interrupt handler pointer. Using dummy");
		module_WDOG_setSecondLevelInterruptHandler( (module_2ndLvl_interruptHandler_t) dummyHandler);
	}

}
void module_WDOG_disableInterrupt()
{
	module_WDOG_instance->WdogControl &= ~(WdogControl_INTEN);
}

void module_WDOG_clearInterrupt()
{
	module_WDOG_instance->WdogIntClr = 0xDEADBEEF;
}

void module_WDOG_enableReset()
{
	module_WDOG_instance->WdogControl |= WdogControl_RESEN;
}

void module_WDOG_disableReset()
{
	module_WDOG_instance->WdogControl |= WdogControl_RESEN;
}

void module_WDOG_setSecondLevelInterruptHandler(module_2ndLvl_interruptHandler_t handler)
{
	if (!handler)
	{
		_assert("WDOG: null interrupt handler pointer. Setting dummy");
		handler = (module_2ndLvl_interruptHandler_t) dummyHandler;
	}
	irqHandler = handler;
}


//void module_WDOG_interruptHandler (void) __attribute__((interrupt ("IRQ")));
void module_WDOG_interruptHandler (void)
{
	module_WDOG_clearInterrupt();
	irqHandler();
	module_VIC_finishHandling();
}

