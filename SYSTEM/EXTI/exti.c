#include "usart.h"
#include "configuration.h"
#include "exti.h"
#include "flywheel_left.h"


int wait_cnt=-1;

void EXTI15_10_IRQHandler(void)
{
	if(EXTI_GetITStatus(EXTI_Line11)!=RESET){//判断某个线上的中断是否发生
		delay_ms(2);
		if (GPIO_ReadInputDataBit(GPIOF,GPIO_Pin_11)==0){ 
			if(home_flag){
				flywheel_left_yawZero();
				home_flag = false;
				USART_SendString(bluetooth,"msg: home!\n");
			}
			
		}
		EXTI_ClearITPendingBit(EXTI_Line11); //清除 LINE 上的中断标志位
	}
}

void EXTI9_5_IRQHandler(void)
{
	if(EXTI_GetITStatus(EXTI_Line8)!=RESET){//判断某个线上的中断是否发生
		delay_ms(2);
		if (GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_8)==0)
		{
			if(home_flag){
				flywheel_left_pitchZero();
				home_flag = false;
			}
		}
		EXTI_ClearITPendingBit(EXTI_Line8); //清除 LINE 上的中断标志位
	}
}
