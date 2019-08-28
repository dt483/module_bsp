/*
 * module-wdog.h
 *
 *  Created on: 7 июн. 2019 г.
 *      Author: aleksey
 */

#ifndef INCLUDE_MODULE_WDOG_H_
#define INCLUDE_MODULE_WDOG_H_

#include "module-base.h"
#include "module-vic.h"
#include "stdint.h"

typedef struct {
	uint32_t	WdogLoad	;	/*< Регистр загрузки.                                        */
	uint32_t	WdogValue	;	/*< Регистр значения.                                        */
	uint32_t	WdogControl	;   /*<Регистр управления.                                       */
	uint32_t	WdogIntClr 	;	/*< Регистр снятия прерываний.                               */
	uint32_t	WdogRIS 	;	/*< Регистр состояния прерывания до наложения маски.         */
	uint32_t	WdogMIS		;	/*< Регистр состояния прерывания после наложения маски.      */
	uint32_t	reserved_0	;	/*< Зарезервировано                                          */
	uint32_t	WdogLock	;	/*< Регистр блокировки.                                      */
} module_WDOG_controller_t;

module_2ndLvl_interruptHandler_t irqHandler = (module_2ndLvl_interruptHandler_t) 0;

/* -- Регистр управления. WdogControl  */
#define WdogControl_RESEN	(1 << 1)/* Разрешающий сигнал блока WDOG для сброса выхода, WDOGRST.             */
									/* 	Работает как маска для сброса выхода. Если установлен высокий        */
									/*	уровень, то разрешен сброс, если низкий, то сброс неактивен.         */
#define WdogControl_INTEN	(1)		/* Сигнал, разрешающий прерывание события, WDOGINT. Если установ-        */
									/*	лен высокий уровень, то счетчик находится в активном состоянии и     */
									/*	разрешены прерывания, если установлен низкий уровень, то счетчик и   */
									/*  прерывания неактивны. Нужно перезагрузить счетчик значением, кото-   */
									/*  рое хранится в WdogLoad, если прерывание раннее было неактивным,     */
									/*  но затем стало разрешенным.     */

#define WDOG_UnLockWord 0x1ACCE551UL

typedef enum {
	WDOG_LOCKED = 1,
	WDOG_UNLOCKED = 0
} module_WDOG_lockState_t;

module_WDOG_lockState_t module_WDOG_toggleLock(module_WDOG_lockState_t lck);


uint32_t module_WDOG_getValue();

void module_WDOG_startCounter(uint32_t value);

void module_WDOG_enableInterrupt();

void module_WDOG_disableInterrupt();

void module_WDOG_clearInterrupt();

void module_WDOG_enableReset();

void module_WDOG_disableReset();

void module_WDOG_setSecondLevelInterruptHandler(module_2ndLvl_interruptHandler_t handler);


void module_WDOG_interruptHandler (void) __attribute__((interrupt ("IRQ")));



#endif /* INCLUDE_MODULE_WDOG_H_ */
