#ifndef __PARAM_H
#define __PARAM_H

#include "link_list.h"
//#include "configuration.h"

typedef struct 		//launch链表节点数据
{
	float pitch;	//俯仰角
	float turn;		//上面的转盘
	float yaw;		//偏航角
	float speed;	//飞轮转速
	float jmp;		//跳台舵机
}Launch_data;

typedef struct 		//目标线性表
{
	u32 launch_num;
	link_list launch_ptr; 
}Target;

typedef struct 		//pos链表节点数据
{
	float x,y,ang;
	Target d[7];
	Target r[7];
}Pos_data;

typedef struct 		//参数总结构线性表
{
	u32 pos_num;
	link_list pos_ptr;
}Param;

extern Param * param;//参数

#define FIRST_RUN 0
#define PARAM_FLASH_SETOR ((uint16_t)0x0028) //  扇区5
#define PARAM_FLASH_ADDR_START ((uint32_t)0x08020000)
#define PARAM_FLASH_ADDR_END ((uint32_t)0x0803FFFF)

int param_init(void); //初始化参数，从flash读取参数

int param_save(void); //将所有参数存入flash

bool cmp_pos(void *a,void *b);
bool cmp_launch(void *a,void *b);

void print_pos(void *a);
void print_launch(void *a);
void print_launch_list(link_list p);
void print_pos_list(link_list p);
void print_all(Param *p);

void clear_launch(link_list * first);
void clear_pos(link_list * first);

Pos_data * local_pos(int no);

#endif


