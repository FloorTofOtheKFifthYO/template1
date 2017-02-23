#ifndef _UNDERPAN_H_
#define _UNDERPAN_H_
#include "sys.h"
#include <stdbool.h>
#include "configuration.h"

/*Ӧ�����
	#define CHASSIS

	#define MOTOR0_ID 0x0A
	#define MOTOR1_ID 0x0B
	#define MOTOR2_ID 0x0C
*/
#ifndef CHASSIS
#error δ�������
#endif

typedef struct 
{
	float X;
	float Y;
	float ANG;
}POSITION;

typedef struct
{
	//ȫ����λ���ص�����
	int g_vega_pos_x; 
	int g_vega_pos_y;   
	float g_vega_angle;
	
	int pos_x,pos_y;
	float angle;
	
	//״̬
	enum {car_stop, car_running, car_ready} car_state;//�����˶�״̬
	
	//����
	POSITION END, START;
	
	//����
	float direction_angle;
	int Speed_min;
	int Speed_max;
	float Move_radium;
	float Angle_radium;
	int Angle_speed;
	int Move_speed;
	float Start_distance;
} Chassis;

extern Chassis chassis;

/** ���̳�ʼ��
  * ����ȫ����λ�ĳ�ʼ��
  *
  */
void chassis_init(void);

//��ѭ����ѯ����
void chassis_update();

/**��ѭ�����ֱ�����
  *������float directoion 	�����
  *		int speed			�ٶ�
  *�����ٶ��Ƿ�Ϊ0����Ϊ0����ֻ��һ��
  */
void chassis_handle(float directoion, int speed);

/** ��ѭ�����Զ��ܵ��״̬ת�ƺ���
  * ͨ����chassis.END������Ŀ�ĵ�
  */
void chassis_auto();

#endif
