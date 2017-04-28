#include "main.h" 
#include "sys.h"
#include "chassis.h"
#include "flywheel_left.h"
#include "flywheel_right.h"
#include "auto.h"
#include "step.h"

bool g_stop_flag = false;

bool switch_side = false;

int ms = 0;

void TIM2_IRQHandler(void){
	if( TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET ) 
	{
		ms++;
		//control_usart_TIM();
		flywheel_left_TIM();
		flywheel_right_TIM();
		TIM_ClearITPendingBit(TIM2, TIM_FLAG_Update);//必须清除中断标志位否则一直中断
	}	
}


//-----------------草稿结束------------------------

int main(void)
{   
	int Hx, Hy;
	
	rcc_config();
	gpio_config();
	delay_init(168);  //初始化延时函数
	nvic_config();
	
reboot:	
	
	usart_init(bluetooth,115200,true);
	//controller_usart_init(&Hx, &Hy);
	
	cmd_init();
	can_init();
	
	chassis_init();
	
	TIM2_Init();
	
	flywheel_left_init();
	flywheel_right_init();
	
	EXTI_config();
	
	
	auto_init();
//	autorun.state = load_arrived;
	
	USART_SendString(UART5,"msg: Let's go!\n");
	
	OPEN_Hander = 0;
	
    while(1) 
	{
		if(switch_side)
		{
			switch_side = false;
			goto reboot;
		}
		if(g_stop_flag){//停止一切运动
			chassis_stop();
			flywheel_left_stop();
			flywheel_right_stop();
		}else{
			bottons_check();
			//sticks_check(Hx,Hy);
			chassis_update();
			flywheel_left_main();
			flywheel_right_main();
			if(OPEN_Hander ==0){
				/**-------------------------自动部分--------------------------------**/
				auto_main();
				chassis_auto();
			}
			else if(OPEN_Hander ==1)
			{
				/********手柄部分***********/
				//USART_SendString(CMD_USARTx,"Ohi\n");
				control_usart_main();
			}
		}
		delay_ms(6);
	}
}
