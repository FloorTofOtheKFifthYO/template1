//唉。。。
//复位：
//	cmd[0] = 0x00, cmd[1] = 0x00 , cmd[2] = 0x00 , cmd[3] = 0x00 , cmd[4] = 0x00;
//设置：
//	cmd[0] ,使能无刷闭环或者码盘反馈，第一位为无刷，第二位码盘反馈
//		例如，cmd[0] |= (1<<1) ,此为使能了码盘反馈
//	cmd[1] ,PWM输出通道，可为6 ，7 ，8 ，9，对应板子的C6，C7，A8,A9。当没有使能无刷闭环时，
//		也可以使用PWM输出功能,使用方法是，不选用无刷闭环功能，但设置PWM输出频率，并设置输出通道
//	cmd[2] ,设置PWM的频率，存放高位，
//	cmd[3] ,设置PWM的频率，存放低位，所以最大支持到65535HZ的P波输出
//	cmd[4] ,设置使用哪个定时器进行无刷的反馈，可用2 3 4，对应TIM2,TIM3,TIM4
//	cmd[5] ,设置反馈定时器，暂时不能使用的功能
//	cmd[6] ,设置反馈周期，暂时不能使用
//控制：
//	单独继电器模式
//		cmd[0] , 第一位--B12 , 第二位--B13 ， 第三位--B14
//		例如，cmd[0] |=1<<2 ,将B14置高
//	速度或P波频率设置
//		两种模式
//		一、单独一路设置，由3个成员组成一个信息块，第一个成员为复选，后两个为参数值
//		cmd[0] ,低4位认为本块为速度设置，高四位认为本块为P波频率设置
//			例如，cmd[0] |= 7<<4,则本次为设置7通道的p波频率
//		cmd[1] ,参数值的高8位
//		cmd[2] ,参数值得低八位
//		二、两路同时设置，区别就是一次指令里有两个信息块，其他一样，其中cmd[0]和cmd[3]位复选

#include "client.h"
#include "can.h"
#include "delay.h"

#define COUNT_NUM 5000

#include "can.h"
u8 setSpeed_1[3] = {0};
u8 setSpeed_2[6] = {0};
u8 setIO[1] = {0};

void init_subsector(u8 ID , u8 EN_unbrush,u8 channel ,u16 frep, u8 feedback_tim){
	int count = 0;
	u8 init_buf[7] = {0};
	count = COUNT_NUM;
	while(count-->0);
	can_send_msg(ID,init_buf,7);
	delay_ms(200);
	init_buf[0] |= EN_unbrush;//使能闭环
	init_buf[1] = channel;
	init_buf[2] =(u8)(frep>>8);
	init_buf[3] = (u8)frep;
	init_buf[4] = feedback_tim;
	init_buf[5] = 0;//暂时未使用
	init_buf[6] = 0;
	
	count = COUNT_NUM;
	while(count-->0);
	can_send_msg(ID,init_buf,7);
	delay_ms(200);
}
//设置速度
void setUnbrushSpeed_1(u8 ID,u8 channel , int speed){
	int count;
	setSpeed_1[0] = channel;
	setSpeed_1[1] = (u8)(speed>>8);
	setSpeed_1[2] = (u8)(speed>>0);
	count = COUNT_NUM;
	while(count-->0);
	can_send_msg(ID,setSpeed_1,3);
}

void setUnbrushSpeed_2(u8 channel_1 , int speed_1,u8 channel_2 , int speed_2){
	
}


void set_IO(u8 ID, u8 io[]){
	setIO[0] = 0;
	setIO[0] |= io[0]<<0;
	setIO[0] |= io[1]<<1;
	setIO[0] |= io[2]<<2;
	can_send_msg(ID,setIO,1);
}
