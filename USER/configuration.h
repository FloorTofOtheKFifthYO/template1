#ifndef __CONFIGURATION_H
#define __CONFIGURATION_H


#include "stdio.h"	
#include "stm32f4xx.h"
#include "stm32f4xx_conf.h"
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

typedef struct 
{
	float X;
	float Y;
	float ANG;
}POSITION;

extern POSITION START,END;

extern float direction_angle;
extern u8 Hand_flag;
extern int WANTSPEED;
extern u8 Turn_R_Flag ,Turn_L_Flag ;
extern u8 TURN_Flag;
extern u8 OPEN_Hander;

void uart_init(u32 bound);
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


