#ifndef __GLOBAL_H
#define __GLOBAL_H
#include "sys.h"
#include <stdbool.h>

typedef struct 
{
	s8 x,y;
}sticks;


typedef struct 
{
	bool ispressed;
	u8 cnt;
//  void * func;
}bottons;

void pitch_move(float v);
void roll_move(float v);


void bottons_check();
void sticks_check(int Hx,int Hy);

#endif

