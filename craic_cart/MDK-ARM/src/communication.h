#ifndef __COMMUNICATION_H
#define __COMMUNICATION_H

#include "stm32f4xx_hal.h"
#include "motor.h"
#include "usart.h"
#include "tim.h"
#include "stdbool.h"
#include "stdio.h"
#include "servo.h"
#include "led.h"
#include "beep.h"
#include "fan.h"

#define BufferSize 1000
//设备类型
#define MotorDevice 0x01
#define ServoDevice 0x02
#define ArmDevice 0x03
#define LedDevice 0x04
#define BeepDevice 0x05
#define FanDevice 0x06
//设备标签
#define LeftFront 0x01
#define LeftRear 0x02
#define RightFront 0x03
#define RightRear 0x04
#define ServoOne 0x05
#define ServoTwo 0x06
#define ServoThree 0x07
#define ServoFour 0x08
#define ServoFive 0x09
#define ServoSix 0x0A
#define RED_light 0x0B
#define BLUE_light 0x0C
#define GREEN_light 0x0D
#define PURPLE_light 0x0E
#define YELLOW_light 0x0F
#define LIGHTBLUE_light 0x10
#define WHITE_light 0x11
#define BLACK_light 0x12
#define Beep_OPEN 0x13
#define Beep_CLOSE 0x14
#define Fan_OPEN 0x15
#define Fan_CLOSE 0x16

//帧定界 帧头和帧尾
#define packgehead 0x33
#define packgetail 0x61

//指令模式
#define Asynchronous 0x01
#define Synchronous 0x02
// 0为测试模式,1为校验模式,测试模式下不需要加校验码
#define unpack_mode (0)



void communication_init(void);
void packgeAnalysis(void);
extern uint8_t rxBuffer[BufferSize];
extern uint8_t dataLength;
extern uint32_t checkSum;
extern bool unpack_flag;
extern uint8_t rxNum;

#endif
