#include "controller_usart.h"
#include "configuration.h"
#include "usart.h"
#include "flywheel_left.h"
#include "chassis.h"
#include <math.h>
#include "auto.h"

extern int wait_cnt;

#define BT_UP 0
#define BT_RIGHT 1
#define BT_DOWN 2
#define BT_LEFT 3

#define DELT_YAW_LEFT -0.005
#define DELT_SPEED_LEFT 0.001
#define FACTOR_SPEED_LEFT 1

//иосробвС
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

/*void USART1_IRQHandler(void)                	
{
	u8 Res;	u8 tmp;
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET) 
	{
		OPEN_Hander = 1;
		Res =USART_ReceiveData(USART1);
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
} */

void controller_usart_init(int *Hx,int *Hy)
{
	usart_init(USART1,115200);
	x=Hx;y=Hy;
	ptrB=-1;
	ptrS=0;
}

void bottons_check(){
	if (ptrB>-1){
		if (ptrB>=4&ptrB<=7)
			b[ptrB]->cnt=2;
		else b[ptrB]->cnt=200;
		b[ptrB]->ispressed=true;
		ptrB=-1;
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
	
	Xianding = 0;
	if (RU.ispressed)
	{ 
		
	}
	if (RR.ispressed) {
		
	}
	if (RD.ispressed) {
	}
	if (RL.ispressed) {
		
	}
	ChassisSpeed = 1000;
	if (!L2.ispressed&&!R2.ispressed){
	if (LU.ispressed) direction_angle = 3*PI/4;
	else if (LD.ispressed) direction_angle = -PI/4;
	else if (LL.ispressed) direction_angle = PI/4;
	else if (LR.ispressed) direction_angle = -3*PI/4;
	else ChassisSpeed = 0;}else
	ChassisSpeed = 0;
	chassis_handle(direction_angle, ChassisSpeed);
	if (L2.ispressed) {
		USART_SendString(bluetooth,"msg: L2\n");
		if (LU.ispressed){
			flywheel_left_setYaw(flywheel_left.pur_yaw+2*convert[autorun.target_l][0][0]*DELT_YAW_LEFT);
			flywheel_left_setBrushless(flywheel_left.pur_duty+2*convert[autorun.target_l][0][1]*DELT_SPEED_LEFT);
		}else if (LR.ispressed){
			flywheel_left_setYaw(flywheel_left.pur_yaw+2*convert[autorun.target_l][1][0]*DELT_YAW_LEFT);
			flywheel_left_setBrushless(flywheel_left.pur_duty+2*convert[autorun.target_l][1][1]*DELT_SPEED_LEFT);
		
		}else if (LD.ispressed){
			flywheel_left_setYaw(flywheel_left.pur_yaw+2*convert[autorun.target_l][2][0]*DELT_YAW_LEFT);
			flywheel_left_setBrushless(flywheel_left.pur_duty+2*convert[autorun.target_l][2][1]*DELT_SPEED_LEFT);
		
		}else if (LL.ispressed){
			flywheel_left_setYaw(flywheel_left.pur_yaw+2*convert[autorun.target_l][3][0]*DELT_YAW_LEFT);
			flywheel_left_setBrushless(flywheel_left.pur_duty+2*convert[autorun.target_l][3][1]*DELT_SPEED_LEFT);
		
		}else{ 
		
			
		};
	}else{ 
		//USART_SendString(bluetooth,"msg: 000!\n");
			
	}
	if (R2.ispressed){
		Xianding=0;
		if (LU.ispressed) Xianding +=200;					
		if (LD.ispressed) Xianding -=200;
		if (Xianding !=0){
			//USART_SendString(bluetooth,"msg: %d\n",Xianding);
		}else{
			
		}
		Xianding=0;
		if (LL.ispressed) Xianding +=200;					
		if (LR.ispressed) Xianding -=200;
		if (Xianding !=0){
			//USART_SendString(bluetooth,"msg: %d\n",Xianding);
			
		}else{
			
		}
	}else{
		
	}
	Xianding=0;
	if (L1.ispressed) Xianding +=200;					
	if (R1.ispressed) Xianding -=200;
	if (Xianding !=0){
		//USART_SendString(bluetooth,"msg: %d\n",Xianding);
		
	}else{
		
	}
	
	if(OPEN_Hander == 1){
		chassis.END.X=chassis.pos_x;
		chassis.END.Y=chassis.pos_y;
		chassis.END.ANG=chassis.angle;
	}
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

