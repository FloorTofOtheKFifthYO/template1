#include "param.h"
#include "usart.h"
#include "flash.h"
#include <stdlib.h>

Param * param;//参数

Param *params[2];

/**
  * @brief  初始化参数结构体
  *
  * @param  param: 参数指针  此处作为全局变量而不包含
  *          
  * @retval int: 返回0，出错，程序直接退出
  *				返回1，初始化成功
  */
int param_init(void)
{
	u32 addr = PARAM_FLASH_ADDR_START;
	int n;
	int i, j, k;
	Pos_data * pos_data;
	Launch_data * launch_data;
	for(n = 0;n < 2;n++)
	{
		params[n] = (Param *) malloc(sizeof(Param));
		list_init(&params[n]->pos_ptr);
	#if FIRST_RUN
		params[n]->pos_num = 0;
		
	#else 
		params[n]->pos_num = STMFLASH_ReadWord_Inc(&addr);
		for (i = 0; i < params[n]->pos_num; ++i)
		{
			pos_data = (Pos_data *) malloc(sizeof(Pos_data));
			if (pos_data == NULL)
				return 0;
			pos_data->x = STMFLASH_ReadFloat_Inc(&addr);
			pos_data->y = STMFLASH_ReadFloat_Inc(&addr);
			pos_data->ang = STMFLASH_ReadFloat_Inc(&addr);
			for (j = 0; j < 7; ++j)
			{
				pos_data->d[j].launch_num = STMFLASH_ReadWord_Inc(&addr);
				list_init(&pos_data->d[j].launch_ptr);
				if(pos_data == NULL) return 0;
				for (k = 0; k < pos_data->d[j].launch_num; ++k)
				{
					launch_data = (Launch_data *) malloc(sizeof(Launch_data));
					launch_data->pitch = STMFLASH_ReadFloat_Inc(&addr);
					launch_data->turn = STMFLASH_ReadFloat_Inc(&addr);
					launch_data->speed = STMFLASH_ReadFloat_Inc(&addr);
					launch_data->yaw = STMFLASH_ReadFloat_Inc(&addr);
					launch_data->jmp = STMFLASH_ReadFloat_Inc(&addr);
					
					if(!list_insert(&pos_data->d[j].launch_ptr, k+1, launch_data))
						return 0;
				}
			}
			
			for (j = 0; j < 7; ++j)
			{
				pos_data->r[j].launch_num = STMFLASH_ReadWord_Inc(&addr);
				list_init(&pos_data->r[j].launch_ptr);
				if(pos_data == NULL) return 0;
				for (k = 0; k < pos_data->r[j].launch_num; ++k)
				{
					launch_data = (Launch_data *) malloc(sizeof(Launch_data));
					launch_data->pitch = STMFLASH_ReadFloat_Inc(&addr);
					launch_data->turn = STMFLASH_ReadFloat_Inc(&addr);
					launch_data->speed = STMFLASH_ReadFloat_Inc(&addr);
					launch_data->yaw = STMFLASH_ReadFloat_Inc(&addr);
					launch_data->jmp = STMFLASH_ReadFloat_Inc(&addr);
					
					if(!list_insert(&pos_data->r[j].launch_ptr, k+1, launch_data))
						return 0;
				}
			}
			
			if(list_insert(&params[n]->pos_ptr, i+1, pos_data) == 0)
				return 0;
		}
	#endif
	}
	#if FIRST_RUN
	if(!param_save())
	{
		USART_SendString(bluetooth,"param save error\n");
		while(1);
	}
	#endif
	param = params[LEFT_RIGHT];
	
	return 1;
}

/**
  * @brief  初始化参数结构体
  *
  * @param  param: 参数指针 此处作为全局变量而不包含 
  *          
  * @retval int: 返回<0，出错，程序直接退出
  *				返回1，保存
  */
int param_save(void)
{
	u32 addr = PARAM_FLASH_ADDR_START;
	list_node * pos_ptr;
	list_node * launch_ptr;
	Pos_data * pos_data;
	Launch_data * launch_data;
	int i,n;
	FLASH_Unlock();  //解锁FLASH，写数据
    FLASH_DataCacheCmd(DISABLE);
    FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR | 
    								FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR|FLASH_FLAG_PGSERR);  //写FLASH之前，先清除标志
    if(FLASH_EraseSector(PARAM_FLASH_SETOR, VoltageRange_3) != FLASH_COMPLETE){ 
        FLASH_DataCacheCmd(ENABLE); 
        FLASH_Lock();//上锁
        return -1;   //擦写失败，，退出
    }
	
	for(n = 0; n<2;n++)
	{
		params[n]->pos_num = list_get_length(&(params[n]->pos_ptr));
		FLASH_ProgramWord(addr,params[n]->pos_num);
		addr+=4;
		pos_ptr = params[n]->pos_ptr->link;
		
		while(pos_ptr!=NULL)
		{
			pos_data = (Pos_data *)pos_ptr->data;

			FLASH_ProgramFloat(addr, pos_data->x);
			addr+=4;
			FLASH_ProgramFloat(addr, pos_data->y);
			addr+=4;
			FLASH_ProgramFloat(addr, pos_data->ang);
			addr+=4;
			
			for (i = 0; i < 7; ++i)
			{
				pos_data->d[i].launch_num = list_get_length(&(pos_data->d[i].launch_ptr));
				FLASH_ProgramWord(addr, pos_data->d[i].launch_num);
				addr+=4;
				launch_ptr = pos_data->d[i].launch_ptr->link;

				while(launch_ptr != NULL)
				{
					launch_data = (Launch_data *)launch_ptr->data;

					FLASH_ProgramFloat(addr, launch_data->pitch);
					addr+=4;
					FLASH_ProgramFloat(addr, launch_data->turn);
					addr+=4;
					FLASH_ProgramFloat(addr, launch_data->speed);
					addr+=4;
					FLASH_ProgramFloat(addr, launch_data->yaw);
					addr+=4;
					FLASH_ProgramFloat(addr, launch_data->jmp);
					addr+=4;
					launch_ptr = launch_ptr->link;
				}
			}

			for (i = 0; i < 7; ++i)
			{
				pos_data->r[i].launch_num = list_get_length(&(pos_data->r[i].launch_ptr));
				FLASH_ProgramWord(addr, pos_data->r[i].launch_num);
				addr+=4;
				launch_ptr = pos_data->r[i].launch_ptr->link;

				while(launch_ptr != NULL)
				{
					launch_data = (Launch_data *)launch_ptr->data;

					FLASH_ProgramFloat(addr, launch_data->pitch);
					addr+=4;
					FLASH_ProgramFloat(addr, launch_data->turn);
					addr+=4;
					FLASH_ProgramFloat(addr, launch_data->speed);
					addr+=4;
					FLASH_ProgramFloat(addr, launch_data->yaw);
					addr+=4;
					FLASH_ProgramFloat(addr, launch_data->jmp);
					addr+=4;
					launch_ptr = launch_ptr->link;
				}
			}
			
			pos_ptr = pos_ptr->link;
		}
	}
    FLASH_DataCacheCmd(ENABLE);
    FLASH_Lock();  //  写保护
    if(addr > PARAM_FLASH_ADDR_END){   //如果超过FLASH的存储空间，则报错返回
        return -2;
    }

    return 1;
}

/**
  * @brief  比较两个发射点坐标
  *     
  * @note	在链表中被调用作比较
  *
  * @param  a,b 指向被比较的(void*)data
  *          
  * @retval bool: 是否相同
  */
bool cmp_pos(void *a,void *b){
	Pos_data* p=(Pos_data*)a;
	Pos_data* q=(Pos_data*)b;
	if (p->x==q->x && p->y==q->y)
		return true;
	return false;
}

/**
  * @brief  比较两个发射参数
  *     
  * @note	在链表中被调用作比较
  *
  * @param  a,b: 被比较的(void*)data
  *          
  * @retval bool: 是否相同
  */
bool cmp_launch(void *a,void *b){
	Launch_data* p=(Launch_data*)a;
	Launch_data* q=(Launch_data*)b;
	if (p->pitch==q->pitch &&
		p->turn==q->turn &&
		p->yaw==q->yaw &&
		p->speed==q->speed && 
		p->jmp==q->jmp)
		return true;
	return false;
}

/**
  * @brief  输出发射点坐标
  *     
  * @note	
  *
  * @param  a: 数据的地址指针  
  *          
  * @retval void
  */
void print_pos(void *a){
	Pos_data* p=(Pos_data*)a;
	USART_SendString(bluetooth,"x:%.3f y:%.3f ang:%.3f\n",p->x,p->y,p->ang);
}

/**
  * @brief  输出发射点参数
  * 
  * @note	
  *
  * @param  a: 数据的地址指针  
  * 
  * @retval void
  */
void print_launch(void *a){
	Launch_data* p=(Launch_data*)a;
	USART_SendString(bluetooth,"pitch:%.3f turn:%.3f speed:%.3f yaw:%.3f jmp:%.3f\n",p->pitch,p->turn,p->speed,p->yaw, p->jmp);
}

/**
  * @brief  输出发射参数链表
  *     
  * @note	
  *
  * @param  p: 第一组发射参数的链表节点指针  
  *          
  * @retval void
  */
void print_launch_list(link_list p){
	int k = 1;
	while (p!=NULL){
		USART_SendString(UART5,"l:%d ", k);
		print_launch(p->data);
		k++;
		p=p->link;
	}
}

/**
  * @brief  输出发射坐标链表
  *     
  * @note	
  *
  * @param  p: 第一组发射坐标的链表节点指针  
  *          
  * @retval void
  */
void print_pos_list(link_list p){
	int k = 1;
	while (p!=NULL){
		USART_SendString(UART5,"p:%d ", k++);
		print_pos(p->data);
		p=p->link;
	}
}

/**
  * @brief  输出整个链表
  *     
  * @note	
  *
  * @param  p: 参数指针  
  *          
  * @retval void
  */
void print_all(Param *p){
	int i;
	Pos_data *pos_temp;
	link_list ptr=p->pos_ptr;
	while (ptr!=NULL){
		print_pos(ptr->data);
		pos_temp=(Pos_data*)(ptr->data);
		for (i=0;i<7;i++)
		if (pos_temp->d[i].launch_num>0)
			print_launch_list(pos_temp->d[i].launch_ptr);
		for (i=0;i<7;i++)
		if (pos_temp->r[i].launch_num>0)
			print_launch_list(pos_temp->r[i].launch_ptr);
		ptr=ptr->link;
	}
}

void clear_launch(link_list * first)
{
	Launch_data * data;
	list_node * launch = (*first)->link;
	while(launch != NULL)
	{
		data = launch->data;
		free(data);
		launch = launch->link;
	}
	list_clear(first);
}

void clear_pos(link_list * first)
{
	list_node * pos = (*first)->link;
	Pos_data *data;
	int i;
	while(pos!=NULL)
	{
		data = pos->data;
		for (i = 0; i < 7; ++i)
		{
			clear_launch(&data->d[i].launch_ptr);
			free(data->d[i].launch_ptr);
			clear_launch(&data->r[i].launch_ptr);
			free(data->r[i].launch_ptr);
		}
		free(data);
		pos = pos->link;
	}
	list_clear(first);
}

Pos_data * local_pos(int no)
{
	Pos_data * data;
	list_node * ptr;
	ptr = list_locate(&param->pos_ptr, no);
	if (ptr == NULL)
	{
		return NULL;
	}
	return ptr->data;
}
