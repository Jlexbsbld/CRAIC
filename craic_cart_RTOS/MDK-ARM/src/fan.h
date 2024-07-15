#ifndef __FAN_H
#define __FAN_H

#include "stm32f4xx_hal.h"
#include "stdbool.h"
#include "tim.h"
#include "pwm.h"

void fan_control(uint8_t select, bool state);
void fan_init(void);

#define FAN_ON {set_duty(&htim9,TIM_CHANNEL_2,5000);}
#define FAN_OFF {set_duty(&htim9,TIM_CHANNEL_2,0);}

extern bool fan1_state;
extern bool fan2_state;

#endif
