#ifndef __USART_H
#define __USART_H
#include "configuration.h"
#include "stdarg.h"

void usart_init(int *x,int *y);

extern u8  UART5_RX_BUF[USART_REC_LEN]; //接收缓冲,最大USART_REC_LEN个字节.末字节为换行符 
extern u16 UART5_RX_STA;         		//接收状态标记	
extern u8  USART3_RX_BUF[USART_REC_LEN];    
extern u16 USART3_RX_STA;

void USART_SendString(USART_TypeDef* USARTx, char *fmt, ...);

#define LU_KEY 0
#define LR_KEY 1
#define LD_KEY 2
#define LL_KEY 3
#define RU_KEY 4
#define RR_KEY 5
#define RD_KEY 6
#define RL_KEY 7
#define L1_KEY 8
#define L2_KEY 9
#define R1_KEY 10
#define R2_KEY 11

#endif


