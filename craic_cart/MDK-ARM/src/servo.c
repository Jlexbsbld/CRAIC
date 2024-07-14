#include "servo.h"
#include "pwm.h"
#include "stdio.h"
#include "math.h"
#include "stdbool.h"

#define pi (3.1415926)
extern TIM_HandleTypeDef htim1,htim2,htim3,htim4,htim5,htim6,htim7,htim8,htim9,htim12;

int16_t Servo_angle[6]={0};
float Arm_angle[3] = {0.0f};
int16_t x,y;
int8_t angle;
float duty = 0;
double cos_angle,sin_angle;
double length_1,length_2,length_3,length_dd;
double x_dd,y_dd,x_d,y_d;
double angle_dd,angle_dd1,angle_dd2;
float angle_1,angle_2,angle_3;

bool Servo_mode[6] = {true,true,true,true,true,true};

void servo_init(void)
{
	
	set_pwm(&htim5,TIM_CHANNEL_1,Servo_freq,0); //50Hz,占空比为0 PA0
	set_pwm(&htim5,TIM_CHANNEL_2,Servo_freq,0); //50Hz,占空比为0 PA1
	set_pwm(&htim3,TIM_CHANNEL_3,Servo_freq,0); //50Hz,占空比为0 PB0
	set_pwm(&htim3,TIM_CHANNEL_4,Servo_freq,0); //50Hz,占空比为0 PB1
	set_pwm(&htim2,TIM_CHANNEL_4,Servo_freq,0); //50Hz,占空比为0 PB11
	set_pwm(&htim2,TIM_CHANNEL_3,Servo_freq,0); //50Hz,占空比为0 PB10
	
	HAL_TIM_PWM_Start(&htim5, TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(&htim5, TIM_CHANNEL_2);
	HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_3);
	HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_4);
	HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_4);
	HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_3);
}

void Servo_control(Servo_device select,float angle)
{
	//度转占空比
	if(select == Servo1)
	{
		if(Servo_mode[0])
			duty = angle_0_duty+(angle/180.0f)*(angle_max_duty-angle_0_duty);
		else
			duty = angle_0_duty+(angle/270.0f)*(angle_max_duty-angle_0_duty);
		set_duty(&htim5,TIM_CHANNEL_1,(uint16_t)duty);//PA0
	}
	else if(select == Servo2)
	{
		if(Servo_mode[1])
			duty = angle_0_duty+(angle/180.0f)*(angle_max_duty-angle_0_duty);
		else
			duty = angle_0_duty+(angle/270.0f)*(angle_max_duty-angle_0_duty);
		set_duty(&htim5,TIM_CHANNEL_2,(uint16_t)duty);//PA1
	}
	else if(select == Servo3)
	{
		if(Servo_mode[2])
			duty = angle_0_duty+(angle/180.0f)*(angle_max_duty-angle_0_duty);
		else
			duty = angle_0_duty+(angle/270.0f)*(angle_max_duty-angle_0_duty);
		set_duty(&htim3,TIM_CHANNEL_3,(uint16_t)duty);//PB0			
	}
	else if(select == Servo4)
	{
		if(Servo_mode[3])
			duty = angle_0_duty+(angle/180.0f)*(angle_max_duty-angle_0_duty);
		else
			duty = angle_0_duty+(angle/270.0f)*(angle_max_duty-angle_0_duty);
		set_duty(&htim3,TIM_CHANNEL_4,(uint16_t)duty);//PB1		
	}
	else if(select == Servo5)
	{
		if(Servo_mode[4])
			duty = angle_0_duty+(angle/180.0f)*(angle_max_duty-angle_0_duty);
		else
			duty = angle_0_duty+(angle/270.0f)*(angle_max_duty-angle_0_duty);
		set_duty(&htim2,TIM_CHANNEL_4,(uint16_t)duty);//PB11		
	}
	else if(select == Servo6)
	{
		if(Servo_mode[5])
			duty = angle_0_duty+(angle/180.0f)*(angle_max_duty-angle_0_duty);
		else
			duty = angle_0_duty+(angle/270.0f)*(angle_max_duty-angle_0_duty);
		set_duty(&htim2,TIM_CHANNEL_3,(uint16_t)duty);//PB10		
	}
}

void Arm_work(void)
{
	bool reverse = false;
	x_d = x;
	y_d = y;
	if(x_d<0)
	{
		x_d = -x_d;
		reverse = true;
	}
	length_1 = 83.0;//63.5
	length_2 = 79.0;
	length_3 = 100.0;
	cos_angle = cos(angle/180.0*pi);
	sin_angle = sin(angle/180.0*pi);
	x_dd = x_d - length_3*cos_angle;
	y_dd = y_d - length_3*sin_angle;
	length_dd = sqrt(x_dd*x_dd+y_dd*y_dd);
	if(x_dd!=0)
	{
		angle_dd = atan2(y_dd,x_dd)/pi*180;
	}
	else
	{
		angle_dd = 90;
	}
	if(angle_dd < 0)
	{
		angle_dd = angle_dd + 180;
	}
	angle_dd1 = acos((length_dd*length_dd+length_1*length_1-length_2*length_2)/(2*length_dd*length_1))/pi*180;
	angle_1 = angle_dd+angle_dd1;
	if(angle_1 > 180)
			angle_1 = 180;
	else if(angle_1 < 0)
			angle_1 = 0;
	angle_dd2 = acos((length_1*length_1+length_2*length_2-length_dd*length_dd)/(2*length_2*length_1))/pi*180;
	angle_2 = 135-(180-angle_dd2);
	if(angle_2 > 270)
			angle_2 = 270;
	else if(angle_2 < 0)
			angle_2 = 0;
	angle_3 = angle+270-angle_1-angle_2;
	if(angle_3 > 270)
			angle_3 = 270;
	else if(angle_3 < 0)
			angle_3 = 0;
	if(reverse)
	{
		angle_1 = 180-angle_1;
		angle_2 = 270-angle_2;
		angle_3 = 270-angle_3;
		Servo_control(Servo1,180-angle_1+9);
		Servo_control(Servo4,angle_2-6.5f);
		Servo_control(Servo2,270-angle_3+4.5f);
	}
	else
	{
		Servo_control(Servo1,180-angle_1+4.5f);
		Servo_control(Servo4,angle_2-0.5f);
		Servo_control(Servo2,270-angle_3+4.5f);
	}
}
