#ifndef _FLYWHEEL_RIGHT_H_
#define _FLYWHEEL_RIGHT_H_
#include "sys.h"
#include <stdbool.h>
#include "configuration.h"
#include "motorCtrl.h"
#include "client.h"

/*
配置左飞轮发射机构需要定义如下,实际配置看configuration.h
#define FLYWHEEL_RIGHT
#define CLIENT_ID_RIGHT 	0x31
#define TURN_ID_RIGHT		0x08
#define PITCH_ID_RIGHT		0x09
#define YAW_ID_RIGHT		0x05
#define FLY_RIGHT			2
#define UP_RIGHT			3
*/

#ifndef FLYWHEEL_RIGHT
#error 未配置右飞轮
#endif

typedef struct
{
	//成员
	
	//状态
	enum {fly_r_ready, fly_r_adj, r_fly, fly_r_finish} state;
	
	//控制
	bool fly_flag;	//允许调整
	float pur_duty;	
	float pur_turn;
	float pur_pitch;
	float pur_yaw;
	float pur_jmp;
	u8 io[3];
}Flywheel_right;

extern Flywheel_right flywheel_right;

/**
  * @brief  初始化
  *     
  * @note
  *
  * @param 
  *          
  * @retval 
  */
void flywheel_right_init();

/**
  * @brief  改俯仰角
  *     
  * @note
  *
  * @param 俯仰角
  *          
  * @retval 
  */
void flywheel_right_setPitch(float pitch);


/**
  * @brief  改无刷
  *     
  * @note
  *
  * @param 占空比
  *          
  * @retval 
  */
void flywheel_right_setBrushless(float duty);

	

void flywheel_right_setJmp(float duty);	

/**
  * @brief  改旋转平台
  *     
  * @note
  *
  * @param 旋转平台
  *          
  * @retval 
  */
void flywheel_right_setYaw(float yaw);

void flywheel_right_fly();

/**
  * @brief  tim2中调用，进行延时等操作
  *     
  * @note
  *
  * @param 旋转平台
  *          
  * @retval 
  */
void flywheel_right_TIM();

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
void flywheel_right_flyn(int n, float duty, float pitch, float yaw);
/**
  * @brief main大循环中检查
  *     
  * @note
  *
  * @param
  *          
  * @retval 
  */
void flywheel_right_main();

/*
	检查姿态是否调整到位
	用于状态转移
*/
bool flywheel_right_check();

/*紧急停*/
void flywheel_right_stop();

void flywheel_right_home();

void flywheel_right_pitchZero();

void flywheel_right_yawZero();

//发射一个飞盘
void flywheel_right_fly1();

void flywheel_right_up(int i);

void flywheel_right_Set()

#endif
