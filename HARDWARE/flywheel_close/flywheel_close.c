#include "flywheel_close.h"
#include "delay.h"

u8 cmdForUnbrush[6] = {0x00};

void unbrush_init(u8 can_id){
	cmdForUnbrush[0] = 0x00;
	can_send_msg(can_id,cmdForUnbrush,6);
	delay_ms(800);
	cmdForUnbrush[0] = 0x02;
	cmdForUnbrush[1] = 0x00;
	cmdForUnbrush[2] = 0x00;
	cmdForUnbrush[3] = 0x00;
	cmdForUnbrush[4] = 0x00;
	cmdForUnbrush[5] = 0x00;
	
	can_send_msg(can_id,cmdForUnbrush,6);
}

void setUnbrushSpeed(u8 can_id,int speed){
	cmdForUnbrush[0] = 0x02;
	cmdForUnbrush[1] = 0x00;
	cmdForUnbrush[2] = 0x00;
	cmdForUnbrush[3] = 0x00;
	cmdForUnbrush[4] = (u8)(speed>>8);
	cmdForUnbrush[5] = (u8)speed;
	
	can_send_msg(can_id,cmdForUnbrush,6);
}

