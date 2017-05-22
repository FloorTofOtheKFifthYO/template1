#include "controller_usart.h"
#include "configuration.h"
#include "usart.h"
#include "flywheel_left.h"
#include "flywheel_right.h"
#include "chassis.h"
#include <math.h>
#include "auto.h"

extern bool g_stop_flag;
extern int wait_cnt;
extern struct {
	int left[7];
	int right[7];
}strategy;
extern Pos_data * now_pos;     //当前点的数据指针
extern bool handle_l;
extern bool handle_r;

#define BT_UP 0
#define BT_RIGHT 1
#define BT_DOWN 2
#define BT_LEFT 3

#define DELT_YAW_LEFT -0.01
#define DELT_SPEED_LEFT 0.00001
#define FACTOR_SPEED_LEFT 1

#define DELT_YAW_RIGHT -0.005
#define DELT_SPEED_RIGHT 0.00005
#define FACTOR_SPEED_RIGHT 1

//上右下左
const float convert[7][4][2] ={
								{{
									0.5548444274479993 , 0.8319541221304826
									},{
									0.8319541221304826 , -0.5548444274479993
									},{
									-0.5548444274479993 , -0.8319541221304826
									},{
									-0.8319541221304826 , 0.5548444274479993
								}},
								{{
									0.3156490369471024 , 0.9488760116444965
									},{
									0.9488760116444965 , -0.3156490369471024
									},{
									-0.3156490369471024 , -0.9488760116444965
									},{
									-0.9488760116444965 , 0.3156490369471024
								}},
								{{
									0.0 , 1.0
									},{
									1.0 , -0.0
									},{
									-0.0 , -1.0
									},{
									-1.0 , 0.0
								}},
								{{
									0.0 , 1.0
									},{
									1.0 , -0.0
									},{
									-0.0 , -1.0
									},{
									-1.0 , 0.0
								}},
								{{
									0.0 , 1.0
									},{
									1.0 , -0.0
									},{
									-0.0 , -1.0
									},{
									-1.0 , 0.0
								}},
								{{
									-0.3156490369471024 , 0.9488760116444965
									},{
									0.9488760116444965 , 0.3156490369471024
									},{
									0.3156490369471024 , -0.9488760116444965
									},{
									-0.9488760116444965 , -0.3156490369471024
								}},
								{{
									-0.5548444274479993 , 0.8319541221304826
									},{
									0.8319541221304826 , 0.5548444274479993
									},{
									0.5548444274479993 , -0.8319541221304826
									},{
									-0.8319541221304826 , -0.5548444274479993
								}}
							};

bottons LU,LR,LD,LL,RU,RR,RD,RL,L1,L2,R1,R2;
bottons* b[12]={&LU,&LR,&LD,&LL,&RU,&RR,&RD,&RL,&L1,&L2,&R1,&R2};
sticks L,R;
sticks* s[2]={&L,&R};

s8 ptrB,ptrS;
static int *x,*y;

extern u8 OPEN_Hander;

extern Chassis chassis;

void USART6_IRQHandler(void)                	
{
	u8 Res;	u8 tmp;
	if(USART_GetITStatus(USART6, USART_IT_RXNE) != RESET) 
	{
		OPEN_Hander = 1;
		Res =USART_ReceiveData(USART6);
		if (Res==0x0d&&ptrS<0) {
			ptrS=-ptrS;
			return;
		}
		if (ptrB==-1){
		switch (Res){
			case 'A':
				ptrB=LU_KEY;
				break;
			case 'D':
				ptrB=LR_KEY;
				break;
			case 'B':
				ptrB=LD_KEY;
				break;
			case 'C':
				ptrB=LL_KEY;
				break;
			case 'F':     
				ptrB=L2_KEY;
				break;
			case 'N':
				ptrB=R2_KEY;
				break;
			case 'M':        
				ptrB=R1_KEY;
				break;
			case 'E':
				ptrB=L1_KEY;
				break;
			case 'I':       
				ptrB=RU_KEY;
				break;
			case 'L':       
				ptrB=RR_KEY;
				break;
			case 'J':       
				ptrB=RD_KEY;
				break;
			case 'K':        
				ptrB=RL_KEY;
				break;
			default:
				break;
		}}
		if (ptrS<=0){
		switch(Res){
			case 'W':       
				ptrS=-1;
				*x=0;
				break;
			case 'P':      
				*y=*x;
				*x=0;
				break;
			case 'Q':      
				ptrS=-2;
				*x=0;
				break;
			case 'S':      
				*y=*x;
				*x=0;
				break;
			default :
				*x=*x * 10 + Res - 48;
				break;
		}}
		//USART_SendString(UART5,"D=%f \n",direction_angle);
     }		
}

void controller_usart_init(int *Hx,int *Hy)
{
	usart_init(USART6,115200,true);
	x=Hx;y=Hy;
	ptrB=-1;
	ptrS=0;
}

void bottons_check(){
	if (ptrB>-1){
		if (ptrB>=4&ptrB<=7)
			b[ptrB]->cnt=2;
		else b[ptrB]->cnt=100;
		b[ptrB]->ispressed=true;
		ptrB=-1;
	}
	if (ptrS>0){
		g_stop_flag = true;
		ptrS=0;
	}
}

void sticks_check(int Hx,int Hy){
	if (ptrS>0){
		if (Hx>=0&&Hx<256&&Hy>=0&&Hy<256){
			s[ptrS-1]->x=Hx-127;
			s[ptrS-1]->y=128-Hy;}
		ptrS=0;
	}
}


void control_usart_main()
{
	int Xianding = 0;
	int ChassisSpeed;
	float direction_angle;
	Launch_data * data;
    list_node * ptr;
	
	Xianding = 0;
	ChassisSpeed = 500;
	if (!L2.ispressed&&!R2.ispressed){
		
		if (LU.ispressed) direction_angle = 3*PI/4;
		else if (LD.ispressed) direction_angle = -PI/4;
		else if (LL.ispressed) direction_angle = PI/4;
		else if (LR.ispressed) direction_angle = -3*PI/4;
		else ChassisSpeed = 0;
		if(!g_stop_flag)
			chassis_handle(direction_angle, ChassisSpeed);
		
		if (RU.ispressed)
		{ 
			if(autorun.state == start)
			{
				autorun.pos_run_flag = true;
			}
		}
		
		if (RL.ispressed) {
			if(LEFT_RIGHT==0)//红场
			{
				switch(autorun.state)//start,load_running,load_arrived,pos_running,pos_arrived,handle_control,start_running
				{
					case start:
						autorun.load_run_flag = true;
						break;
					case load_running:
					case load_arrived:
						break;
					case pos_running:
					case pos_arrived:
					//case handle_control:
						autorun.load_run_flag = true;
						break;
					case start_running:
						break;
				}
			}else if(LEFT_RIGHT==1)//蓝场
			{
				switch(autorun.state)//start,load_running,load_arrived,pos_running,pos_arrived,handle_control,start_running
				{
					case start:
						break;
					case load_running:
						autorun.start_run_flag = true;
						break;
					case load_arrived:
						autorun.pos_run_flag = true;
						break;
					case pos_running:
						break;
					case pos_arrived:
					//case handle_control:
						autorun.start_run_flag = true;
						break;
					case start_running:
						break;
				}
			}
		}
		
		if (RR.ispressed) {
			RR.ispressed = false;
			if(LEFT_RIGHT==0)//红场
			{
				switch(autorun.state)//start,load_running,load_arrived,pos_running,pos_arrived,handle_control,start_running
				{
					case start:
						break;
					case load_running:
						autorun.start_run_flag = true;
						break;
					case load_arrived:
						autorun.pos_run_flag = true;
						break;
					case pos_running:
						break;
					case pos_arrived:
					//case handle_control:
						autorun.start_run_flag = true;
						break;
					case start_running:
						break;
				}
			}else if(LEFT_RIGHT==1)//蓝场
			{
				switch(autorun.state)//start,load_running,load_arrived,pos_running,pos_arrived,handle_control,start_running
				{
					case start:
						autorun.load_run_flag = true;
						break;
					case load_running:
					case load_arrived:
						break;
					case pos_running:
					case pos_arrived:
					//case handle_control:
						autorun.load_run_flag = true;
						break;
					case start_running:
						break;
				}
			}
		}
		if (RD.ispressed) {
			RD.ispressed = false;
			//g_stop_flag = !g_stop_flag;
		}
		
	}else{
	//ChassisSpeed = 0;
	//chassis_handle(direction_angle, ChassisSpeed);
		
	}
	if (L2.ispressed && !R2.ispressed) {
		//USART_SendString(bluetooth,"msg: L2\n");
		if (LU.ispressed){
			flywheel_left_setYaw(flywheel_left.pur_yaw+2*convert[strategy.left[autorun.target_l]][0][0]*DELT_YAW_LEFT);
			flywheel_left_setBrushless(flywheel_left.pur_duty+2*convert[strategy.left[autorun.target_l]][0][1]*DELT_SPEED_LEFT);
		}
		if (LR.ispressed){
			flywheel_left_setYaw(flywheel_left.pur_yaw+2*convert[strategy.left[autorun.target_l]][1][0]*DELT_YAW_LEFT);
			flywheel_left_setBrushless(flywheel_left.pur_duty+2*convert[strategy.left[autorun.target_l]][1][1]*DELT_SPEED_LEFT);
		
		}
		if (LD.ispressed){
			flywheel_left_setYaw(flywheel_left.pur_yaw+2*convert[strategy.left[autorun.target_l]][2][0]*DELT_YAW_LEFT);
			flywheel_left_setBrushless(flywheel_left.pur_duty+2*convert[strategy.left[autorun.target_l]][2][1]*DELT_SPEED_LEFT);
		
		}
		if (LL.ispressed){
			flywheel_left_setYaw(flywheel_left.pur_yaw+2*convert[strategy.left[autorun.target_l]][3][0]*DELT_YAW_LEFT);
			flywheel_left_setBrushless(flywheel_left.pur_duty+2*convert[strategy.left[autorun.target_l]][3][1]*DELT_SPEED_LEFT);
		
		}
		if(RD.ispressed){ 
			RD.ispressed = false;
			flywheel_left_fly1();
		};
		
		if (RU.ispressed)
		{ 
			RU.ispressed = false;
			if(autorun.state == pos_arrived && handle_l==false)
			{
				autorun.launch_l_continute = true;
			}else if(autorun.state == pos_arrived && handle_l == true)
			{
				auto_select_l((autorun.target_l) % 3);
			}
		}
		if (RR.ispressed) {
			RR.ispressed = false;
			USART_SendString(bluetooth,"left:x pitch:%f yaw:%f speed:%f\n",flywheel_left.pur_pitch,flywheel_left.pur_yaw,flywheel_left.pur_duty);
			ptr = list_locate(&now_pos->d[strategy.left[autorun.target_l]].launch_ptr, 1);
			
			if (ptr == NULL)
			{
				USART_SendString(bluetooth, "msg: Error\n");
				return;
			}
			data = ptr->data;
			data->pitch = flywheel_left.pur_pitch;
			data->turn = 0;
			data->speed = flywheel_left.pur_duty;
			data->yaw = flywheel_left.pur_yaw;
			data->jmp = 0;
		}
		if (RL.ispressed) {
			RL.ispressed = false;
		}
		
	}else{ 
		//USART_SendString(bluetooth,"msg: 000!\n");
			
	}
	if (R2.ispressed && !L2.ispressed){
		if (LU.ispressed){
			flywheel_right_setYaw(flywheel_right.pur_yaw+2*convert[strategy.right[autorun.target_r]][0][0]*DELT_YAW_RIGHT);
			flywheel_right_setBrushless(flywheel_right.pur_duty+2*convert[strategy.right[autorun.target_r]][0][1]*DELT_SPEED_RIGHT);
		}
		if (LR.ispressed){
			flywheel_right_setYaw(flywheel_right.pur_yaw+2*convert[strategy.right[autorun.target_r]][1][0]*DELT_YAW_RIGHT);
			flywheel_right_setBrushless(flywheel_right.pur_duty+2*convert[strategy.right[autorun.target_r]][1][1]*DELT_SPEED_RIGHT);
		
		}
		if (LD.ispressed){
			flywheel_right_setYaw(flywheel_right.pur_yaw+2*convert[strategy.right[autorun.target_r]][2][0]*DELT_YAW_RIGHT);
			flywheel_right_setBrushless(flywheel_right.pur_duty+2*convert[strategy.right[autorun.target_r]][2][1]*DELT_SPEED_RIGHT);
		
		}
		if (LL.ispressed){
			flywheel_right_setYaw(flywheel_right.pur_yaw+2*convert[strategy.right[autorun.target_r]][3][0]*DELT_YAW_RIGHT);
			flywheel_right_setBrushless(flywheel_right.pur_duty+2*convert[strategy.right[autorun.target_r]][3][1]*DELT_SPEED_RIGHT);
		
		}
		if(RD.ispressed){ 
			RD.ispressed = false;
			flywheel_right_fly1();
		}
		if (RU.ispressed)
		{
			RU.ispressed = false;
			if(autorun.state == pos_arrived&&handle_r==false)
			{
				autorun.launch_r_continute = true;
			}else if(autorun.state == pos_arrived&&handle_r==true)
			{
				auto_select_r((autorun.target_r + 1) % 4);
			}
		}
		if (RR.ispressed) {
			RR.ispressed = false;
			USART_SendString(bluetooth,"right:x pitch:%f yaw:%f speed:%f\n",flywheel_right.pur_pitch,flywheel_right.pur_yaw,flywheel_right.pur_duty);
			
			ptr = list_locate(&now_pos->r[strategy.right[autorun.target_r]].launch_ptr, 1);
			
			if (ptr == NULL)
			{
				USART_SendString(bluetooth, "msg: Error\n");
			}else{
			data = ptr->data;
			data->pitch = flywheel_right.pur_pitch;
			data->turn = 0;
			data->speed = flywheel_right.pur_duty;
			data->yaw = flywheel_right.pur_yaw;
			data->jmp = 0;
			}
			
		}
		if (RL.ispressed) {
			RL.ispressed = false;
		}
	}else{
		
	}
	
	if (L1.ispressed) 
	{
		
	}		
	if (R1.ispressed) {
		
	}	
	
	
	/*if(OPEN_Hander == 1){
		chassis.END.X=chassis.pos_x;
		chassis.END.Y=chassis.pos_y;
		chassis.END.ANG=chassis.angle;
	}*/
}

void control_usart_TIM()
{
	int i;
	for (i=0;i<12;i++) 
	{
		if (b[i]->cnt>0) b[i]->cnt--;
		if (b[i]->cnt==0)
			 b[i]->ispressed=false;
	}
}

