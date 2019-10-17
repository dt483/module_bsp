#include "module-uart.h"

//#include "module-dit.h"


/*Default values*/
//static module_UART_stopBitsNum_t 	default_stopBits 		= UART_StopBits_1;
//static module_UART_evenParity_t 	default_parity 			= UART_Parity_NoControl;
//static module_UART_wordLength_t 	default_wordLength 		= UART_wordLength_8bit;
//static module_UART_baudRate_t 	default_speedDivider 	= UART_Baud_115200;



static volatile module_UART_controller_t * const module_UART0_devInstance = (module_UART_controller_t *) (UART0_BASE );
static volatile module_UART_controller_t * const module_UART1_devInstance = (module_UART_controller_t *) (UART1_BASE );

void module_UART_setConfig ( module_UART_number_t uartN, module_UART_config_t * config)
{
	volatile module_UART_controller_t * uartInstance;
	if (uartN)
		uartInstance = module_UART1_devInstance;
	else
		uartInstance = module_UART0_devInstance;

	uartInstance->LCR |= LCR_DLAB; //DLAB >> 1 DLL DLM enabled

	SET_BIT_FIELD (uartInstance->IER_DLM, DLM_DL, (0xFF & (config->baudRate >> 8))); //MSB part of divider
	SET_BIT_FIELD (uartInstance->RFR_TFR_DLL, DLL_DL,(0xFF & config->baudRate)); //LSB part of divider

	uartInstance->LCR &= ~(LCR_DLAB);

	// length setting
	SET_BIT_FIELD (uartInstance->LCR, LCR_WLS, config->wordLength);


	// parity setting
	if (config->parity == UART_Parity_NoControl)
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

	module_UART_clearFIFO (uartN);

}

void module_UART_send (module_UART_number_t uartN, char data)
{

	volatile module_UART_controller_t * uartInstance;
	if (uartN)
		uartInstance = module_UART1_devInstance;
	else
		uartInstance = module_UART0_devInstance;

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

module_UART_RetCode module_UART_recieve (module_UART_number_t uartN, char * data, int wait_cycles)
{

	volatile module_UART_controller_t * uartInstance;
	if (uartN)
		uartInstance = module_UART1_devInstance;
	else
		uartInstance = module_UART0_devInstance;

	int uart_no_data_counter = 0;
    while (!(uartInstance->LSR & 0x1) | (uart_no_data_counter > wait_cycles) ) uart_no_data_counter++;
	if (wait_cycles > 100000)
	{
		_assert ("SERIAL: UART rx byte wait time exceeded");
		return UART_RETURN_FAIL;
	}
	*data = uartInstance->RFR_TFR_DLL;

	return UART_RETURN_SUCCESS;
}


void module_UART_clearFIFO (module_UART_number_t uartN)
{
	volatile module_UART_controller_t * uartInstance;
	if (uartN)
		uartInstance = module_UART1_devInstance;
	else
		uartInstance = module_UART0_devInstance;

	volatile uint32_t dummy =0x0UL;
	dummy= uartInstance->LSR ;
	while (uartInstance->LSR & LSR_DR)
		dummy = uartInstance->RFR_TFR_DLL; //clean rx fifo

	dummy++;
}





