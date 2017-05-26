#include "test.h"
#include "can.h"
#include "main.h"
#include "auto.h"

void test_rcv_callback(CanRxMsg *can_rx_msg){
	u8 temp[8];
	data_convert data;
	int i;
    if(can_rx_msg->StdId == TEST_RX_ID){ 
        if(can_rx_msg->DLC == 2){
            switch(can_rx_msg->Data[0])	
			{
				case 's':
					 if(can_rx_msg->Data[1]){
						 switch_side = true;
						 LEFT_RIGHT = can_rx_msg->Data[1];
						 temp[0] = 's';
						 temp[1] = 1;
						 can_send_msg(TEST_TX_ID,temp,2);
					 }
					 else{
						 switch_side = true;
						 LEFT_RIGHT = can_rx_msg->Data[1];
						 temp[0] = 's';
						 temp[1] = 0;
						 can_send_msg(TEST_TX_ID,temp,2);
					 }
					break;
				case 'd':
					if(can_rx_msg->Data[1])
					{
						debug = true;
						temp[0] = 'd';
						temp[1] = 1;
						can_send_msg(TEST_TX_ID,temp,2);
					}
					else
					{
						debug = false;
						temp[0] = 'd';
						temp[1] = 0;
						can_send_msg(TEST_TX_ID,temp,2);
					}
					break;
			}
        }else if(can_rx_msg->DLC == 1)
		{
			switch(can_rx_msg->Data[0])
			{
				case 'a':
					temp[0] = 'x';
					data.float_form = autorun.load_area.X;
					for(i = 0;i < 4;i++)
					{
						temp[i+1] = data.u8_form[i];
					}
					can_send_msg(TEST_TX_ID,temp,5);
					temp[0] = 'y';
					data.float_form = autorun.load_area.Y;
					for(i = 0;i < 4;i++)
					{
						temp[i+1] = data.u8_form[i];
					}
					can_send_msg(TEST_TX_ID,temp,5);
					temp[0] = 'a';
					data.float_form = autorun.load_area.ANG;
					for(i = 0;i < 4;i++)
					{
						temp[i+1] = data.u8_form[i];
					}
					can_send_msg(TEST_TX_ID,temp,5);
					temp[0] = 'o';
					data.float_form = ((Pos_data *)(autorun.now_pos_ptr->data))->x;
					for(i = 0;i < 4;i++)
					{
						temp[i+1] = data.u8_form[i];
					}
					can_send_msg(TEST_TX_ID,temp,5);
					temp[0] = 'p';
					data.float_form = ((Pos_data *)(autorun.now_pos_ptr->data))->y;
					for(i = 0;i < 4;i++)
					{
						temp[i+1] = data.u8_form[i];
					}
					can_send_msg(TEST_TX_ID,temp,5);
					temp[0] = 'q';
					data.float_form = ((Pos_data *)(autorun.now_pos_ptr->data))->ang;
					for(i = 0;i < 4;i++)
					{
						temp[i+1] = data.u8_form[i];
					}
					can_send_msg(TEST_TX_ID,temp,5);
					temp[0] = 't';
					data.s32_form = autorun.state;
					for(i = 0;i < 4;i++)
					{
						temp[i+1] = data.u8_form[i];
					}
					can_send_msg(TEST_TX_ID,temp,5);
					break;
			}
		}
    } 
		
}

void test_init()
{
	can_add_callback(TEST_RX_ID,test_rcv_callback);
}

void test_left_target(u8 number)
{
	u8 temp[8];
	temp[0] = 'l';
	temp[1] = number;
	can_send_msg(TEST_TX_ID,temp,2);
}

void test_right_target(u8 number)
{
	u8 temp[8];
	temp[0] = 'r';
	temp[1] = number;
	can_send_msg(TEST_TX_ID,temp,2);
}
