#ifndef __BEEP_H
#define __BEEP_H

#include "stm32f4xx_hal.h"
#include "stdbool.h"

#define ON true
#define OFF false
	
void beep_control(bool state);
	
#define Beep_ON {beep_control(ON);}
#define Beep_OFF {beep_control(OFF);}
#define Beep_Toggle {HAL_GPIO_TogglePin(GPIOA,GPIO_PIN_4);}

#endif
