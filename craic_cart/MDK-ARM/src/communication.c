#include "communication.h"

uint8_t rxData;
uint8_t rxBuffer[BufferSize];
uint8_t rxNum = 0;
uint8_t dataLength;
uint32_t checkSum;
uint8_t frameStart;
uint8_t frameEnd;
bool unpack_flag = false;

void communication_init(void)
{
	//HAL_UART_Receive_DMA(&huart3, &rxData, 1);
	HAL_UARTEx_ReceiveToIdle_DMA(&huart3, rxBuffer, BufferSize);
	//HAL_UART_Receive_IT(&huart3, &rxData, 1);
}
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	//if(huart->Instance == USART3)
	//{
//	if(rxNum < sizeof(rxBuffer))
//	{
//		rxBuffer[rxNum] = rxData;
//		rxNum++;
//	}
//	htim11.Instance->CNT = 0;
//	HAL_UART_Receive_DMA(&huart3, &rxData, 1);
	//HAL_UART_Receive_IT(&huart3, &rxData, 1);
	//}
}
void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
	rxNum = Size;
	packgeAnalysis();
	//HAL_UARTEx_ReceiveToIdle_DMA(&huart1, rxBuffer, BufferSize);
	HAL_UARTEx_ReceiveToIdle_DMA(&huart3, rxBuffer, BufferSize);
}
//包分析
void packgeAnalysis(void)
{
	if(rxNum > 0)
	{
		if(unpack_mode)
		{
			for(uint8_t i=0;i<rxNum;i++)
			{
				if(rxBuffer[i] == packgehead)
				{
					//确定帧的起始位置和终止位置
					frameStart = i;
					dataLength = rxBuffer[frameStart+1];
					//为了防止上位机突然中断残余数据导致的越界问题
					if(dataLength == 0 || i+dataLength > rxNum)
					{
						continue;
					}
					frameEnd = frameStart+dataLength-1;
					checkSum = 0;
					if(rxBuffer[frameEnd] == packgetail)
					{
						//只对数据部分做校验
						for(uint8_t i = frameStart+1;i<=frameEnd-2;i++)
						{
							checkSum+=rxBuffer[i];
						}
						//checkSum+=packgehead;
						//checkSum+=packgetail;
						if((checkSum&0xff) == rxBuffer[frameEnd-1])
						{
							i+=dataLength-1;
							switch(rxBuffer[frameStart+2])
							{
								case MotorDevice:
								{
									if(rxBuffer[frameStart+3] == Asynchronous)
									{
										switch(rxBuffer[frameStart+4])
										{
											case LeftFront:{
												FL_target_speed = (int8_t)rxBuffer[frameStart+5];
											}break;
											case LeftRear:{
												FR_target_speed = (int8_t)rxBuffer[frameStart+5];
											}break;
											case RightFront:{
												RL_target_speed = (int8_t)rxBuffer[frameStart+5];
											}break;
											case RightRear:{
												RR_target_speed = (int8_t)rxBuffer[frameStart+5];
											}break;
										}
									}
									else if(rxBuffer[frameStart+3] == Synchronous)
									{
										FL_target_speed = (int8_t)rxBuffer[frameStart+4];
										FR_target_speed = (int8_t)rxBuffer[frameStart+5];
										RL_target_speed = (int8_t)rxBuffer[frameStart+6];
										RR_target_speed = (int8_t)rxBuffer[frameStart+7];
									}
								}break;
							}
						}
					}
				}
			}
		}
		else
		{
			for(uint8_t i=0;i<rxNum;i++)
			{
				if(rxBuffer[i] == packgehead)
				{
					//确定帧的起始位置和终止位置
					frameStart = i;
					dataLength = rxBuffer[frameStart+1];
					//为了防止上位机突然中断残余数据导致的越界问题
					if(dataLength == 0 || i+dataLength > rxNum)
					{
						continue;
					}
					frameEnd = frameStart+dataLength-1;
					if(rxBuffer[frameEnd] == packgetail)
					{
						i+=dataLength-1;
						switch(rxBuffer[frameStart+2])
						{
							case MotorDevice:
							{
								if(rxBuffer[frameStart+3] == Asynchronous)
								{
									switch(rxBuffer[frameStart+4])
									{
										case LeftFront:{
											FL_target_speed = (int8_t)rxBuffer[frameStart+5];
										}break;
										case LeftRear:{
											FR_target_speed = (int8_t)rxBuffer[frameStart+5];
										}break;
										case RightFront:{
											RL_target_speed = (int8_t)rxBuffer[frameStart+5];
										}break;
										case RightRear:{
											RR_target_speed = (int8_t)rxBuffer[frameStart+5];
										}break;
									}
								}
								else if(rxBuffer[frameStart+3] == Synchronous)
								{
									FL_target_speed = (int8_t)rxBuffer[frameStart+4];
									RL_target_speed = (int8_t)rxBuffer[frameStart+5];
									FR_target_speed = (int8_t)rxBuffer[frameStart+6];
									RR_target_speed = (int8_t)rxBuffer[frameStart+7];
								}
							}break;
							case ServoDevice:
							{
								if(rxBuffer[frameStart+3] == Asynchronous)
								{
									switch(rxBuffer[frameStart+4])
									{
										case ServoOne:{
											Servo_angle[0] = (rxBuffer[frameStart+5]<<8)+rxBuffer[frameStart+6];
											Servo_control(Servo1,1.0f*Servo_angle[0]);
										}break;
										case ServoTwo:{
											Servo_angle[1] = (rxBuffer[frameStart+5]<<8)+rxBuffer[frameStart+6];
											Servo_control(Servo2,1.0f*Servo_angle[1]);
										}break;
										case ServoThree:{
											Servo_angle[2] = (rxBuffer[frameStart+5]<<8)+rxBuffer[frameStart+6];
											Servo_control(Servo3,1.0f*Servo_angle[2]);
										}break;
										case ServoFour:{
											Servo_angle[3] = (rxBuffer[frameStart+5]<<8)+rxBuffer[frameStart+6];
											Servo_control(Servo4,1.0f*Servo_angle[3]);
										}break;
										case ServoFive:{
											Servo_angle[4] = (rxBuffer[frameStart+5]<<8)+rxBuffer[frameStart+6];
											Servo_control(Servo5,1.0f*Servo_angle[4]);
										}break;
										case ServoSix:{
											Servo_angle[5] = (rxBuffer[frameStart+5]<<8)+rxBuffer[frameStart+6];
											Servo_control(Servo6,1.0f*Servo_angle[5]);
										}break;
									}									
								}
								else if(rxBuffer[frameStart+3] == Synchronous)
								{
									Servo_angle[0] = rxBuffer[frameStart+4];
									Servo_angle[1] = rxBuffer[frameStart+5];
									Servo_angle[2] = rxBuffer[frameStart+6];
									Servo_angle[3] = rxBuffer[frameStart+7];
									Servo_angle[4] = rxBuffer[frameStart+8];
									Servo_angle[5] = rxBuffer[frameStart+9];
									Servo_control(Servo1,1.0f*Servo_angle[0]);
									Servo_control(Servo2,1.0f*Servo_angle[1]);
									Servo_control(Servo3,1.0f*Servo_angle[2]);
									Servo_control(Servo4,1.0f*Servo_angle[3]);
									Servo_control(Servo5,1.0f*Servo_angle[4]);
									Servo_control(Servo6,1.0f*Servo_angle[5]);
								}										
							}break;
							case ArmDevice:
							{
								if(rxBuffer[frameStart+3] == Asynchronous)
								{
								}
								else if(rxBuffer[frameStart+3] == Synchronous)
								{
									x = (rxBuffer[frameStart+4]<<8)+rxBuffer[frameStart+5];
									y = (rxBuffer[frameStart+6]<<8)+rxBuffer[frameStart+7];
									angle = rxBuffer[frameStart+8];
									Arm_work();
								}										
							}break;
							case LedDevice:
							{
								if(rxBuffer[frameStart+3] == RED_light)
								{
									led_state = red;
								}
								else if(rxBuffer[frameStart+3] == BLUE_light)
								{
									led_state = blue;
								}
								else if(rxBuffer[frameStart+3] == GREEN_light)
								{
									led_state = green;
								}
								else if(rxBuffer[frameStart+3] == PURPLE_light)
								{
									led_state = purple;
								}
								else if(rxBuffer[frameStart+3] == YELLOW_light)
								{
									led_state = yellow;
								}
								else if(rxBuffer[frameStart+3] == LIGHTBLUE_light)
								{
									led_state = lightblue;
								}
								else if(rxBuffer[frameStart+3] == WHITE_light)
								{
									led_state = white;
								}
								else if(rxBuffer[frameStart+3] == BLACK_light)
								{
									led_state = black;
								}								
							}break;
							case BeepDevice:
							{
								if(rxBuffer[frameStart+3] == Beep_OPEN)
								{
									Beep_ON
								}
								if(rxBuffer[frameStart+3] == Beep_CLOSE)
								{
									Beep_OFF
								}									
							}break;
							case FanDevice:
							{
								if(rxBuffer[frameStart+3] == Fan_OPEN)
								{
									FAN_ON
								}
								if(rxBuffer[frameStart+3] == Fan_CLOSE)
								{
									FAN_OFF
								}						
							}
						}
					}
				}
			}
		}
		rxNum = 0;
	}
}
