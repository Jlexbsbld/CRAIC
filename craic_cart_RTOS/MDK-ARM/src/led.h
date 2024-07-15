#ifndef __LED_H
#define __LED_H

#include "stm32f4xx_hal.h"

typedef enum{
	red,
	green,
	blue,
	yellow,
	white,
	black,
	purple,
	lightblue
}color_light;

void led_control(color_light state);

extern color_light led_state;

#endif
