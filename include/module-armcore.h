/**
 * @file module-arm.h
 * @brief Specific arm-asm functions
 *
 * Created on: 8 февр. 2019 г.
 *      Author: aleksey
 */

#ifndef MODULE_LIB_MODULE_ARM_H_
#define MODULE_LIB_MODULE_ARM_H_

#include "module-base.h"


	/* ASM macro defs */
#define NOP()						__asm volatile ( "NOP" );


/**
 * @brief module_ARM_irqUnblock
 */
void module_ARM_irqUnblock (void);
void module_ARM_irqBlock (void);
void module_ARM_vicEnable (void);


#endif /* MODULE_LIB_MODULE_ARM_H_ */
