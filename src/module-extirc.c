/*
 * module-extirc.c
 *
 *  Created on: 12 февр. 2019 г.
 *      Author: aleksey
 */

#include "module-extirc.h"
#include "module-vic.h"

#define EXTIRC_ok 0
#define EXTIRC_err -1

static volatile module_EXTIRC_controller_t  * const module_EXTIRC_instance = (module_EXTIRC_controller_t *) (EXTIRC_BASE);

int module_EXTIRC_setupLevel(module_EXTIRC_Channel_t channel, module_EXTIRC_Level_t level)
{
	if (level > EXTIRC_LEVEL_Fall)
	{
		_assert ("EXTIRC: invalid level number");
		return EXTIRC_err;
	}

	switch (channel)
	{
		case 	EXTIRC_XINT0:
		{
			SET_BIT_FIELD(module_EXTIRC_instance->EILVL, EILVL_LVL0, level);
			break;
		}
		case 	EXTIRC_XINT1:
		{
			SET_BIT_FIELD(module_EXTIRC_instance->EILVL, EILVL_LVL1, level);
			break;
		}
		case 	EXTIRC_XINT2:
		{
			SET_BIT_FIELD(module_EXTIRC_instance->EILVL, EILVL_LVL2, level);
			break;
		}
		case 	EXTIRC_XINT3:
		{
			SET_BIT_FIELD(module_EXTIRC_instance->EILVL, EILVL_LVL3, level);
			break;
		}
		default:
		{
			_assert ("EXTIRC: invalid channel number");
			return EXTIRC_err;
		}
	}
	return EXTIRC_ok;
}


int module_EXTIRC_Enable(module_EXTIRC_Channel_t channel)
{
	if (channel > 3)
	{
		_assert ("EXTIRC: invalid channel number");
		return EXTIRC_err;
	}

	module_EXTIRC_instance->EIENB |= 1 << channel;
	return EXTIRC_ok;
}

int module_EXTIRC_Disable(module_EXTIRC_Channel_t channel)
{
	if (channel > 3)
	{
		_assert ("EXTIRC: invalid channel number");
		return EXTIRC_err;
	}

	module_EXTIRC_instance->EIENB &= ~(1 << channel);
	return EXTIRC_ok;
}

int module_EXTIRC_clearInterrupt(module_EXTIRC_Channel_t channel)
{
	if (channel > 3)
	{
		_assert ("EXTIRC: invalid channel number");
		return EXTIRC_err;
	}
	module_EXTIRC_instance->EIREQ &= ~(1 << channel);
	return EXTIRC_ok;
}

int module_EXTIRC_Setup(module_EXTIRC_Channel_t channel, module_EXTIRC_Level_t level)
{
	if (module_EXTIRC_Disable(channel) == EXTIRC_err)  return EXTIRC_err;
	if (module_EXTIRC_setupLevel(channel,level)== EXTIRC_err)return EXTIRC_err;
	if (module_EXTIRC_clearInterrupt(channel) == EXTIRC_err)return EXTIRC_err;
	if (module_EXTIRC_Enable(channel) == EXTIRC_err) return EXTIRC_err;
	return EXTIRC_ok;
}


