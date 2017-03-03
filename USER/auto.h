#ifndef _AUTO_H_
#define _AUTO_H_

#include "sys.h"
#include "chassis.h"

typedef struct {
	POSITION start_area;
	POSITION load_area;
	enum {start,load_running,load_arrived,pos_running,pos_arrived,handle_control,start_running} state;
	
	//¿ØÖÆ×Ö
	bool load_run_flag;
	bool pos_run_flag;
	bool start_run_flag;

} AutoStrategy;

#endif
