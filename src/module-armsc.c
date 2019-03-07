/*
 * module-armsc.c
 *
 *  Created on: 13 февр. 2019 г.
 *      Author: aleksey
 */


#define ARMSC_ok 0
#define ARMSC_err -1

#include <include/module-armsc.h>

static volatile module_ARMSC_controller_t  * const module_ARMSC_instance = (module_ARMSC_controller_t *) (ARMSC_BASE);


static int check_int_number(module_ARMSC_intNum_t interruptNumber)
{
	/*Returns VIC_ok if number of int source is illegal*/
	if ( (interruptNumber > 5) | (interruptNumber < 0))
	{
		_runtime_error("ARMSC: illegal interrupt number");
		return ARMSC_err;
	}
	return ARMSC_ok;
}

int module_ARMSC_get_NMU_interrupt_status (module_ARMSC_intNum_t interruptNumber)
{
	if (check_int_number(interruptNumber) == ARMSC_ok)
	{
		return ((module_ARMSC_instance->NMUINTSTA) & (1UL << interruptNumber)) >> interruptNumber;
	} else return ARMSC_err;
}

int module_ARMSC_clear_NMU_interrupt (module_ARMSC_intNum_t interruptNumber)
{
	if (check_int_number(interruptNumber) == ARMSC_ok)
	{
		 module_ARMSC_instance->NMUINTCLR |= (1UL << interruptNumber);
		 return ARMSC_ok;
	} else return ARMSC_err;
}


int module_ARMSC_generate_NMU_interrupt (module_ARMSC_NMCnum_t nmcNumber, module_ARMSC_typeInt_t typeInt)
{
	uint32_t reg_mask = 0;

	if (!((typeInt==2)||(typeInt==4)||(typeInt==6)))
	{
		_assert("ARMSC: illegal interrupt type number");
		return ARMSC_err;
	}
	if ( nmcNumber > 1)
	{
		_assert("ARMSC: illegal NMC number");
		return ARMSC_err;
	}

	module_ARMSC_instance->NMUINTREQ |= (1UL << (typeInt+nmcNumber));
	return ARMSC_ok;
}

int module_ARMSC_set_NMU_interrupt_priority (module_ARMSC_NMCnum_t nmcNumber)
{
	if ( nmcNumber > 1)
	{
		_assert("ARMSC: illegal NMC number");
		return ARMSC_err;
	}
	if (nmcNumber == ARMSC_NMC0)
		module_ARMSC_instance->NMUCTRL &= !(1UL);
	else
		module_ARMSC_instance->NMUCTRL |= 1UL;
	return ARMSC_ok;
}
