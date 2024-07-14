#include "pwm.h"

extern TIM_HandleTypeDef htim1,htim2,htim3,htim4,htim5,htim6,htim7,htim8,htim9,htim12;

uint32_t freq[6] = {0,0,0,0,0,0};

void set_duty(TIM_HandleTypeDef* htim, uint32_t Channel, uint16_t duty)
{
	if(htim->Instance == TIM1)
	{
		switch(Channel)
		{
			case TIM_CHANNEL_1:{
				htim->Instance->ARR = 1000000/freq[0];
				htim->Instance->CCR1 = 1000000.0f/DUTY_MAX/freq[0]*duty;
				if(htim->Instance->CNT > htim->Instance->ARR)
				{
					htim->Instance->CNT = 0;
				}
			}break;
			case TIM_CHANNEL_2:{
				htim->Instance->ARR = 1000000/freq[0];
				htim->Instance->CCR2 = 1000000.0f/DUTY_MAX/freq[0]*duty;
				if(htim->Instance->CNT > htim->Instance->ARR)
				{
					htim->Instance->CNT = 0;
				}
			}break;
			case TIM_CHANNEL_3:{
				htim->Instance->ARR = 1000000/freq[0];
				htim->Instance->CCR3 = 1000000.0f/DUTY_MAX/freq[0]*duty;
				if(htim->Instance->CNT > htim->Instance->ARR)
				{
					htim->Instance->CNT = 0;
				}
			}break;
			case TIM_CHANNEL_4:{
				htim->Instance->ARR = 1000000/freq[0];
				htim->Instance->CCR4 = 1000000.0f/DUTY_MAX/freq[0]*duty;
				if(htim->Instance->CNT > htim->Instance->ARR)
				{
					htim->Instance->CNT = 0;
				}
			}break;
		}
	}
	else if(htim->Instance == TIM2)
	{
		switch(Channel)
		{
			case TIM_CHANNEL_1:{
				htim->Instance->ARR = 50000000/freq[1];
				htim->Instance->CCR1 = 50000000.0f/DUTY_MAX/freq[1]*duty;
				if(htim->Instance->CNT > htim->Instance->ARR)
				{
					htim->Instance->CNT = 0;
				}
			}break;
			case TIM_CHANNEL_2:{
				htim->Instance->ARR = 50000000/freq[1];
				htim->Instance->CCR2 = 50000000.0f/DUTY_MAX/freq[1]*duty;
				if(htim->Instance->CNT > htim->Instance->ARR)
				{
					htim->Instance->CNT = 0;
				}			
			}break;
			case TIM_CHANNEL_3:{
				htim->Instance->ARR = 50000000/freq[1];
				htim->Instance->CCR3 = 50000000.0f/DUTY_MAX/freq[1]*duty;
				if(htim->Instance->CNT > htim->Instance->ARR)
				{
					htim->Instance->CNT = 0;
				}			
			}break;
			case TIM_CHANNEL_4:{
				htim->Instance->ARR = 50000000/freq[1];
				htim->Instance->CCR4 = 50000000.0f/DUTY_MAX/freq[1]*duty;
				if(htim->Instance->CNT > htim->Instance->ARR)
				{
					htim->Instance->CNT = 0;
				}			
			}break;
		}		
	}
	else if(htim->Instance == TIM3)
	{
		switch(Channel)
		{
			case TIM_CHANNEL_1:{
				htim->Instance->ARR = 1000000/freq[2];
				htim->Instance->CCR1 = 1000000.0f/DUTY_MAX/freq[2]*duty;
				if(htim->Instance->CNT > htim->Instance->ARR)
				{
					htim->Instance->CNT = 0;
				}
			}break;
			case TIM_CHANNEL_2:{
				htim->Instance->ARR = 1000000/freq[2];
				htim->Instance->CCR2 = 1000000.0f/DUTY_MAX/freq[2]*duty;
				if(htim->Instance->CNT > htim->Instance->ARR)
				{
					htim->Instance->CNT = 0;
				}			
			}break;
			case TIM_CHANNEL_3:{
				htim->Instance->ARR = 1000000/freq[2];
				htim->Instance->CCR3 = 1000000.0f/DUTY_MAX/freq[2]*duty;
				if(htim->Instance->CNT > htim->Instance->ARR)
				{
					htim->Instance->CNT = 0;
				}			
			}break;
			case TIM_CHANNEL_4:{
				htim->Instance->ARR = 1000000/freq[2];
				htim->Instance->CCR4 = 1000000.0f/DUTY_MAX/freq[2]*duty;
				if(htim->Instance->CNT > htim->Instance->ARR)
				{
					htim->Instance->CNT = 0;
				}			
			}break;
		}		
	}
	else if(htim->Instance == TIM5)
	{
		switch(Channel)
		{
			case TIM_CHANNEL_1:{
				htim->Instance->ARR = 50000000/freq[3];
				htim->Instance->CCR1 = 50000000.0f/DUTY_MAX/freq[3]*duty;
				if(htim->Instance->CNT > htim->Instance->ARR)
				{
					htim->Instance->CNT = 0;
				}
			}break;
			case TIM_CHANNEL_2:{
				htim->Instance->ARR = 50000000/freq[3];
				htim->Instance->CCR2 = 50000000.0f/DUTY_MAX/freq[3]*duty;
				if(htim->Instance->CNT > htim->Instance->ARR)
				{
					htim->Instance->CNT = 0;
				}			
			}break;
			case TIM_CHANNEL_3:{
				htim->Instance->ARR = 50000000/freq[3];
				htim->Instance->CCR3 = 50000000.0f/DUTY_MAX/freq[3]*duty;
				if(htim->Instance->CNT > htim->Instance->ARR)
				{
					htim->Instance->CNT = 0;
				}			
			}break;
			case TIM_CHANNEL_4:{
				htim->Instance->ARR = 50000000/freq[3];
				htim->Instance->CCR4 = 50000000.0f/DUTY_MAX/freq[3]*duty;
				if(htim->Instance->CNT > htim->Instance->ARR)
				{
					htim->Instance->CNT = 0;
				}			
			}break;
		}		
	}
	else if(htim->Instance == TIM8)
	{
		switch(Channel)
		{
			case TIM_CHANNEL_1:{
				htim->Instance->ARR = 1000000/freq[4];
				htim->Instance->CCR1 = 1000000.0f/DUTY_MAX/freq[4]*duty;
				if(htim->Instance->CNT > htim->Instance->ARR)
				{
					htim->Instance->CNT = 0;
				}
			}break;
			case TIM_CHANNEL_2:{
				htim->Instance->ARR = 1000000/freq[4];
				htim->Instance->CCR2 = 1000000.0f/DUTY_MAX/freq[4]*duty;
				if(htim->Instance->CNT > htim->Instance->ARR)
				{
					htim->Instance->CNT = 0;
				}			
			}break;
			case TIM_CHANNEL_3:{
				htim->Instance->ARR = 1000000/freq[4];
				htim->Instance->CCR3 = 1000000.0f/DUTY_MAX/freq[4]*duty;
				if(htim->Instance->CNT > htim->Instance->ARR)
				{
					htim->Instance->CNT = 0;
				}			
			}break;
			case TIM_CHANNEL_4:{
				htim->Instance->ARR = 1000000/freq[4];
				htim->Instance->CCR4 = 1000000.0f/DUTY_MAX/freq[4]*duty;
				if(htim->Instance->CNT > htim->Instance->ARR)
				{
					htim->Instance->CNT = 0;
				}			
			}break;
		}			
	}
	else if(htim->Instance == TIM9)
	{
		switch(Channel)
		{
			case TIM_CHANNEL_1:{
				htim->Instance->ARR = 1000000/freq[5];
				htim->Instance->CCR1 = 1000000.0f/DUTY_MAX/freq[5]*duty;
				if(htim->Instance->CNT > htim->Instance->ARR)
				{
					htim->Instance->CNT = 0;
				}
			}break;
			case TIM_CHANNEL_2:{
				htim->Instance->ARR = 1000000/freq[5];
				htim->Instance->CCR2 = 1000000.0f/DUTY_MAX/freq[5]*duty;
				if(htim->Instance->CNT > htim->Instance->ARR)
				{
					htim->Instance->CNT = 0;
				}			
			}break;
			case TIM_CHANNEL_3:{
				htim->Instance->ARR = 1000000/freq[5];
				htim->Instance->CCR3 = 1000000.0f/DUTY_MAX/freq[5]*duty;
				if(htim->Instance->CNT > htim->Instance->ARR)
				{
					htim->Instance->CNT = 0;
				}			
			}break;
			case TIM_CHANNEL_4:{
				htim->Instance->ARR = 1000000/freq[5];
				htim->Instance->CCR4 = 1000000.0f/DUTY_MAX/freq[5]*duty;
				if(htim->Instance->CNT > htim->Instance->ARR)
				{
					htim->Instance->CNT = 0;
				}			
			}break;
		}	
	}
}

void pwm_init(void)
{
	set_pwm(&htim1,TIM_CHANNEL_2,17000,0); //17K,占空比为0 PA9
	set_pwm(&htim1,TIM_CHANNEL_4,17000,0); //17K,占空比为0 PA11
	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2);
	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_4);
	
	set_pwm(&htim2,TIM_CHANNEL_3,17000,0); //17K,占空比为0 PB10
	set_pwm(&htim2,TIM_CHANNEL_4,17000,0); //17K,占空比为0 PB11
	HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_3);
	HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_4);
	
	set_pwm(&htim3,TIM_CHANNEL_3,17000,0); //17K,占空比为0 PB0
	set_pwm(&htim3,TIM_CHANNEL_4,17000,0); //17K,占空比为0 PB1
	HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_3);
	HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_4);
	
	set_pwm(&htim5,TIM_CHANNEL_1,17000,0); //17K,占空比为0 PA0
	set_pwm(&htim5,TIM_CHANNEL_2,17000,0); //17K,占空比为0 PA1
	HAL_TIM_PWM_Start(&htim5, TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(&htim5, TIM_CHANNEL_2);
	
	set_pwm(&htim8,TIM_CHANNEL_2,17000,0); //17K,占空比为0 PC7
	set_pwm(&htim8,TIM_CHANNEL_4,17000,0); //17K,占空比为0 PC9
	HAL_TIM_PWM_Start(&htim8, TIM_CHANNEL_2);
	HAL_TIM_PWM_Start(&htim8, TIM_CHANNEL_4);
	
	set_pwm(&htim9,TIM_CHANNEL_2,17000,0); //17K,占空比为0 PE6
	HAL_TIM_PWM_Start(&htim9, TIM_CHANNEL_2);
}

void set_pwm(TIM_HandleTypeDef* htim,uint32_t Channel, uint32_t f, uint16_t duty)
{
	if(htim->Instance == TIM1)
	{
		freq[0] = f;
	}
	else if(htim->Instance == TIM2)
	{
		freq[1] = f;
	}
	else if(htim->Instance == TIM3)
	{
		freq[2] = f;
	}
	else if(htim->Instance == TIM5)
	{
		freq[3] = f;
	}
	else if(htim->Instance == TIM8)
	{
		freq[4] = f;
	}
	else if(htim->Instance == TIM9)
	{
		freq[5] = f;
	}
	set_duty(htim,Channel,duty);
}

