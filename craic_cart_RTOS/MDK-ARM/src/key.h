#ifndef __KEY_H
#define __KEY_H

#include "stm32f4xx_hal.h"
#include "cmsis_os.h"

typedef struct key{
	GPIO_PinState key_state;
	uint8_t key_judge;
	uint8_t key_flag;
}key;

void key_scan(void);

extern key keys[4];

#endif

