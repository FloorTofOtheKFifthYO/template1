#include "stm32f4xx.h"

#define MOTOR_UART USART1

#define NUMBER    2
#define DATA_4    5
#define DATA_3    6
#define DATA_2    7
#define DATA_1    8
#define CHECK     9



void setspeed_i(u8 number , int si);
void setspeed_p(u8 number , int p);
void setspeed(USART_TypeDef* USARTx, int speed);
void save();

