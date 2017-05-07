#ifndef RADAR_H_ 
#define RADAR_H_

#include "sys.h"
#include "can.h"
#include <stdbool.h>

#define RADAR_TX_ID 0x30
#define RADAR_RX_ID 0x31

typedef union{
    u8 u8_form[4];
    int s32_form;
    float float_form;
}data_convert;

void radar_init(float *_theta,float *_distance);

void radar_start();


#endif
