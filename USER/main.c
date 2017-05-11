#include "main.h" 
#include "sys.h"
#include "chassis.h"
#include "flywheel_left.h"
#include "flywheel_right.h"
#include "auto.h"

bool g_stop_flag = false;

bool switch_side = false;

int ms = 0;

void TIM2_IRQHandler(void){
	static int tmpcount = 1000;
	if( TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET ) 
	{
		ms++;
		tmpcount--;
		if(tmpcount==0){
			tmpcount = 1000;
			USART_SendString(bluetooth,"msg: left pitch:%d yaw:%d\n",ReturnData(PITCH_ID_LEFT)->Position,ReturnData(YAW_ID_LEFT)->Position);
			USART_SendString(bluetooth,"msg: right pitch:%d yaw:%d\n",ReturnData(PITCH_ID_RIGHT)->Position,ReturnData(YAW_ID_RIGHT)->Position);
		}
		control_usart_TIM();
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
	usart_init(bluetooth,115200,true);
	cmd_init();
	EXTI_config();
	
reboot:	
	controller_usart_init(&Hx, &Hy);
	
	can_init();
	
	chassis_init();
	
	TIM2_Init();
	
	flywheel_left_init();
	flywheel_right_init();
	
	auto_init();
//	autorun.state = pos_arrived;
	
	USART_SendString(UART5,"msg: Let's go!\n");
	
	OPEN_Hander = 0;
	
    while(1) 
	{
		bottons_check();
		//sticks_check(Hx,Hy);
		/********手柄部分***********/
		//USART_SendString(CMD_USARTx,"Ohi\n");
		control_usart_main();
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
			
			chassis_update();
			flywheel_left_main();
			flywheel_right_main();
			auto_main();
			//if(OPEN_Hander ==0){
				/**-------------------------自动部分--------------------------------**/
				chassis_auto();
			/*}
			else if(OPEN_Hander ==1)
			{*/
				/********手柄部分***********/
				//USART_SendString(CMD_USARTx,"Ohi\n");
				//control_usart_main();
			//}
		}
		//delay_ms(2);
	}
}
