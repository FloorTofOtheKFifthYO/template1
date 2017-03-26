#include "flywheel_left.h"
#include <string.h>
#include "auto.h"
#include <math.h>

#define PITCH_MAXSPEED 500
#define YAW_MAXSPEED 50

Flywheel_left flywheel_left;

static int fly_count = 0;
static int fly_n = 0;//记录打出的个数

/**
  * @brief  初始化
  *     
  * @note
  *
  * @param 
  *          
  * @retval 
  */
void flywheel_left_init()
{
	//角度电机的初始化
	RoboModule_RESET(YAW_ID_LEFT,PITCH_ID_LEFT,0);
	delay_ms(5);
	RoboModule_CHOICE_mode(PositionMode ,YAW_ID_LEFT ,PITCH_ID_LEFT,0);
	RoboModule_Add_Callback(databack,RoboModule_Feedback_Callback,YAW_ID_LEFT,PITCH_ID_LEFT,0);
	RoboModule_SETUP(2,0,YAW_ID_LEFT,PITCH_ID_LEFT,0);
	
	//从板设置
	flywheel_left.left.can_id = CLIENT_ID_LEFT;
	Client_RESET(flywheel_left.left);
	flywheel_left.left.pwm1 = 770;
	flywheel_left.left.pwm2 = 600;
	memset(flywheel_left.left.relay,0,sizeof(flywheel_left.left.relay));
	Client_SET(flywheel_left.left);
	
	flywheel_left.pur_duty = 7.7;
	flywheel_left.pur_jmp = 6;
	flywheel_left.pur_pitch = 0;
	flywheel_left.pur_yaw = 0;
	flywheel_left.state = fly_ready;
	flywheel_left.fly_flag = false;
}

/**
  * @brief  改俯仰角
  *     
  * @note
  *
  * @param 俯仰角
  *          
  * @retval 
  */
void flywheel_left_setPitch(float pitch)
{
	RoboModule_SET_Position(PITCH_ID_LEFT,5000,pitch*100,PITCH_MAXSPEED);
}

void flywheel_left_fly()
{
	flywheel_left.left.relay[FLY_LEFT] = 1 - flywheel_left.left.relay[FLY_LEFT];
	Client_SET(flywheel_left.left);
}

/**
  * @brief  改无刷
  *     
  * @note
  *
  * @param 占空比
  *          
  * @retval 
  */
void flywheel_left_setBrushless(float duty)
{
	flywheel_left.left.pwm1 = (u16)duty*100;
	Client_SET(flywheel_left.left);
}


void flywheel_left_setJmp(float duty)
{
	flywheel_left.left.pwm2 = (u16)duty*100;
	Client_SET(flywheel_left.left);
}

/**
  * @brief  改旋转平台
  *     
  * @note
  *
  * @param 旋转平台
  *          
  * @retval 
  */
void flywheel_left_setYaw(float yaw)
{
	RoboModule_SET_Position(YAW_ID_LEFT,5000,yaw*10000,YAW_MAXSPEED);
}

/**
  * @brief  tim2中调用，进行延时等操作
  *     
  * @note
  *
  * @param 旋转平台
  *          
  * @retval 
  */
void flywheel_left_TIM()
{
	if(fly_count != 0)
		fly_count--;
}

/**
  * @brief 完成n次飞
  *     
  * @note int n,	次数
		float duty, 无刷占空比
		float roll,	俯仰角
		float turn  转角
  *
  * @param 旋转平台
  *          
  * @retval 
  */
void flywheel_left_flyn(int n, float duty, float pitch, float yaw, float jmp)
{
	fly_n = n*2;
	flywheel_left.pur_duty = duty;
	flywheel_left.pur_pitch = pitch;
	flywheel_left.pur_jmp = jmp;
	flywheel_left.pur_yaw = yaw;
	flywheel_left.fly_flag = true;
}

/*
	检查姿态是否调整到位
	用于状态转移
*/
bool flywheel_left_check()
{
	if(fabs(ReturnData(PITCH_ID_LEFT)->Position -flywheel_left.pur_pitch*10000)<=10 && 
		fabs(ReturnData(YAW_ID_LEFT)->Position -flywheel_left.pur_yaw*10000)<=50)
		return true;
	else
		return false;
}

void flywheel_left_stop()
{
	flywheel_left_setBrushless(7.7);
	flywheel_left_setPitch(ReturnData(PITCH_ID_LEFT)->Position/100.f);
	flywheel_left_setYaw(ReturnData(YAW_ID_LEFT)->Position/10000.f);
	flywheel_left_setJmp(6);
}

/**
  * @brief main大循环中检查
  *     
  * @note
  *
  * @param
  *          
  * @retval 
  */
void flywheel_left_main()
{
		switch(flywheel_left.state)
		{
			case fly_ready:
				if(flywheel_left.fly_flag)
				{
					flywheel_left_setPitch(flywheel_left.pur_pitch);
					flywheel_left_setYaw(flywheel_left.pur_yaw);
					flywheel_left.state = fly_adj;
					flywheel_left.left.pwm1 = flywheel_left.pur_duty*100;
					flywheel_left.left.pwm2 = flywheel_left.pur_jmp*100;
					flywheel_left.left.relay[FLY_LEFT] = 0;
					Client_SET(flywheel_left.left);
				}
				break;
			case fly_adj:
				if(flywheel_left_check() && autorun.state == pos_arrived)
				{
					flywheel_left.state = fly;
					if(fly_n == 0)
					{
						fly_count = 0;
						flywheel_left.state = fly_ready;
						flywheel_left.fly_flag = false;
					}else{
						flywheel_left_fly();
						fly_count = 200;
						fly_n--;
					}
				}
				break;
			case fly:
					if(fly_count == 0)
					{
						if(fly_n == 0)
						{
							fly_count = 0;
							flywheel_left.state = fly_ready;
							flywheel_left.fly_flag = false;
						}else{
							flywheel_left_fly();
							fly_count = 200;
							fly_n--;
						}
					}
				break;
		}
}
