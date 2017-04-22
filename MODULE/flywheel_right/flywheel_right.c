#include "flywheel_right.h"
#include "usart.h"
#include <string.h>
#include "auto.h"
#include <math.h>

/**
	高的发射机构，
	有3个自由度，分别叫 pitch, turn, yaw.都是电机。
	1. 更改can节点号 在configuration.h中更改
	2. 改电机的驱动方式，分别在初始化，set，判定到位和回到原点的函数中更改
*/

#define PITCH_MAXSPEED 500
#define YAW_MAXSPEED 300
#define TURN_MAXSPEED 50

Flywheel_right flywheel_right;

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
void flywheel_right_init()
{
	int i;
	//角度电机的初始化
	RoboModule_RESET(YAW_ID_RIGHT,PITCH_ID_RIGHT, TURN_ID_RIGHT,0);
	delay_ms(500);
	RoboModule_CHOICE_mode(PositionMode ,YAW_ID_RIGHT ,PITCH_ID_RIGHT, TURN_ID_RIGHT,0);
	delay_ms(500);
	RoboModule_Add_Callback(databack,RoboModule_Feedback_Callback,YAW_ID_RIGHT,TURN_ID_RIGHT ,PITCH_ID_RIGHT,0);
	RoboModule_SETUP(49,0,YAW_ID_RIGHT,PITCH_ID_RIGHT, TURN_ID_RIGHT,0);
	
	flywheel_right_setTurn(0);
	
	//从板设置
	flywheel_right.right.can_id = CLIENT_ID_RIGHT;
	Client_RESET(flywheel_right.right);
	delay_ms(50);
	flywheel_right.right.pwm1 = 400;
	flywheel_right.right.pwm2 = 600;
	for(i = 0;i<4;i++)
		flywheel_right.right.relay[i] = 0;
	Client_SET(flywheel_right.right);
	
	flywheel_right.pur_duty = 4;
	flywheel_right.pur_jmp = 6;
	flywheel_right.pur_pitch = 0;
	flywheel_right.pur_yaw = 0;
	flywheel_right.pur_turn = 0;
	flywheel_right.state = fly_r_finish;
	flywheel_right.fly_flag = false;
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
void flywheel_right_setPitch(float pitch)
{
	RoboModule_SET_Position(PITCH_ID_RIGHT,5000,pitch*100,PITCH_MAXSPEED);
}

/**
  * @brief  改上旋转角
  *     
  * @note
  *
  * @param 旋转角
  *          
  * @retval 
  */
void flywheel_right_setTurn(float turn)
{
//	RoboModule_SET_Position(PITCH_ID_RIGHT,5000,turn*10000,TURN_MAXSPEED);
	RoboModule_SET_Positiono(TURN_ID_RIGHT,500,turn*10000);
}

void flywheel_right_fly()
{
	flywheel_right.right.relay[FLY_RIGHT] = 1 - flywheel_right.right.relay[FLY_RIGHT];
	Client_SET(flywheel_right.right);
}

void flywheel_right_up()
{
	flywheel_right.right.relay[UP_RIGHT] = 1 - flywheel_right.right.relay[UP_RIGHT];
	Client_SET(flywheel_right.right);
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
void flywheel_right_setBrushless(float duty)
{
	flywheel_right.right.pwm1 = duty*100;
	Client_SET(flywheel_right.right);
}


void flywheel_right_setJmp(float duty)
{
	flywheel_right.right.pwm2 = duty*100;
	Client_SET(flywheel_right.right);
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
void flywheel_right_setYaw(float yaw)
{
	RoboModule_SET_Position(YAW_ID_RIGHT,5000,yaw*10000,YAW_MAXSPEED);
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
void flywheel_right_TIM()
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
void flywheel_right_flyn(int n, float duty, float turn, float pitch, float yaw, float jmp)
{
	fly_n = n * 2;
	flywheel_right.pur_duty = duty;
	flywheel_right.pur_pitch = pitch;
	flywheel_right.pur_turn = turn;
	flywheel_right.pur_yaw = yaw;
	flywheel_right.pur_jmp = jmp;
	flywheel_right.fly_flag = true;
}

/*
	检查姿态是否调整到位
	用于状态转移
*/

bool flywheel_right_check()
{
	static int flag = 1;
	static int flag1 = 1;
	static int flag2 = 1;
	/*USART_SendString(bluetooth,"msg: pitch:%d yaw:%d turn:%d\n",ReturnData(PITCH_ID_RIGHT)->Position,
					ReturnData(YAW_ID_RIGHT)->Position,
					ReturnData(TURN_ID_RIGHT)->Position);*/
	if(fabs(ReturnData(PITCH_ID_RIGHT)->Position -flywheel_right.pur_pitch*100)<=30 )
	{
			if(flag != 0)
			{
				flag = 0;
				USART_SendString(bluetooth,"msg: pitch:%d\n",ReturnData(PITCH_ID_RIGHT)->Position);
			}
		if(fabs(ReturnData(YAW_ID_RIGHT)->Position -flywheel_right.pur_yaw*10000)<=30)
		{
			if(flag1 != 0)
			{
				flag1 = 0;
				USART_SendString(bluetooth,"msg: yaw:%d\n",ReturnData(YAW_ID_RIGHT)->Position);
			}
			if(fabs(ReturnData(TURN_ID_RIGHT)->Position -flywheel_right.pur_turn*10000)<=30)
			{
				if(flag2 != 0)
				{
					flag2 = 0;
					fly_count = 5000;
					USART_SendString(bluetooth,"msg: turn:%d\n",ReturnData(TURN_ID_RIGHT)->Position);
				}
				
				return true;
			}
		}
	}
	else
	{
		flag = flag1 = flag2 = 1;
		return false;
	}
}


void flywheel_right_stop()
{
	flywheel_right_setBrushless(0);
	flywheel_right_setPitch(ReturnData(PITCH_ID_RIGHT)->Position/100.f);
	flywheel_right_setYaw(ReturnData(YAW_ID_RIGHT)->Position/10000.f);
	flywheel_right_setJmp(6);
	flywheel_right_setTurn(ReturnData(TURN_ID_RIGHT)->Position/10000.f);
}



void flywheel_right_home()
{
	flywheel_right_setBrushless(4);
	flywheel_right_setPitch(0);
	flywheel_right_setYaw(0);
	flywheel_right_setJmp(6);
	flywheel_right_setTurn(0);
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
void flywheel_right_main()
{
		switch(flywheel_right.state)
		{
			case fly_r_ready:
				flywheel_right_setPitch(flywheel_right.pur_pitch);
				flywheel_right_setYaw(flywheel_right.pur_yaw);
				flywheel_right_setTurn(flywheel_right.pur_turn);
				flywheel_right.state = fly_r_adj;
				flywheel_right.right.pwm1 = flywheel_right.pur_duty*100;
				flywheel_right.right.pwm2 = flywheel_right.pur_jmp*100;
				flywheel_right.right.relay[FLY_RIGHT] = 0;
				flywheel_right.right.relay[UP_RIGHT] = 1;
				Client_SET(flywheel_right.right);
			case fly_r_adj:
				if(flywheel_right_check() && fly_count == 0 && autorun.state == pos_arrived)
				{
					flywheel_right.state = r_fly;
					if(fly_n == 0)
					{
						fly_count = 0;
						flywheel_right.state = fly_r_ready;
						flywheel_right.fly_flag = false;
					}else{
						USART_SendString(bluetooth,"msg:low fly!!\n");
						flywheel_right_fly();
						fly_count = 400;
						fly_n--;
					}
				}
				break;
			case r_fly:	
				if(fly_n%2 ==0 && fly_count == 700)
					flywheel_right_up();
				else if(fly_n%2 ==0 && fly_count == 300)
				if(fly_count == 0)
				{	
					if(fly_n == 0)
					{
						fly_count = 0;
						flywheel_right.right.relay[UP_RIGHT] = 1;
						Client_SET(flywheel_right.right);
						flywheel_right.state = fly_r_finish;
						flywheel_right.fly_flag = false;
					}else{
						flywheel_right_fly();
						if(fly_n%2 == 0)
						{
							flywheel_right_up();
							fly_count = 400;
						}
						else
							fly_count = 1000;
						fly_n--;
					}
				}
				break;
			case fly_r_finish:
				if(flywheel_right.fly_flag)
				{
					flywheel_right.state = fly_r_ready;
				}
				break;
		}
}
