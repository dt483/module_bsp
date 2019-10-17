/*
 * module-spi.c
 *
 *      Author: Aleksey Elyutin
 *
*/

 /*TODO: Описать управление прерываниями*/
#include "module-spi.h"

#define MAX_PENDING 10000


/*static instances*/
static volatile module_SPI_controller_t * const module_SPI_devInstance = (module_SPI_controller_t *) (SPI_BASE );

uint32_t module_SPI_getFrequency()
{
	uint32_t freq = 0;
	uint32_t scr = GET_BIT_FIELD (module_SPI_devInstance->SSPCR0, SSPCR0_SCR);
	int32_t cpsdvsr = GET_BIT_FIELD (module_SPI_devInstance->SSPCPSR, SSPCPSR_CPSDVSR);
	freq =  SYSTEM_SPI_FREQUENCY/ (cpsdvsr * (1+scr));
	return freq;
}

static module_SPI_RetCode module_SPI_setDividers(uint32_t scr, uint32_t cpsdvsr)
{
	if (scr > 255)
	{
		_runtime_error("SPI: scr must be less then 255");
		return SPI_RETURN_FAIL;
	}
	else if ((cpsdvsr > 254) | (cpsdvsr%2))
	{
		_runtime_error("SPI: cpsdvsr must be less then 254 and must be even");
		return SPI_RETURN_FAIL;
	}
	else
		SET_BIT_FIELD (module_SPI_devInstance->SSPCR0, SSPCR0_SCR, scr);
		SET_BIT_FIELD (module_SPI_devInstance->SSPCPSR, SSPCPSR_CPSDVSR, cpsdvsr);
	return SPI_RETURN_SUCCESS;
}

static void module_SPI_setMode (module_SPI_mode_t mode)
{
	switch (mode)
	{
		case SPI_MODE_0: //	SPI mode 0 (SPO=0, SPH=0)
		{
			module_SPI_devInstance->SSPCR0 &= ~(SSPCR0_SPO);
			module_SPI_devInstance->SSPCR0 &= ~(SSPCR0_SPH);
			break;
		}
		case SPI_MODE_1: //	SPI mode 1 (SPO=0, SPH=1)
		{
			module_SPI_devInstance->SSPCR0 &= ~(SSPCR0_SPO);
			module_SPI_devInstance->SSPCR0 |= (SSPCR0_SPH);
			break;
		}
		case SPI_MODE_2://	SPI mode 2 (SPO=1, SPH=0)
		{
			module_SPI_devInstance->SSPCR0 |= (SSPCR0_SPO);
			module_SPI_devInstance->SSPCR0 &= ~(SSPCR0_SPH);
			break;
		}
		case SPI_MODE_3://	SPI mode 3 (SPO=1, SPH=1)
		{
			module_SPI_devInstance->SSPCR0 |= (SSPCR0_SPO);
			module_SPI_devInstance->SSPCR0 |= (SSPCR0_SPH);
			break;
		}
		default:
		{
			_assert("SPI: selected invalid mode value. Setting mode 0 (SPO=0, SPH=0) ");
			module_SPI_devInstance->SSPCR0 &= ~(SSPCR0_SPO);
			module_SPI_devInstance->SSPCR0 &= ~(SSPCR0_SPH);
			break;
		}
	}

}

module_SPI_RetCode module_SPI_setConfig (module_SPI_config_t * config)
{
	module_SPI_devInstance->SSPCR1 = 0x0; //disable port

	if (module_SPI_setDividers(config->scr_div, config->cps_div) == SPI_RETURN_FAIL) return SPI_RETURN_FAIL;
/*#ifdef DEBUG
	_debug("SPI: Calculated CLK frequency:  = %f", (double) module_SPI_getFrequency());
#endif*/

	module_SPI_setMode (config->mode);

	SET_BIT_FIELD (module_SPI_devInstance->SSPCR0, SSPCR0_DSS, (uint32_t) config->datasize);

	module_GPIO_SPI_setDevice(config->slave_number);

	module_SPI_devInstance->SSPDMACR = SSPDMACR_TXDMAE;

	return SPI_RETURN_SUCCESS;
}

module_SPI_RetCode module_SPI_trancieve (uint8_t * txDataPointer, uint8_t * rxDataPointer, uint32_t DataLen)
{
	uint32_t pending = 0, currentTxWord = 0, currentRxWord = 0;
	uint8_t dummy0 = 0,dummy1 = 0xFF;


	while ( ( (module_SPI_devInstance->SSPSR & SSPSR_BSY) != 0 ) && (pending < MAX_PENDING) )
	{
		dummy0 = module_SPI_devInstance->SSPDR;
		pending++;
	}

	if (pending >= MAX_PENDING)
	{
		_assert("SPI: port is busy");
		return SPI_RETURN_FAIL;
	}
	else
	{
		while ( (module_SPI_devInstance->SSPSR & SSPSR_RNE) != 0 ) //clean rx fifo
		{
			dummy0 = module_SPI_devInstance->SSPDR;
		}

		module_SPI_devInstance->SSPCR1 = 0x0A; //enable port
		while ((currentTxWord < DataLen) || (currentRxWord < DataLen))
		{

			while ( ( module_SPI_devInstance->SSPSR & SSPSR_TNF) && (currentTxWord < DataLen))
			{
				if (txDataPointer == 0)
				{
					module_SPI_devInstance->SSPDR = dummy1 ;
					currentTxWord++;
				}
				else
					module_SPI_devInstance->SSPDR = txDataPointer[currentTxWord++]; //try to store data to tx fifo
			}

			while  ( (module_SPI_devInstance->SSPSR & SSPSR_RNE)  )
			{
				if (rxDataPointer == 0)
				{
					dummy0 = module_SPI_devInstance->SSPDR;
					currentRxWord++;
				}
				else
					rxDataPointer[currentRxWord++] = module_SPI_devInstance->SSPDR;
			}

		}
	}
	module_SPI_devInstance->SSPCR1 = 0x0; //disable port

	//_printf("SPI: %3i words sent, %3u words recieved",currentTxWord,currentRxWord);

	return SPI_RETURN_SUCCESS;
}

/*int module_SPI_readFromFLASH (uint32_t startAddr, uint32_t len, uint8_t* rxDataPointer )
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
		return SPI_RETURN_SUCCESS;
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
					rxDataPointer[currentRxWord++] = module_SPI_instance->SSPDR;
			}

		}
	}
	module_SPI_instance->SSPCR1 = 0x0; //disable port
	return SPI_ok;

}
*/
void module_SPI_setLoopBack(module_SPI_loopBack_t lbm)
{
	if (lbm == SPI_LOOPBACK_MODE)
	{
		module_SPI_devInstance->SSPCR1 |= SSPCR1_LBM;
		_assert("SPI: LOOPBACK ENABLED !");
	}
	else
		module_SPI_devInstance->SSPCR1 &= ~SSPCR1_LBM;
}



