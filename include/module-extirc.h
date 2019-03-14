/*
 * module-extirc.h
 *
 *  Created on: 12 февр. 2019 г.
 *      Author: aleksey
 */

#ifndef MODULE_EXTIRC_H_
#define MODULE_EXTIRC_H_

#include <module-base.h>
#include <stdint.h>

typedef struct {
	uint32_t	EIENB;		 /*Состояние обычных прерываний (IRQ)*/
	uint32_t	EIREQ;		 /*Состояние быстрых прерываний (FIQ)*/
	uint32_t	EILVL;		 /*Состояние входов*/
} module_EXTIRC_controller_t;

///* -- Регистр разрешения внешних прерываний. EIENB */
//#define EIENB_ENB3	(1 << 3)	/*< Если бит ENB нуль, то по соответствующему входу внешний запрос на   */
//#define EIENB_ENB2	(1 << 2)    /*		прерывание любого типа игнорируется.                            */
//#define EIENB_ENB1	(1 << 1)    /*		0 – внешнее прерывание заблокировано (по умолчанию)             */
//#define EIENB_ENB0	(1 << 0)    /*		1 – внешнее прерывание разрешено                                */
//
///* -- Регистр запроса внешних прерываний. EIREQ */
//#define EIREQ_REQ3	(1 << 3)	/*< Если есть запрос на прерывание, бит REQ устанавливается в 1.        */
//#define EIREQ_REQ2	(1 << 2)	/*		0 – Нет запроса на прерывание (по умолчанию)                    */
//#define EIREQ_REQ1	(1 << 1)	/*		1 – Есть запрос на прерывание                                   */
//#define EIREQ_REQ0	(1 << 0)	/*	                                                                    */

/* -- Регистр уровня внешних прерываний. EILVL */
#define EILVL_LVL3_MASK	(0x3)	/*Ниже приведены возможные значения бит LVLx[1:0] для каждого из четырех  */
#define EILVL_LVL3_OFFS	(6	)	/* каналов. Значение этого регистра определяет, при каком типе            */
#define EILVL_LVL2_MASK	(0x3)	/*внешнего прерывания будет генерироваться внутреннее прерывание.         */
#define EILVL_LVL2_OFFS	(4	)	/*	                                                                      */
#define EILVL_LVL1_MASK	(0x3)	/*	                                                                      */
#define EILVL_LVL1_OFFS	(2	)	/*	                                                                      */
#define EILVL_LVL0_MASK	(0x3)	/*	                                                                      */
#define EILVL_LVL0_OFFS	(0	)	/*	                                                                      */

typedef enum {
	EXTIRC_XINT0,
	EXTIRC_XINT1,
	EXTIRC_XINT2,
	EXTIRC_XINT3
}module_EXTIRC_Channel_t;

typedef enum {
	EXTIRC_LEVEL_Low = 0,
	EXTIRC_LEVEL_High = 1,
	EXTIRC_LEVEL_Front = 2,
	EXTIRC_LEVEL_Fall = 3
} module_EXTIRC_Level_t;


int module_EXTIRC_setupLevel(module_EXTIRC_Channel_t channel, module_EXTIRC_Level_t level);
int module_EXTIRC_Enable(module_EXTIRC_Channel_t channel);
int module_EXTIRC_Disable(module_EXTIRC_Channel_t channel);
int module_EXTIRC_clearInterrupt(module_EXTIRC_Channel_t channel);
int module_EXTIRC_Setup(module_EXTIRC_Channel_t channel, module_EXTIRC_Level_t level);


#endif /* MODULE_EXTIRC_H_ */
