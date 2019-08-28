#ifndef MODULE_SYSTIMER_H
#define MODULE_SYSTIMER_H

/*Блок сдвоенных таймеров DIT*/

#include "module-base.h"
#include <stdint.h>


#define DIT_TIMER1_ADDR 		( DIT_BASE+0x0UL	)
#define DIT_TIMER2_ADDR 		( DIT_BASE+0x20UL	)

typedef enum {
	TIMER_1,
	TIMER_2
}  module_DIT_timerNum_t;

#define DIT_TICKRATE    ( 40960000 ) //Hz

typedef struct {
	uint32_t	Load   ;		/*Регистр загрузки. */
	uint32_t	Value  ;      /*Регистр текущего значения.. */
	uint32_t	Control;      /*Регистр управления.*/
	uint32_t	IntClr ;		/*Регистр снятия прерываний.*/
	uint32_t	RIS ;			/*Регистр состояния прерывания до наложения маски. */
	uint32_t	MIS;      	/*Регистр состояния прерывания после наложения маски.  */
	uint32_t	BGLoad;		/*Фоновый регистр загрузки.*/
} module_DIT_controller_t;


//#define TimerITCR        ( DIT_BASE + 0xF00  )
//#define TimerITOP        ( DIT_BASE + 0xF04  )
//
//#define TimerPeriphID0   ( DIT_BASE + 0xFE0  ) /*Идентификатор периферийного устройства. TimerPeriphID0 биты  [7:0]*/
//#define TimerPeriphID1   ( DIT_BASE + 0xFE4  ) /*Идентификатор периферийного устройства. TimerPeriphID1 биты  [15:8]*/
//#define TimerPeriphID2   ( DIT_BASE + 0xFE8  ) /*Идентификатор периферийного устройства. TimerPeriphID2 биты  [23:16]*/
//#define TimerPeriphID3   ( DIT_BASE + 0xFEC  ) /*Идентификатор периферийного устройства. TimerPeriphID3 биты  [31:24]*/
//
//#define TimerPCellID0    ( DIT_BASE + 0xFF0  ) /*Идентификатор PrimeCellID0 биты  [7:0]*/
//#define TimerPCellID1    ( DIT_BASE + 0xFF4  ) /*Идентификатор PrimeCellID1 биты  [15:8]*/
//#define TimerPCellID2    ( DIT_BASE + 0xFF8  ) /*Идентификатор PrimeCellID2 биты  [23:16]*/
//#define TimerPCellID3    ( DIT_BASE + 0xFFC  ) /*Идентификатор PrimeCellID3 биты  [31:24]*/

/* Формат регистра TimerXControl */
#define TimerEn			(1 << 7	)            /*Бит разрешения*/
#define TimerMode		(1 << 6	)          /*Бит режима:*/
#define IntEnable		(1 << 5 )         /*Бит разрешения прерываний*/
#define TimerPre_MASK	(0x3	)          	/*Биты масштабирования частоты счета (делитель частоты) - не делится*/
#define TimerPre_OFFS	(0x2	)
#define TimerSize		( 1 << 1) /*Выбирает16/32 битный счетчик используется*/
#define OneShot			( 1 	)                /*Выбирает однократный или многократный режим счетчика*/

typedef enum {
	DIT_divider_nodiv= 0,
	DIT_divider_16,
	DIT_divider_256
}  module_DIT_divider_t;

typedef enum {
	DIT_timeExp_Seconds = 1,
	DIT_timeExp_millSeconds = 1000,
	DIT_timeExp_microSeconds = 1000000
}  module_DIT_timeExp_t;



/*Function prototypes*/
module_DIT_controller_t* module_DIT_getInstance(module_DIT_timerNum_t num);

int module_DIT_runFreeCounter(module_DIT_controller_t * timerInstance, module_DIT_divider_t divider);
int module_DIT_runPeriodicCounter(module_DIT_controller_t * timerInstance, module_DIT_divider_t divider, uint32_t value);
int module_DIT_runOneShotCounter(module_DIT_controller_t * timerInstance, module_DIT_divider_t divider, uint32_t value);

void module_DIT_EnableInterrupt(module_DIT_controller_t * timerInstance);
void module_DIT_DisableInterrupt(module_DIT_controller_t * timerInstance);
void module_DIT_clearInterrupt(module_DIT_controller_t * timerInstance);

//need floating point for good precision void module_DIT_WaitMicroSeconds
uint32_t module_DIT_countTicks(double timeValue, module_DIT_timeExp_t timeExp , module_DIT_divider_t timerDiv);
//void module_DIT_WaitMillSeconds(module_DIT_controller_t * timerInstance, uint32_t ms );
//void module_DIT_WaitMicroSeconds(module_DIT_controller_t * timerInstance, uint32_t us );
void module_DIT_Wait(module_DIT_controller_t * timerInstance, double timeValue , module_DIT_timeExp_t timeExp);

#endif
