#include "maxon.h"
#include "usart.h"

//速度发送协议，第三个元素为节点，最后一个为校验位
u8 sendToSetSpeed[10] = {0x55,0xAA,0x01,0x00,0x04,0x00,0x00,0x00,0x00,0x00};
u8 sendToSetSpeedPI[10] = {0x55,0xAA,0x01,0x00,0x84,0x00,0x00,0x00,0x00,0x00};

void maxon_setSpeed(USART_TypeDef* USARTx, int speed){
	int i,j;
	u8 temp = 0;
	int check = 0;
	
	sendToSetSpeed[NUMBER] = 0x01;
	sendToSetSpeed[DATA_4] = (u8)(speed>>24);
	sendToSetSpeed[DATA_3] = (u8)(speed>>16);
	sendToSetSpeed[DATA_2] = (u8)(speed>>8);
	sendToSetSpeed[DATA_1] = (u8)(speed>>0);
	
	for(i = 0; i<9 ; i++){
		check += sendToSetSpeed[i];
	}
	sendToSetSpeed[CHECK] = (u8)check; 
	
	
	for(j = 0 ; j<10 ; j++){
		while (USART_GetFlagStatus(USARTx, USART_FLAG_TXE) == RESET);
		USART_SendData(USARTx, (u8) sendToSetSpeed[j]);
		//USART_SendData(UART5, (u8) sendToSetSpeed[j]);
	}
}

//p:7000 i:4000

void maxon_setSpeed_p(USART_TypeDef* USARTx , int p){
	int i,j;
	u8 temp = 0;
	int check = 0;
	sendToSetSpeedPI[5] = 0x02;//命令偏移量
	sendToSetSpeedPI[6] = 0x02;//数据长度
	sendToSetSpeedPI[7] = (u8)(p>>8);
	sendToSetSpeedPI[8] = (u8)(p>>0);
	
	for(i = 0; i<9 ; i++){
		check += sendToSetSpeedPI[i];
	}
	sendToSetSpeedPI[CHECK] = (u8)check; 
	

	for(j = 0 ; j<10 ; j++){
		while (USART_GetFlagStatus(USARTx, USART_FLAG_TXE) == RESET);
		USART_SendData(USARTx, (u8) sendToSetSpeedPI[j]);
		//USART_SendData(UART5, (u8) sendToSetSpeedPI[j]);
	}
}

void maxon_setSpeed_i(USART_TypeDef* USARTx , int si){
	u8 temp = 0;
	int i;
	int j;
	int check = 0;

	sendToSetSpeedPI[5] = 0x03;//命令偏移量
	sendToSetSpeedPI[6] = 0x02;//数据长度
	sendToSetSpeedPI[7] = (u8)(si>>8);
	sendToSetSpeedPI[8] = (u8)(si>>0);
	
	for(i = 0; i<9 ; i++){
		check += sendToSetSpeedPI[i];
	}
	sendToSetSpeedPI[CHECK] = (u8)check; 
	
	
	for(j = 0 ; j<10 ; j++){
		while (USART_GetFlagStatus(USARTx, USART_FLAG_TXE) == RESET);
		USART_SendData(USARTx, (u8) sendToSetSpeedPI[j]);
		//USART_SendData(UART5, (u8) sendToSetSpeedPI[j]);
	}
}


void maxon_save(USART_TypeDef* USARTx){
	int j;
	u8 saveinfo[8] = {0x55 , 0xAA , 0x01 , 0x00 , 0x03 , 0x01 ,0x00 , 0x04};
	
	for(j = 0 ; j<8 ; j++){
		while (USART_GetFlagStatus(USARTx, USART_FLAG_TXE) == RESET);
		USART_SendData(USARTx, (u8) saveinfo[j]);
	}
}
