#include "radar.h"
#include "usart.h"
#include "vega.h"
#include <string.h>

bool data_recv = true;
extern bool debug_print;
static float *theta;
static float *distance;
char recv[8];
void can_test_func(CanRxMsg* can_rx_msg)
{
	data_convert converter;
	if (data_recv){
		converter.u8_form[0] = can_rx_msg->Data[0];
		converter.u8_form[1] = can_rx_msg->Data[1];
		converter.u8_form[2] = can_rx_msg->Data[2];
		converter.u8_form[3] = can_rx_msg->Data[3];
		memcpy((void*)theta,&converter.float_form,4);
		converter.u8_form[0] = can_rx_msg->Data[4];
		converter.u8_form[1] = can_rx_msg->Data[5];
		converter.u8_form[2] = can_rx_msg->Data[6];
		converter.u8_form[3] = can_rx_msg->Data[7];
		memcpy((void*)distance,&converter.float_form,4);
		if(debug_print){
			USART_SendString(bluetooth,"theta:%f distance:%f",(*theta)/180.f*3.1415926,*distance);
			USART_SendString(bluetooth,"\n");
		}
	}
	else{
		memcpy((void*)recv,can_rx_msg->Data,can_rx_msg->DLC);
		USART_SendString(bluetooth,recv);
		USART_SendString(bluetooth,"\n");
	}
}

void radar_init(float *_theta,float *_distance)
{
	theta = _theta;
	distance = _distance;
	can_add_callback(RADAR_RX_ID,can_test_func);
}

void radar_start()
{
	u8 can_tx_msg[5]={'s','t','a','r','t'};
	can_send_msg(RADAR_TX_ID,can_tx_msg,5);
}

void radar_shoot()
{
	u8 can_tx_msg[5]={'s','h','o','o','t'};
	can_send_msg(RADAR_TX_ID,can_tx_msg,5);
}

void radar_stop()
{
	u8 can_tx_msg[4]={'s','t','o','p'};
	can_send_msg(RADAR_TX_ID,can_tx_msg,4);
}
