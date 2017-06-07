#ifndef _AUTO_H_
#define _AUTO_H_

/*
	�������������̣�ע��Ҫ�����صĵ㣬
	���ֶ�ȷ��pos�㣬����flash���ټ����Զ�������Ῠ����ĳ״̬
	�����״̬�£�Ҫ�뵽��һ����ʱ�����flash��û�е��ˣ�ֻ��ȥװ����������������
*/

#include "sys.h"
#include "configuration.h"
#include "chassis.h"
#include "link_list.h"
#include "param.h"

#define AUTO_FIRST_RUN 0
#define AUTO_FLASH_SETOR FLASH_Sector_7 //  ����7
#define AUTO_FLASH_ADDR_START ((uint32_t)0x08060000)
#define AUTO_FLASH_ADDR_END ((uint32_t)0x0807FFFF)

#define LEFT_N 3
#define RIGHT_N 4

typedef struct {
	POSITION start_area;
	POSITION load_area;
	list_node * now_pos_ptr;
	enum {start,load_running,load_arrived,pos_running,pos_arrived,/*handle_control,*/start_running} state;
	
	//������
	bool load_run_flag;
	bool pos_run_flag;
	bool start_run_flag;
	bool launch_l_continute;
	bool launch_r_continute;
	
	int ball_l;
	int ball_r;
	int last_l;
	int last_r;
	int target_l;
	int target_r;

} AutoStrategy;

extern AutoStrategy autorun;

struct strategy{
	int left[7];
	int right[7];
};

extern struct strategy strategy;
extern int switch_time[8][8];

extern int launch_left_time[7];

extern int launch_right_time[7];

/**
  * @brief  ��ʼ��
  *     
  * @note
  *
  * @param  
  *          
  * @retval 
  */
void auto_init();

/**
  * @brief  ��ѭ��״̬ת��
  *     
  * @note
  *
  * @param 
  *          
  * @retval 
  */
void auto_main();

/**
  * @brief  �������
  *     
  * @note
  *
  * @param 
  *          
  * @retval �ɹ�����1��������-1��-2
  */
int auto_save();

void auto_select_l(int target);

void auto_select_r(int target);

void auto_ball_l();

void auto_ball_r(int target);

#endif
