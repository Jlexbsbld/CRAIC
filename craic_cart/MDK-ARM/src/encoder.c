#include "encoder.h"

int16_t FL_encoder=0,FR_encoder=0,RL_encoder=0,RR_encoder=0;
int32_t distance = 0;
//int32_t lastdistance = 0;

extern TIM_HandleTypeDef htim1,htim2,htim3,htim4,htim5,htim6,htim7,htim8,htim9,htim12;

void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
	if(htim->Instance == TIM2)
	{
		uint8_t PA15 = HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_15);
		uint8_t PB3 = HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_3);
		if(htim->Channel == HAL_TIM_ACTIVE_CHANNEL_2)//PB3
		{
			if(HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_15) == GPIO_PIN_RESET)//PA15
			{
				distance++;
			}
			else
			{
				distance--;
			}
		}
		else//PA15
		{
			if(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_3) == GPIO_PIN_SET)//PB3
			{
				distance++;
			}
			else
			{
				distance--;
			}			
		}
	}
	else if(htim->Instance == TIM3)
	{
		uint8_t PB5 = HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_5);
		uint8_t PB4 = HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_4);
		if(htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1)
		{
			if(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_5) == GPIO_PIN_RESET)//PB5
			{
				RL_encoder++;
			}
			else
			{
				RL_encoder--;
			}
		}
		else
		{
			if(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_4) == GPIO_PIN_SET)//PB4
			{
				RL_encoder++;
			}
			else
			{
				RL_encoder--;
			}			
		}	
	}
	else if(htim->Instance == TIM4)
	{
		uint8_t PB9 = HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_9);
		uint8_t PB8 = HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_8);
		uint8_t PD12 = HAL_GPIO_ReadPin(GPIOD,GPIO_PIN_12);
		uint8_t PD13 = HAL_GPIO_ReadPin(GPIOD,GPIO_PIN_13);
		if(htim->Channel == HAL_TIM_ACTIVE_CHANNEL_3)//PB8
		{
			if(PB9 == GPIO_PIN_RESET)//PB9
			{
				FL_encoder++;
			}
			else
			{
				FL_encoder--;
			}
		}
		else if(htim->Channel == HAL_TIM_ACTIVE_CHANNEL_4)//PB9
		{
			if(PB8 == GPIO_PIN_SET)//PB8
			{
				FL_encoder++;
			}
			else
			{
				FL_encoder--;
			}			
		}
		else if(htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1)//PD12
		{
			if(PD13 == GPIO_PIN_RESET)//PD13
			{
				FR_encoder++;
			}
			else
			{
				FR_encoder--;
			}					
		}
		else if(htim->Channel == HAL_TIM_ACTIVE_CHANNEL_2)//PD13
		{
			if(PD12 == GPIO_PIN_SET)//PD12
			{
				FR_encoder++;
			}
			else
			{
				FR_encoder--;
			}					
		}
	}
	else if(htim->Instance == TIM12)
	{
		uint8_t PB15 = HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_15);
		uint8_t PB14 = HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_14);
		if(htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1)//PB14
		{
			if(PB15 == GPIO_PIN_RESET)//PB15
			{
				RR_encoder++;
			}
			else
			{
				RR_encoder--;
			}
		}
		else//PB15
		{
			if(PB14 == GPIO_PIN_SET)//PB14
			{
				RR_encoder++;
			}
			else
			{
				RR_encoder--;
			}			
		}		
	}
}

void encoder_init(void)
{
//	htim4.Instance->CNT = 0xFFFF/2;
//	HAL_TIM_Encoder_Start(&htim4, TIM_CHANNEL_ALL);
	distance = 0;
	//FL
	HAL_TIM_IC_Start_IT(&htim4, TIM_CHANNEL_3);//PB8
	HAL_TIM_IC_Start_IT(&htim4, TIM_CHANNEL_4);//PB9
	//FR
	HAL_TIM_IC_Start_IT(&htim3, TIM_CHANNEL_1);//PB4
	HAL_TIM_IC_Start_IT(&htim3, TIM_CHANNEL_2);//PB5
	//RL
	HAL_TIM_IC_Start_IT(&htim2, TIM_CHANNEL_2);//PB3
	HAL_TIM_IC_Start_IT(&htim2, TIM_CHANNEL_1);//PA15
	//RR
	HAL_TIM_IC_Start_IT(&htim12, TIM_CHANNEL_1);//PB14
	HAL_TIM_IC_Start_IT(&htim12, TIM_CHANNEL_2);//PB15
	//Move_arm
	HAL_TIM_IC_Start_IT(&htim4, TIM_CHANNEL_1);//PD12
	HAL_TIM_IC_Start_IT(&htim4, TIM_CHANNEL_2);//PD13
	
}
//short get_encoder(void)
//{
//	short temp;
//	temp = (short)(htim4.Instance->CNT-0xFFFF/2);
//	htim4.Instance->CNT = 0xFFFF/2;
//	return temp;
//}

