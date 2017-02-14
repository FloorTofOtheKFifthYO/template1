#ifndef __CONFIGURATION_H
#define __CONFIGURATION_H


#include "stdio.h"	
#include "stm32f4xx.h"
#include "sys.h" 
#include "delay.h"
#include <stdbool.h>

#define USART_printf UART4
#define _printf 1

#ifndef bluetooth
#define bluetooth UART5
#endif

#define STR_BUFFER_LEN 128
#define MOTOR_USARTx UART4
#define PI 3.1415926

#define USART_REC_LEN  			200 
//extern u8  UART5_RX_BUF[USART_REC_LEN]; //���ջ���,���USART_REC_LEN���ֽ�.ĩ�ֽ�Ϊ���з� 
//extern u16 UART5_RX_STA;         		//����״̬���	
//extern u8  USART1_RX_BUF[USART_REC_LEN];    
//extern u16 USART1_RX_STA;

//��ˢ����
#ifndef BRUSHLESS

#define BRUSHLESS
#define BRUSHLESS_TIM 		TIM9			//��ʱ��
#define BRUSHLESS_OC 		1				//ͨ��
#define BRUSHLESS_GPIOx 	GPIOE			//GPIO
#define BRUSHLESS_PIN GPIO_	Pin_5			//�ܽ�
#define BRUSHLESS_PINSOURCE GPIO_PinSource5 //���ùܽ�
#define BRUSHLESS_AF 		GPIO_AF_TIM9	//�ܽŸ��ù���

#endif

#ifndef CHASSIS
#define CHASSIS

#define MOTOR0_ID 0x0A
#define MOTOR1_ID 0x0B
#define MOTOR2_ID 0x0C

#endif

extern float direction_angle;
extern u8 Hand_flag;
extern int WANTSPEED;
extern u8 Turn_R_Flag ,Turn_L_Flag ;
extern u8 TURN_Flag;
extern u8 OPEN_Hander;

void rcc_config(void);
void gpio_config(void);
void EXTI_config(void);
void nvic_config(void);
void NVIC_Configuration(uint8_t NVIC_IRQChannel,uint8_t NVIC_IRQChannelPreemptionPriority,
	uint8_t NVIC_IRQChannelSubPriority,FunctionalState NVIC_IRQChannelCmd);
void GPIO_Configuration(uint16_t GPIO_Pin,GPIOMode_TypeDef  GPIO_Mode,GPIOOType_TypeDef GPIO_OType,
	GPIOSpeed_TypeDef GPIO_Speed,GPIOPuPd_TypeDef GPIO_PuPd,GPIO_TypeDef* GPIOx);
void EXTI_Configuration(uint32_t EXTI_Line,
                EXTIMode_TypeDef EXTI_Mode,
                EXTITrigger_TypeDef EXTI_Trigger,
                FunctionalState EXTI_LineCmd);


#endif


