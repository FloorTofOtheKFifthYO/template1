#include "usart.h"
#include "configuration.h"
#include "exti.h"
#include "flywheel_left.h"
#include "flywheel_right.h"

extern bool debug_print;

int wait_cnt=-1;

void EXTI15_10_IRQHandler(void)
{
	/*if(EXTI_GetITStatus(EXTI_Line13)!=RESET){//�ж�ĳ�����ϵ��ж��Ƿ���
		delay_ms(2);
		if (GPIO_ReadInputDataBit(GPIOD,GPIO_Pin_13)==0){ 
			if(home_flag){
				flywheel_left_yawZero();
				home_flag = false;
				USART_SendString(bluetooth,"msg: home!\n");
			}
			
		}
		EXTI_ClearITPendingBit(EXTI_Line13); //��� LINE �ϵ��жϱ�־λ
	}else */
	if(EXTI_GetITStatus(EXTI_Line12)!=RESET){//�ж�ĳ�����ϵ��ж��Ƿ���
		delay_ms(2);
		if (GPIO_ReadInputDataBit(GPIOD,GPIO_Pin_12)==0){ 
			if(home_flag){
				flywheel_left_pitchZero();
				home_flag = false;
				if(debug_print)
					USART_SendString(bluetooth,"msg: home!\n");
			}
			
		}
		EXTI_ClearITPendingBit(EXTI_Line12); //��� LINE �ϵ��жϱ�־λ
	}
}

/*
void EXTI0_IRQHandler(void)
{
	if(EXTI_GetITStatus(EXTI_Line0)!=RESET){//�ж�ĳ�����ϵ��ж��Ƿ���
		delay_ms(2);
		if (GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_0)==0)
		{
			if(home_flag){
				flywheel_right_yawZero();
				USART_SendString(bluetooth,"msg: home!\n");
				home_flag = false;
			}
		}
		EXTI_ClearITPendingBit(EXTI_Line0); //��� LINE �ϵ��жϱ�־λ
	}
}*/

void EXTI1_IRQHandler(void)
{
	if(EXTI_GetITStatus(EXTI_Line1)!=RESET){//�ж�ĳ�����ϵ��ж��Ƿ���
		delay_ms(2);
		if (GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_1)==0)
		{
			if(home_flag){
				flywheel_right_pitchZero();
				if(debug_print)
					USART_SendString(bluetooth,"msg: home!\n");
				home_flag = false;
			}
		}
		EXTI_ClearITPendingBit(EXTI_Line1); //��� LINE �ϵ��жϱ�־λ
	}
}
