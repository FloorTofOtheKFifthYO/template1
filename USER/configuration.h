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
//extern u8  UART5_RX_BUF[USART_REC_LEN]; //接收缓冲,最大USART_REC_LEN个字节.末字节为换行符 
//extern u16 UART5_RX_STA;         		//接收状态标记	
//extern u8  USART1_RX_BUF[USART_REC_LEN];    
//extern u16 USART1_RX_STA;

#define FLYWHEEL_RIGHT
#define CLIENT_ID_RIGHT 	0x30
#define TURN_ID_RIGHT		0x08
#define PITCH_ID_RIGHT		0x04
#define YAW_ID_RIGHT		0x06
#define FLY_RIGHT		2

#define FLYWHEEL_LEFT
#define CLIENT_ID_LEFT 	0x31
#define PITCH_ID_LEFT	0x09
#define YAW_ID_LEFT		0x05
#define FLY_LEFT		2


//底盘
#define CHASSIS
#define MOTOR0_ID 0x0A
#define MOTOR1_ID 0x0B
#define MOTOR2_ID 0x0C
#define MOTOR3_ID 0x0D



extern u8 OPEN_Hander;
extern u8 LEFT_RIGHT;

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


