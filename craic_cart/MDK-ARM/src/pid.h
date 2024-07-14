#ifndef __PID_H
#define __PID_H

#include "stm32f4xx_hal.h"

typedef struct pid
{
	float p;
	float i;
	float d;
	int16_t error;
	int16_t last_error;
	int16_t last_last_error;
	int16_t output;
}pid;

void Pos_pid(pid *pid,int16_t target,int16_t real);
void Incre_pid(pid *pid,int16_t target,int16_t real);

#endif
