#include "key.h"

key keys[4] = {{GPIO_PIN_SET,0,0},{GPIO_PIN_SET,0,0},{GPIO_PIN_SET,0,0},{GPIO_PIN_SET,0,0}};

extern TaskHandle_t KeyTask_Handler;
void key_scan(void)
{
	keys[0].key_state = HAL_GPIO_ReadPin(GPIOE,GPIO_PIN_0);
	keys[1].key_state = HAL_GPIO_ReadPin(GPIOE,GPIO_PIN_1);
	keys[2].key_state = HAL_GPIO_ReadPin(GPIOC,GPIO_PIN_14);
	keys[3].key_state = HAL_GPIO_ReadPin(GPIOC,GPIO_PIN_15);
	for(uint8_t i=0;i<4;i++)
	{
		switch(keys[i].key_judge)
		{
			case 0:{
				if(keys[i].key_state == GPIO_PIN_RESET)
				{
					keys[i].key_judge++;
				}
			}break;
			case 1:{
				if(keys[i].key_state == GPIO_PIN_RESET)
				{
					keys[i].key_judge++;
					keys[i].key_flag = 1;
					vTaskNotifyGiveFromISR((TaskHandle_t)KeyTask_Handler,NULL);   /* 接收任务通知的任务句柄 */
				}
				else
				{
					keys[i].key_judge = 0;
				}
			}break;
			case 2:{
				if(keys[i].key_state == GPIO_PIN_RESET)
				{
					
				}
				else
				{
					keys[i].key_judge = 0;
				}		
			}break;
		}
	}
}

