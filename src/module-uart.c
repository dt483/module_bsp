#include "module-uart.h"

//#include "module-dit.h"


/*static instances*/
//static volatile module_UART_controller_t * module_UART_console_instance;
//static volatile module_UART_controller_t * const module_UART1_instance = (module_UART_controller_t *) (UART1_BASE );

/*Default values*/
static module_UART_stopBitsNum_t 	default_stopBits 		= UART_StopBits_1;
static module_UART_evenParity_t 	default_parity 			= UART_Parity_NoControl;
static module_UART_wordLength_t 	default_wordLength 		= UART_wordLength_8bit;
static module_UART_speedDivider_t 	default_speedDivider 	= UART_Speed_115200;


module_UART_controller_t* module_UART_getInstance(module_UART_uartNum_t num)
{
    if (num == UART_0) return (module_UART_controller_t*) UART0_BASE;
    else if (num == UART_1) return (module_UART_controller_t*) UART1_BASE;
    else {
    	_assert("UART: UART number not valid");
    	return (module_UART_controller_t*) 0;
    }
}

void module_UART_init (	volatile module_UART_controller_t * uartInstance,
						module_UART_speedDivider_t speedDivider,
						module_UART_wordLength_t wordLength,
						module_UART_evenParity_t parity,
						module_UART_stopBitsNum_t stopBits)
{
	uartInstance->LCR |= LCR_DLAB; //DLAB >> 1 DLL DLM enabled

	//module_UART_speedDivider_t divider = UART_Speed_115200;// 115200;
	SET_BIT_FIELD (uartInstance->IER_DLM, DLM_DL, (0xFF & (speedDivider >> 8))); //MSB part of divider
	SET_BIT_FIELD (uartInstance->RFR_TFR_DLL, DLL_DL,(0xFF & speedDivider)); //LSB part of divider

	uartInstance->LCR &= ~(LCR_DLAB);

	// length setting


		// parity setting
		if (parity == UART_Parity_NoControl)
			uartInstance->LCR &= ~(LCR_PEN); //no parity case
		else if (UART_Parity_OddControl)
		{
			uartInstance->LCR |= LCR_PEN; //no parity case
			uartInstance->LCR &= ~(LCR_EPS); //odd parity case
		}
		else if (UART_Parity_EvenControl)
		{
			uartInstance->LCR |= LCR_PEN; //no parity case
			uartInstance->LCR |= LCR_EPS; //even parity case
		}

		SET_BIT_FIELD (uartInstance->LCR, LCR_WLS, UART_wordLength_8bit);



	uint32_t fcr_tmp = 0x0UL;

	fcr_tmp = (FCR_TxF_RST) | (FCR_RxF_RST); //reset Tx Rx fifo
	//uartInstance->IIR_FCR = ~(FCR_DMA_MODE); //DMAC single mode
	//SET_BIT_FIELD (fcr_tmp, FCR_RCVR, 0xC6); //set 10 bytes Rx fifo depth
	//SET_BIT_FIELD (fcr_tmp, FCR_RCVR, 0x1); //set 4 byte Rx fifo depth
	uartInstance->IIR_FCR = fcr_tmp;

	fcr_tmp = 0x0UL;
	SET_BIT_FIELD (fcr_tmp, FCR_RCVR, 0x2);
	uartInstance->IIR_FCR = fcr_tmp;


	uartInstance->IER_DLM &= ~IER_EDSSI;
	uartInstance->IER_DLM &= ~IER_ELSI;
	uartInstance->IER_DLM &= ~IER_ETBEI;
	uartInstance->IER_DLM |= IER_ERBFI;


	module_UART_clearFIFO (uartInstance);


}

void module_UART_defaultInit (volatile module_UART_controller_t * uartInstance)
{
	module_UART_init(
			uartInstance,
			default_speedDivider,
			default_wordLength,
			default_parity,
			default_stopBits
			);
}

void module_UART_send (volatile module_UART_controller_t * uartInstance, char data)
{
    volatile uint32_t dummy_counter = 0;
    while ( !(uartInstance->LSR & LSR_TEMT) )
    {
        dummy_counter++;
    }
    dummy_counter = 0;

    uartInstance->RFR_TFR_DLL = data;

    while ( !(uartInstance->LSR & LSR_TEMT) )
    {
       // assert_blink();
        dummy_counter++;
    }
    uartInstance->IIR_FCR |= FCR_TxF_RST;
}

int module_UART_recieve (volatile module_UART_controller_t * uartInstance, char * data, int wait_cycles)
{

	int uart_no_data_counter = 0;
    while (!(uartInstance->LSR & 0x1) | (uart_no_data_counter > wait_cycles) ) uart_no_data_counter++;
	if (wait_cycles > 100000)
	{
		_assert ("SERIAL: UART rx byte wait time exceeded");
		return 1;
	}
	*data = uartInstance->RFR_TFR_DLL;

	return 0;
}


void module_UART_clearFIFO (volatile module_UART_controller_t * uartInstance)
{
	volatile uint32_t dummy =0x0UL;
	dummy= uartInstance->LSR ;
	while (uartInstance->LSR & LSR_DR)
		dummy = uartInstance->RFR_TFR_DLL; //clean rx fifo

	dummy++;
}




//char module_UART_recieve (module_uart_t *uart_descriptor, uint32_t timeout)
//{
//    char _inbyte = 0;
//    uint32_t tick_timeout = 0;
//    tick_timeout = timeout * ((TIMER_TICKRATE)/1000);
//    //printf ("DBG: module_UART_recieve: tick_timeout =  %u \n\r", (unsigned int) tick_timeout);
//
//    //volatile uint32_t _counter = 0;
//    volatile uint32_t timestamp1=0, timestamp2=0;;
//    timestamp1 =  module_Systimer_stamp();
//    //printf ("DBG: module_UART_recieve: timestamp1 =  %u \n\r",(unsigned int) timestamp1);
//    while ( !((read_reg(LSR, uart_descriptor->base) & DR) == DR) )
//    {
//        //_counter++;
//
//        //module_Systimer_WaitMilSeconds(1);
//        timestamp2 = module_Systimer_stamp();
//        //printf ("DBG: module_UART_recieve: timestamp2 =  %u \n\r", (unsigned int)timestamp2);
//        //printf ("DBG: module_UART_recieve: timestamp2-timestamp1 =  %u \n\r", (unsigned int)timestamp2-timestamp1);
//        if (( timestamp1-timestamp2) >= tick_timeout)
//        {
//            //printf("ERR: module_UART_recieve: timed out: %u > %u \n\r",timestamp2-timestamp1,  tick_timeout);
//            return '\0';
//        }
//    }
//    _inbyte = (char) read_reg(RFR, uart_descriptor->base);
//   /* if ((_counter < 1000 )) _inbyte = (char) read_reg(RFR, uart_descriptor->base);
//    else
//    {
//        _inbyte = '@';
//
//    }*/
//    write_reg(RxF_RST, FCR, uart_descriptor->base);
//
//    return _inbyte;
//}






