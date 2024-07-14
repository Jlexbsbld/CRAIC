#ifndef __SERVO_H
#define __SERVO_H

#include "stm32f4xx_hal.h"
#include "stdbool.h"

#define Servo_freq (50)

#define angle_0_duty (250)
#define angle_max_duty (1250)

typedef enum Servo{
	Servo1,
	Servo2,
	Servo3,
	Servo4,
	Servo5,
	Servo6
}Servo_device;

void Servo_control(Servo_device select,float angle);
void servo_init(void);
void Arm_work(void);
extern int16_t Servo_angle[6];
extern float Arm_angle[3];
extern int16_t x,y;
extern int8_t angle;

#endif
