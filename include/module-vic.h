#ifndef MODULE_VIC_H
#define MODULE_VIC_H

#include "module-armcore.h"
#include "module-base.h"
#include <stdint.h>



typedef enum {
	VIC_RETURN_FAIL = 0,
	VIC_RETURN_SUCCESS = 1
}module_VIC_RetCode;



//#define VICADDRESS-c -fmessage-length=0 -ffixed-r8 -march=armv6zk  -mtune=arm1176jzf-s		( VIC_BASE + 0xF00 )/*Адрес процедуры обработки для текущего IRQ прерывания*/

typedef struct {
	uint32_t	VICIRQSTATUS;		 /*Состояние обычных прерываний (IRQ)*/
	uint32_t	VICFIQSTATUS;		 /*Состояние быстрых прерываний (FIQ)*/
	uint32_t	VICRAWINTR;			 /*Состояние входов*/
	uint32_t	VICINTSELECT;		 /*Выбор типа прерывания(IRQ/FIQ, 1 – FIQ)*/
	uint32_t	VICINTENABLE;		 /*Маска прерываний*/
	uint32_t	VICINTENCLEAR;		 /*Обнуление регистра маски прерываний*/
	uint32_t	VICSOFTINT;			 /*Программируемые прерывания*/
	uint32_t	VICSOFTINTCLEAR;	 /*Обнуление регистра программируемых прерывания*/
	uint32_t	VICPROTECTION;		 /*Флаг защищенного режима*/
	uint32_t	VICSWPRIORITY_MASK;	 /*Маска программных приоритетов*/
	uint32_t	VICPRIORITYDAISY;	 /*Приоритет подчиненного VIC (Не используется)*/
} module_VIC_controller_t;

typedef uint32_t module_VIC_HANDLER_t;
typedef uint32_t module_VIC_PRIORITY_t;
typedef uint32_t module_VIC_VICADDRESS_t;

typedef enum {
	SWINT0		 = 0 , /*Программное прерывание 0 */
	SWINT1		 = 1 , /*Программное прерывание 1 */
	COMMRX		 = 2 , /*Debug Communication Channel Rx от ARM1176 */
	COMMTX		 = 3 , /*Debug Communication Channel Tx от ARM1176 */
	TIMINT1		 = 4 , /*Прерывание таймера 1 */
	TIMINT2		 = 5 , /*Прерывание таймера 2 */
	WDOGINT		 = 6 , /*Прерывание сторожевого таймера */
	UART0INT	 = 7 , /*Прерывание канала UART0 */
	UART1INT	 = 8 , /*Прерывание канала UART1 */
	NMC0HP		 = 9 , /*NMC0 высокоприоритетное прерывание */
	NMC1HP		 = 10, /*NMC1 высокоприоритетное прерывание */
	ITMS		 = 11, /*Прерывание NewMS от интервального таймера */
	RTCINT		 = 12, /*Прерывание от блока RTC */
	SSPINT		 = 13, /*Общее прерывание от SSP блока */
	DMACSENDINT	 = 14, /*Общее прерывание от 4 каналов FtS контроллера DMAC (каналов память->периферия) */
	DMACRECVINT	 = 15, /*Общее прерывание от 4 каналов StF контроллера DMAC (каналов периферия->память) */
	ITAUX		 = 16, /*Дополнительное прерывание от интервального таймера*/
	USBCONINT	 = 20, /*Прерывание от USB контроллера, генерируется при подключении и отключении USB кабеля*/
	USBINT		 = 21, /*Прерывание от USB контроллера */
	EXTINT0		 = 22, /*Внешнее прерывание 0 */
	EXTINT1		 = 23, /*Внешнее прерывание 1 */
	EXTINT2		 = 24, /*Внешнее прерывание 2 */
	EXTINT3		 = 25, /*Внешнее прерывание 3 */
	NMC0LP		 = 26, /*NMC0 низкоприоритетное прерывание */
	NMC1LP		 = 27, /*NMC1 низкоприоритетное прерывание */
	nPMUIRQ		 = 28, /*От System Metric блока ARM1176 */
	PLLRDYINT	 = 29, /*Готовность PLL от CRG11 */
	nVALFIQ		 = 30, /*Тестовое FIQ прерывание от ARM1176 */
	nVALIRQ		 = 31  /*Тестовое IRQ прерывание от ARM1176 */
} module_VIC_INT_NUM_t;



typedef enum {
	IRQ_NON_ACTIVE = 0,
	IRQ_ACTIVE = 1
}  module_VIC_INT_STATUS_t;

typedef enum {
	IRQ = 0,
	FIQ = 1
}  module_VIC_INT_TYPE_t;

typedef uint32_t module_VIC_HANDLER_t;
typedef uint32_t module_VIC_PRIORITY_t;
typedef uint32_t module_VIC_VICADDRESS_t;


#define VICPERIPHID0	( VIC_BASE + 0xFE0 ) /*Идентификатор периферийного устройства, разряды [7:0]*/
#define VICPERIPHID1	( VIC_BASE + 0xFE4 ) /*Идентификатор периферийного устройства, разряды [15:8]*/
#define VICPERIPHID2	( VIC_BASE + 0xFE8 ) /*Идентификатор периферийного устройства, разряды [23:16]*/
#define VICPERIPHID3	( VIC_BASE + 0xFEC ) /*Идентификатор периферийного устройства,разряды [31:24]*/

#define VICPCELLID0		( VIC_BASE + 0xFF0 ) /*Идентификатор PrimeCell,разряды [7:0]*/
#define VICPCELLID1		( VIC_BASE + 0xFF4 ) /*Идентификатор PrimeCell,разряды [15:8]*/
#define VICPCELLID2		( VIC_BASE + 0xFF8 ) /*Идентификатор PrimeCell,разряды [23:16]*/
#define VICPCELLID3		( VIC_BASE + 0xFFC ) /*Идентификатор PrimeCell,разряды [31:24]*/




/* Function prototypes */

int module_VIC_set_interrupt_handler (module_VIC_INT_NUM_t interruptNumber, void* handlerPointer);
int module_VIC_get_RAW_interrupt_status (module_VIC_INT_NUM_t interruptNumber);
module_VIC_INT_STATUS_t module_VIC_get_interrupt_status (module_VIC_INT_NUM_t interruptNumber);
int module_VIC_set_interrupt_type (module_VIC_INT_NUM_t interruptNumber, module_VIC_INT_TYPE_t type);
int module_VIC_enable_interrupt (module_VIC_INT_NUM_t interruptNumber);
int module_VIC_disable_interrupt (module_VIC_INT_NUM_t interruptNumber);
int module_VIC_invoke_soft_interrupt (module_VIC_INT_NUM_t interruptNumber);
int module_VIC_clear_soft_interrupt (module_VIC_INT_NUM_t interruptNumber);
void module_VIC_finishHandling ();





#endif








