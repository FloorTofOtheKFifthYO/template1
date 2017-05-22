#include "chassis.h"
#include "vega.h"
#include "delay.h"
#include <math.h>
#include "flash.h"
#include "maxon.h"
#include "radar.h"
#include "cmd.h"

Chassis chassis;

static int ChassisSpeed;
static int TURN_speed;
static int Chassis_motor0 =0 , Chassis_motor1 =0 , Chassis_motor2 = 0, Chassis_motor3 =0;

//根据vega坐标正方向决定各个轮子的偏角
static float CH_angle_M0 = PI/2, CH_angle_M1 = PI/2 - PI/2, CH_angle_M2 = PI/2 - 2*PI/2, CH_angle_M3 = PI/2 - 3*PI/2; 

void chassis_init(void)
{
	u32 addr = CHASSIS_FLASH_ADDR_START;
	
	radar_init(&(chassis.radar_theta),&(chassis.radar_distance));
	vega_init(&(chassis.g_vega_pos_x),&(chassis.g_vega_pos_y),&(chassis.g_vega_angle));
    vega_reset();
	delay_ms(2000);
	usart_init(MOTOR0_ID,115200,false);
	usart_init(MOTOR1_ID,115200,false);
	usart_init(MOTOR2_ID,115200,false);
	usart_init(MOTOR3_ID,115200,false);
	
	/*maxon_setSpeed_p(MOTOR0_ID,4000);
	maxon_setSpeed_p(MOTOR1_ID,4000);
	maxon_setSpeed_p(MOTOR2_ID,4000);
	maxon_setSpeed_p(MOTOR3_ID,4000);
	
	maxon_setSpeed_i(MOTOR0_ID,2000);
	maxon_setSpeed_i(MOTOR1_ID,2000);
	maxon_setSpeed_i(MOTOR2_ID,2000);
	maxon_setSpeed_i(MOTOR3_ID,2000);
	
	maxon_save(MOTOR0_ID);
	maxon_save(MOTOR1_ID);
	maxon_save(MOTOR2_ID);
	maxon_save(MOTOR3_ID);*/
	
	maxon_setSpeed(MOTOR0_ID,0);
	maxon_setSpeed(MOTOR3_ID,0);
	maxon_setSpeed(MOTOR1_ID,0);
	maxon_setSpeed(MOTOR2_ID,0);

#if CHASSIS_FIRST_RUN == 1

	chassis.Speed_max = 6700;
	chassis.Move_radium = 0.0004;
	chassis.Angle_radium = 0.0001;
	chassis.Angle_speed = 3000;
	chassis.Move_speed = 3700;
	chassis.Speed_min = 30;
	chassis.Start_distance = 0.001;
	chassis.factor = 2.7;
	chassis.xfactor = 0.25;
	
	if(chassis_save()<0)
	{
		USART_SendString(bluetooth,"msg: write into flash error\n");
		while(1);
	}
	
#else

	chassis.Angle_radium = STMFLASH_ReadFloat_Inc(&addr);
	chassis.Angle_speed = STMFLASH_ReadWord_Inc(&addr);
	chassis.factor = STMFLASH_ReadFloat_Inc(&addr);
	chassis.Move_radium = STMFLASH_ReadFloat_Inc(&addr);
	chassis.Move_speed = STMFLASH_ReadWord_Inc(&addr);
	chassis.Speed_max = STMFLASH_ReadWord_Inc(&addr);
	chassis.Speed_min = STMFLASH_ReadWord_Inc(&addr);
	chassis.Start_distance = STMFLASH_ReadFloat_Inc(&addr);
	chassis.xfactor = STMFLASH_ReadFloat_Inc(&addr);

#endif

	chassis.START.X = 0;
	chassis.START.Y = 0;
	chassis.START.ANG = 0;
	chassis.END = chassis.START;
	chassis.car_state = car_stop;
}


void chassis_param_print()
{
	USART_SendString(bluetooth,"speedmax:%d ",chassis.Speed_max);
	USART_SendString(bluetooth,"speedmin:%d ",chassis.Speed_min);
	USART_SendString(bluetooth,"movespeed:%d ",chassis.Move_speed);
	USART_SendString(bluetooth,"moveradium:%f ",chassis.Move_radium);
	USART_SendString(bluetooth,"angleradium:%f ",chassis.Angle_radium);
	USART_SendString(bluetooth,"anglespeed:%d ",chassis.Angle_speed);
	USART_SendString(bluetooth,"start:%f ",chassis.Start_distance);
	USART_SendString(bluetooth,"factor:%f ",chassis.factor);
	USART_SendString(bluetooth,"xfactor:%f\n",chassis.xfactor);
}

int chassis_save()
{
	u32 addr = CHASSIS_FLASH_ADDR_START;
	FLASH_Unlock();  //解锁FLASH，写数据
    FLASH_DataCacheCmd(DISABLE);
    FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR | 
    								FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR|FLASH_FLAG_PGSERR);  //写FLASH之前，先清除标志
    if(FLASH_EraseSector(CHASSIS_FLASH_SETOR, VoltageRange_3) != FLASH_COMPLETE){ 
        FLASH_DataCacheCmd(ENABLE); 
        FLASH_Lock();//上锁
        return -1;   //擦写失败，，退出
    }
	
	FLASH_ProgramFloat(addr,chassis.Angle_radium);
	addr+=4;
	FLASH_ProgramWord(addr,chassis.Angle_speed);
	addr+=4;
	FLASH_ProgramFloat(addr,chassis.factor);
	addr+=4;
	FLASH_ProgramFloat(addr,chassis.Move_radium);
	addr+=4;
	FLASH_ProgramWord(addr,chassis.Move_speed);
	addr+=4;
	FLASH_ProgramWord(addr,chassis.Speed_max);
	addr+=4;
	FLASH_ProgramWord(addr,chassis.Speed_min);
	addr+=4;
	FLASH_ProgramFloat(addr,chassis.Start_distance);
	addr+=4;
	FLASH_ProgramFloat(addr,chassis.xfactor);
	addr+=4;
	
	FLASH_DataCacheCmd(ENABLE);
    FLASH_Lock();  //  写保护
    if(addr > CHASSIS_FLASH_ADDR_END){   //如果超过FLASH的存储空间，则报错返回
        return -2;
    }
	
	return 1;
}

//主循环轮询更新
void chassis_update()
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
	
	Chassis_motor0 = -(speed*cos((CH_angle_M0 + chassis.angle) - directoion));
	Chassis_motor1 = -(speed*cos((CH_angle_M1 + chassis.angle) - directoion));
	Chassis_motor2 = -(speed*cos((CH_angle_M2 + chassis.angle) - directoion));
	Chassis_motor3 = -(speed*cos((CH_angle_M3 + chassis.angle) - directoion));
	
	if(fabs(Chassis_motor0)<1e-6 && fabs(Chassis_motor1)<1e-6 && fabs(Chassis_motor2)<1e-6)
	{
		 if(!stop)
		 {
			stop = true;
			maxon_setSpeed(MOTOR0_ID,0);
			maxon_setSpeed(MOTOR1_ID,0);
			maxon_setSpeed(MOTOR2_ID,0);
			maxon_setSpeed(MOTOR3_ID,0);
			maxon_setSpeed(MOTOR0_ID,0);
			maxon_setSpeed(MOTOR1_ID,0);
			maxon_setSpeed(MOTOR2_ID,0);
			maxon_setSpeed(MOTOR3_ID,0);
		 }
	}else{
		stop = false;
		maxon_setSpeed(MOTOR0_ID,Chassis_motor0);
		maxon_setSpeed(MOTOR1_ID,Chassis_motor1);
		maxon_setSpeed(MOTOR2_ID,Chassis_motor2);
		maxon_setSpeed(MOTOR3_ID,Chassis_motor3);
	}
}

extern int ms;

/** 主循环中自动跑点的状态转移函数
  * 通过改chassis.END来控制目的地
  */
void chassis_auto()
{
	float errorAngle;
	float error_X, error_Y, Serror_X, Serror_Y, direrror_X,direrror_Y;
	double Sroute,Eroute;
	static float Oerror_X,Oerror_Y;
	static float dir_dot_X, dir_dot_Y; 
	static float distance;
	float direction_angle;
	
	int arrived_flag;
	float i;
	
	
	if(chassis.car_state == car_ready){
		chassis.START.X = chassis.g_vega_pos_x* 0.0001 * 0.81;
		chassis.START.Y = chassis.g_vega_pos_y* 0.0001 * 0.81;
		chassis.START.ANG = (chassis.g_vega_angle/180.f)*PI;
		ms = 0;
		chassis.car_state = car_running;
		
		Oerror_X = chassis.END.X - chassis.START.X;
		Oerror_Y = chassis.END.Y - chassis.START.Y;
		distance = sqrtf(powf(Oerror_X,2)+powf(Oerror_Y,2));
		Oerror_X = Oerror_X/distance;
		Oerror_Y = Oerror_Y/distance;
	}
	if(chassis.car_state == car_running){
		arrived_flag = 0;
		errorAngle = chassis.angle - chassis.END.ANG;
		error_X = chassis.END.X - chassis.pos_x;
		error_Y = chassis.END.Y - chassis.pos_y;
		
		Serror_X = chassis.START.X - chassis.pos_x;
		Serror_Y = chassis.START.Y - chassis.pos_y;
		
		Sroute = sqrt(powf(Serror_X,2)+powf(Serror_Y,2))+chassis.Start_distance;
		Eroute = sqrt(powf(error_X,2)+powf(error_Y,2));
		
		i = Sroute+0.5;
		
		dir_dot_X = Oerror_X*i + chassis.START.X;
		dir_dot_Y = Oerror_Y*i + chassis.START.Y;
		
		direrror_X = dir_dot_X - chassis.pos_x;
		direrror_Y = dir_dot_Y - chassis.pos_y;

		if(sqrtf(chassis.factor) * sqrtf(Sroute) < chassis.xfactor*Eroute + sqrtf(Eroute)) {//加速
			//if(Sroute*chassis.factor<1)

				ChassisSpeed = sqrtf(Sroute)*chassis.Move_speed * sqrtf(chassis.factor);
		}else {
			/*if(Eroute > 0.5)
				ChassisSpeed = sqrtf(Eroute) * chassis.Move_speed;
			else
				ChassisSpeed = 1.414 * Eroute * chassis.Move_speed;*/
			/*if(Eroute > 1)
				ChassisSpeed = sqrtf(Eroute) * chassis.Move_speed;
			else
				ChassisSpeed = Eroute * chassis.Move_speed;*/
			
			ChassisSpeed = (sqrtf(Eroute)+ chassis.xfactor*Eroute) * chassis.Move_speed;
			
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
		else if(ChassisSpeed < chassis.Speed_min)
		{
			ChassisSpeed = chassis.Speed_min;
		}
		
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
			arrived_flag += 1;
			TURN_speed= 0;
		}
		
		if(TURN_speed>0 && TURN_speed<20)
		{
			TURN_speed = 20;
		}else if (TURN_speed<0 && TURN_speed>-20)
		{
			TURN_speed = -20;
		}
		
		if(powf(error_X,2)+powf(error_Y,2) <= chassis.Move_radium)
		{//已经到达
			i = 1;
			arrived_flag += 1;
			ChassisSpeed = 0;
		}else {
			
			if(distance<=1 || powf(error_X,2)+powf(error_Y,2) <= 0.5 || Sroute >= distance)
			{	
				direction_angle = atan2(error_Y,error_X);
			}
			else
				direction_angle = atan2(direrror_Y,direrror_X);
		}

		Chassis_motor0 = -(ChassisSpeed * cos((CH_angle_M0 - chassis.angle) - direction_angle) - TURN_speed);//Y轴方向，这里direction_angle代表小车相对于场地坐标系的方向
		Chassis_motor1 = -(ChassisSpeed * cos((CH_angle_M1 - chassis.angle) - direction_angle) - TURN_speed);
		Chassis_motor2 = -(ChassisSpeed * cos((CH_angle_M2 - chassis.angle) - direction_angle) - TURN_speed);
		Chassis_motor3 = -(ChassisSpeed * cos((CH_angle_M3 - chassis.angle) - direction_angle) - TURN_speed);
		
		//USART_SendString(bluetooth,"%d,%d,%d,%d,%f",Chassis_motor0,Chassis_motor1,Chassis_motor2, Chassis_motor3,direction_angle);
		//USART_SendString(bluetooth,",%d,%d,%d,%d,%f\n",ReturnData(MOTOR0_ID)->Speed,ReturnData(MOTOR1_ID)->Speed,ReturnData(MOTOR2_ID)->Speed,ReturnData(MOTOR3_ID)->Speed,Sroute);
		
		//USART_SendString(bluetooth,"dot x:%f,y:%f\n",dir_dot_X,dir_dot_Y);
		
		if(arrived_flag == 2)
		{
			maxon_setSpeed(MOTOR0_ID,0);
			maxon_setSpeed(MOTOR1_ID,0);
			maxon_setSpeed(MOTOR2_ID,0);
			maxon_setSpeed(MOTOR3_ID,0);
			maxon_setSpeed(MOTOR0_ID,0);
			maxon_setSpeed(MOTOR1_ID,0);
			maxon_setSpeed(MOTOR2_ID,0);
			maxon_setSpeed(MOTOR3_ID,0);
			chassis.car_state = car_stop;
		}
		else
		{
			maxon_setSpeed(MOTOR0_ID,Chassis_motor0);
			maxon_setSpeed(MOTOR1_ID,Chassis_motor1);
			maxon_setSpeed(MOTOR0_ID,Chassis_motor0);
			maxon_setSpeed(MOTOR1_ID,Chassis_motor1);
			maxon_setSpeed(MOTOR2_ID,Chassis_motor2);
			maxon_setSpeed(MOTOR3_ID,Chassis_motor3);
			maxon_setSpeed(MOTOR2_ID,Chassis_motor2);
			maxon_setSpeed(MOTOR3_ID,Chassis_motor3);
		}
	}
	else if(chassis.car_state == car_stop)
	{
		if(ms>200)
			USART_SendString(CMD_USARTx,"msg: %fs\n",ms/1000.f);
		ms = 0;
		/*pos_x = temp_x* 0.0001 * 0.81;
		pos_y = temp_y* 0.0001 * 0.81;
		angle = (temp_angle/180.f)*PI;*/
		//state = ready;
		//OPEN_Hander = 1;
	}
}

//stop
void chassis_stop()
{
	maxon_setSpeed(MOTOR0_ID,0);
	maxon_setSpeed(MOTOR1_ID,0);
	maxon_setSpeed(MOTOR2_ID,0);
	maxon_setSpeed(MOTOR3_ID,0);
	delay_ms(5);
	maxon_setSpeed(MOTOR0_ID,0);
	maxon_setSpeed(MOTOR1_ID,0);
	maxon_setSpeed(MOTOR2_ID,0);
	maxon_setSpeed(MOTOR3_ID,0);
}
