#ifndef _FLYWHEEL_RIGHT_H_
#define _FLYWHEEL_RIGHT_H_
#include "sys.h"
#include <stdbool.h>
#include "configuration.h"
#include "motorCtrl.h"
#include "client.h"

/*
配置左飞轮发射机构需要定义如下
#define FLYWHEEL_RIGHT
#define CLIENT_ID_RIGHT 	0x32
#define TURN_ID_RIGHT	0x04
#define PITCH_ID_RIGHT	0x07
#define YAW_ID_RIGHT		0x08
#define FLY_RIGHT		2
*/

#ifndef FLYWHEEL_RIGHT
#error 未配置右飞轮
#endif

typedef struct
{
	//成员
	Client	right;
	
	//状态
	enum {fly_r_ready, fly_r_adj, r_fly} state;
	
	//控制
	bool fly_flag;	//允许调整
	float pur_duty;	
	float pur_turn;
	float pur_pitch;
	float pur_yaw;
	float pur_jmp;
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

void flywheel_right_setTurn(float turn);

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
void flywheel_right_flyn(int n, float duty, float turn, float pitch, float yaw, float jmp);

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

void flywheel_right_stop();

#endif
