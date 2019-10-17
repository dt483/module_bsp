


#ifndef MODULE_UART_H_
#define MODULE_UART_H_

#include "module-base.h"
#include "stdint.h"

typedef enum {
	UART_RETURN_FAIL = 0,
	UART_RETURN_SUCCESS = 1
}module_UART_RetCode;


typedef enum {
	UART_0 = 0,
	UART_1 = 1
}module_UART_number_t;


typedef enum {  //
	UART_Baud_1200   = 4255,
	UART_Baud_2400   = 2133,
	UART_Baud_4800   = 1066,
	UART_Baud_9600   = 532,
	UART_Baud_14400  = 355,
	UART_Baud_19200  = 266,
	UART_Baud_28800  = 178,
	UART_Baud_38400  = 134,
	UART_Baud_57600  = 89,
	UART_Baud_76800  = 66,
	UART_Baud_115200 = 44,
	UART_Baud_230400 = 22,
	UART_Baud_460800 = 11,
	UART_Baud_921600 = 6,
	UART_Baud_1382400= 4,
	UART_Baud_2764800= 2,
	UART_Baud_5529600= 1,
} module_UART_baudRate_t;

typedef struct {
	uint32_t RFR_TFR_DLL;	/*< Регистр принимающего FIFO. DLAB=0 [чтение]                                   */
							/*		Регистр передающего FIFO. DLAB=0 [запись]                                */
							/*		Регистр значения делителя частоты (младший байт) DLAB=1 [чтение][запись] */
	uint32_t IER_DLM	;	/*< Регистр разрешения прерываний. DLAB=0 [чтение][запись]                       */
							/*< Регистр значения делителя частоты (старший байт) DLAB=1 [чтение][запись]     */
	uint32_t IIR_FCR	;	/*< Регистр распознавания прерываний. DLAB=0/1 [чтение]                          */
						/*		Регистр управления FIFO. DLAB=0/1 [запись]                                   */
	uint32_t LCR		;	/*< Регистр управления линией. DLAB=0/1 [чтение][запись]                         */
	uint32_t MCR		;	/*< Регистр управления модемом. DLAB=0/1 [чтение][запись]                        */
	const uint32_t LSR	;	/*< Регистр состояния линии. DLAB=0/1 [чтение]                                   */
	const uint32_t MSR	;	/*< Регистр состояния модема. DLAB=0/1 [чтение]                                  */
	uint32_t SCR_TST	;	/*< Рабочий регистр. DLAB=0 [чтение][запись]                                     */
							/*	 	Регистр тестового режима. DLAB=1 [запись]                                */
} module_UART_controller_t;

typedef enum {
	UART_wordLength_5bit = 0,
	UART_wordLength_6bit = 1,
	UART_wordLength_7bit = 2,
	UART_wordLength_8bit = 3
} module_UART_wordLength_t;

typedef enum {
	UART_Parity_NoControl = 0,
	UART_Parity_OddControl = 1,
	UART_Parity_EvenControl = 2,
} module_UART_evenParity_t;

typedef enum  {
	UART_StopBits_1 = 0,
	UART_StopBits_2 = 1
} module_UART_stopBitsNum_t;

typedef struct {
	module_UART_baudRate_t baudRate;
	module_UART_wordLength_t wordLength;
	module_UART_evenParity_t parity;
	module_UART_stopBitsNum_t stopBits;
} module_UART_config_t;



/* -- Регистр разрешения прерываний. IER  DLAB = 0*/
#define IER_EDSSI	(1 << 3)	/*< Бит, разрешающий прерывание от модема. Если EDSSI установлен                        */
								/*		в “1”, и если установлены биты [3:0] регистра MSR, то генерируется прерывание   */
#define IER_ELSI	(1 << 2)	/*< Бит, разрешающий прерывание по состоянию приема. Если ELSI установлен в “1”,        */
								/*		и если установлены биты [4:1] регистра LSR, то генерируется прерывание          */
#define IER_ETBEI	(1 << 1)	/*< Бит, разрешающий прерывание по пустоте регистра TFR. Если TBEI установлен в “1”,    */
								/*		и если регистр TFR пуст, то генерируется прерывание                             */
#define IER_ERBFI	(1 << 0)	/*< Бит, разрешающий прерывание по достижению порога заполнения                         */
								/*		приемного FIFO. Если ERBFI установлен в “1”, и если данные в приемном FIFO      */
								/*		достигли определенного количества, то генерируется прерывание                   */

/* -- Регистры значения делителя частот. DLL и DLM    DLAB = 1*/
#define DLM_DL_MASK	(0xFF)	/*< Если бит DLAB регистра LCR установлен в «1», то регистры DLL и   */
#define DLM_DL_OFFS	(0)		/*  	DLM программно доступны на чтение и на запись.               */
#define DLL_DL_MASK	(0xFF)	/*  	После сброса DLL и DLM устанавливаются в значение 00h        */
#define DLL_DL_OFFS	(0)		/*  	Скорость передачи программируется в                          */
							/*		диапазоне {DLM,DLL} = 0xFFFF ... 0x0001 (65536 ... 1)        */
							/*		Скорость передачи вычисляется по следующей формуле:          */
							/*  	F uart (bps) = F apb (Hz)/[16*{DLM,DLL}]                     */


/* -- Регистр распознавания прерываний. IIR  */
#define IIR_FIFO_MASK	(0x3)	/*< Состояние FIFO  «11» фиксировано                                              */
#define IIR_FIFO_OFFS	(0x6)	/*                                                                                */
#define IIR_ID_MASK		(0x7)	/*<Типы прерываний (значения действительны при NINT = 0):                         */
#define IIR_ID_OFFS		(0x1)	/*		011(1) – состояние приемной линии. Сбрасывается чтением из регистра       */
								/*		LSR.                                                                      */
								/*		010(2) – наличие данных в приемном FIFO. Сбрасывается чтением из          */
								/*		приёмного FIFO.                                                           */
								/*		110(2)- истечение периода timeout. Сбрасывается чтением из приёмного      */
								/*		FIFO.                                                                     */
								/*		001(3) – передающее FIFO пусто. Сбрасывается чтением из регистра IIR      */
								/*		или записью в передающее FIFO.                                            */
								/*		000(4) – состояние модема. Сбрасывается чтением из регистра MSR           */
								/*		В скобочках указан приоритет прерывания.                                  */
#define IIR_NINT		(0x1)		/*		Бит состояния прерывания:                                                 */
								/*		0 – прерывание активно                                                    */
								/*      1 – нет сигнала прерывания, ID[2:0] = 3’b000                              */

/* -- Регистр управления FIFO. FCR*/
#define FCR_RCVR_MASK	(0x3)	/*< Пороговый уровень принимающего FIFO:                                 */
#define FCR_RCVR_OFFS	(0x6)		/*		00 – 1 байт                                                      */
									/*		01 – 4 байт                                                      */
									/*		10 – 8 байт                                                      */
									/*		11 – 14 байт                                                     */
#define FCR_DMA_MODE	(1 << 3)	/*< Режим работы ПДП интерфейса:                                         */
									/*		0 – одиночный режим передачи                                     */
									/*		1 – пакетный режим передачи (этот режим не совместим с DMAC)     */
									/*		Для правильной работы контроллером ПДП, этот бит должен быть     */
									/*		обязательно сброшен в 0.                                         */
#define FCR_TxF_RST		(1 << 2)	/*< Сброс передающего FIFO.  1 – сброс                                   */
#define FCR_RxF_RST		(1 << 1)	/*< Сброс принимающего FIFO. 1 – сброс                                   */



/*Формат регистра LCR*/
#define LCR_DLAB		(1 << 7)	/*< Бит переключения регистровых полей                                      */
									/*		0 – доступ к регистрам RFR, TFR, IER, SCR                           */
									/*		1 - доступ к регистрам DLL,DLM,TST                                  */
#define LCR_SB			(1 << 6)	/*Set Break. Принудительная остановка передачи.                         */
									/*		1 – сигнал UARTxTXD устанавливается в 0                             */
#define LCR_SP			(1 << 5)	/*< Stick Parity. Фиксация бита четности                                    */
									/*		0 – Бит четности формируется на основе EPS и PEN.                   */
									/*		1 – Бит четности фиксирован и зависит от состояния EPS и PEN.       */
									/*		Значение бита выдается при передаче и проверяется при приеме.       */
									/*		PEN=1, EPS=0, бит равен 1                                           */
									/*		PEN=1, EPS=1, бит равен 0                                           */
#define LCR_EPS			(1 << 4)	/*< Even Parity Select. Выбор типа контроля по четности                     */
									/*		0 – контроль нечетности                                             */
									/*e		1 – контроль четности                                               */
#define LCR_PEN			(1 << 3)	/*< Parity Enable. Разрешение контроля четности.                            */
									/*		0 – бит четности не посылается, не проверяется                      */
									/*		1 – бит четности посылается и проверяется                           */
									/*		Бит четности прибавляется после поля данных, бит остановки сле-     */
									/*		дует самым последним.                                               */
#define LCR_STB			(1 << 2)	/*< Stop Bit Length. Количество бит stop-последовательности.                */
									/*		0 – 1 бит                                                           */
									/*		1 – 1,5 бита (последовательность данных 5 бит)                      */
									/*		2 бита (последовательность данных 6 – 8 бит)                        */
#define LCR_WLS_MASK	(0x3)		/*< Выбор длины передаваемого/принимаемого слова                            */
#define LCR_WLS_OFFS	(0)			/*		00 – 5 бит   01 – 6 бит   10 – 7 бит   11 – 8 бит                   */



/*Формат регистра LSR*/
#define LSR_ERRF	(1 << 7)	/*< Ошибка в приемном FIFO. Бит устанавливается, когда в приемном           */
								/*		FIFO присутствуют данные, имеющие ошибку контроля четности,         */
								/*		ошибку кадрирования или ошибку определения останова. Бит            */
								/*		сбрасывается при чтении LSR, если больше нет ошибок в прием-        */
								/*		ном FIFO, за исключением той, которая вызвала установку ERRF.       */
#define LSR_TEMT	(1 << 6)	/*< Сигнал пустоты передатчика.                                             */
								/*		TEMT устанавливается в «1», когда сдвиговый регистр передатчика     */
								/*		и передающее FIFO становятся пустыми                                */
#define LSR_THRE	(1 << 5)	/*< Сигнал пустоты передающего FIFO.                                        */
								/*		Устанавливается в 1, когда передающее FIFO становится пустым.       */
#define LSR_BI		(1 << 4)	/*< Прерывание по останову.                                                 */
								/*		Бит устанавливается, когда UARTxRXD удерживается в “0” доль-        */
								/*		ше, чем время необходимое для передачи (START бит + DATA би-        */
								/*		ты +PARITY бит + STOP бит). BI обнуляется, когда этот регистр       */
								/*		считывается процессором.                                            */
#define LSR_FE		(1 << 3)	/*< Ошибка кадрирования.                                                    */
								/*		Бит устанавливается, когда принятые данные не имеют правильно-      */
								/*		го бита остановки. FE обнуляется, когда этот регистр считывается    */
								/*		процессорным ядром.                                                 */
#define LSR_PE		(1 << 2)	/*      Ошибка контроля четности.                                           */
								/*		Бит устанавливается, когда принятые данные не имеют правильно-      */
								/*		го бита четности. PE обнуляется, когда этот регистр считывается     */
								/*		процессорным ядром.                                                 */
#define LSR_OE		(1 << 1)	/*< Ошибка переполнения принимающего FIFO.                                  */
								/*		Бит устанавливается, когда принимающее FIFO полное и имеет          */
								/*		место прием следующих данных. OE обнуляется, когда этот ре-         */
								/*		гистр считывается процессорным ядром.                               */
#define LSR_DR		(1)			/*< Готовность данных (данные находятся в приемном FIFO).                   */
								/*		Бит показывает, что в FIFO хранится более чем 1 байт данных. Бит    */
								/*			устанавливается, когда данные присутствуют в FIFO. DR обнуляет- */
								/*		ся, когда из FIFO считаны все данные                                */


/*Формат регистра MCR*/
#define MCR_LOOP (1 << 4) /*< Кольцевой режим (режим самопроверки).                   */


/* Function prototypes*/

void module_UART_setConfig ( module_UART_number_t uartN, module_UART_config_t * config);

void module_UART_send (module_UART_number_t uartN, char data);

module_UART_RetCode module_UART_recieve (module_UART_number_t uartN, char * data, int wait_cycles);

void module_UART_clearFIFO (module_UART_number_t uartN);

#endif /* MODULE_UART_H_*/
