#ifndef _FLYWHEEL_LEFT_H_
#define _FLYWHEEL_LEFT_H_
#include "sys.h"
#include <stdbool.h>
#include "configuration.h"
#include "motorCtrl.h"
#include "client.h"

/*
配置左飞轮发射机构需要定义如下
#define FLYWHEEL_LEFT
#define FLYWHEEL_ID_LEFT 	0x20
#define CLIENT_ID_LEFT 		0x30
#define PITCH_ID_LEFT		0x04
#define YAW_ID_LEFT			0x06
#define FLY_LEFT			2
*/

#ifndef FLYWHEEL_LEFT
#error 未配置左飞轮
#endif

typedef struct
{
	//状态
	enum {fly_ready, fly_adj, fly,fly_l_finish} state;
	
	//控制
	bool fly_flag;	//允许调整

	float pur_duty;	
	float pur_pitch;
	float pur_yaw;
	float pur_jmp;
	u8 fly_relay;
	u8 up_relay;
	
	u8 io[3];
}Flywheel_left;

extern Flywheel_left flywheel_left;

/**
  * @brief  初始化
  *     
  * @note
  *
  * @param 
  *          
  * @retval 
  */
void flywheel_left_init();

/**
  * @brief  改俯仰角
  *     
  * @note
  *
  * @param 俯仰角
  *          
  * @retval 
  */
void flywheel_left_setPitch(float pitch);


/**
  * @brief  改无刷
  *     
  * @note
  *
  * @param 占空比
  *          
  * @retval 
  */
void flywheel_left_setBrushless(float duty);

	

void flywheel_left_setJmp(float duty);	

/**
  * @brief  改旋转平台
  *     
  * @note
  *
  * @param 旋转平台
  *          
  * @retval 
  */
void flywheel_left_setYaw(float yaw);

void flywheel_left_fly();

void flywheel_left_Set();

/**
  * @brief  tim2中调用，进行延时等操作
  *     
  * @note
  *
  * @param 旋转平台
  *          
  * @retval 
  */
void flywheel_left_TIM();

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
void flywheel_left_flyn(int n, float duty, float pitch, float yaw);

/**
  * @brief main大循环中检查
  *     
  * @note
  *
  * @param
  *          
  * @retval 
  */
void flywheel_left_main();

/*
	检查姿态是否调整到位
	用于状态转移
*/
bool flywheel_left_check();

/*紧急停*/
void flywheel_left_stop();

void flywheel_left_home();

void flywheel_left_pitchZero();

void flywheel_left_yawZero();

//发射一个飞盘
void flywheel_left_fly1();

void flywheel_left_up(int i);

#endif
