/*
 * module-dmac.h
 *
 *  Created on: 13 февр. 2019 г.
 *      Author: aleksey
 */


#ifndef MODULE_BSP_INCLUDE_MODULE_DMAC_H_
#define MODULE_BSP_INCLUDE_MODULE_DMAC_H_

#include "module-base.h"
#include <stdint.h>



typedef struct {
	uint32_t*	SrcAdr;			//Base + 00		/* Адрес источника */
	uint32_t*	DstAdr;			//Base + 04		/* Адрес приемника */
	uint32_t	TrSize;			//Base + 08		/* Число передаваемых байт */
	uint32_t	Enable;			//Base + 0C		/* Запуск/останов канала */
	uint32_t	Interrupt;		//Base + 10		/* Состояние прерывания */
	uint32_t	Busy;			//Base + 14		/* Занятость канала */
	uint32_t	PackType;		//Base + 18		/* Тип упаковки данных */
	uint32_t	PerHandsh;		//Base + 1C		/* Тип ПДП интерфейса */
} module_DMAC_channel_t;

typedef struct {
	module_DMAC_channel_t *	SPI_MemToPeri		;
	module_DMAC_channel_t *	USB_MemToPeri		;
	module_DMAC_channel_t *	UART0_MemToPeri		;
	module_DMAC_channel_t *	UART1_MemToPeri		;
	module_DMAC_channel_t *	SPI_PeriToMem	;
	module_DMAC_channel_t *	USB_PeriToMem  	;
	module_DMAC_channel_t *	UART0_PeriToMem	;
	module_DMAC_channel_t *	UART1_PeriToMem	;
} module_DMAC_controller_t;

#define CHANNEL_FtS1 ( DMAC_BASE + 0x000 ) /*< канал SPI     память-периферия            */
#define CHANNEL_FtS2 ( DMAC_BASE + 0x040 ) /*< канал USB     память-периферия            */
#define CHANNEL_FtS3 ( DMAC_BASE + 0x080 ) /*< канал UART0   память-периферия            */
#define CHANNEL_FtS4 ( DMAC_BASE + 0x0C0 ) /*< канал UART1   память-периферия            */
#define CHANNEL_StF1 ( DMAC_BASE + 0x200 ) /*< канал SPI     периферия–память            */
#define CHANNEL_StF2 ( DMAC_BASE + 0x240 ) /*< канал USB     периферия–память            */
#define CHANNEL_StF3 ( DMAC_BASE + 0x280 ) /*< канал UART0   периферия–память            */
#define CHANNEL_StF4 ( DMAC_BASE + 0x2C0 ) /*< канал UART1   периферия–память            */


typedef enum {
	DMAC_CHANNEL_SPI_MemToPeri	=0		,
	DMAC_CHANNEL_USB_MemToPeri			,
	DMAC_CHANNEL_UART0_MemToPeri		,
	DMAC_CHANNEL_UART1_MemToPeri		,
	DMAC_CHANNEL_SPI_PeriToMem		,
	DMAC_CHANNEL_USB_PeriToMem  	,
	DMAC_CHANNEL_UART0_PeriToMem	,
	DMAC_CHANNEL_UART1_PeriToMem
}  module_DMAC_chNum_t;


typedef enum {
	DMAC_Pack_8bit	= 0,
	DMAC_Pack_16bit,
	DMAC_Pack_32bit,
}  module_DMAC_packType_t;


typedef struct {
	module_DMAC_chNum_t channelNumber;
	uint32_t* srcAddr;
	uint32_t* dstAddr;
	uint32_t byteNumber;
	module_DMAC_packType_t PackType;
} module_DMAC_channelConfig_t;


/* Function prototypes */
module_DMAC_channel_t* module_DMAC_getChannelInstance(module_DMAC_chNum_t num);
int module_DMAC_setupChannel(module_DMAC_channelConfig_t config);
int module_DMAC_startChannel(module_DMAC_chNum_t num);
int module_DMAC_stopChannel(module_DMAC_chNum_t num);
int module_DMAC_getInterruptStatus(module_DMAC_chNum_t num);
int module_DMAC_clearInterrupt(module_DMAC_chNum_t num);
module_DMAC_chNum_t module_DMAC_getInterruptSource();


#endif /* MODULE_BSP_INCLUDE_MODULE_DMAC_H_ */
