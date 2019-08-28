
#include "module-armcore.h"
#include "module-dit.h"
#include "module-gpio.h"
#include <stdint.h>

#define DIT_ok 0
#define DIT_err -1

/*static instances*/
//static volatile module_DIT_controller_t * const module_DIT_Timer1 = (module_DIT_controller_t *) (DIT_TIMER1_ADDR );
//static volatile module_DIT_controller_t * const module_DIT_Timer2 = (module_DIT_controller_t *) (DIT_TIMER2_ADDR );

module_DIT_controller_t* module_DIT_getInstance(module_DIT_timerNum_t num)
{
    if (num == TIMER_1) return (module_DIT_controller_t*) DIT_TIMER1_ADDR;
    else if (num == TIMER_2) return (module_DIT_controller_t*) DIT_TIMER2_ADDR;
    else {
    	_assert("DIT: Timer number not valid");
    	return (module_DIT_controller_t*) 0;
    }
}
int module_DIT_runFreeCounter(module_DIT_controller_t * timerInstance, module_DIT_divider_t divider)
{
	timerInstance->Control &= ~(TimerEn); // stop timer
	uint32_t ctrl_temp = timerInstance->Control;

	SET_BIT_FIELD(ctrl_temp, TimerPre, divider);

	ctrl_temp &= ~(TimerMode);  // Free access mode
	ctrl_temp &= ~OneShot;  	// Multiple shots
	ctrl_temp |= TimerSize; 	// 32-bit counter size

	timerInstance->Control = ctrl_temp;
	timerInstance->Control |= TimerEn; // start timer
	return DIT_ok;
}
int module_DIT_runPeriodicCounter(module_DIT_controller_t * timerInstance, module_DIT_divider_t divider, uint32_t value)
{

	//timerInstance->Control &= ~(IntEnable);//diasable interrupt

	timerInstance->Control &= ~(TimerEn); // stop timer
	uint32_t ctrl_temp = timerInstance->Control;

	SET_BIT_FIELD(ctrl_temp, TimerPre, divider);

	ctrl_temp |= TimerMode;  	// Periodic mode
	ctrl_temp &= ~OneShot;  	// Multiple shots
	ctrl_temp |= TimerSize; 	// 32-bit counter size

	timerInstance->Load = value; //setting start value

	timerInstance->Control = ctrl_temp;

	timerInstance->IntClr = 0xDEADBEEF; //clear interrupt

	//timerInstance->Control |= IntEnable; //enable interrupt

	timerInstance->Control |= TimerEn; // start timer
	return DIT_ok;
}

int module_DIT_runOneShotCounter(module_DIT_controller_t * timerInstance, module_DIT_divider_t divider, uint32_t value)
{
	timerInstance->Control &= ~(TimerEn); // stop timer
	uint32_t ctrl_temp = timerInstance->Control;

	SET_BIT_FIELD(ctrl_temp, TimerPre, divider);

	ctrl_temp |= TimerMode;  	// Periodic mode
	ctrl_temp |= OneShot;  		// One shot
	ctrl_temp |= TimerSize; 	// 32-bit counter size

	timerInstance->Load = value; //setting start value

	timerInstance->Control = ctrl_temp;
	timerInstance->Control |= TimerEn; // start timer
	return DIT_ok;
}

inline void module_DIT_EnableInterrupt(module_DIT_controller_t * timerInstance)
{
		timerInstance->Control |= IntEnable;
}
inline void module_DIT_DisableInterrupt(module_DIT_controller_t * timerInstance)
{
		timerInstance->Control &= ~(IntEnable);
}

inline void module_DIT_clearInterrupt(module_DIT_controller_t * timerInstance)
{
	timerInstance->IntClr = 0x1;
}


inline uint32_t module_DIT_countTicks(double timeValue, module_DIT_timeExp_t timeExp ,
		module_DIT_divider_t timerDiv)
{
	double div = 1;
	if (timerDiv == DIT_divider_16) div = 16;
		else if (timerDiv == DIT_divider_256) div = 256;
	double tick_num = ( ((double)DIT_TICKRATE / ((double) div)) / (double) timeExp) * timeValue;
	return (uint32_t) tick_num;
}

void module_DIT_Wait(module_DIT_controller_t * timerInstance, double timeValue , module_DIT_timeExp_t timeExp)
{
	module_DIT_DisableInterrupt(timerInstance); //disable interrupt

	uint32_t tick_num = module_DIT_countTicks(timeValue, timeExp, DIT_divider_16);
	module_DIT_runOneShotCounter(timerInstance ,DIT_divider_16, tick_num);
	while( (timerInstance->Value ) > 0 )
    {
       NOP(); NOP(); NOP(); NOP();
    }
}



/*void module_DIT_WaitMillSeconds(module_DIT_controller_t * timerInstance, uint32_t ms )
{
	module_DIT_DisableInterrupt(timerInstance); //disable interrupt

	uint32_t tick_num = ((DIT_TICKRATE / (256))/1000) * ms;
	/// 1000000
	module_DIT_runOneShotCounter(timerInstance ,DIT_divider_256, tick_num);
    while( (timerInstance->Value ) > 0 )
    {
       NOP(); NOP(); NOP(); NOP();
    }
}

void module_DIT_WaitMicroSeconds(module_DIT_controller_t * timerInstance, uint32_t us )
{
	module_DIT_DisableInterrupt(timerInstance); //disable interrupt

	double tick_num = (((double)DIT_TICKRATE / ( (double) 16))/ (double) 1000000) * (double) us;
	uint32_t tick_num_uint = (uint32_t) tick_num;
	/// 1000000
	module_DIT_runOneShotCounter(timerInstance ,DIT_divider_16,  tick_num_uint);
    while( (timerInstance->Value ) > 0 )
    {
       NOP(); NOP(); NOP(); NOP();
    }
}*/


