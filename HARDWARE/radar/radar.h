#ifndef RADAR_H_ 
#define RADAR_H_

#include "sys.h"
#include "can.h"
#include <stdbool.h>

#define RADAR_TX_ID 0x30
#define RADAR_RX_ID 0x31


void radar_init(float *_theta,float *_distance);

void radar_start();

void radar_stop();

void radar_shoot();

#endif
