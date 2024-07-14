#ifndef __PWM_H
#define __PWM_H

#include "stm32f4xx_hal.h"

#define DUTY_MAX (10000)

void pwm_init(void);
void set_duty(TIM_HandleTypeDef* htim, uint32_t Channel, uint16_t duty);
void set_pwm(TIM_HandleTypeDef* htim,uint32_t Channel, uint32_t f,uint16_t duty);

#endif

