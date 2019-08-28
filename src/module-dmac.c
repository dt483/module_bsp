/*
 * module-dmac.c
 *
 *  Created on: 13 февр. 2019 г.
 *      Author: aleksey
 */

#include "module-dmac.h"


#define DMAC_ok 0
#define DMAC_err -1

#define MAX_PENDING 1000

static module_DMAC_controller_t module_DMAC_instance = {
	(module_DMAC_channel_t* ) CHANNEL_FtS1,  // SPI_MemToPeri
	(module_DMAC_channel_t* ) CHANNEL_FtS2,  // USB_MemToPeri
	(module_DMAC_channel_t* ) CHANNEL_FtS3,  // UART0_MemToPeri
	(module_DMAC_channel_t* ) CHANNEL_FtS4,  // UART1_MemToPeri
	(module_DMAC_channel_t* ) CHANNEL_StF1,  // SPI_StF_PeriToMem
	(module_DMAC_channel_t* ) CHANNEL_StF2,  // USB_StF_PeriToMem
	(module_DMAC_channel_t* ) CHANNEL_StF3,  // UART0_StF_PeriToMem
	(module_DMAC_channel_t* ) CHANNEL_StF4  // UART1_StF_PeriToMem
};



static inline int check_memory_addr(uint32_t * memAddr)
{
//	Проверка данных для запись SrcAdr_FtSx или DstAdr_StFx. Этот адрес может принадлежать любому из
//	следующих банков памяти: SMBx, NM1Bx,NM2Bx, External SDRAM или AMBx,
//	т.е. любой области памяти кроме BROM и ARM Peripheral Area.
//
	uint32_t addr = (uint32_t) memAddr;

	//check 64 alignment
	if ( (0x7UL & addr) )
	{
		_runtime_error("DMAC: illegal memory address alignment");
		return DMAC_err;
	}

	if ( !(
		( (addr > AMB0_0_START_ADDR) && (addr< (AMB0_0_START_ADDR + AMB0_LENGTH)) ) ||
		( (addr > AMB0_START_ADDR) && (addr< (AMB0_START_ADDR + AMB0_LENGTH)) ) ||
		( (addr > AMB1_START_ADDR) && (addr< (AMB1_START_ADDR + AMB1_LENGTH)) ) ||

		( (addr > SMB0_START_ADDR) && (addr< (SMB0_START_ADDR + SMB0_LENGTH)) ) ||
		( (addr > SMB1_START_ADDR) && (addr< (SMB1_START_ADDR + SMB1_LENGTH)) ) ||
		( (addr > SMB2_START_ADDR) && (addr< (SMB2_START_ADDR + SMB2_LENGTH)) ) ||
		( (addr > SMB3_START_ADDR) && (addr< (SMB3_START_ADDR + SMB3_LENGTH)) ) ||

		( (addr > NM1B0_START_ADDR) && (addr < (NM1B0_START_ADDR + NM1B1_LENGTH)) ) ||
		( (addr > NM1B2_START_ADDR) && (addr < (NM1B2_START_ADDR + NM1B3_LENGTH)) ) ||
		( (addr > NM2B0_START_ADDR) && (addr < (NM2B0_START_ADDR + NM2B1_LENGTH)) ) ||
		( (addr > NM2B2_START_ADDR) && (addr < (NM2B2_START_ADDR + NM2B3_LENGTH)) )
	))
	{
		_runtime_error("DMAC: address not in memory range");
		return DMAC_err;
	}

		return DMAC_ok;
}

static inline int check_periph_addr(uint32_t * memAddr)
{
	//Проверка данных для запись DstAdr_FtSx или SrcAdr_StFx

	uint32_t addr = (uint32_t) memAddr;
	//check 64 alignment
	if ( (0x3UL & addr) )
	{
		_runtime_error("DMAC: illegal peripheral address alignment");
		return DMAC_err;
	}

	/*Returns VIC_ok if number of int source is illegal*/
	if ( !( (addr > PERIPHERAL_BASE) && (addr< (PERIPHERAL_BASE + PERIPHERAL_LENGTH)) ) )
	{
		_runtime_error("DMAC: address  not in peripheral range");
		return DMAC_err;
	}
	return DMAC_ok;
}

module_DMAC_channel_t* module_DMAC_getChannelInstance(module_DMAC_chNum_t num)
{
	if (num <= DMAC_CHANNEL_UART1_PeriToMem )
	{
		switch (num)
		{
			case DMAC_CHANNEL_SPI_MemToPeri:
			{
				return module_DMAC_instance.SPI_MemToPeri;
				break;
			}
			case DMAC_CHANNEL_USB_MemToPeri:
			{
				return module_DMAC_instance.USB_MemToPeri;
				break;
			}
			case DMAC_CHANNEL_UART0_MemToPeri:
			{
				return module_DMAC_instance.UART0_MemToPeri;
				break;
			}
			case DMAC_CHANNEL_UART1_MemToPeri:
			{
				return module_DMAC_instance.UART1_MemToPeri;
				break;
			}
			case DMAC_CHANNEL_SPI_PeriToMem:
			{
				return module_DMAC_instance.SPI_PeriToMem;
				break;
			}
			case DMAC_CHANNEL_USB_PeriToMem:
			{
				return module_DMAC_instance.USB_PeriToMem;
				break;
			}
			case DMAC_CHANNEL_UART0_PeriToMem:
			{
				return module_DMAC_instance.UART0_PeriToMem;
				break;
			}
			case DMAC_CHANNEL_UART1_PeriToMem:
			{
				return module_DMAC_instance.UART1_PeriToMem;
				break;
			}
		}
	}
	_runtime_error("DMAC: invalid channel number");
	return 0x0;
}

int module_DMAC_setupChannel(module_DMAC_channelConfig_t config)
{

	module_DMAC_channel_t* channel_inst = module_DMAC_getChannelInstance(config.channelNumber);

	//check channel busy
	if (channel_inst->Busy == 0x1)
	{
		_runtime_error("DMAC: channel is busy");
		return DMAC_err;
	}

	//checking addresses
	if (config.channelNumber<4)
	{ //mem to periph case
		if (check_memory_addr (config.srcAddr) == DMAC_err) return DMAC_err;
		if (check_periph_addr (config.dstAddr) == DMAC_err)return DMAC_err;
	}
	else
	{ //periph to mem case
		if (check_periph_addr (config.srcAddr) == DMAC_err)return DMAC_err;;
		if (check_memory_addr (config.dstAddr) == DMAC_err)return DMAC_err;;
	}
	//setting addresses
	channel_inst->SrcAdr = config.srcAddr;
	channel_inst->DstAdr = config.dstAddr;

	//setting bytenumber
	if (config.byteNumber > 0xFFFFFF)
	{
		_runtime_error("DMAC: byte number not fit on 24 bit");
		return DMAC_err;
	}
	//setting transfer size
	channel_inst->TrSize =  config.byteNumber;

	//checking packtype
	if (config.PackType > DMAC_Pack_32bit)
	{
		_runtime_error("DMAC:invalid package size");
		return DMAC_err;
	}

	//setting channeltype
	channel_inst->PackType = config.PackType;

	if (
			(config.channelNumber == DMAC_CHANNEL_UART0_PeriToMem) ||
			(config.channelNumber == DMAC_CHANNEL_UART1_PeriToMem )||
			(config.channelNumber == DMAC_CHANNEL_UART0_MemToPeri) ||
			(config.channelNumber == DMAC_CHANNEL_UART1_MemToPeri)
	)
		channel_inst->PerHandsh |= 0x1UL;
	else
		channel_inst->PerHandsh &= ~(0x1UL);


	return DMAC_ok;
}

int module_DMAC_startChannel(module_DMAC_chNum_t num)
{
	module_DMAC_channel_t* channel_inst = module_DMAC_getChannelInstance(num);
	if (!(channel_inst))
		return DMAC_err;
	channel_inst->Enable = 1UL;
	return DMAC_ok;
}

int module_DMAC_stopChannel(module_DMAC_chNum_t num)
{
	module_DMAC_channel_t* channel_inst = module_DMAC_getChannelInstance(num);
	if (!(channel_inst))
		return DMAC_err;
	channel_inst->Enable = 0UL;

	uint32_t pending = 0;

	while ( ( channel_inst->Busy == 0x1UL) && (pending < MAX_PENDING) )
			pending++;

	if (pending < MAX_PENDING)
	{
		return DMAC_ok;
	}

	_runtime_error("DMAC: cannot stop channel %i", num);
		return DMAC_err;

}

int module_DMAC_getInterruptStatus(module_DMAC_chNum_t num)
{
	module_DMAC_channel_t* channel_inst = module_DMAC_getChannelInstance(num);
	if (!(channel_inst))
		return DMAC_err;
	return channel_inst->Interrupt;
}

int module_DMAC_clearInterrupt(module_DMAC_chNum_t num)
{
	module_DMAC_channel_t* channel_inst = module_DMAC_getChannelInstance(num);
	if (!(channel_inst))
		return DMAC_err;
	channel_inst->Interrupt = (0UL);
	channel_inst->TrSize = 0x1UL;
	if 	(channel_inst->Interrupt == 0UL)
		return DMAC_ok;
	_runtime_error("DMAC: cannot clear interrupt on channel %i", num);
	return DMAC_err;
}

module_DMAC_chNum_t module_DMAC_getInterruptSource()
{
	module_DMAC_chNum_t ch=0;
	for (ch=0;ch<=DMAC_CHANNEL_UART1_PeriToMem;ch++)
	{
		if (module_DMAC_getInterruptStatus(ch)) return ch;
	}
	return DMAC_err;

}



