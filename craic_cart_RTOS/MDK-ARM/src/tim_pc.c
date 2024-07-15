#include "tim_pc.h"
#include "key.h"
#include "pid.h"
#include "motor.h"
#include "encoder.h"
#include "vofa.h"
#include "communication.h"
#include "led.h"
#include "fan.h"

int32_t abs(int32_t value)
{
	return value>=0?value:-value;
}
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if(htim->Instance == TIM6)
	{
		//10ms
		//led_control(led_state);
		key_scan();
	}
	else if(htim->Instance == TIM10)
	{
		//20ms中断
		FL_real_speed = FL_encoder;
		FL_encoder = 0;
		FR_real_speed = -FR_encoder;
		FR_encoder = 0;
		RL_real_speed = RL_encoder;
		RL_encoder = 0;
		RR_real_speed = -RR_encoder;
		RR_encoder = 0;
		Move_speed_real = distance;
		distance = 0;
		//lastdistance = 0;
		
//		Motor_control(0,5000);//右下
//		Motor_control(1,5000);//右上
//		Motor_control(2,5000);//Move_ARM
//		Motor_control(3,5000);//左上
//		Motor_control(4,5000);//左下
		MotorRun(FL_Motor);
		MotorRun(FR_Motor);
		MotorRun(RL_Motor);
		MotorRun(RR_Motor);
		//if(abs(distance-distance_target)>20)
//		MotorRun(Move_Motor);
		vofa_send();
	}
	else if(htim->Instance == TIM14)
	{
		//1ms中断
		//packgeAnalysis();
//		led_control(led_state);
	}
	else if(htim->Instance == TIM13)
	{
		//1ms中断
		//packgeAnalysis();
//		fan_control(0,fan1_state);
//		fan_control(1,fan2_state);
	}
}
