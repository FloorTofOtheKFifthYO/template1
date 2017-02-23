#include "main.h" 
#include "sys.h"
#include "chassis.h"
#include "flywheel.h"


SwitchTIM encoder;
bool g_stop_flag = false;

int ms = 0;


void TIM2_IRQHandler(void){
	if( TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET ) 
	{
		ms++;
		control_usart_TIM();
		TIM_ClearITPendingBit(TIM2, TIM_FLAG_Update);//��������жϱ�־λ����һֱ�ж�
	}	
}


//-----------------�ݸ����------------------------

int main(void)
{   
	int Hx, Hy;
	
	rcc_config();
	gpio_config();
	delay_init(168);  //��ʼ����ʱ����
//	EXTI_config();
	nvic_config();
	
	usart_init(bluetooth,115200);
	usart_init(MOTOR_USARTx, 115200);
	controller_usart_init(&Hx, &Hy);
	cmd_init();
	param_init();

	can_init();
	chassis_init();
	encoder_init(&encoder);
	flywheel_init();
	SetTime(5);
	TIM2_Init();
	USART_SendString(UART5,"msg: Let's go!\n");
	
	OPEN_Hander = 0;
	
    while(1) 
	{
		if(g_stop_flag){//ֹͣһ���˶�
			
		}else{
			//if((encoder.GetTim5-temp_speed)!=0)
			//	USART_SendString(UART5,"msg: %d\n",encoder.GetTim5);
			
			bottons_check();
			sticks_check(Hx,Hy);
			chassis_update();
			if(OPEN_Hander ==0){
				/**-------------------------�Զ�����--------------------------------**/
				flywheel_main();
				chassis_auto();
			}
			else if(OPEN_Hander ==1)
			{
				/********�ֱ�����***********/
				//USART_SendString(CMD_USARTx,"Ohi\n");
				control_usart_main();
			}
		}
	}
}
