/*
 * module-spi.c
 *
 *  Created on: 4 февр. 2019 г.
 *      Author: Aleksey Elyutin
 *

*/

 /*TODO: Описать управление прерываниями*/
#include "module-spi.h"

#define SPI_ok 0
#define SPI_err -1

#define MAX_PENDING 10000

/*Default values*/
static uint32_t default_SCR = 255;
static uint32_t default_CPSDVSR = 254;
static module_SPI_mode_t default_MODE = SPI_MODE_0;
static module_SPI_datasize_t default_datasize = SPI_DATASIZE_8_BIT;


/*static instances*/
static volatile module_SPI_controller_t * const module_SPI_instance = (module_SPI_controller_t *) (SPI_BASE );

void module_SPI_init()
{
	module_SPI_instance->SSPCR1 = 0x0; //disable port

	_debug("SPI: Initialising SPI instance with default values: ");
	_debug("SPI: \t setting default SSPCR0_SCR  = %u", default_SCR);
	_debug("SPI: \t setting default SSPCR0_SCR  = %u", default_CPSDVSR);
	module_SPI_setDividers(default_SCR, default_CPSDVSR);
	_debug("SPI: \t \t CLK frequency:  = %u", (uint32_t) module_SPI_getFrequency());
	_debug("SPI: \t setting default MODE to %u : ",default_MODE);
	 module_SPI_setMode (default_MODE);
	 _debug("SPI: \t setting default DATASIZE to %u",default_datasize+1);
	 module_SPI_setDataSize (default_datasize);

	 _debug("SPI: \t disabling loopback mode");
	// module_SPI_setLoopBack( SPI_NORMAL_MODE);

	 SET_BIT_FIELD(module_SPI_instance->SSPCR0,SSPCR0_FRF, 0x0);



	 	 /*ONLY FOR mc7601*/
		module_GPIO_SetOutput( module_GPIO7);
		module_GPIO_SetLow( module_GPIO7);

}

int module_SPI_setDividers(uint32_t scr, uint32_t cpsdvsr)
{
	if (scr > 255)
	{
		_runtime_error("SPI: scr must be less then 255");
		return SPI_err;
	}
	else if ((cpsdvsr > 254) | (cpsdvsr%2))
	{
		_runtime_error("SPI: cpsdvsr must be less then 254 and must be even");
		return SPI_err;
	}
	else
		SET_BIT_FIELD (module_SPI_instance->SSPCR0, SSPCR0_SCR, scr);
		SET_BIT_FIELD (module_SPI_instance->SSPCPSR, SSPCPSR_CPSDVSR, cpsdvsr);
	return SPI_ok;
}

uint32_t module_SPI_getFrequency()
{
	uint32_t scr = GET_BIT_FIELD (module_SPI_instance->SSPCR0, SSPCR0_SCR);
	int32_t cpsdvsr = GET_BIT_FIELD (module_SPI_instance->SSPCPSR, SSPCPSR_CPSDVSR);
	return (uint32_t ) SYSTEM_SPI_FREQUENCY/ (cpsdvsr * (1+scr));
}

int module_SPI_setMode (module_SPI_mode_t mode)
{
	switch (mode)
	{
		case SPI_MODE_0: //	SPI mode 0 (SPO=0, SPH=0)
		{
			module_SPI_instance->SSPCR0 &= ~(SSPCR0_SPO);
			module_SPI_instance->SSPCR0 &= ~(SSPCR0_SPH);
			break;
		}
		case SPI_MODE_1: //	SPI mode 1 (SPO=0, SPH=1)
		{
			module_SPI_instance->SSPCR0 &= ~(SSPCR0_SPO);
			module_SPI_instance->SSPCR0 |= (SSPCR0_SPH);
			break;
		}
		case SPI_MODE_2://	SPI mode 2 (SPO=1, SPH=0)
		{
			module_SPI_instance->SSPCR0 |= (SSPCR0_SPO);
			module_SPI_instance->SSPCR0 &= ~(SSPCR0_SPH);
			break;
		}
		case SPI_MODE_3://	SPI mode 3 (SPO=1, SPH=1)
		{
			module_SPI_instance->SSPCR0 |= (SSPCR0_SPO);
			module_SPI_instance->SSPCR0 |= (SSPCR0_SPH);
			break;
		}
		default:
		{
			_assert("SPI: selected invalid mode value. Setting mode 0 (SPO=0, SPH=0) ");
			module_SPI_instance->SSPCR0 &= ~(SSPCR0_SPO);
			module_SPI_instance->SSPCR0 &= ~(SSPCR0_SPH);
			break;
		}
	}
	return SPI_ok;
}

int module_SPI_setDataSize (module_SPI_datasize_t datasize)
{
	if (datasize > SPI_DATASIZE_16_BIT)
	{
		_assert("SPI:invalid datasize value");
		return SPI_err;
	}
	else
	{
		SET_BIT_FIELD (module_SPI_instance->SSPCR0, SSPCR0_DSS, (uint32_t) datasize);
		return SPI_ok;
	}
}



int module_SPI_exchangeSingle(uint8_t* TxDataPointer, uint8_t* RxDataPointer)
{
	uint8_t dummy;
	uint32_t pending = 0;
	//while ( ( (module_SPI_instance->SSPSR & SSPSR_BSY) ) && (pending < MAX_PENDING) )
	//	pending++;

	//if (pending <= MAX_PENDING)
	{
	//	pending = 0;
		module_SPI_instance->SSPDR = *TxDataPointer;
	//	if ( !(module_SPI_instance->SSPCR1 & SSPCR1_SSE))
	//		module_SPI_instance->SSPCR1 |= SSPCR1_SSE; //enable port if non active
	//}
	//else
	{
	//	_assert("SPI: port is busy");
	//	return SPI_err;
	}
	if (RxDataPointer != 0x0)
	{

		//while ( ( (module_SPI_instance->SSPSR & SSPSR_RNE) == 0 ) && (pending < MAX_PENDING) )
		//	pending++;

		//if (pending <= MAX_PENDING)
		//{
		//	pending = 0;
			*RxDataPointer = module_SPI_instance->SSPDR;
		//}
		//else
		//{
		//	_assert("SPI: slave not send data");
		//	return SPI_err;
		//}
	}
	//else {
		//while ( (module_SPI_instance->SSPSR & SSPSR_RNE) != 0 ) //clean rx fifo
		//{
		//	dummy = module_SPI_instance->SSPDR;
		//}
	}


	//module_SPI_instance->SSPCR1 |= SSPCR1_SSE; //disable port
	return SPI_ok;
}

void module_SPI_sendMulti (uint8_t* TxDataPointer, uint32_t DataLen)
{
	uint32_t pending = 0, currentTxWord = 0, currentRxWord = 0;
	uint8_t dummy0 = 0,dummy1 = 0xFF;

	while ( ( (module_SPI_instance->SSPSR & SSPSR_BSY) != 0 ) && (pending < MAX_PENDING) )
	{
		dummy0 = module_SPI_instance->SSPDR;
		pending++;
	}

	if (pending >= MAX_PENDING)
	{
		_assert("SPI: port is busy");
		return; //SPI_err;
	}
	else
	{
		while ( (module_SPI_instance->SSPSR & SSPSR_RNE) != 0 ) //clean rx fifo
		{
			dummy0 = module_SPI_instance->SSPDR;
		}

		module_SPI_instance->SSPCR1 = 0x0A; //enable port
		while ((currentTxWord < DataLen) || (currentRxWord < DataLen))
		{

			while ( ( module_SPI_instance->SSPSR & SSPSR_TNF) && (currentTxWord < DataLen))
			{
				if (TxDataPointer == 0)
				{
					module_SPI_instance->SSPDR = dummy1 ;
					currentTxWord++;
				}
				else
					module_SPI_instance->SSPDR = TxDataPointer[currentTxWord++]; //try to store data to tx fifo
			}

			while  ( (module_SPI_instance->SSPSR & SSPSR_RNE)  )
			{
				//if (RxDataPointer == 0)
				//{
					dummy0 = module_SPI_instance->SSPDR;
					currentRxWord++;
				//}
				//else
				//	RxDataPointer[currentRxWord++] = module_SPI_instance->SSPDR;
			}

		}
	}
	module_SPI_instance->SSPCR1 = 0x0; //disable port

	//_printf("SPI: %3i words sent, %3u words recieved",currentTxWord,currentRxWord);

	//return SPI_ok;
}

/*int module_SPI_readFromFLASH (uint32_t startAddr, uint32_t len, uint8_t* RxDataPointer )
{
   typedef struct  {
    	uint8_t command_id;
    	uint8_t addr2     ;
    	uint8_t addr1     ;
    	uint8_t addr0     ;
    	uint8_t dummy     ;
    } fastRead_command_t ;

    fastRead_command_t frc = {
		0x0B,
		(startAddr << 16) & (0xFF),
		(startAddr  << 8) & (0xFF),
		startAddr & (0xFF),
		0xFF
	};

    uint8_t * frc_8 = (uint8_t *) &frc;

    int fr_size = 5;// sizeof(fastRead_command);
    int fr_sent_bytes = 0, fr_rcv_bytes = 0;


	uint32_t DataLen = fr_size + len;

	uint32_t pending = 0, currentTxWord = 0, currentRxWord = 0;
	uint8_t dummy0 = 0,dummy1 = 0xFF;

	while ( ( (module_SPI_instance->SSPSR & SSPSR_BSY) != 0 ) && (pending < MAX_PENDING) )
	{
		dummy0 = module_SPI_instance->SSPDR;
		pending++;
	}

	if (pending >= MAX_PENDING)
	{
		_assert("SPI: port is busy");
		return SPI_err;
	}
	else
	{
		while ( (module_SPI_instance->SSPSR & SSPSR_RNE) != 0 ) //clean rx fifo
		{
			dummy0 = module_SPI_instance->SSPDR;
		}

		module_SPI_instance->SSPCR1 = 0x0A; //enable port
		while ((currentTxWord < DataLen) || (currentRxWord < DataLen))
		{

			while ( ( module_SPI_instance->SSPSR & SSPSR_TNF) && (currentTxWord < DataLen))
			{
					if (currentTxWord < fr_size)
					{
						module_SPI_instance->SSPDR = frc_8[currentTxWord++];
						fr_sent_bytes++;
					}
					else
					{
						module_SPI_instance->SSPDR = dummy1 ;
						currentTxWord++;
					}
			}

			while  ( (module_SPI_instance->SSPSR & SSPSR_RNE)  )
			{
				if (currentRxWord < fr_size)
				{
					dummy0 = module_SPI_instance->SSPDR;
					currentRxWord++;
					//fr_rcv_bytes++;
				}
				else
					RxDataPointer[currentRxWord++] = module_SPI_instance->SSPDR;
			}

		}
	}
	module_SPI_instance->SSPCR1 = 0x0; //disable port
	return SPI_ok;

}

*/

int module_SPI_exchangeMulti (uint8_t* TxDataPointer, uint8_t* RxDataPointer, uint32_t DataLen)
{
	uint32_t pending = 0, currentWord = 0;
	uint8_t dummy;

	while ( ( (module_SPI_instance->SSPSR & SSPSR_BSY) != 0 ) && (pending < MAX_PENDING) )
		pending++;

	if (pending >= MAX_PENDING)
	{
		_assert("SPI: port is busy");
		return SPI_err;
	}
	else
	{
		while ( (module_SPI_instance->SSPSR & SSPSR_RNE) != 0 ) //clean rx fifo
		{
			dummy = module_SPI_instance->SSPDR;
		}

		module_SPI_instance->SSPCR1 = 0x2; //enable port

		pending = 0;
		for (currentWord = 0; currentWord < DataLen ;currentWord++)
		{
			while ( ( (module_SPI_instance->SSPSR & SSPSR_TNF) == 0 ) && (pending < MAX_PENDING) )
					pending++; //make sure that tx fifo not full
			if (pending < MAX_PENDING)
			{
				module_SPI_instance->SSPDR = TxDataPointer[currentWord]; //try to store data to tx fifo
				//if ( !(module_SPI_instance->SSPCR1 & SSPCR1_SSE))
				//module_SPI_instance->SSPCR1 = 0xA; //enable port if non active

				pending = 0;
				if (RxDataPointer != 0x0)
				{
					while ( ( (module_SPI_instance->SSPSR & SSPSR_RNE) == 0 ) && (pending < MAX_PENDING) )
							pending++;

						if (pending < MAX_PENDING)
						{
							pending = 0;
							RxDataPointer[currentWord] = (uint8_t) module_SPI_instance->SSPDR;
							//try to read data from rx fifo
						}
						else
						{
							_assert("SPI: slave not send data");
							return SPI_err;
						}
				}
				else
				{
					while ( (module_SPI_instance->SSPSR & SSPSR_RNE) != 0 ) //clean rx fifo
					{
						dummy = module_SPI_instance->SSPDR;
					}
				}

			}
			else
			{
				_assert("SPI: tx fifo full");
				return SPI_err;
			}


		}
	}
	module_SPI_instance->SSPCR1 = 0x0; //disable port
	//module_SPI_instance->SSPCR1 |= SSPCR1_SSE; //disable port
	return SPI_ok;
}


void module_SPI_enableCS()
{

	module_SPI_instance->SSPCR1 = 0x0A; //enable port
}
void module_SPI_disableCS()
{
	module_SPI_instance->SSPCR1 = 0x0; //disable port
}

void module_SPI_setDevice( module_SPI_slaveNumber_t dev)
{
	module_GPIO_SPI_setDevice((module_GPIO_SPICS_numDevice_t) dev);
}

void module_SPI_setLoopBack(module_SPI_loopBack_t lbm)
{
	if (lbm == SPI_LOOPBACK_MODE)
	{
		module_SPI_instance->SSPCR1 |= SSPCR1_LBM;
		_assert("SPI: LOOPBACK ENABLED !");
	}
	else
		module_SPI_instance->SSPCR1 &= ~SSPCR1_LBM;

}


int module_SPI_setDmaStatus(module_SPI_DMAstatus_t dmaSt)
{
	if (dmaSt > SPI_RxDMA_disabled)
	{
		_assert("SPI: illegal control dma parameter");
		return SPI_err;
	}

	switch (dmaSt)
		{
			case SPI_TxDMA_enabled:
			{
				module_SPI_instance->SSPDMACR |= SSPDMACR_TXDMAE;
				break;
			}
			case SPI_TxDMA_disabled:
			{
				module_SPI_instance->SSPDMACR &= ~(SSPDMACR_TXDMAE);
				break;
			}
			case SPI_RxDMA_enabled:
			{
				module_SPI_instance->SSPDMACR |= SSPDMACR_RXDMAE;
				break;
			}
			case SPI_RxDMA_disabled:
			{
				module_SPI_instance->SSPDMACR &= ~(SSPDMACR_RXDMAE);
				break;
			}
		}

	return SPI_ok;
}

