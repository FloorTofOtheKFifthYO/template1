#include "stm32f4xx.h"

#define MOTOR_UART USART1

#define NUMBER    2
#define DATA_4    5
#define DATA_3    6
#define DATA_2    7
#define DATA_1    8
#define CHECK     9



void maxon_setSpeed_i(u8 number , int si);
void maxon_setSpeed_p(u8 number , int p);
void maxon_setSpeed(USART_TypeDef* USARTx, int speed);
void maxon_save();

