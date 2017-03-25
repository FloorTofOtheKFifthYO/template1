#include "flywheel_right.h"
#include <string.h>
#include "auto.h"
#include <math.h>

#define PITCH_MAXSPEED 50
#define YAW_MAXSPEED 50
#define TURN_MAXSPEED 50

Flywheel_right flywheel_right;

static int fly_count = 0;
static int fly_n = 0;//��¼����ĸ���

/**
  * @brief  ��ʼ��
  *     
  * @note
  *
  * @param 
  *          
  * @retval 
  */
void flywheel_right_init()
{
	//�Ƕȵ���ĳ�ʼ��
	RoboModule_RESET(YAW_ID_RIGHT,PITCH_ID_RIGHT, TURN_ID_RIGHT,0);
	delay_ms(5);
	RoboModule_CHOICE_mode(PositionMode ,YAW_ID_RIGHT ,PITCH_ID_RIGHT, TURN_ID_RIGHT,0);
	RoboModule_Add_Callback(databack,RoboModule_Feedback_Callback,YAW_ID_RIGHT,TURN_ID_RIGHT ,PITCH_ID_RIGHT,0);
	RoboModule_SETUP(2,0,YAW_ID_RIGHT,PITCH_ID_RIGHT, TURN_ID_RIGHT,0);
	
	//�Ӱ�����
	flywheel_right.right.can_id = CLIENT_ID_RIGHT;
	Client_RESET(flywheel_right.right);
	flywheel_right.right.pwm1 = 770;
	flywheel_right.right.pwm2 = 600;
	memset(flywheel_right.right.relay,0,sizeof(flywheel_right.right.relay));
	Client_SET(flywheel_right.right);
	
	flywheel_right.pur_duty = 7.7;
	flywheel_right.pur_jmp = 6;
	flywheel_right.pur_pitch = 0;
	flywheel_right.pur_yaw = 0;
	flywheel_right.pur_turn = 0;
	flywheel_right.state = fly_r_ready;
	flywheel_right.fly_flag = false;
}

/**
  * @brief  �ĸ�����
  * 
  * @note
  *
  * @param ������
  *          
  * @retval 
  */
void flywheel_right_setPitch(float pitch)
{
	RoboModule_SET_Position(PITCH_ID_RIGHT,5000,pitch*100,PITCH_MAXSPEED);
}

/**
  * @brief  ������ת��
  *     
  * @note
  *
  * @param ��ת��
  *          
  * @retval 
  */
void flywheel_right_setTurn(float turn)
{
	RoboModule_SET_Position(PITCH_ID_RIGHT,5000,turn*10000,TURN_MAXSPEED);
}

void flywheel_right_fly()
{
	flywheel_right.right.relay[FLY_RIGHT] = 1 - flywheel_right.right.relay[FLY_RIGHT];
	Client_SET(flywheel_right.right);
}

/**
  * @brief  ����ˢ
  *     
  * @note
  *
  * @param ռ�ձ�
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
  * @brief  ����תƽ̨
  *     
  * @note
  *
  * @param ��תƽ̨
  *          
  * @retval 
  */
void flywheel_right_setYaw(float yaw)
{
	RoboModule_SET_Position(YAW_ID_RIGHT,5000,yaw*10000,YAW_MAXSPEED);
}

/**
  * @brief  tim2�е��ã�������ʱ�Ȳ���
  *     
  * @note
  *
  * @param ��תƽ̨
  *          
  * @retval 
  */
void flywheel_right_TIM()
{
	if(fly_count != 0)
		fly_count--;
}

/**
  * @brief ���n�η�
  *     
  * @note int n,	����
		float duty, ��ˢռ�ձ�
		float roll,	������
		float turn  ת��
  *
  * @param ��תƽ̨
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
	�����̬�Ƿ������λ
	����״̬ת��
*/
bool flywheel_right_check()
{
	if(fabs(ReturnData(PITCH_ID_RIGHT)->Position -flywheel_right.pur_pitch*100)<=50 && 
		fabs(ReturnData(YAW_ID_RIGHT)->Position -flywheel_right.pur_yaw*10000)<=50 && 
		fabs(ReturnData(TURN_ID_RIGHT)->Position -flywheel_right.pur_turn*10000)<=50)
		return true;
	else
		return false;
}


void flywheel_right_stop()
{
	flywheel_right_setBrushless(7.7);
	flywheel_right_setPitch(ReturnData(PITCH_ID_RIGHT)->Position/100.f);
	flywheel_right_setYaw(ReturnData(YAW_ID_RIGHT)->Position/10000);
	flywheel_right_setJmp(6);
	flywheel_right_setTurn(ReturnData(TURN_ID_RIGHT)->Position/100004);
}


/**
  * @brief main��ѭ���м��
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
				if(flywheel_right.fly_flag)
				{
					flywheel_right_setPitch(flywheel_right.pur_pitch);
					flywheel_right_setYaw(flywheel_right.pur_yaw);
					flywheel_right_setTurn(flywheel_right.pur_turn);
					flywheel_right.state = fly_r_adj;
					flywheel_right.right.pwm1 = flywheel_right.pur_duty*100;
					flywheel_right.right.pwm2 = flywheel_right.pur_jmp*100;
					flywheel_right.right.relay[FLY_RIGHT] = 0;
					Client_SET(flywheel_right.right);
				}
				break;
			case fly_r_adj:
				if(flywheel_right_check() && autorun.state == pos_arrived)
				{
					flywheel_right.state = r_fly;
					if(fly_n == 0)
					{
						fly_count = 0;
						flywheel_right.state = fly_r_ready;
						flywheel_right.fly_flag = false;
					}else{
						flywheel_right_fly();
						fly_count = 200;
						fly_n--;
					}
				}
				break;
			case r_fly:	
				if(fly_count == 0)
				{
					if(fly_n == 0)
					{
						fly_count = 0;
						flywheel_right.state = fly_r_ready;
						flywheel_right.fly_flag = false;
					}else{
						flywheel_right_fly();
						fly_count = 200;
						fly_n--;
					}
				}
				break;
		}
}
