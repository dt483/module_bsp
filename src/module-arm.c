/*
 * module-arm.c
 *
 *  Created on: 8 февр. 2019 г.
 *      Author: aleksey
 */


#include <include/module-arm.h>
/**
 * 	@brief
 */
void module_ARM_vicEnable (void)
{
	__asm volatile ("mrc	p15, 0, r0, c1, c0, 0	"::: "memory");
	__asm volatile ("orr 	r0, r0, #0x1000000		"::: "memory");
	__asm volatile ("mcr	p15, 0, r0, c1, c0, 0	"::: "memory");
}

void module_ARM_irqUnblock (void)
{
	__asm volatile ("cpsie i" ::: "memory");
}

void module_ARM_irqBlock (void)
{
	__asm volatile ("cpsid i" ::: "memory");
}

