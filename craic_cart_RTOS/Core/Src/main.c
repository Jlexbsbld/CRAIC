/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "cmsis_os.h"
#include "dma.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"
#include "fsmc.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "beep.h"
#include "key.h"
#include "pwm.h"
#include "led.h"
#include "encoder.h"
#include "motor.h"
#include "stdio.h"
#include "communication.h"
#include "servo.h"
#include "lcd.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
__IO uint32_t lcdTick = 0;
char text[30];
TaskHandle_t StartTask_Handler,KeyTask_Handler,LcdTask_Handler,CommunicationalTask_Handler;
TickType_t Lcd_xLastWakeTime;
BaseType_t task;
uint32_t key_task_count=0,lcd_task_count=0,communicational_task_count=0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void MX_FREERTOS_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void lcd_task(void)
{
	Lcd_xLastWakeTime = xTaskGetTickCount();
	while(1)
	{
		vTaskDelayUntil(&Lcd_xLastWakeTime,50);
		//taskENTER_CRITICAL(); /* 进入临界区 */
		lcd_task_count++;
		sprintf(text,"Hello!");
		lcd_show_string(0,0,320,32,32,text,BLACK);
		//taskEXIT_CRITICAL(); /* 退出临界区 */
	}
}
void key_task(void)
{
	while(1)
	{
		ulTaskNotifyTake((BaseType_t)pdTRUE,(TickType_t)portMAX_DELAY);//等待通知进入阻塞状态
		//taskENTER_CRITICAL(); /* 进入临界区 */
		key_task_count++;
		if(keys[0].key_flag == 1)
		{
			printf("1\n");
			keys[0].key_flag = 0;
		}
		if(keys[1].key_flag == 1)
		{
			printf("2\n");
			keys[1].key_flag = 0;
		}
		if(keys[2].key_flag == 1)
		{
			printf("3\n");
			keys[2].key_flag = 0;
		}
		if(keys[3].key_flag == 1)
		{
			printf("4\n");
			Beep_Toggle
			keys[3].key_flag = 0;
		}
		//taskEXIT_CRITICAL(); /* 退出临界区 */
	}
}
void communicational_task(void *pvParameters)
{
	//package* databuffer;
	while(1)
	{
		ulTaskNotifyTake((BaseType_t)pdTRUE,(TickType_t)portMAX_DELAY);//等待通知进入阻塞状态
		//xQueueReceive(xQueue_datastream, &databuffer, portMAX_DELAY);//队列为空进入阻塞状态
		//taskENTER_CRITICAL(); /* 进入临界区 */
		//packgeAnalysis(databuffer->Buffer,databuffer->rxNum); 
		communicational_task_count++;
		packgeAnalysis(communicate_data.Buffer,communicate_data.rxNum); //communicate_data
		//taskEXIT_CRITICAL(); /* 退出临界区 */
	}
}
int fputc(int ch, FILE *f) 
{
  /* Your implementation of fputc(). */
	HAL_UART_Transmit(&huart3, (uint8_t *)&ch, 1, 50);
  return ch;
}
void start_task(void *pvParameters)
{
	taskENTER_CRITICAL(); /* 进入临界区 */
	//xQueue_datastream = xQueueCreate(QUEUE_TRANSMIT_LENGTH, QUEUE_TRANSMIT_ITEM_SIZE);
	 /* 创建任务 1 */
	xTaskCreate((TaskFunction_t )key_task,
						 (const char* )"key_task",
						 (uint16_t )64,
						 (void* )NULL,
						 (UBaseType_t )1,
						 (TaskHandle_t* )&KeyTask_Handler);
	 /* 创建任务 2 */
	xTaskCreate((TaskFunction_t )lcd_task,
						 (const char* )"lcd_task",
						 (uint16_t )64,
						 (void* )NULL,
						 (UBaseType_t )1,
						 (TaskHandle_t* )&LcdTask_Handler);
	/* 创建任务 3 */
	xTaskCreate((TaskFunction_t )communicational_task,
						 (const char* )"communicational_task",
						 (uint16_t )64,
						 (void* )NULL,
						 (UBaseType_t )2,
						 (TaskHandle_t* )&CommunicationalTask_Handler);
	vTaskDelete(StartTask_Handler); /* 删除开始任务 */
	taskEXIT_CRITICAL(); /* 退出临界区 */
}
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_TIM1_Init();
  MX_TIM2_Init();
  MX_TIM3_Init();
  MX_TIM4_Init();
  MX_TIM8_Init();
  MX_TIM9_Init();
  MX_TIM12_Init();
  MX_USART3_UART_Init();
  MX_SPI1_Init();
  MX_TIM5_Init();
  MX_TIM6_Init();
  MX_TIM7_Init();
  MX_TIM10_Init();
  MX_TIM11_Init();
  MX_USART1_UART_Init();
  MX_FSMC_Init();
  MX_TIM13_Init();
  MX_TIM14_Init();
  /* USER CODE BEGIN 2 */
	//printf("Hello\r\n");
	htim11.Instance->ARR = 65535;
	communication_init();
	lcd_init();
	motor_init();
	encoder_init();
	servo_init();
	fan_init();
	HAL_TIM_Base_Start_IT(&htim6);
	HAL_TIM_Base_Start_IT(&htim10);
	//HAL_TIM_Base_Start_IT(&htim14);
	task = xTaskCreate((TaskFunction_t )start_task, /* 任务函数 */
							 (const char* )"start_task", /* 任务名称 */
							 (uint16_t )64, /* 任务堆栈大小 */
							 (void* )NULL, /* 传入给任务函数的参数 */
							 (UBaseType_t )1, /* 任务优先级 */
							 (TaskHandle_t* )&StartTask_Handler); /* 任务句柄 */
	if(task == pdPASS)
	{
		//说明任务创建成功
	}
  /* USER CODE END 2 */

  /* Call init function for freertos objects (in freertos.c) */
  MX_FREERTOS_Init();
  /* Start scheduler */
  osKernelStart();

  /* We should never get here as control is now taken by the scheduler */
  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 100;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
