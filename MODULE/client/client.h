#ifndef _CLIENT_H_
#define _CLIENT_H_

#include "sys.h"


void init_subsector(u8 ID , u8 EN_unbrush,u8 channel ,u16 frep, u8 feedback_tim);


void setUnbrushSpeed_1(u8 ID,u8 channel , int speed);


//¼ÌµçÆ÷¿ØÖÆ
void set_IO(u8 ID, u8 io[]);

#endif
