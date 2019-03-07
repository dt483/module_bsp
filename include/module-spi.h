/*
 * module-spi.h
 *
 *  Created on: 4 февр. 2019 г.
 *      Author: aleksey
 */

#ifndef MODULE_SPI_H_
#define MODULE_SPI_H_

#include <module-base.h>
#include <module-gpio.h>
#include <stdint.h>


#define module_SPI_NoRecieve (uint8_t*)0x0

/*TODO: системная частота должна определяться блоком CRG11
 * Пока остоавляем по умолчанию 81
 * УТОЧНИТЬ ЧАСТОТЫ*/

typedef  module_GPIO_SPICS_numDevice_t module_SPI_slaveNumber_t;
/*typedef enum {
	module_GPIO_SPI_CS0 = 0,
	module_GPIO_SPI_CS1,
	module_GPIO_SPI_CS2,
	module_GPIO_SPI_CS3,
	module_GPIO_SPI_CS4,
	module_GPIO_SPI_CS5,
	module_GPIO_SPI_CS6,
	module_GPIO_SPI_CS7
} module_GPIO_SPICS_numDevice_t;*/

#define SYSTEM_SPI_FREQUENCY 81920000 //Hz

typedef struct {
	uint32_t	SSPCR0	;	/*< Регистр управления 0                                 */
	uint32_t	SSPCR1  ;   /*< Регистр управления 1                                 */
	uint32_t	SSPDR	;   /*< Регистр данных.                                      */
	uint32_t	SSPSR 	;	/*< Регистр состояния.                                   */
	uint32_t	SSPCPSR ;	/*< Регистр множителя синхросигнала.                     */
	uint32_t	SSPIMSC	;   /*< Регистр снятия или установки маски прерываний        */
	uint32_t	SSPRIS	;   /*< Регистр состояния прерывания до наложения маски.     */
	uint32_t	SSPMIS	;   /*< Регистр состояния прерывания после наложения маски.  */
	uint32_t	SSPICR	;   /*< Регистр сброса прерывания                            */
	uint32_t  	SSPDMACR;	/*< Регистр управления DMA                               */

} module_SPI_controller_t;

/* -- Регистр управления. SSPCR0 */
#define SSPCR0_SCR_MASK		(0xFF	)	/*< Serial Clock Rate. Значение SCR используется для определения */
#define SSPCR0_SCR_OFFS		(0x8	)	/*< скорости приема и передачи блока SSP.                        */
#define SSPCR0_SPH			(1 << 7	)   /*< Фаза сигнала SSPCLKOUT                                       */
#define SSPCR0_SPO			(1 << 6	)   /*< Полярность SSPCLKOUT                                         */
#define SSPCR0_FRF_MASK		(0x3	)   /*< Формат данных: 00 – Motorola SPI формат данных               */
#define	SSPCR0_FRF_OFFS		(0x4	)   /*                                                               */
#define SSPCR0_DSS_MASK		(0xF	)   /*< Выбор размера данных                                         */
#define SSPCR0_DSS_OFFS		(0x0	)   /*	определен типом  module_SPI_DATASIZE_t                       */

typedef enum
{
	SPI_DATASIZE_4_BIT	= 0x3,	//< 	0011 – 4 - битные данные
	SPI_DATASIZE_5_BIT,			//< 	0100 – 5 - битные данные
	SPI_DATASIZE_6_BIT,			//< 	0101 – 6 - битные данные
	SPI_DATASIZE_7_BIT,			//< 	0110 – 7 - битные данные
	SPI_DATASIZE_8_BIT,			//< 	0111 – 8 - битные данные
	SPI_DATASIZE_9_BIT,			//< 	1000 – 9 - битные данные
	SPI_DATASIZE_10_BIT,		//< 	1001 – 10-битные данные
	SPI_DATASIZE_11_BIT,		//< 	1010 – 11-битные данные
	SPI_DATASIZE_12_BIT,		//< 	1011 – 12-битные данные
	SPI_DATASIZE_13_BIT,		//< 	1100 – 13-битные данные
	SPI_DATASIZE_14_BIT,		//< 	1101 – 14-битные данные
	SPI_DATASIZE_15_BIT,		//< 	1110 – 15-битные данные
	SPI_DATASIZE_16_BIT			//< 	1111 – 16-битные данные
} module_SPI_datasize_t;

typedef enum
{
	SPI_MODE_0 = 0,	/*< 	SPI mode 0 (SPO=0, SPH=0)   */
	SPI_MODE_1,		/*< 	SPI mode 1 (SPO=0, SPH=1)   */
	SPI_MODE_2,		/*< 	SPI mode 2 (SPO=1, SPH=0)   */
	SPI_MODE_3		/*< 	SPI mode 3 (SPO=1, SPH=1)   */
}  module_SPI_mode_t;


/* -- Регистр управления. SSPCR1 */
#define SSPCR1_SOD		(1 << 3	)   /*< Slave-mode output disable. Бит используется только при работе      */
 	 	 	 	 	 	 			/* 	 SSP в режиме приема.                                              */
#define SSPCR1_MS		(1 << 2	)   /*< Master or Slave mode Select. Этот бит должен быть всегда равен 0   */
#define SSPCR1_SSE		(1 << 1	)   /*< Synchronous Serial Port Enable. Управление портом:                 */
									/*	0 – порт неактивен 1 – порт активен                                */
#define SSPCR1_LBM		(1 << 0	)   /*< Loop Back Mode. Управление тестовым кольцевым режимом:             */
									/*	0 – нормальный режим функционирования порта                        */
									/*		1 – выходы передающего сдвигового регистра соединены с входами */
									/*		принимающего сдвигового регистра                               */
typedef enum
{
	SPI_NORMAL_MODE = 0,	/*< 	SPI mode 0 (SPO=0, SPH=0)   */
	SPI_LOOPBACK_MODE		/*< 	SPI mode 1 (SPO=0, SPH=1)   */
}  module_SPI_loopBack_t;


/* -- Регистр состояния. SSPSR*/
#define SSPSR_BSY		(1 << 4	)	/*<  Флаг занятости порта SSP 0 = SSP простаивает                        */
									/*		1 = SSP передает и/или принимает пакет данных	               */
#define SSPSR_RFF		(1 << 3	)   /*<  Сигнал полноты принимающего FIFO: 0 = принимающее FIFO не полное    */
							    	/*		1 = принимающее FIFO полное                                    */
#define SSPSR_RNE		(1 << 2	)   /*<  Сигнал не пустоты принимающего FIFO: 0 = принимающее FIFO пустое    */
							    	/*		1 = принимающее FIFO не пустое                                 */
#define SSPSR_TNF		(1 << 1	)   /*<  Сигнал не полноты передающего FIFO: 0 = передающее FIFO полное      */
							    	/*		1 = передающее FIFO не полное                                  */
#define SSPSR_TFE		(1 << 0	)   /*<  Сигнал пустоты передающего FIFO: 0 = передающее FIFO не пустое      */
							    	/*		1 = передающее FIFO пустое                                     */


/* -- Регистр множителя синхросигнала. SSPCPSR */
#define SSPCPSR_CPSDVSR_MASK	(0xFF) /*Clock Prescale Devisor. Значение делителя синхросигнала. Должен	*/
#define SSPCPSR_CPSDVSR_OFFS	(0x0)  /* 	принимать четные значения от 2 до 254, в зависимости от    	    */
							   	   	   /*	частоты сигнала SSPCLK. Младшие значащие биты всегда при-       */
							   	   	   /* 	нимают значение нуль при чтении                                 */


/* -- Регистр снятия или установки маски прерываний. SSPIMSC  */
#define SSPIMSC_TXIM	(1 << 3)  /*< Transmit Interrupt mask. Маска прерывания передающего FIFO(SSPTXINTR):    */
								  /*  0 = Прерывание по заполнению TxFIFO более чем на половину замаскировано   */
								  /*  1 = Прерывание по заполнению TxFIFO более чем на половину не маскировано  */
#define SSPIMSC_RXIM	(1 << 2)  /*< Receive Interrupt Mask. Маска прерывания принимающего FIFO(SSPRXINTR):    */
							   	  /*  0 = Прерывание по заполнению Rx FIFO менее чем на половину замаскировано  */
								  /*  1 = Прерывание по заполнению Rx FIFO менее чем на половину не маскировано */
#define SSPIMSC_RTIM	(1 << 1)  /*< Receive Timeout Interrupt Mask. Маска прерывания по превышению            */
							   	  /*  timeout-периода чтения из приемного FIFO (SSPRTINTR).                     */
								  /*  0 = прерывание замаскировано 1 = прерывание не маскировано                */
#define SSPIMSC_RORIM	(1 << 0)  /*< Receive Overrun Interrupt Mask. Маска прерывания по                       */
								  /*  попытке записи в полное Rx FIFO:                                          */
                                  /*  0 = прерывание замаскировано 1 = прерывание не маскировано                */

/* -- Регистр состояния прерывания после наложения маски. SSPMIS  */
#define SSPMIS_TXMIS	(1 << 3)	/*< Определяет состояние прерывания SSPTXINTR после наложения маски         */
#define SSPMIS_RXMIS	(1 << 2)    /*< Определяет состояние прерывания SSPRXINTR после наложения маски         */
#define SSPMIS_RTMIS	(1 << 1)    /*< Определяет состояние прерывания SSPRTINTR после наложения маски         */
#define SSPMIS_RORMIS	(1 << 0)    /*< Определяет состояние прерывания SSPRORINTR после наложения маски        */


/* -- Регистр сброса прерывания. SSPICR */
#define SSPMIS_RTIC		/*Снимает прерывание SSPRTINTR          */
#define SSPMIS_RORIC	/*Снимает прерывание SSPRORINTR         */

/* -- Регистр управления DMA. SSPDMACR */
#define SSPDMACR_TXDMAE	(1 << 1)	/*< Если бит установлен в 1, то DMA-доступ к передающему FIFO разрешен.*/
#define SSPDMACR_RXDMAE	(1 << 0)	/*< Если бит установлен в 1, то DMA-доступ к принимающему FIFO разрешен */
typedef enum
{
	SPI_TxDMA_enabled = 0,	/*< 	SPI mode 0 (SPO=0, SPH=0)   */
	SPI_TxDMA_disabled = 1,
	SPI_RxDMA_enabled = 2,
	SPI_RxDMA_disabled = 3	/*< 	SPI mode 1 (SPO=0, SPH=1)   */
}  module_SPI_DMAstatus_t;





/*Function prototypes*/
// void module_GPIO_SPI_setDevice( module_GPIO_SPICS_numDevice_t dev); <- from module-gpio.hm
void module_SPI_init();
int module_SPI_setDividers(uint32_t scr, uint32_t cpsdvsr);
uint32_t module_SPI_getFrequency();
int module_SPI_setMode (module_SPI_mode_t mode);
int module_SPI_setDataSize (module_SPI_datasize_t datasize);
int module_SPI_exchangeSingle(uint8_t* TxDataPointer, uint8_t* RxDataPointer);
int module_SPI_exchangeMulti (uint8_t* TxDataPointer, uint8_t* RxDataPointer, uint32_t DataLen);
void module_SPI_setLoopBack(module_SPI_loopBack_t lbm);
void module_SPI_setDevice( module_SPI_slaveNumber_t dev);



#endif /* MODULE_LIB_MODULE_SPI_H_ */
