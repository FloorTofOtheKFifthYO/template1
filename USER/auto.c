#include "auto.h"
#include "chassis.h"
#include "vega.h"	
#include "flash.h"
#include "usart.h"
#include "flywheel_left.h"
#include "flywheel_right.h"
#include "radar.h"
#include "test.h"
#include "main.h"

struct strategy strategy;


int switch_time[8][8] = {//最后一行是一开始的转移到某台的参数
	{0   ,1000,1000,1000,1000,1000,1000,1000},
	{1000,0   ,1000,1000,1000,1000,1000,1000},
	{1000,1000,0   ,1000,1000,1000,1000,1000},
	{1000,1000,1000,0   ,1000,1000,1000,1000},
	{1000,1000,1000,1000,0   ,1000,1000,1000},
	{1000,1000,1000,1000,1000,0   ,1000,1000},
	{1000,1000,1000,1000,1000,1000,0   ,1000},
	{1000,1000,1000,1000,1000,1000,1000,0   }
};

int launch_left_time[7]={700,700,700,700,700,700,700};

int launch_right_time[7]={700,700,700,700,700,700,700};

extern Pos_data * now_pos;     //当前点的数据指针

AutoStrategy autorun;
static bool select_target_l = false;
static bool select_target_r = false;
bool handle_l = false;
bool handle_r = false;
static bool first_run = true;

static bool select_ball_l = false;
static bool select_ball_r = false;

//下面两个标志位是为了特殊发射的和普通发射的切换设的标志位
bool spe_l=false;
bool spe_r=false;

POSITION load_area[2];

/**
  * @brief  初始化
  *     
  * @note	初始化状态，参数，从flash中读出点和发射的参数
  *	
  * @param  
  *          
  * @retval 
  */
void auto_init()
{
	u32 addr = AUTO_FLASH_ADDR_START;
	int i,j;
	select_target_l = false;
	select_target_r = false;
	handle_l = false;
	handle_r = false;
	first_run = true;
	autorun.start_area.X  = 0;
	autorun.start_area.Y  = 0;
	autorun.start_area.ANG  = 0;
	autorun.target_l = 0;
	autorun.target_r = 0;
	
#if AUTO_FIRST_RUN == 1

	autorun.load_area.X = 7;
	autorun.load_area.Y  = 7;
	autorun.load_area.ANG  = 0;
	load_area[0]=autorun.load_area;
	autorun.load_area.X = -7;
	autorun.load_area.Y  = -7;
	autorun.load_area.ANG  = 0;
	load_area[1]=autorun.load_area;
	for(i = 0;i<7;i++)
	{
		strategy.left[i] = i;
		strategy.right[i] = 6-i;
	}
	autorun.load_area = load_area[LEFT_RIGHT];
	
	if(auto_save()<0)
	{
		USART_SendString(bluetooth,"msg: 写入flash出错\n");
		while(1);
	}
	
#else
	for(i = 0;i < 2; i++)
	{
		load_area[i].X = STMFLASH_ReadFloat_Inc(&addr);
		load_area[i].Y = STMFLASH_ReadFloat_Inc(&addr);
		load_area[i].ANG = STMFLASH_ReadFloat_Inc(&addr);
	}
	
	for(i = 0;i<7;i++)
	{
		strategy.left[i] = STMFLASH_ReadWord_Inc(&addr);
		strategy.right[i] = STMFLASH_ReadWord_Inc(&addr);
	}
	
	for(i = 0;i<8;i++)
	{
		for(j = 0;j<8;j++)
		{
			switch_time[i][j] = STMFLASH_ReadWord_Inc(&addr);
		}
	}
	
	for(i = 0;i<7;i++)
	{
		launch_left_time[i] = STMFLASH_ReadWord_Inc(&addr);
		launch_right_time[i] = STMFLASH_ReadWord_Inc(&addr);
	}
	
#endif
	
	autorun.load_area = load_area[LEFT_RIGHT];
	
	if(!param_init())
	{
		USART_SendString(bluetooth,"msg: 参数初始化出错\n");
		while(1);
	}
	autorun.now_pos_ptr = param->pos_ptr->link;
	if(autorun.now_pos_ptr == NULL)
	{
		USART_SendString(bluetooth,"msg: 没有点\n");
	}else
	{
		now_pos = autorun.now_pos_ptr->data;
	}
	autorun.state = start;
	
	autorun.load_run_flag = false;
	autorun.pos_run_flag = false;
	autorun.start_run_flag = false;
	if(debug || !auto_mode)
	{
		autorun.launch_l_continute = false;
		autorun.launch_r_continute = false;
	}else{
		autorun.launch_l_continute = true;
		autorun.launch_r_continute = true;
	}
	autorun.last_l = 7;
	autorun.last_r = 7;
	
	autorun.ball_l = 2;
	
	//暂定这么多，可能会改
}

void auto_select_l(int target)
{
	select_target_l = true;
	
	if(spe_l == true)
	{
		spe_l = false;
		autorun.last_l = autorun.ball_l;
		autorun.target_l = 2;
	}else{
		if(autorun.target_l<0 || autorun.target_l>6)
			autorun.last_l = 7;
		else
			autorun.last_l = strategy.left[autorun.target_l];
		autorun.target_l = target;
	}
	
}

void auto_select_r(int target)
{
	select_target_r = true;
	if(spe_r == true)
	{
		spe_r = false;
		autorun.last_r = autorun.ball_r;
		autorun.target_r = 3;
	}else{
		if(autorun.target_r<0 || autorun.target_r>6)
			autorun.last_r = 7;
		else
			autorun.last_r = strategy.left[autorun.target_r];
		autorun.target_r = target;
	}
}


void auto_ball_l()
{
	select_ball_l = true;
	if(spe_l == false)
	{
		spe_l = true;
	}
		
	autorun.ball_l = 5-autorun.ball_l;
}

void auto_ball_r(int target)
{
	select_ball_r = true;
	if(spe_r == false)
	{
		spe_r = true;
	}
		
	autorun.ball_r = target;
}

/**
  * @brief  保存参数
  *     
  * @note
  *
  * @param 
  *          
  * @retval 成功返回1，出错返回-1或-2
  */
int auto_save()
{
	u32 addr = AUTO_FLASH_ADDR_START;
	int i,j;
	load_area[LEFT_RIGHT] = autorun.load_area;
	FLASH_Unlock();  //解锁FLASH，写数据
    FLASH_DataCacheCmd(DISABLE);
    FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR | 
    								FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR|FLASH_FLAG_PGSERR);  //写FLASH之前，先清除标志
    if(FLASH_EraseSector(AUTO_FLASH_SETOR, VoltageRange_3) != FLASH_COMPLETE){ 
        FLASH_DataCacheCmd(ENABLE); 
        FLASH_Lock();//上锁
        return -1;   //擦写失败，，退出
    }
	for(i = 0; i < 2;i++)
	{
		FLASH_ProgramFloat(addr,load_area[i].X);
		addr+=4;
		FLASH_ProgramFloat(addr,load_area[i].Y);
		addr+=4;
		FLASH_ProgramFloat(addr,load_area[i].ANG);
		addr+=4;
	}
	for(i = 0;i<7;i++)
	{
		FLASH_ProgramWord(addr,strategy.left[i]);
		addr+=4;
		FLASH_ProgramWord(addr,strategy.right[i]);
		addr+=4;
	}
	for(i = 0;i<8;i++)
	{
		for(j = 0;j<8;j++)
		{
			FLASH_ProgramWord(addr,switch_time[i][j]);
			addr+=4;
		}
	}
	for(i = 0;i<7;i++)
	{
		FLASH_ProgramWord(addr,launch_left_time[i]);
		addr+=4;
		FLASH_ProgramWord(addr,launch_right_time[i]);
		addr+=4;
	}
	FLASH_DataCacheCmd(ENABLE);
    FLASH_Lock();  //  写保护
    if(addr > AUTO_FLASH_ADDR_END){   //如果超过FLASH的存储空间，则报错返回
        return -2;
    }
	
	return 1;

}

/**
  * @brief  主循环状态转移
  *     
  * @note
  *
  * @param 
  *          
  * @retval 
  */
void auto_main()
{
	static Pos_data * pos_data;
	static bool finish_left = false;
	Launch_data * launch_data_l;
	Launch_data * launch_data_r;
	static list_node* launch_ptr_l;
	static list_node* launch_ptr_r;

	
	if(autorun.start_run_flag)	//申请重启和强制重启
	{
		autorun.start_run_flag = false;
		chassis.END = autorun.start_area;
		chassis.car_state = car_ready;
		OPEN_Hander = 0;
		
		//发射机构复位
		flywheel_left_home();
		flywheel_right_home();
		radar_stop();
		
		autorun.state = start_running;
	}
	
	switch(autorun.state)
	{
		case start:
			if(autorun.load_run_flag)
			{
/*!!!!!*/		if(first_run){
					first_run = false;
					autorun.target_l = 0;
					autorun.target_r = 0;
					autorun.last_l = 7;
					autorun.last_r = 7;
				}
				finish_left = false;
				autorun.load_run_flag = false;
				chassis.END = autorun.load_area;
				flywheel_left_setPitch(-3);
				flywheel_right_setPitch(-4);
				chassis.car_state = car_ready;
				OPEN_Hander = 0;
				autorun.state = load_running;
			}else if(autorun.pos_run_flag)
			{
/*!!!!!*/		if(first_run){
					first_run = false;
				}
				finish_left = false;
				autorun.pos_run_flag = false;
				pos_data = autorun.now_pos_ptr->data;
				if(pos_data == NULL)
				{
					USART_SendString(bluetooth,"msg: 数据出错\n");
					autorun.state = start;
				}else
				{
					chassis.END.X = pos_data->x;
					chassis.END.Y = pos_data->y;
					chassis.END.ANG = pos_data->ang;
					
					//加坐标的判断	！！！
					
					chassis.car_state = car_ready;
					OPEN_Hander = 0;
					autorun.state = pos_running;
				}
			}
			break;
		case load_running:
			if(chassis.car_state == car_stop)
				autorun.state = load_arrived;
			else if(autorun.pos_run_flag)// 跑的过程中突然改变方向
			{
				autorun.pos_run_flag = false;
				pos_data = autorun.now_pos_ptr->data;
				chassis.END.X = pos_data->x;
				chassis.END.Y = pos_data->y;
				chassis.END.ANG = pos_data->ang;
				
				//加坐标的判断	！！！
				
				chassis.car_state = car_ready;
				OPEN_Hander = 0;
				
				flywheel_left_up(1);
				flywheel_right_up(1);
				
				//发射机构调整姿态，但不能发射
				do{
					launch_ptr_l = pos_data->d[strategy.left[autorun.target_l]].launch_ptr->link;
					autorun.target_l++;
				}while(launch_ptr_l==NULL);
					autorun.target_l--;
				if(launch_ptr_l != NULL)
				{
					launch_data_l = launch_ptr_l->data;
					if(launch_data_l != NULL){
						flywheel_left_flyn(1,launch_data_l->speed,launch_data_l->pitch,launch_data_l->yaw);
						test_left_target(strategy.left[autorun.target_l]);
						if(debug_print)
							USART_SendString(bluetooth,"left:x pitch:%f yaw:%f speed:%f\n",launch_data_l->pitch,launch_data_l->yaw,launch_data_l->speed);
					}
				}
				
				do{
					launch_ptr_r = pos_data->r[strategy.right[autorun.target_r]].launch_ptr->link;
					autorun.target_r++;
				}while(launch_ptr_r==NULL);
					autorun.target_r--;
				if(launch_ptr_r != NULL)
				{
					launch_data_r = launch_ptr_r->data;
					if(launch_data_r != NULL){
						test_right_target(strategy.right[autorun.target_r]);
						flywheel_right_flyn(3,launch_data_r->speed,launch_data_r->pitch,launch_data_r->yaw);
						if(debug_print)
							USART_SendString(bluetooth,"right:x pitch:%f turn:%f yaw:%f speed:%f\n",launch_data_r->pitch,launch_data_r->turn,launch_data_r->yaw,launch_data_r->speed);
					}
				}
				
				autorun.state = pos_running;
			}
			break;
		case load_arrived:
			if(autorun.pos_run_flag)
			{
				autorun.pos_run_flag = false;
				pos_data = autorun.now_pos_ptr->data;
				chassis.END.X = pos_data->x;
				chassis.END.Y = pos_data->y;
				chassis.END.ANG = pos_data->ang;

				chassis.car_state = car_ready;
				OPEN_Hander = 0;
				
				flywheel_left_up(1);
				flywheel_right_up(1);
				
				//发射机构调整姿态，但不能发射
				do{
					launch_ptr_l = pos_data->d[strategy.left[autorun.target_l]].launch_ptr->link;
					autorun.target_l++;
				}while(launch_ptr_l==NULL);
					autorun.target_l--;
				if(launch_ptr_l != NULL)
				{
					launch_data_l = launch_ptr_l->data;
					if(launch_data_l != NULL){
						test_left_target(strategy.left[autorun.target_l]);
						if(autorun.target_l==0)
							flywheel_left_flyn(3,launch_data_l->speed,launch_data_l->pitch,launch_data_l->yaw);
						else
							flywheel_left_flyn(0,launch_data_l->speed,launch_data_l->pitch,launch_data_l->yaw);
						if(debug_print)
							USART_SendString(bluetooth,"left:x pitch:%f yaw:%f speed:%f\n",launch_data_l->pitch,launch_data_l->yaw,launch_data_l->speed);
					}
				}
				if(!debug){
					do{
						launch_ptr_r = pos_data->r[strategy.right[autorun.target_r]].launch_ptr->link;
						autorun.target_r++;
					}while(launch_ptr_r==NULL);
						autorun.target_r--;
					if(launch_ptr_r != NULL)
					{
						launch_data_r = launch_ptr_r->data;
						if(launch_data_r != NULL){
							test_right_target(strategy.right[autorun.target_r]);
							if(autorun.target_r==0)
								flywheel_right_flyn(3,launch_data_r->speed,launch_data_r->pitch,launch_data_r->yaw);
							else
								flywheel_right_flyn(0,launch_data_r->speed,launch_data_r->pitch,launch_data_r->yaw);
							if(debug_print)
								USART_SendString(bluetooth,"right:x pitch:%f turn:%f yaw:%f speed:%f\n",launch_data_r->pitch,launch_data_r->turn,launch_data_r->yaw,launch_data_r->speed);
						}
					}
				}else
				{
					autorun.target_r = -1;
				}
				autorun.state = pos_running;
			}
			break;
		case pos_running:
			if(chassis.car_state == car_stop || chassis.fire)
			{
				//if(autorun.target_l != 0)
				radar_shoot();
				autorun.state = pos_arrived;
			}else if(autorun.load_run_flag)//跑的过程中突然改变方向
			{
				autorun.load_run_flag = false;
				flywheel_left_setPitch(-5);
				flywheel_right_setPitch(-3.5);
				chassis.END = autorun.load_area;
				chassis.car_state = car_ready;
				OPEN_Hander = 0;
				autorun.state = load_running;
			}
			break;
		case pos_arrived:
		
			if(debug){
				if(flywheel_left.state == fly_l_finish && autorun.target_l != -2 && autorun.launch_l_continute == true && handle_r == false)
				{
					autorun.launch_l_continute = false;
					/*if(autorun.target_l == 0)
						radar_shoot();*/
					if(spe_l == true)
					{
						spe_l = false;
						autorun.last_l = autorun.ball_l;
					}else{
						if(autorun.target_l<0 || autorun.target_l>6)
							autorun.last_l = 7;
						else
							autorun.last_l = strategy.left[autorun.target_l];
					}
					autorun.target_l++;
					USART_SendString(bluetooth,"msg: target%d\n",autorun.target_l);
					
					if(autorun.target_l == 7 || strategy.left[autorun.target_l] == -1)
						autorun.target_l = -2;
					else{
						launch_ptr_l = pos_data->d[strategy.left[autorun.target_l]].launch_ptr->link;
						if(launch_ptr_l != NULL)
						{
							launch_data_l = launch_ptr_l->data;
							if(launch_data_l != NULL){
								test_left_target(strategy.left[autorun.target_l]);
								flywheel_left_flyn(3,launch_data_l->speed,launch_data_l->pitch,launch_data_l->yaw);
								if(debug_print)
									USART_SendString(bluetooth,"left:x pitch:%f yaw:%f speed:%f\n",launch_data_l->pitch,launch_data_l->yaw,launch_data_l->speed);
							}
						}
					}
				}
				
				if(autorun.target_l == -2 && finish_left == false)
				{
					autorun.target_l = 2;
					flywheel_left_home();
					finish_left = true;
				}
				
				if(finish_left == true && flywheel_right.state == fly_r_finish && autorun.target_r != -2 && autorun.launch_r_continute == true&&handle_r == false)
				{
					autorun.launch_r_continute = false;
					if(spe_r == true)
					{
						spe_r = false;
						autorun.last_r = autorun.ball_r;
					}else{
						if(autorun.target_r<0 || autorun.target_r>6)
							autorun.last_r = 7;
						else
							autorun.last_r = strategy.left[autorun.target_r];
					}
					autorun.target_r++;
					USART_SendString(bluetooth,"msg: target%d\n",autorun.target_r);
					if(autorun.target_r == 7 || strategy.right[autorun.target_r] == -1)
						autorun.target_r = -2;
					else{
						launch_ptr_r = pos_data->r[strategy.right[autorun.target_r]].launch_ptr->link;
						if(launch_ptr_r != NULL)
						{
							launch_data_r = launch_ptr_r->data;
							if(launch_data_r != NULL){
								test_right_target(strategy.right[autorun.target_r]);
								flywheel_right_flyn(3,launch_data_r->speed,launch_data_r->pitch,launch_data_r->yaw);
								if(debug_print)
									USART_SendString(bluetooth,"right:x pitch:%f turn:%f yaw:%f speed:%f\n",launch_data_r->pitch,launch_data_r->turn,launch_data_r->yaw,launch_data_r->speed);
							}
						}
					}
				}
				
				if(finish_left == true && handle_l == false && autorun.target_r == -2 && handle_r == false)
				{
					autorun.target_l = 2;
					//select_target_l = true;
					handle_l = true;
					autorun.target_r = 3;
					//select_target_r = true;
					handle_r = true;
				}
				
				if(select_target_l && handle_l)
				{
					select_target_l = false;
					USART_SendString(bluetooth,"msg: target%d\n",autorun.target_l);
					if(autorun.target_l>=0 && autorun.target_l<7)
					{
						launch_ptr_l = pos_data->d[strategy.left[autorun.target_l]].launch_ptr->link;
						if(launch_ptr_l != NULL)
						{
							launch_data_l = launch_ptr_l->data;
							if(launch_data_l != NULL){
								test_left_target(strategy.left[autorun.target_l]);
								flywheel_left_flyn(3,launch_data_l->speed,launch_data_l->pitch,launch_data_l->yaw);
								if(debug_print)
									USART_SendString(bluetooth,"left:x pitch:%f yaw:%f speed:%f\n",launch_data_l->pitch,launch_data_l->yaw,launch_data_l->speed);
								
							}
						}
					}
				}
				
				if(select_target_r && handle_r)
				{
					select_target_r = false;
					USART_SendString(bluetooth,"msg: target%d\n",autorun.target_r);
					if(autorun.target_r>=0 && autorun.target_r<7)
					{
						launch_ptr_r = pos_data->r[strategy.right[autorun.target_r]].launch_ptr->link;
						if(launch_ptr_r != NULL)
						{
							launch_data_r = launch_ptr_r->data;
							if(launch_data_r != NULL){
								test_right_target(strategy.right[autorun.target_r]);
								flywheel_right_flyn(3,launch_data_r->speed,launch_data_r->pitch,launch_data_r->yaw);
								if(debug_print)
									USART_SendString(bluetooth,"right:x pitch:%f turn:%f yaw:%f speed:%f\n",launch_data_r->pitch,launch_data_r->turn,launch_data_r->yaw,launch_data_r->speed);
							}
						}
					}
				}
				
				if(select_ball_l)
				{
					select_ball_l = false;
					
					USART_SendString(bluetooth,"msg: spe l target%d\n",autorun.ball_l);
					launch_ptr_l = pos_data->d[autorun.ball_l].launch_ptr->link;
					if(launch_ptr_l != NULL)
					{
						launch_data_l = launch_ptr_l->data;
						if(launch_data_l != NULL){
							test_left_target(autorun.ball_l);
							flywheel_left_flyn(0,launch_data_l->speed,launch_data_l->pitch,launch_data_l->yaw);
							if(debug_print)
								USART_SendString(bluetooth,"left:x pitch:%f yaw:%f speed:%f\n",launch_data_l->pitch,launch_data_l->yaw,launch_data_l->speed);
						}
					}
					
				}
				
				if(select_ball_r)
				{
					select_ball_r = false;
					
					USART_SendString(bluetooth,"msg: spe r target%d\n",autorun.ball_r);
					launch_ptr_r = pos_data->r[autorun.ball_r].launch_ptr->link;
					if(launch_ptr_r != NULL)
					{
						launch_data_r = launch_ptr_r->data;
						if(launch_data_r != NULL){
							test_right_target(autorun.ball_r);
							flywheel_right_flyn(0,launch_data_r->speed,launch_data_r->pitch,launch_data_r->yaw);
							if(debug_print)
								USART_SendString(bluetooth,"right:x pitch:%f yaw:%f speed:%f\n",launch_data_r->pitch,launch_data_r->yaw,launch_data_r->speed);
						}
					}
					
				}
			
			}else{
				//循环发射所有台子，发完之后直接转移到下一状态
				if(flywheel_left.state == fly_l_finish && autorun.target_l != -2 && autorun.launch_l_continute == true&& handle_l == false)
				{
					if(!auto_mode)
						autorun.launch_l_continute = false;
					/*if(autorun.target_l == 0)
						radar_shoot();*/
					if(spe_l == true)
					{
						spe_l = false;
						autorun.last_l = autorun.ball_l;
					}else{
						if(autorun.target_l<0 || autorun.target_l>6)
							autorun.last_l = 7;
						else
							autorun.last_l = strategy.left[autorun.target_l];
					}
						
					autorun.target_l++;
					USART_SendString(bluetooth,"msg: target%d\n",autorun.target_l);
					if(autorun.target_l == 7 || strategy.left[autorun.target_l] == -1)
						autorun.target_l = -2;
					else{
						launch_ptr_l = pos_data->d[strategy.left[autorun.target_l]].launch_ptr->link;
						if(launch_ptr_l != NULL)
						{
							test_left_target(strategy.left[autorun.target_l]);
							launch_data_l = launch_ptr_l->data;
							if(launch_data_l != NULL){
								flywheel_left_flyn(3,launch_data_l->speed,launch_data_l->pitch,launch_data_l->yaw);
								if(debug_print)
									USART_SendString(bluetooth,"left:x pitch:%f yaw:%f speed:%f\n",launch_data_l->pitch,launch_data_l->yaw,launch_data_l->speed);
							}
						}
					}
					
				}
				if(flywheel_right.state == fly_r_finish && autorun.target_r != -2 && autorun.launch_r_continute == true && handle_r == false)
				{
					if(!auto_mode)
						autorun.launch_r_continute = false;
					if(spe_r == true)
					{
						spe_r = false;
						autorun.last_r = autorun.ball_r;
					}else{
						if(autorun.target_r<0 || autorun.target_r>6)
							autorun.last_r = 7;
						else
							autorun.last_r = strategy.left[autorun.target_r];
					}
					autorun.target_r++;
					USART_SendString(bluetooth,"msg: target%d\n",autorun.target_r);
					if(autorun.target_r == 7 || strategy.right[autorun.target_r] == -1)
						autorun.target_r = -2;
					else{
						launch_ptr_r = pos_data->r[strategy.right[autorun.target_r]].launch_ptr->link;
						if(launch_ptr_r != NULL)
						{
							launch_data_r = launch_ptr_r->data;
							if(launch_data_r != NULL){
								test_right_target(strategy.right[autorun.target_r]);
								if(strategy.right[autorun.target_r] == 3)
									flywheel_right_flyn(1,launch_data_r->speed,launch_data_r->pitch,launch_data_r->yaw);
								else
									flywheel_right_flyn(3,launch_data_r->speed,launch_data_r->pitch,launch_data_r->yaw);
								if(debug_print)
									USART_SendString(bluetooth,"right:x pitch:%f turn:%f yaw:%f speed:%f\n",launch_data_r->pitch,launch_data_r->turn,launch_data_r->yaw,launch_data_r->speed);
							}
						}
					}
				}
				
				if(autorun.target_l == -2 && handle_l == false)
				{
					autorun.target_l = 2;
					if(!auto_mode){
						autorun.target_l = 0;
						select_target_l = true;
					}
					handle_l = true;
				}
				
				if(autorun.target_r == -2 && handle_r == false)
				{
					autorun.target_r = 3;
					if(!auto_mode){
						autorun.target_r = 0;
						select_target_r = true;
					}
					handle_r = true;
				}
				
				/*if(autorun.target_l == -2 && autorun.target_r == -2)
				{
					autorun.target_l = -1;
					autorun.target_r = -1;
					radar_stop();
					autorun.state = handle_control;
				}
				break;
				case handle_control:*/
				if(select_target_l && handle_l)
				{
					select_target_l = false;
					USART_SendString(bluetooth,"msg: target%d\n",autorun.target_l);
					if(autorun.target_l>=0 && autorun.target_l<7)
					{
						USART_SendString(bluetooth,"msg: left:lasttarget%d\n",autorun.last_l);
						launch_ptr_l = pos_data->d[strategy.left[autorun.target_l]].launch_ptr->link;
						if(launch_ptr_l != NULL)
						{
							test_left_target(strategy.left[autorun.target_l]);
							launch_data_l = launch_ptr_l->data;
							if(launch_data_l != NULL){
								flywheel_left_flyn(0,launch_data_l->speed,launch_data_l->pitch,launch_data_l->yaw);
								if(debug_print)
									USART_SendString(bluetooth,"left:x pitch:%f yaw:%f speed:%f\n",launch_data_l->pitch,launch_data_l->yaw,launch_data_l->speed);
								
							}
						}
					}
				}
				
				if(select_target_r && handle_r)
				{
					select_target_r = false;
					USART_SendString(bluetooth,"msg: target%d\n",autorun.target_r);
					if(autorun.target_r>=0 && autorun.target_r<7)
					{
						launch_ptr_r = pos_data->r[strategy.right[autorun.target_r]].launch_ptr->link;
						if(launch_ptr_r != NULL)
						{
							launch_data_r = launch_ptr_r->data;
							if(launch_data_r != NULL){
								test_right_target(strategy.right[autorun.target_r]);
								flywheel_right_flyn(0,launch_data_r->speed,launch_data_r->pitch,launch_data_r->yaw);
								if(debug_print)
									USART_SendString(bluetooth,"right:x pitch:%f turn:%f yaw:%f speed:%f\n",launch_data_r->pitch,launch_data_r->turn,launch_data_r->yaw,launch_data_r->speed);
							}
						}
					}
				}
				
				
				if(select_ball_l)
				{
					select_ball_l = false;
					
					USART_SendString(bluetooth,"msg: spe l target%d\n",autorun.ball_l);
					launch_ptr_l = pos_data->d[autorun.ball_l].launch_ptr->link;
					if(launch_ptr_l != NULL)
					{
						launch_data_l = launch_ptr_l->data;
						if(launch_data_l != NULL){
							test_left_target(autorun.ball_l);
							flywheel_left_flyn(0,launch_data_l->speed,launch_data_l->pitch,launch_data_l->yaw);
							if(debug_print)
								USART_SendString(bluetooth,"left:x pitch:%f yaw:%f speed:%f\n",launch_data_l->pitch,launch_data_l->yaw,launch_data_l->speed);
						}
					}
					
				}
				
				if(select_ball_r)
				{
					select_ball_r = false;
					
					USART_SendString(bluetooth,"msg: spe r target%d\n",autorun.ball_r);
					launch_ptr_r = pos_data->r[autorun.ball_r].launch_ptr->link;
					if(launch_ptr_r != NULL)
					{
						launch_data_r = launch_ptr_r->data;
						if(launch_data_r != NULL){
							test_right_target(autorun.ball_r);
							flywheel_right_flyn(2,launch_data_r->speed,launch_data_r->pitch,launch_data_r->yaw);
							if(debug_print)
								USART_SendString(bluetooth,"right:x pitch:%f yaw:%f speed:%f\n",launch_data_r->pitch,launch_data_r->yaw,launch_data_r->speed);
						}
					}
					
				}
				
			}
		
			
			
			if(autorun.load_run_flag)
			{
				autorun.load_run_flag = false;
				flywheel_left_setPitch(-5);
				flywheel_right_setPitch(-3.5);
				chassis.END = autorun.load_area;
				chassis.car_state = car_ready;
				OPEN_Hander = 0;
				
				//发射机构复位
				flywheel_left_home();
				flywheel_right_home();
				radar_stop();
				
				autorun.state = load_running;
			}
			break;
		case start_running:
			if(chassis.car_state == car_stop)
				autorun.state = start;
			break;
	}
}
