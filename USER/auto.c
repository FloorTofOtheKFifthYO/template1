#include "auto.h"
#include "chassis.h"
#include "flash.h"
#include "usart.h"
#include "flywheel_left.h"
#include "flywheel_right.h"

AutoStrategy autorun;

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
	int i;
	autorun.start_area.X  = 0;
	autorun.start_area.Y  = 0;
	autorun.start_area.ANG  = 0;
	
#if AUTO_FIRST_RUN == 1

	autorun.load_area.X = 6;
	autorun.load_area.Y  = 6;
	autorun.load_area.ANG  = 0;
	load_area[0]=autorun.load_area;
	load_area[1]=autorun.load_area;
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
	autorun.load_area = load_area[LEFT_RIGHT];
	
#endif
	if(!param_init())
	{
		USART_SendString(bluetooth,"msg: 参数初始化出错\n");
		while(1);
	}
	autorun.now_pos_ptr = param->pos_ptr->link;
	if(autorun.now_pos_ptr == NULL)
	{
		USART_SendString(bluetooth,"msg: 没有点\n");
	}
	autorun.state = start;
	
	autorun.load_run_flag = false;
	autorun.pos_run_flag = false;
	autorun.start_run_flag = false;
	//暂定这么多，可能会改
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
	int i;
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
	Pos_data * pos_data;
	
	if(autorun.start_run_flag)	//申请重启和强制重启
	{
		autorun.start_run_flag = false;
		chassis.END = autorun.start_area;
		chassis.car_state = car_ready;
		OPEN_Hander = 0;
		
		//发射机构复位
		
		autorun.state = start_running;
	}
	
	switch(autorun.state)
	{
		case start:
			if(autorun.load_run_flag)
			{
				autorun.load_run_flag = false;
				chassis.END = autorun.load_area;
				chassis.car_state = car_ready;
				OPEN_Hander = 0;
				autorun.state = load_running;
			}else if(autorun.pos_run_flag)
			{
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
			break;
		case load_arrived:
			if(autorun.pos_run_flag)
			{
				autorun.pos_run_flag = false;
				pos_data = autorun.now_pos_ptr->data;
				chassis.END.X = pos_data->x;
				chassis.END.Y = pos_data->y;
				chassis.END.ANG = pos_data->ang;
				
				//加坐标的判断	！！！
				
				chassis.car_state = car_ready;
				OPEN_Hander = 0;
				
				//发射机构调整姿态，但不能发射
				
				autorun.state = pos_running;
			}
			break;
		case pos_running:
			if(chassis.car_state == car_stop)
			{
				autorun.state = pos_arrived;
			}
			break;
		case pos_arrived:
			
			//循环发射所有台子，发完之后直接转移到下一状态
		
			break;
		case handle_control:
		
			if(autorun.load_run_flag)
			{
				autorun.load_run_flag = false;
				chassis.END = autorun.load_area;
				chassis.car_state = car_ready;
				OPEN_Hander = 0;
				
				//发射机构复位
				
				autorun.state = load_running;
			}else if(autorun.pos_run_flag)
			{
				autorun.pos_run_flag = false;
				
				if(autorun.now_pos_ptr->link == NULL)
				{
					USART_SendString(bluetooth,"msg: 没点了\n");
					autorun.state = handle_control;
				}else{
					autorun.now_pos_ptr = autorun.now_pos_ptr->link;
					pos_data = autorun.now_pos_ptr->data;
					if(pos_data == NULL)
					{	
						USART_SendString(bluetooth,"msg: 数据出错\n");
						autorun.state = handle_control;
					}else{
						chassis.END.X = pos_data->x;
						chassis.END.Y = pos_data->y;
						chassis.END.ANG = pos_data->ang;
						chassis.car_state = car_ready;
						OPEN_Hander = 0;
						
						//发射机构调整姿态，但不能发射
						
						autorun.state = pos_running;
					}
				}
			}
			break;
		case start_running:
			if(chassis.car_state == car_stop)
				autorun.state = start;
			break;
	}
}
