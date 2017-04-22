#ifndef _FLYWHEEL_CLOSE_H_
#define _FLYWHEEL_CLOSE_H_

#include "sys.h"
#include "can.h"

void unbrush_init(u8 can_id);

void setUnbrushSpeed(u8 can_id,int speed);
#endif
