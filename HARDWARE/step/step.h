#ifndef _STEP_H
#define _STEP_H

#include <stdbool.h>

/**
  * @brief  初始化步进电机1
  *     
  * @note  内部初始化了TIM13 使能端0使能 方向1
  *
  * @param 
  *          
  * @retval 
  */
void Step1_Init(void);

/**
  * @brief  步进电机1当前位置
  *     
  * @note  
  *
  * @param 
  *          
  * @retval int 位置
  */
int Step1_get_count(void);

/**
  * @brief  设置步进电机1预期位置
  *     
  * @note  
  *
  * @param int 位置
  *          
  * @retval 
  */
void Step1_moveto(int count);

/**
  * @brief  设置步进电机1速度
  *     
  * @note  设置定时器频率
  *
  * @param int speed 频率
  *          
  * @retval 
  */
void Step1_set_speed(int speed);

/**
  * @brief  主循环中调用
  *     
  * @note  
  *
  * @param 
  *          
  * @retval 
  */
void Step_check(void);

void Step1_stop();

/**
  * @brief  初始化步进电机2
  *     
  * @note  内部初始化了TIM14 使能端0使能 方向1
  *
  * @param 
  *          
  * @retval 
  */
void Step2_Init(void);

/**
  * @brief  步进电机1当前位置
  *     
  * @note  
  *
  * @param 
  *          
  * @retval int 位置
  */
int Step2_get_count(void);

/**
  * @brief  设置步进电机2预期位置
  *     
  * @note  
  *
  * @param int 位置
  *          
  * @retval 
  */
void Step2_moveto(int count);

/**
  * @brief  设置步进电机2速度
  *     
  * @note  设置定时器频率
  *
  * @param int speed 频率
  *          
  * @retval 
  */
void Step2_set_speed(int speed);

void Step2_stop();

/**
  * @brief  设置步进电机1状态
  *     
  * @note
  *
  * @param
  *          
  * @retval true表示到达预定位置
  */
bool Step1_state();


/**
  * @brief  设置步进电机2状态
  *     
  * @note
  *
  * @param
  *          
  * @retval true表示到达预定位置
  */
bool Step2_state();

#endif
