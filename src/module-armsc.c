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
	_debug ("ARMSC instance pointer: 0x%X", (uint32_t) module_ARMSC_instance);
	if (check_int_number(interruptNumber) == ARMSC_ok)
	{
		return ((module_ARMSC_instance->NMUINTSTA) & (1UL << interruptNumber)) >> interruptNumber;
	} else return ARMSC_err;
}

int module_ARMSC_clear_NMU_interrupt (module_ARMSC_intNum_t interruptNumber)
{
	if (check_int_number(interruptNumber) == ARMSC_ok)
	{
		 module_ARMSC_instance->NMUINTCLR = (1UL << interruptNumber);
		 return ARMSC_ok;
	} else return ARMSC_err;
}


int module_ARMSC_generate_NMU_interrupt (module_ARMSC_NMCnum_t nmcNumber, module_ARMSC_typeInt_t typeInt)
{
	__asm volatile ( "NOP" );
	__asm volatile ( "NOP" );
	__asm volatile ( "NOP" );
	__asm volatile ( "NOP" );

	volatile uint32_t reg_value = 1;
	//uint32_t * addr_inst;
	//_debug ("ARMSC instance pointer: 0x%X", (uint32_t) module_ARMSC_instance);

	if (!((typeInt==0)||(typeInt==2)||(typeInt==4)))
	{
		_assert("ARMSC: illegal interrupt type number");
		return ARMSC_err;
	}
	if ( nmcNumber > 1)
	{
		_assert("ARMSC: illegal NMC number");
		return ARMSC_err;
	}


	if (nmcNumber ==  ARMSC_NMC1) reg_value <<= ARMSC_NMC1;
	else if (nmcNumber ==  ARMSC_NMC2) reg_value <<= ARMSC_NMC2;

	if (typeInt == ARMSC_LOW_PRIORITY_INT)	reg_value<<=0;
	else if (typeInt == ARMSC_HIGH_PRIORITY_INT)   reg_value<<=2;
	else if (typeInt == ARMSC_NONMASKABLE_INT)  reg_value<<=4;

	module_ARMSC_instance->NMUINTREQ = reg_value;

	return ARMSC_ok;
}

int module_ARMSC_set_NMU_interrupt_priority (module_ARMSC_NMCnum_t nmcNumber)
{
	if ( nmcNumber > 1)
	{
		_assert("ARMSC: illegal NMC number");
		return ARMSC_err;
	}
	if (nmcNumber == ARMSC_NMC1)
		module_ARMSC_instance->NMUCTRL &= !(1UL);
	else
		module_ARMSC_instance->NMUCTRL |= 1UL;
	return ARMSC_ok;
}
