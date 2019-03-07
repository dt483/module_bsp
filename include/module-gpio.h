#ifndef MODULE_GPIO_H
#define MODULE_GPIO_H

#include <module-base.h>
#include <stdint.h>

 /*TODO: изменить вывод сообщений отладки и ошибок*/

typedef struct {
	uint32_t	PDR0;	/*Регистр порта данных 0/               */
	uint32_t	PDR1;	/*Регистр порта данных 1/               */
	uint32_t	PDR2;	/*Регистр порта данных 2                */
	uint32_t	PDR3;	/*Регистр порта данных 3                */
	uint32_t	DDR0;	/*Регистр направления передачи данных 0 */
	uint32_t	DDR1;	/*Регистр направления передачи данных 1 */
	uint32_t    DDR2;	/*Регистр направления передачи данных 2 */
	uint32_t	DDR3;	/*Регистр направления передачи данных 3 */
} module_GPIO_controller_t;

//#define PDR0  (GPIO_BASE + 0x00)
//#define PDR1  (GPIO_BASE + 0x04)
//#define PDR2  (GPIO_BASE + 0x08)
//#define PDR3  (GPIO_BASE + 0x0C)
//
//#define DDR0  (GPIO_BASE + 0x10)
//#define DDR1  (GPIO_BASE + 0x14)
//#define DDR2  (GPIO_BASE + 0x18)
//#define DDR3  (GPIO_BASE + 0x1C)


#define LED0_GPIO        module_GPIO0
#define LED1_GPIO        module_GPIO1


///* A mask to be able to clear the bits in the register before setting the
//   value we require */
//#define GPIO_REG_MASK     (0xFF)


typedef enum {
    module_GPIO0,
    module_GPIO1,
    module_GPIO2,
    module_GPIO3,
    module_GPIO4,
    module_GPIO5,
    module_GPIO6,
    module_GPIO7,
    module_GPIO8,
    module_GPIO9,
    module_GPIO10,
    module_GPIO11,
    module_GPIO12,
    module_GPIO13,
    module_GPIO14,
    module_GPIO15,
} module_GPIO_pin_t;

/*Регистр PDR3 задает активный сигнал SPI_CSx */
#define PDR3_SPI_MASK	(0xF)
#define PDR3_SPI_OFFS	(0x4)


typedef enum {
	module_GPIO_SPI_CS0 = 0,
	module_GPIO_SPI_CS1,
	module_GPIO_SPI_CS2,
	module_GPIO_SPI_CS3,
	module_GPIO_SPI_CS4,
	module_GPIO_SPI_CS5,
	module_GPIO_SPI_CS6,
	module_GPIO_SPI_CS7
} module_GPIO_SPICS_numDevice_t;


typedef enum {
    module_GPIO_VALUE_LO = 0,
    module_GPIO_VALUE_HI,
} module_GPIO_value_t;


typedef enum {
    module_GPIO_DIRECTION_INPUT = 0,
    module_GPIO_DIRECTION_OUTPUT,
} module_GPIO_direction_t;


/*Function prototypes*/
module_GPIO_direction_t module_GPIO_GetDirection( module_GPIO_pin_t gpio );
void module_GPIO_SetOutput( module_GPIO_pin_t gpio );
void module_GPIO_SetInput( module_GPIO_pin_t gpio );
void module_GPIO_SetDirection( module_GPIO_pin_t gpio, module_GPIO_direction_t dir );

module_GPIO_value_t module_GPIO_GetGpioValue(module_GPIO_pin_t gpio );
void module_GPIO_SetHigh( module_GPIO_pin_t gpio );
void module_GPIO_SetLow( module_GPIO_pin_t gpio );
void module_GPIO_SetValue( module_GPIO_pin_t gpio, module_GPIO_value_t val );
void module_GPIO_toggle( module_GPIO_pin_t gpio);


void module_GPIO_SPI_setDevice( module_GPIO_SPICS_numDevice_t dev);


void module_GPIO_ledOn();
void module_GPIO_ledOff();
void module_GPIO_assertBlink();


#endif
