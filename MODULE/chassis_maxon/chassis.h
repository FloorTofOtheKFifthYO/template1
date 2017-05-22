#ifndef _UNDERPAN_H_
#define _UNDERPAN_H_
#include "sys.h"
#include <stdbool.h>
#include "configuration.h"

#define CHASSIS_FIRST_RUN 0
#define CHASSIS_FLASH_SETOR FLASH_Sector_6 //  扇区6
#define CHASSIS_FLASH_ADDR_START ((uint32_t)0x08040000)
#define CHASSIS_FLASH_ADDR_END ((uint32_t)0x0805FFFF)

/*应定义出
	#ifndef CHASSIS
	#define CHASSIS

	#define MOTOR0_ID 0x0A
	#define MOTOR1_ID 0x0B
	#define MOTOR2_ID 0x0C

	#endif
*/
#ifndef CHASSIS
#error 未定义底盘
#endif

typedef struct 
{
	float X;
	float Y;
	float ANG;
}POSITION;

typedef struct
{
	//全场定位传回的数据
	int g_vega_pos_x; 
	int g_vega_pos_y;   
	float g_vega_angle;
	
	float radar_theta;
	float radar_distance;
	
	float pos_x,pos_y;
	float angle;
	
	//状态
	enum {car_stop, car_running, car_ready} car_state;//车的运动状态
	
	//控制
	POSITION END, START;
	
	//参数
	float Angle_radium;//停角度范围
	int Angle_speed;
	float factor;
	float Move_radium;
	int Move_speed;
	int Speed_max;
	int Speed_min;
	float Start_distance;
	float xfactor;
} Chassis;

extern Chassis chassis;

/** 底盘初始化
  * 包括全场定位的初始化
  *
  */
void chassis_init(void);

/**保存参数
成功返回1，出错返回-1或-2
  */
int chassis_save();

//主循环轮询更新
void chassis_update();

/**主循环中手柄控制
  *参数：float directoion 	方向角
  *		int speed			速度
  *会检查速度是否为0，若为0，则只发一次
  */
void chassis_handle(float directoion, int speed);

/** 主循环中自动跑点的状态转移函数
  * 通过改chassis.END来控制目的地
  */
void chassis_auto();

//stop
void chassis_stop();


void chassis_param_print();


#endif
