#include "motor.h"
#include "pid.h"
#include "pwm.h"
#include "encoder.h"

extern TIM_HandleTypeDef htim1,htim2,htim3,htim4,htim5,htim6,htim7,htim8,htim9,htim12;

int16_t FL_target_speed=0,FR_target_speed=0,RL_target_speed=0,RR_target_speed=0;
int16_t FL_real_speed=0,FR_real_speed=0,RL_real_speed=0,RR_real_speed=0;
int16_t Move_speed = 0,Move_speed_real=0;
int32_t distance_target=0;
int32_t distance_target_new=0;
// p,i,d,e,le,lle,output
//左前0,右前1，左后2，右后3
pid motorpid[4] = {{40.0f,20.0f,0,0,0,0,0},{40.0f,20.0f,0,0,0,0,0},{40.0f,20.0f,0,0,0,0,0},{40.0f,20.0f,0,0,0,0,0}};
pid Armpid = {0.5f,2.0f,0,0,0,0,0};//Pospid0.5f,2.0f
pid Armspeedpid = {40.0f,20.0f,0,0,0,0,0};
pid speedpid = {40.0f,20.0f,0,0,0,0,0};

void motor_init(void)
{
	set_pwm(&htim1,TIM_CHANNEL_1,20000,0); //10K,占空比为0 PA9
	set_pwm(&htim1,TIM_CHANNEL_3,20000,0); //10K,占空比为0 PA11
	set_pwm(&htim8,TIM_CHANNEL_1,20000,0); //10K,占空比为0 PC7
	set_pwm(&htim8,TIM_CHANNEL_3,20000,0); //10K,占空比为0 PC9
	set_pwm(&htim9,TIM_CHANNEL_1,20000,0); //10K,占空比为0 PE6
	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_3);
	HAL_TIM_PWM_Start(&htim8, TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(&htim8, TIM_CHANNEL_3);
	HAL_TIM_PWM_Start(&htim9, TIM_CHANNEL_1);
}

void MotorRun(Motor_device select)
{
	if(select == FL_Motor)
	{
		Incre_pid(&motorpid[0],FL_target_speed,FL_real_speed);
		if(motorpid[0].output > LIMIT_DUTY)
		{
			motorpid[0].output = LIMIT_DUTY;
		}
		else if(motorpid[0].output < -LIMIT_DUTY)
		{
			motorpid[0].output = -LIMIT_DUTY;
		}
		Motor_control(3,motorpid[0].output);
	}
	else if(select == FR_Motor)
	{
		Incre_pid(&motorpid[1],FR_target_speed,FR_real_speed);
		if(motorpid[1].output > LIMIT_DUTY)
		{
			motorpid[1].output = LIMIT_DUTY;
		}
		else if(motorpid[1].output < -LIMIT_DUTY)
		{
			motorpid[1].output = -LIMIT_DUTY;
		}
		Motor_control(1,motorpid[1].output);
	}
	else if(select == RL_Motor)
	{
		Incre_pid(&motorpid[2],RL_target_speed,RL_real_speed);
		if(motorpid[2].output > LIMIT_DUTY)
		{
			motorpid[2].output = LIMIT_DUTY;
		}
		else if(motorpid[2].output < -LIMIT_DUTY)
		{
			motorpid[2].output = -LIMIT_DUTY;
		}
		Motor_control(4,motorpid[2].output);
	}
	else if(select == RR_Motor)
	{
		Incre_pid(&motorpid[3],RR_target_speed,RR_real_speed);
		if(motorpid[3].output > LIMIT_DUTY)
		{
			motorpid[3].output = LIMIT_DUTY;
		}
		else if(motorpid[3].output < -LIMIT_DUTY)
		{
			motorpid[3].output = -LIMIT_DUTY;
		}
		Motor_control(0,motorpid[3].output);
	}
	else if(select == Move_Motor)
	{
//		Pos_pid(&Armpid,distance_target,distance);
//		if(Armpid.output > LIMIT_ARM)
//		{
//			Armpid.output = LIMIT_ARM;
//		}
//		else if(Armpid.output < -LIMIT_ARM)
//		{
//			Armpid.output = -LIMIT_ARM;
//		}
		Incre_pid(&speedpid,Move_speed,Move_speed_real);
		Motor_control(2,speedpid.output);
	}
}

void Motor_control(uint8_t select,int16_t duty)
{
	switch(select)
	{
		case 0:{
			if(duty >= 0)
			{
				HAL_GPIO_WritePin(GPIOC,GPIO_PIN_9,GPIO_PIN_RESET);//PC9
				set_duty(&htim8,TIM_CHANNEL_3,duty);//PC8
			}
			else
			{
				HAL_GPIO_WritePin(GPIOC,GPIO_PIN_9,GPIO_PIN_SET);//PC9
				set_duty(&htim8,TIM_CHANNEL_3,-duty);//PC8		
			}
		}break;
		case 1:{
			if(duty >= 0)
			{
				HAL_GPIO_WritePin(GPIOC,GPIO_PIN_7,GPIO_PIN_RESET);//PC7
				set_duty(&htim8,TIM_CHANNEL_1,duty);//PC6
			}
			else
			{
				HAL_GPIO_WritePin(GPIOC,GPIO_PIN_7,GPIO_PIN_SET);//PC7
				set_duty(&htim8,TIM_CHANNEL_1,-duty);//PC6		
			}	
		}break;
		case 2:{
			if(duty >= 0)
			{
				HAL_GPIO_WritePin(GPIOA,GPIO_PIN_9,GPIO_PIN_SET);//PA9
				set_duty(&htim1,TIM_CHANNEL_1,duty);//PA8
			}
			else
			{
				HAL_GPIO_WritePin(GPIOA,GPIO_PIN_9,GPIO_PIN_RESET);//PA9
				set_duty(&htim1,TIM_CHANNEL_1,-duty);//PA8		
			}
		}break;
		case 3:{
			if(duty >= 0)
			{
				HAL_GPIO_WritePin(GPIOE,GPIO_PIN_6,GPIO_PIN_SET);//PE6
				set_duty(&htim9,TIM_CHANNEL_1,duty);//PE5
			}
			else
			{
				HAL_GPIO_WritePin(GPIOE,GPIO_PIN_6,GPIO_PIN_RESET);//PE6
				set_duty(&htim9,TIM_CHANNEL_1,-duty);//PE5		
			}
		}break;
		case 4:{
			if(duty >= 0)
			{
				HAL_GPIO_WritePin(GPIOA,GPIO_PIN_11,GPIO_PIN_SET);//PA11
				set_duty(&htim1,TIM_CHANNEL_3,duty);//PA10
			}
			else
			{
				HAL_GPIO_WritePin(GPIOA,GPIO_PIN_11,GPIO_PIN_RESET);//PA11
				set_duty(&htim1,TIM_CHANNEL_3,-duty);//PA10	
			}
		}break;
	}
}
