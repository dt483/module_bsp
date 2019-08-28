/*
 * module-vic.c
 *
 *  Created on: 30 янв. 2019 г.
 *      Author: aleksey
 */

/*TODO: настройка приоритетов прерыванйи*/

#define VIC_ok 0
#define VIC_err -1

#include "module-vic.h"


static volatile module_VIC_controller_t  * const vicControlRegs = (module_VIC_controller_t *) (VIC_BASE);
static volatile module_VIC_HANDLER_t  * const vicHandlers = (module_VIC_HANDLER_t *) (VIC_BASE + 0x100);
static volatile module_VIC_PRIORITY_t * const vicPrioritys = (module_VIC_PRIORITY_t *) (VIC_BASE + 0x200);
static volatile module_VIC_VICADDRESS_t * const vicAddress  = (module_VIC_VICADDRESS_t *) ( VIC_BASE + 0xF00 );


static int check_int_number(module_VIC_INT_NUM_t interruptNumber)
{
	/*Returns VIC_ok if number of int source is illegal*/
	if ( (interruptNumber > 31) | (interruptNumber < 0))
	{
		_runtime_error("VIC: illegal interrupt number");
		return VIC_err;
	}
	return VIC_ok;
}
static int check_int_priority(module_VIC_INT_NUM_t interruptNumber)
{
	/*Returns VIC_ok if number of int source is illegal*/
	if ( (interruptNumber > 31) | (interruptNumber < 0))
	{
		_runtime_error("VIC: illegal interrupt number");
		return VIC_err;
	}
	return VIC_ok;
}

int module_VIC_set_interrupt_handler (module_VIC_INT_NUM_t interruptNumber, void* handlerPointer)
{
	if (check_int_number(interruptNumber) == VIC_ok)
	{
		if (handlerPointer == 0)
		{
			_runtime_error("VIC: illegal handler pointer");
			 return VIC_err;
		} else
		{
			//module_ARM_irqBlock();
			module_VIC_disable_interrupt(interruptNumber);
			vicHandlers[interruptNumber] = (void*) handlerPointer;
			_debug("VIC: For interrupt souce No. %u sets handler on 0x%X "
					,interruptNumber,(int*)handlerPointer);
			module_VIC_enable_interrupt(interruptNumber);
			//module_ARM_irqUnblock();
			return VIC_ok;
		}
	} else return VIC_err;
}

/*int module_VIC_set_interrupt_priority (module_VIC_INT_NUM_t interruptNumber, void* handlerPointer)
{
	if (check_int_number(interruptNumber) == VIC_ok)
	{
		if (handlerPointer == 0)
		{
			_runtime_error("VIC: illegal handler pointer");
			 return VIC_err;
		} else
		{
			vicHandlers[interruptNumber] = (void*) handlerPointer;
			_debug("VIC: For interrupt souce No. %i sets handler on 0x%X ",interruptNumber,(int*)handlerPointer);
			return VIC_ok;
		}
	} else return VIC_err;
}*/


int module_VIC_get_RAW_interrupt_status (module_VIC_INT_NUM_t interruptNumber)
{
	if (check_int_number(interruptNumber) == VIC_ok)
	{
		return ((vicControlRegs->VICRAWINTR) & (1UL << interruptNumber)) >> interruptNumber;
	} else return VIC_err;
}

module_VIC_INT_STATUS_t module_VIC_get_interrupt_status (module_VIC_INT_NUM_t interruptNumber)
{
	if (check_int_number(interruptNumber) == VIC_ok)
	{
		uint32_t mask = 1UL << interruptNumber;
		if (vicControlRegs->VICINTSELECT & mask) //IRQ ? FIQ
		{//IRQ
			if ((vicControlRegs->VICIRQSTATUS & mask) != 0) return IRQ_ACTIVE;
			else return IRQ_NON_ACTIVE;
		}
		else
		{//FIQ
			if ((vicControlRegs->VICFIQSTATUS & mask) != 0) return IRQ_ACTIVE;
			else return IRQ_NON_ACTIVE;
		}
	} else return VIC_err;

}


int module_VIC_set_interrupt_type (module_VIC_INT_NUM_t interruptNumber, module_VIC_INT_TYPE_t type)
{
	if (check_int_number(interruptNumber) == VIC_ok)
	{
		uint32_t mask = 1UL << interruptNumber;
		if (type == FIQ)
		{
			vicControlRegs->VICINTSELECT |= mask;
		}
		else
		{
			 if (type!=FIQ & type!=IRQ) _assert("VIC: illegal int type. Will set to IRQ.");
			 mask = ~mask;
			 vicControlRegs->VICINTSELECT &= mask;
		}
		return VIC_ok;
	}
	else return VIC_err;
}


int module_VIC_enable_interrupt (module_VIC_INT_NUM_t interruptNumber)
{
	if (check_int_number(interruptNumber) == VIC_ok)
	{
		vicControlRegs->VICINTENABLE |= 1UL << interruptNumber;
		return VIC_ok;
	}
	else return VIC_err;
}

int module_VIC_disable_interrupt (module_VIC_INT_NUM_t interruptNumber)
{
	if (check_int_number(interruptNumber) == VIC_ok)
	{
		vicControlRegs->VICINTENCLEAR |= 1UL << interruptNumber;
		return VIC_ok;
	}
	else return VIC_err;
}

int module_VIC_invoke_soft_interrupt (module_VIC_INT_NUM_t interruptNumber)
{
	if (check_int_number(interruptNumber) == VIC_ok)
	{
		vicControlRegs->VICSOFTINT |= 1UL << interruptNumber;
		return VIC_ok;
	}
	else return VIC_err;
}

int module_VIC_clear_soft_interrupt (module_VIC_INT_NUM_t interruptNumber)
{
	if (check_int_number(interruptNumber) == VIC_ok)
	{
		vicControlRegs->VICSOFTINTCLEAR |= 1UL << interruptNumber;
		return VIC_ok;
	}
	else return VIC_err;
}

void module_VIC_finishHandling ()
{
	*vicAddress = 0x0;
}


