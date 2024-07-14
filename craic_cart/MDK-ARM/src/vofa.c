#include "vofa.h"
#include "motor.h"
#include "encoder.h"
#include "stdio.h"

void vofa_send(void)
{
	printf("%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",FL_real_speed,FL_target_speed,FR_real_speed,FR_target_speed,RL_real_speed,\
	      RL_target_speed,RR_real_speed,RR_target_speed,distance,distance_target,distance_target_new);
}
