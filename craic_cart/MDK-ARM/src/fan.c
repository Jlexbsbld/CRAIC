#include "fan.h"

bool fan1_state = true;
bool fan2_state = true;

void delay_us_fan(uint16_t us)
{
	htim7.Instance->CNT = 0;
	HAL_TIM_Base_Start(&htim7);
	while(htim7.Instance->CNT < us);
	HAL_TIM_Base_Stop(&htim7);
}

void fan_init(void)
{
	set_pwm(&htim9,TIM_CHANNEL_2,17000,0); //17K,Õ¼¿Õ±ÈÎª0 PE6
	HAL_TIM_PWM_Start(&htim9, TIM_CHANNEL_2);
}

void fan_control(uint8_t select, bool state)
{
	switch(select)
	{
		case 0:{
			if(state)
			{
				HAL_GPIO_WritePin(GPIOA,GPIO_PIN_12,GPIO_PIN_SET);
				delay_us_fan(500);
				HAL_GPIO_WritePin(GPIOA,GPIO_PIN_12,GPIO_PIN_RESET);
			}
			else
			{
				HAL_GPIO_WritePin(GPIOA,GPIO_PIN_12,GPIO_PIN_RESET);
			}
		}break;
		case 1:{
			if(state)
			{
				HAL_GPIO_WritePin(GPIOD,GPIO_PIN_6,GPIO_PIN_SET);
				delay_us_fan(500);
				HAL_GPIO_WritePin(GPIOD,GPIO_PIN_6,GPIO_PIN_RESET);
			}
			else
			{
				HAL_GPIO_WritePin(GPIOD,GPIO_PIN_6,GPIO_PIN_RESET);
			}
		}break;		
	}
}
