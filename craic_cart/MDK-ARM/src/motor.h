#ifndef __MOTOR_H
#define __MOTOR_H

#include "stm32f4xx_hal.h"

extern int16_t FL_target_speed,FR_target_speed,RL_target_speed,RR_target_speed;
extern int16_t	FL_real_speed,FR_real_speed,RL_real_speed,RR_real_speed;
extern int32_t distance_target;
extern int16_t Move_speed,Move_speed_real;
extern int32_t distance_target_new;

typedef enum Motor
{
	FL_Motor,
	FR_Motor,
	RL_Motor,
	RR_Motor,
	Move_Motor
}Motor_device;

#define LIMIT_DUTY (7500)
#define LIMIT_ARM  (10)

void MotorRun(Motor_device select);
void motor_init(void);
void Motor_control(uint8_t select,int16_t duty);

#endif
