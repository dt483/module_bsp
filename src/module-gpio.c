
#include "module-gpio.h"

/*static instances*/
static volatile module_GPIO_controller_t * const module_GPIO_devInstance = (module_GPIO_controller_t *) (GPIO_BASE );

 module_GPIO_direction_t  module_GPIO_GetDirection( module_GPIO_pin_t gpio )
{

	if (gpio < 8)
		return ( module_GPIO_direction_t )( (module_GPIO_devInstance->DDR0 & (1 << gpio)) >> gpio);
	else
		return ( module_GPIO_direction_t )( (module_GPIO_devInstance->DDR1 & (1 << gpio)) >> gpio);

}
void module_GPIO_SetOutput( module_GPIO_pin_t gpio )
{
	if (gpio < 8)
		module_GPIO_devInstance->DDR0 |= (1 << gpio);
	else
		module_GPIO_devInstance->DDR1 |= (1 << (gpio-8));
}
void module_GPIO_SetInput( module_GPIO_pin_t gpio )
{
	if (gpio < 8)
		module_GPIO_devInstance->DDR0 &= ~(1 << gpio);
	else
		module_GPIO_devInstance->DDR1 &= ~(1 << (gpio-8));
}
void module_GPIO_SetDirection( module_GPIO_pin_t gpio, module_GPIO_direction_t dir )
{
	if (dir == module_GPIO_DIRECTION_INPUT)
		module_GPIO_SetInput(gpio);
	if (dir == module_GPIO_DIRECTION_OUTPUT)
		module_GPIO_SetOutput(gpio);
}

 module_GPIO_value_t module_GPIO_GetValue( module_GPIO_pin_t gpio )
{
	if (gpio < 8)
		return ( module_GPIO_value_t ) ( module_GPIO_devInstance->PDR0 & (1 << gpio) );
	else
		return ( module_GPIO_value_t ) ( module_GPIO_devInstance->PDR1 & (1 << gpio) );
}
void module_GPIO_SetHigh( module_GPIO_pin_t gpio )
{
//	if (module_GPIO_GetDirection(gpio) == module_GPIO_DIRECTION_OUTPUT)
	{
		if (gpio < 8)
			module_GPIO_devInstance->PDR0 |= (1 << gpio);
		else
			module_GPIO_devInstance->PDR1 |= ((1 << (gpio-8)));
	}
//	else
//		_assert("GPIO assert: failed set gpio %i to HIGH, gpio direction is INPUT", gpio);

}
void module_GPIO_SetLow( module_GPIO_pin_t gpio )
{
//	if (module_GPIO_GetDirection(gpio) == module_GPIO_DIRECTION_OUTPUT)
	{
		if (gpio < 8) module_GPIO_devInstance->PDR0 &= ~(1 << gpio);
		else module_GPIO_devInstance->PDR1 &= ~(1 << (gpio-8));
	}
//	else
//		_assert("GPIO assert: failed set gpio %i to LOW, gpio direction is INPUT", gpio);
}
void module_GPIO_SetValue( module_GPIO_pin_t gpio, module_GPIO_value_t val )
{
	module_GPIO_SetOutput(gpio);
	if (val == module_GPIO_VALUE_HI)
		module_GPIO_SetHigh(gpio);
	if (val == module_GPIO_VALUE_LO)
		module_GPIO_SetLow(gpio);
}


void module_GPIO_SPI_setDevice( module_GPIO_SPICS_numDevice_t dev)
{
	module_GPIO_devInstance->DDR3 = 0xF0; //set all CS as input
	SET_BIT_FIELD(module_GPIO_devInstance->PDR3, PDR3_SPI, dev);
}

void module_GPIO_ledOn()
{
	module_GPIO_SetOutput(LED0_GPIO);
	module_GPIO_SetLow(LED0_GPIO);

}
void module_GPIO_ledOff()
{
	module_GPIO_SetOutput(LED0_GPIO);
	module_GPIO_SetHigh(LED0_GPIO);
}


void module_GPIO_assertBlink()
{
	module_GPIO_SetOutput(LED1_GPIO);
	module_GPIO_SetHigh(LED1_GPIO);
	volatile uint32_t n,i,time = 100000;
	for (n=0;n<5;n++)
	{
		module_GPIO_SetLow(LED1_GPIO);
		for (i=0;i<time;i++);
		module_GPIO_SetHigh(LED1_GPIO);
		for (i=0;i<time;i++);
	}
	for (i=0;i<500000;i++);
	module_GPIO_SetHigh(LED1_GPIO);
}
