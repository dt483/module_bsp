/*
 * module-armsc.h
 *
 *  Created on: 13 февр. 2019 г.
 *      Author: aleksey
 */

#include <module-base.h>
#include <stdint.h>


#ifndef MODULE_LIB_INCLUDE_MODULE_ARMSC_H_
#define MODULE_LIB_INCLUDE_MODULE_ARMSC_H_

/*TODO:*/
/*Implemented only NMU interrupts control */

typedef struct {
	uint32_t	REMAPSTA	;	/*Текущий тип отображения памяти                                                       */
    uint32_t	REMAPSET	;	/* Установить REMAPSTA                                                                 */
    uint32_t	REMAPCLR	;	/* Сбросить REMAPSTA                                                                   */
    uint32_t	reserved_0	;	/* зарезервировано                                                                     */
    uint32_t	VIHSTA		;	/* Регистр управления режимом High Vectors Mode                                        */
    uint32_t	VIHSET		;	/* Установить VIHSTA                                                                   */
    uint32_t	VIHCLR		;	/* Сбросить VIHSTA                                                                     */
    uint32_t	BOOTM		;	/* Регистр состояния внешних входов, управляющих загрузкой                             */
    uint32_t	DFISTA		;	/* Регистр готовности физическогоинтерфейса DMC                                        */
    uint32_t	DFICONFIG	;	/* Регистр конфигурации физического интерфейса DMC                                     */
    uint32_t	DFIRST		;	/* Регистр программного сброса DMC                                                     */
    uint32_t	ADCCTRL		;	/* Регистр управления потреблением АЦП                                                 */
    uint32_t	NMUINTSTA	;	/* Регистр состояния прерываний от NMU части СБИС                           *          */
    uint32_t	NMUINTCLR	;	/* Регистр сброса прерываний от NMU части СБИС                              *          */
    uint32_t	NMUINTREQ	;	/* Регистр генерации прерывания к NMU части                                 *          */
    uint32_t	NMUCTRL		;	/* Регистр управления приоритетом ядер NMC доступа в SMU                               */
    uint32_t	USBRESET	;	/* Регистр программного сброса USB контроллера                                         */
    uint32_t	USBCTRL		;	/* Регистр управления подключения к USB шине                                           */
    uint32_t	USBSTATE	;	/* Регистр состояния USB шины                                                          */
    uint32_t	USBINTCLR	;	/* Регистр сброса прерывания по подключению к шине USB                                 */
} module_ARMSC_controller_t;


typedef enum {
	ARMSC_INT_NMC0LP = 0,
	ARMSC_INT_NMC1LP ,
	ARMSC_INT_NMC0HP ,
	ARMSC_INT_NMC1HP ,
	ARMSC_INT_ITMS ,
	ARMSC_INT_ITAUX ,
}  module_ARMSC_intNum_t;

typedef enum {
	ARMSC_NMC0 = 0,
	ARMSC_NMC1 = 1,
}  module_ARMSC_NMCnum_t;

typedef enum {
	ARMSC_LOW_PRIORITY_INT = 0,
	ARMSC_HIGH_PRIORITY_INT = 2 ,
	ARMSC_NONMASKABLE_INT = 4
}  module_ARMSC_typeInt_t;

/*Function protorypes*/
int module_ARMSC_get_NMU_interrupt_status (module_ARMSC_intNum_t interruptNumber);
int module_ARMSC_clear_NMU_interrupt (module_ARMSC_intNum_t interruptNumber);
int module_ARMSC_generate_NMU_interrupt (module_ARMSC_NMCnum_t nmcNumber, module_ARMSC_typeInt_t typeInt);
int module_ARMSC_set_NMU_interrupt_priority (module_ARMSC_NMCnum_t nmcNumber);




#endif /* MODULE_LIB_INCLUDE_MODULE_ARMSC_H_ */
