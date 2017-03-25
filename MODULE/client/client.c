#include "client.h"
#include "can.h"

#define COUNT_NUM 0
/*

*功能：复位从板
*参数：Client client 从机的结构体
*		
*/
void Client_RESET(Client client){
	int count;
	u8 motor_message[8];
	
	motor_message[0] = 0x55;
	motor_message[1] = 0x55;
	motor_message[2] = 0x55;
	motor_message[3] = 0x55;
	motor_message[4] = 0x55;
	motor_message[5] = 0x55;
	motor_message[6] = 0x55;
	motor_message[7] = 0x55;

	count = COUNT_NUM;
	while(count-->0);
	can_send_msg(client.can_id , motor_message , 8);
}

/*

*功能：设置从板的pwm和继电器
*参数：Client client 从机的结构体
*		
*/
void Client_SET(Client client){
	int count;
	u8 motor_message[8];
	
	motor_message[0] = (unsigned char)((client.pwm1>>8)&0xff);
	motor_message[1] = (unsigned char)((client.pwm1)&0xff);
	motor_message[2] = (unsigned char)((client.pwm2>>8)&0xff);
	motor_message[3] = (unsigned char)((client.pwm2)&0xff);
	motor_message[4] = (unsigned char)(client.relay[0]);
	motor_message[5] = (unsigned char)(client.relay[0]);
	motor_message[6] = (unsigned char)(client.relay[0]);
	motor_message[7] = (unsigned char)(client.relay[0]);

	count = COUNT_NUM;
	while(count-->0);
	can_send_msg(client.can_id , motor_message , 8);
}