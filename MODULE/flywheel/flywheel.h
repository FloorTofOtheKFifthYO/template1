#ifndef _FLYWHEEL_H_
#define _FLYWHEEL_H_
#include "sys.h"
#include <stdbool.h>
#include "configuration.h"
#include "motor_brushless.h"
#include "putter.h"
#include "motor_usart.h"

/** 在各模块中的.h文件注释里指明每个部件应该分配的资源，如下：
  * 确定资源分配后，将下面的写在configuration.h
  *
  * #define BRUSHLESS
  * #define BRUSHLESS_TIM 		TIMx			//定时器
  * #define BRUSHLESS_OC 		1				//通道
  * #define BRUSHLESS_GPIOx 	GPIOx			//GPIO
  *	#define BRUSHLESS_PIN GPIO_	Pin_x			//管脚
  * #define BRUSHLESS_PINSOURCE GPIO_PinSourcex //复用管脚
  * #define BRUSHLESS_AF 		GPIO_AF_TIMx	//管脚复用功能
  *
  * 用一个宏定义将配置包起来，方便下面检查
  
  */
#ifndef BRUSHLESS
#error 未定义无刷
#endif


/*根据实际情况在configuration.h中配置
#define ROLL
#define ROLL_ID 		4
#define ROLL_SP			2000
#define ROLL_GETTIMX	4
*/
#ifndef ROLL
#error 未定义电推杆
#endif

/*根据实际配置旋转平台

*/
#ifndef TURN
#error	未配置旋转平台
#endif

#ifndef FLY
#error 未配置继电器
#endif

typedef struct
{
	//成员
	Brushless wheel;
	putter roll;
	motor_usart turn;
	
	//状态
	enum {fly_ready, fly_adj, fly} state;
	
	//控制
	bool fly_flag;
	float pur_duty;
	float pur_roll;
	float pur_turn;
	
}Flywheel;

extern Flywheel flywheel;

/**
  * @brief  初始化
  *     
  * @note
  *
  * @param 
  *          
  * @retval 
  */
void flywheel_init();

/**
  * @brief  改俯仰角
  *     
  * @note
  *
  * @param 俯仰角
  *          
  * @retval 
  */
void flywheel_setRoll(float roll);


/**
  * @brief  改无刷
  *     
  * @note
  *
  * @param 占空比
  *          
  * @retval 
  */
void flywheel_setBrushless(float duty);

	
/**
  * @brief  改旋转平台
  *     
  * @note
  *
  * @param 旋转平台
  *          
  * @retval 
  */
void flywheel_setTurn(float turn);

void flywheel_fly();

/**
  * @brief  tim2中调用，进行延时等操作
  *     
  * @note
  *
  * @param 旋转平台
  *          
  * @retval 
  */
void flywheel_TIM();

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
void flywheel_flyn(int n, float duty, float roll, float turn);

/**
  * @brief main大循环中检查
  *     
  * @note
  *
  * @param
  *          
  * @retval 
  */
void flywheel_main();

#endif
