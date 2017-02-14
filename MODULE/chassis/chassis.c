#include "chassis.h"
#include "vega.h"
#include "delay.h"
#include <math.h>
#include "motorCtrl.h"
#include "cmd.h"

Chassis chassis;

static int ChassisSpeed;
static int TURN_speed;
static int Chassis_motor0 =0 , Chassis_motor1 =0 , Chassis_motor2 =0 ;

//根据vega安装方式决定各个轮子的偏角
static float CH_angle_M0 = -PI + 2*PI/3, CH_angle_M1 = -PI - 2*PI/3, CH_angle_M2 = -PI;

void chassis_init(void)
{
	vega_init(&(chassis.g_vega_pos_x),&(chassis.g_vega_pos_y),&(chassis.g_vega_angle));
    vega_reset();
	delay_ms(1000);
	RoboModule_RESET(MOTOR0_ID,MOTOR1_ID,MOTOR2_ID,0);
	delay_ms(500);
	RoboModule_CHOICE_mode(SpeedMode ,MOTOR0_ID ,MOTOR0_ID,MOTOR0_ID,0);
	delay_ms(500);
	
	chassis.Speed_min = 30;
	chassis.Speed_max = 6000;
	chassis.Move_radium = 0.00001;
	chassis.Angle_radium = 0.001;
	chassis.Angle_speed = 2400;
	chassis.Move_speed = 3500;
	chassis.Start_distance = 0.03;
	
	chassis.direction_angle = PI/2;
	chassis.START.X = chassis.g_vega_pos_x* 0.0001 * 0.81;
	chassis.START.Y = chassis.g_vega_pos_y* 0.0001 * 0.81;
	chassis.START.ANG = (chassis.g_vega_angle/180.f)*PI;
	chassis.END = chassis.START;
	chassis.car_state = car_stop;
	
}

//主循环轮询更新
void chassis_updata()
{
	chassis.pos_x = chassis.g_vega_pos_x* 0.0001 * 0.81;
	chassis.pos_y = chassis.g_vega_pos_y* 0.0001 * 0.81;
	chassis.angle = (chassis.g_vega_angle/180.f)*PI;
	//USART_SendString(UART5,"\n\nX=%f   Y=%f  angle=%f\n",pos_x, pos_y , angle);
	ChassisSpeed= chassis.Move_speed;
}

/**主循环中手柄控制
  *参数：float directoion 	方向角
  *		int speed			速度
  *会检查速度是否为0，若为0，则只发一次
  */
void chassis_handle(float directoion, int speed)
{
	static bool stop = false;
	
	Chassis_motor0 = (speed/7*5*cos((CH_angle_M0 + chassis.angle) - directoion));
	Chassis_motor1 = (speed/7*5*cos((CH_angle_M1 + chassis.angle) - directoion));
	Chassis_motor2 = (speed/7*5*cos((CH_angle_M2 + chassis.angle) - directoion));
	
	if(fabs(Chassis_motor0)<1e-6 && fabs(Chassis_motor1)<1e-6 && fabs(Chassis_motor2)<1e-6)
	{
		 if(!stop)
		 {
			stop = true;
			RoboModule_SET_speed(MOTOR0_ID ,5000 , Chassis_motor0);
			RoboModule_SET_speed(MOTOR1_ID ,5000 , Chassis_motor1);
			RoboModule_SET_speed(MOTOR2_ID ,5000 , Chassis_motor2);
		 }
	}else{
		stop = false;
		RoboModule_SET_speed(MOTOR0_ID ,5000 , Chassis_motor0);
		RoboModule_SET_speed(MOTOR1_ID ,5000 , Chassis_motor1);
		RoboModule_SET_speed(MOTOR2_ID ,5000 , Chassis_motor2);
	}
}

/** 主循环中自动跑点的状态转移函数
  * 通过改chassis.END来控制目的地
  */
void chassis_auto()
{
	int ms;
	float errorAngle;
	float error_X, error_Y, Serror_X, Serror_Y;
	float factor = 5;
	
	if(chassis.car_state == car_ready){
		chassis.START.X = chassis.g_vega_pos_x* 0.0001 * 0.81;
		chassis.START.Y = chassis.g_vega_pos_y* 0.0001 * 0.81;
		chassis.START.ANG = (chassis.g_vega_angle/180.f)*PI;
		ms = 0;
		chassis.car_state = car_running;
	}
	if(chassis.car_state == car_running){
		errorAngle = chassis.angle - chassis.END.ANG;
		error_X = chassis.END.X - chassis.pos_x;
		error_Y = chassis.END.Y - chassis.pos_y;
		
		Serror_X = chassis.START.X - chassis.pos_x;
		Serror_Y = chassis.START.Y - chassis.pos_y;
		
		if(factor * factor * (powf(Serror_X,2)+powf(Serror_Y,2)) < (powf(error_X,2)+powf(error_Y,2))) {//加速
			if((powf(Serror_X,2)+powf(Serror_Y,2)) > chassis.Start_distance * chassis.Start_distance) // 0<start_distance<1
				ChassisSpeed = sqrt(powf(Serror_X,2)+powf(Serror_Y,2))*chassis.Move_speed * factor;
			else 
				ChassisSpeed = chassis.Start_distance * chassis.Move_speed * factor;
		}else {
			if((powf(error_X,2)+powf(error_Y,2)) > 1)
				ChassisSpeed = sqrt(powf(error_X,2)+powf(error_Y,2))*chassis.Move_speed;
			else //if((powf(error_X,2)+powf(error_Y,2)) > 0.0025)
				ChassisSpeed = sqrt(sqrt(powf(error_X,2)+powf(error_Y,2))) * chassis.Move_speed;
			//else
			//	ChassisSpeed = 89.44 * (powf(error_X,2)+powf(error_Y,2)) * chassis.Move_speed;
		}
		/*if((powf(error_X,2)+powf(error_Y,2)) > 1)
			ChassisSpeed = sqrt(powf(error_X,2)+powf(error_Y,2))*Move_speed;
		else if((powf(error_X,2)+powf(error_Y,2)) > 0.01)
			ChassisSpeed = sqrt(sqrt(powf(error_X,2)+powf(error_Y,2)))*Move_speed;
		else 
			ChassisSpeed = 31.6 * (powf(error_X,2)+powf(error_Y,2)) * Move_speed;
		*/
		if(ChassisSpeed>chassis.Speed_max)
			ChassisSpeed = chassis.Speed_max;
		
		if(errorAngle >= chassis.Angle_radium && errorAngle < 2)
		{          //角度调整
			TURN_speed = -1*chassis.Angle_speed*errorAngle;
		}
		else if(errorAngle <=-chassis.Angle_radium && errorAngle > -2)
		{
			TURN_speed = -1*chassis.Angle_speed*errorAngle;
		}
		else if(errorAngle>=2)
		{
			TURN_speed =-1*chassis.Angle_speed*2;
		}
		else if(errorAngle<=-2)
		{
			TURN_speed =-1*chassis.Angle_speed*(-2);
		}
		else
		{
			TURN_speed= 0;
		}
		
		if(powf(error_X,2)+powf(error_Y,2) <= chassis.Move_radium)
		{//已经到达
			ChassisSpeed = 0;
		}
		else
		{
			direction_angle = atan2(error_Y,error_X);
		}

		Chassis_motor0 = (ChassisSpeed * cos((CH_angle_M0 + chassis.angle) - direction_angle) + TURN_speed);//Y轴方向，这里direction_angle代表小车相对于场地坐标系的方向
		Chassis_motor1 = -1*(ChassisSpeed * cos((CH_angle_M1 + chassis.angle) - direction_angle) + TURN_speed);
		Chassis_motor2 = (ChassisSpeed * cos((CH_angle_M2 + chassis.angle) - direction_angle) + TURN_speed);
		
		if(fabs(Chassis_motor2) < 1 && fabs(Chassis_motor1) < 1 && fabs(Chassis_motor0) < 1)
		{
			USART_SendString(MOTOR_USARTx,"2v0\r1v0\r0v0\r");
			chassis.car_state = car_stop;
		}
		else
		{
			USART_SendString(MOTOR_USARTx,"1v%d\r2v%d\r0v%d\r",Chassis_motor1 , Chassis_motor2 , Chassis_motor0);
		}
	}
	else if(chassis.car_state == car_stop)
	{
		if(ms>200)
			USART_SendString(CMD_USARTx,"msg: %fs\n",ms*5.0/1000);
		ms = 0;
		/*pos_x = temp_x* 0.0001 * 0.81;
		pos_y = temp_y* 0.0001 * 0.81;
		angle = (temp_angle/180.f)*PI;*/
		//state = ready;
		//OPEN_Hander = 1;
	}
}
