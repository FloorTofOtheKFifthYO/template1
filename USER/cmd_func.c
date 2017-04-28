#include "cmd_func.h"
#include "cmd.h"
#include <stdlib.h>
#include "chassis.h"
#include "flywheel_left.h"
#include "flywheel_right.h"
#include "global.h"
#include "configuration.h"
#include "string.h"
#include "auto.h"
#include "delay.h"
#include "math.h"
#include "param.h"

extern s8 ptrS,ptrB;
extern Param * param;
extern bool g_stop_flag;
extern bool switch_side;

u8 target=0;       				//目标0-6

static list_node * now_pos_ptr;
static int pos_no = 0;
static Pos_data * now_pos;     //当前点的数据指针

extern float pur_pitch;
extern float pur_roll;
extern int pur_step;
extern float pur_pull;
extern bool pitch_flag,roll_flag;

void cmd_reboot_func(int argc,char *argv[]){
    NVIC_SystemReset();
}

void cmd_stop_func(int argc,char *argv[]){
    if(argc == 1){
        g_stop_flag = true;
    }else if(argc == 2){
        g_stop_flag = atoi(argv[1]);
    }
}

void cmd_auto_func(int argc, char*argv[])
{
	if (strcmp(argv[1],"load") == 0)
	{
		autorun.load_run_flag = true;
	}else if (strcmp(argv[1],"select") == 0)
	{
		if(strcmp(argv[2],"l") == 0)
		{
			auto_select_l(atoi(argv[3]));
		}else if(strcmp(argv[2],"r") == 0)
		{
			auto_select_r(atoi(argv[3]));
		}
	}else if (strcmp(argv[1],"pos") == 0)
	{
		autorun.pos_run_flag = true;
	}else if (strcmp(argv[1],"start") == 0)
	{
		autorun.start_run_flag = true;
	}else if (strcmp(argv[1],"loadarea") == 0)
	{
		autorun.load_area.X = atof(argv[2]);
		autorun.load_area.Y = atof(argv[3]);
		autorun.load_area.ANG = atof(argv[4]);
	}else if (strcmp(argv[1],"save") == 0)
	{
		auto_save();
	}else if (strcmp(argv[1],"print") == 0)
	{
//		USART_SendString(bluetooth,"startx:%f ",autorun.start_area.X);
//		USART_SendString(bluetooth,"starty:%f ",autorun.start_area.Y);
//		USART_SendString(bluetooth,"startang:%f ",autorun.start_area.ANG);
		USART_SendString(bluetooth,"loadx:%f ",autorun.load_area.X);
		USART_SendString(bluetooth,"loady:%f ",autorun.load_area.Y);
		USART_SendString(bluetooth,"loadang:%f ",autorun.load_area.ANG);
		if(autorun.now_pos_ptr == NULL || autorun.now_pos_ptr->data == NULL)
		{
			USART_SendString(bluetooth,"msg:没有接下来的点了 ");
			USART_SendString(bluetooth,"nowx:%f ",0);
			USART_SendString(bluetooth,"nowy:%f ",0);
			USART_SendString(bluetooth,"nowang:%f ",0);
		}else{
			USART_SendString(bluetooth,"nowx:%f ",((Pos_data *)(autorun.now_pos_ptr->data))->x);
			USART_SendString(bluetooth,"nowy:%f ",((Pos_data *)(autorun.now_pos_ptr->data))->y);
			USART_SendString(bluetooth,"nowang:%f ",((Pos_data *)(autorun.now_pos_ptr->data))->ang);
		}
		USART_SendString(bluetooth,"state:%d\n ",autorun.state);
	}
}

void cmd_hello_func(int argc,char *argv[]){
    USART_SendString(CMD_USARTx, "msg: Hello World\n");
}

void cmd_test_func(int argc,char *argv[]){
	if(strcmp(argv[1],"home")==0)
	{
		home_flag = true;
	}
}

void cmd_pos_func(int argc,char *argv[])
{
    int no;
    int no0;
    int i;
    float x,y,ang;
    Pos_data *data;
    list_node * ptr;
    int erro_no;
    if (strcmp(argv[1], "now") == 0)
    {
        USART_SendString(CMD_USARTx, "x:%f y:%f yaw:%f speed:%d\n", chassis.pos_x,chassis.pos_y,chassis.angle,0);
    }else if(strcmp(argv[1],"select") == 0){
		if(argc < 2){
            USART_SendString(CMD_USARTx,"msg: Error!please enter:\n");
            USART_SendString(CMD_USARTx,"msg:    pos select <no> \n");
			return;
        }else if(argc == 2)
		{
			USART_SendString(bluetooth,"pos: %d", pos_no);
		}
		no = atoi(argv[2]);
		now_pos_ptr = list_locate(&param->pos_ptr, no);
		if(now_pos_ptr == NULL)
		{
            USART_SendString(CMD_USARTx,"msg: Error NO.\n");
			return;
		}else
		{
			pos_no = no;
			now_pos = now_pos_ptr->data;
		}
	}
	else if(strcmp(argv[1],"add") == 0){
        if(argc < 6){
            USART_SendString(CMD_USARTx,"msg: Error!please enter:\n");
            USART_SendString(CMD_USARTx,"msg:    pos add <no> <x> <y> <ang>\n");
			return;
        }
        no = atoi(argv[2]);
        x = atof(argv[3]);
        y = atof(argv[4]);
		ang = atof(argv[5]);
        data = (Pos_data *)malloc(sizeof(Pos_data));
        data->x = x;
        data->y =  y;
		data->ang = ang;
        for (i = 0; i < 7; ++i)
        {
            data->d[i].launch_num = 0;
            list_init(&data->d[i].launch_ptr);
			data->r[i].launch_num = 0;
            list_init(&data->r[i].launch_ptr);
        }
        if((erro_no = list_insert(&param-> pos_ptr, no, data)) <= 0){
            USART_SendString(CMD_USARTx,"msg: Error:%d\n",erro_no);
        }else{
			param->pos_num++;
			now_pos_ptr = list_locate(&param->pos_ptr,no);
			now_pos = now_pos_ptr->data;
			pos_no = no;
		}
		print_pos_list(param->pos_ptr->link);
    }else if(strcmp(argv[1],"print") == 0){
        print_pos_list(param->pos_ptr->link);
    }else if(strcmp(argv[1],"del") == 0){
		if(argc < 3){
            USART_SendString(CMD_USARTx,"msg: Error!please enter:\n");
            USART_SendString(CMD_USARTx,"msg:    pos del <no>\n");
			return;
        }
        no = atoi(argv[2]);
        ptr = list_locate(&param->pos_ptr, no);
        if (ptr != NULL)
        {
            data = ptr->data;
            for (i = 0; i < 7; ++i)
            {
                clear_launch(&data->d[i].launch_ptr); 
				clear_launch(&data->r[i].launch_ptr); 
            }
            free(data);
            list_remove_num(&param->pos_ptr,no);
            param->pos_num-=1;
			USART_SendString(CMD_USARTx,"msg: del success\n");
        }
        print_pos_list(param->pos_ptr->link);
    }else if(strcmp(argv[1],"clear") == 0){
        clear_pos(&param->pos_ptr);
        param->pos_num = 0;
    }else if(strcmp(argv[1],"save") == 0){
        erro_no = param_save();
        if(erro_no < 0){
            USART_SendString(CMD_USARTx,"msg: Save error:%d\n",erro_no);
        }else
			USART_SendString(CMD_USARTx,"msg: Save success:%d\n",erro_no);
        
    }else if(strcmp(argv[1],"modi") == 0){
        no = atoi(argv[2]);
        if((data = local_pos(no)) == NULL){
            USART_SendString(CMD_USARTx,"msg: Not found point No:%d\n",no);
            return;
        }
        if(argc < 6){
            USART_SendString(CMD_USARTx,"msg: Error cmd format\n");
            return;
        }

        data->x =  atof(argv[3]);
        data->y =  atof(argv[4]);
		data->ang = atof(argv[5]);
        print_pos_list(param->pos_ptr->link);
    }else if (strcmp(argv[1], "jmp")==0)
    {
		if(argc < 3){
            USART_SendString(CMD_USARTx,"msg: Error!please enter:\n");
            USART_SendString(CMD_USARTx,"msg:    pos jmp <no> <no0>\n");
			return;
        }
        no = atoi(argv[2]);
        no0 = atoi(argv[3]);
        ptr = list_locate(&param->pos_ptr, no);
        if (ptr != NULL)
        {
            if((node_move(&param->pos_ptr, no0, ptr)) == 0)
				USART_SendString(CMD_USARTx, "msg: Error\n");
			else
				USART_SendString(CMD_USARTx, "msg: jmp success\n");
        }else{
            USART_SendString(CMD_USARTx, "msg: Error\n");
        }
        print_pos_list(param->pos_ptr->link);
    }
}

void cmd_action_func(int argc,char *argv[])
{
    int no;
    list_node * ptr;
    if (argc == 1)
    {//跑到下一个点
		if(now_pos_ptr->link == NULL)
		{
			USART_SendString(bluetooth, "mag: no pos\n");
			return;
		}else if(now_pos_ptr->link->data == NULL)
		{
			USART_SendString(bluetooth, "msg: data error\n");
			return;
		}else{
			now_pos_ptr = now_pos_ptr->link;
			now_pos = now_pos_ptr->data;
			
			chassis.END.X = now_pos->x;
			chassis.END.Y = now_pos->y;
			chassis.END.ANG = now_pos->ang;
			OPEN_Hander = 0;
			chassis.car_state = car_ready;
		}
    }else if (argc == 2){
        no = atoi(argv[1]);
        ptr = list_locate(&param->pos_ptr, no);
		if(ptr == NULL)
		{
			USART_SendString(bluetooth, "msg: no error\n");
			return;
		}else if(ptr->data == NULL)
		{
			USART_SendString(bluetooth, "msg: data error\n");
			return;
		}
        now_pos = ptr->data;
        now_pos_ptr = ptr;
		chassis.END.X = now_pos->x;
		chassis.END.Y = now_pos->y;
		chassis.END.ANG = now_pos->ang;
		OPEN_Hander = 0;
        //跑到指定的点去
    }else if (argc == 4){
        //跑到指定的位置
		chassis.END.X = atof(argv[1]);
		chassis.END.Y = atof(argv[2]);
		chassis.END.ANG = atof(argv[3]);
		OPEN_Hander = 0;
		chassis.car_state = car_ready;
    }
	
}

void cmd_param_func(int argc,char *argv[]){
	if (strcmp(argv[1],"speedmax")==0)
		chassis.Speed_max = atoi(argv[2]);
	else if (strcmp(argv[1],"speedmin")==0)
		chassis.Speed_min = atoi(argv[2]);
	else if (strcmp(argv[1],"movespeed")==0)
		chassis.Move_speed = atoi(argv[2]);
	else if (strcmp(argv[1],"moveradium")==0)
		chassis.Move_radium = atof(argv[2]);
	else if (strcmp(argv[1],"angleradium")==0)
		chassis.Angle_radium = atof(argv[2]);
	else if (strcmp(argv[1],"anglespeed")==0)
		chassis.Angle_speed = atoi(argv[2]);
	else if (strcmp(argv[1],"start")==0)
		chassis.Start_distance = atof(argv[2]);
	else if (strcmp(argv[1],"factor")== 0)
		chassis.factor = atof(argv[2]);
	else if (strcmp(argv[1],"save") == 0)
	{	
		if(chassis_save()<0)
		{
			USART_SendString(bluetooth,"msg: 写入flash出错\n");
			while(1);
		}
	}
	else if (strcmp(argv[1],"print") == 0)
		chassis_param_print();
}

void cmd_switch_func(int argc,char *argv[])
{
	USART_SendString(bluetooth,"msg:left(0) or right(1):%d\n",LEFT_RIGHT);
	if(strcmp(argv[1],"left") == 0)
	{
		LEFT_RIGHT = 0;
		switch_side = true;
	}else if(strcmp(argv[1],"right") == 0)
	{
		LEFT_RIGHT = 1;
		switch_side = true;
	}
}

void cmd_launch_func(int argc,char *argv[])
{
    int no, no0;
    int erro_no;
    static float pitch, turn, speed =7.7, yaw, jmp;
    Launch_data * data;
    list_node * ptr;
    if(strcmp(argv[1],"fly")==0)
    {
		if(argc < 3){
            USART_SendString(CMD_USARTx,"msg: Error!please enter:\n");
            USART_SendString(CMD_USARTx,"msg:    launch fly <l or r> \n");
			return;
        }
		if(strcmp(argv[2],"l")==0)
		{
			flywheel_left_fly1();
		}else{
			flywheel_right_fly1();
		}			
		//USART_SendString(bluetooth,"msg: fly\r");
	}else if(strcmp(argv[1],"continute")==0)
    {
		if(argc < 3){
            USART_SendString(CMD_USARTx,"msg: Error!please enter:\n");
            USART_SendString(CMD_USARTx,"msg:    launch continute <l or r> \n");
			return;
        }
		if(strcmp(argv[2],"l")==0)
		{
			autorun.launch_l_continute = true;
		}else{
			autorun.launch_r_continute = true;
		}			
		//USART_SendString(bluetooth,"msg: fly\r");
	}else if(strcmp(argv[1],"flyn")==0)
    {
		if(argc < 3){
            USART_SendString(CMD_USARTx,"msg: Error!please enter:\n");
            USART_SendString(CMD_USARTx,"msg:    launch flyn <l or r> <n> <speed> <turn> <pitch> <yaw> <jmp>\n");
			return;
        }
		no = atoi(argv[3]);
		speed = atof(argv[4]);
		turn = atof(argv[5]);
		pitch = atof(argv[6]);
		yaw = atof(argv[7]);
		jmp = atof(argv[8]);
		if(strcmp(argv[2],"l")==0)
		{
			flywheel_left_flyn(no,speed,pitch,yaw);
		}else{
			flywheel_right_flyn(no,speed,pitch,yaw);
		}			
		//USART_SendString(bluetooth,"msg: fly\r");
	}else if (strcmp(argv[1],"stop")==0)
    {
		flywheel_left_stop();
        flywheel_right_stop();
    }else if (strcmp(argv[1],"load")==0)
    {
		if(argc < 3){
            USART_SendString(CMD_USARTx,"msg: Error!please enter:\n");
            USART_SendString(CMD_USARTx,"msg:    launch load <l or r> <no>\n");
			return;
        }
        no = atoi(argv[3]);
		if(strcmp(argv[2], "l") == 0)
			ptr = list_locate(&now_pos->d[target].launch_ptr, no);
		else
			ptr = list_locate(&now_pos->r[target].launch_ptr, no);
        if (ptr == NULL)
        {
            USART_SendString(CMD_USARTx, "msg: Error\n");
            return;
        }
        data = ptr->data;
        pitch = data->pitch;
        turn = data->turn;
        speed = data->speed;
        yaw = data->yaw;
		jmp = data->jmp;
        //调整姿态
		if(strcmp(argv[2], "l") == 0)
		{	flywheel_left_setBrushless(speed);//左边调整
			flywheel_left_setPitch(pitch);
			flywheel_left_setYaw(yaw);
			//flywheel_left_setJmp(jmp);
		}else{
			flywheel_right_setBrushless(speed);//左边调整
			flywheel_right_setPitch(pitch);
			flywheel_right_setYaw(yaw);
			//flywheel_right_setTurn(turn);
			//flywheel_right_setJmp(jmp);
		}
		OPEN_Hander = 0;
    }else if (strcmp(argv[1], "set")==0)
    {
		if(strcmp(argv[2],"home") == 0)
		{
			flywheel_left_home();
			flywheel_right_home();
		}else if(strcmp(argv[3], "pitch")==0)
		{
			OPEN_Hander = 0;
			pitch = atof(argv[4]);//0-100
			if(strcmp(argv[2], "l") == 0)
				flywheel_left_setPitch(pitch);//左边调整
			else
				flywheel_right_setPitch(pitch);//右边调整
		}else if(strcmp(argv[3], "speed")==0)
		{
			speed = atof(argv[4]);
			if(strcmp(argv[2], "l") == 0)
				flywheel_left_setBrushless(speed);//左边调整
			else
				flywheel_right_setBrushless(speed);//右边调整
		}else if(strcmp(argv[3], "yaw")==0)
		{
			yaw = atof(argv[4]);
			if(strcmp(argv[2], "l") == 0)
				flywheel_left_setYaw(yaw);//左边调整
			else
				flywheel_right_setYaw(yaw);//右边调整
        }/*else if(strcmp(argv[3], "jmp")==0)
		{
			jmp = atof(argv[4]);
			//跳台调整
			if(strcmp(argv[2], "l") == 0)
				flywheel_left_setJmp(jmp);//左边调整
			else
				flywheel_right_setJmp(jmp);//右边调整
        }*/
		else if(argc == 8) {
		//直接调整
			pitch = atof(argv[3]);
			turn = atof(argv[4]);
			speed = atof(argv[5]);
			yaw = atof(argv[6]);
			jmp = atof(argv[7]);
			//调整姿态
			if(strcmp(argv[2], "l") == 0)
			{	
				flywheel_left_setBrushless(speed);//左边调整
				flywheel_left_setPitch(pitch);
				flywheel_left_setYaw(yaw);
				//flywheel_left_setJmp(jmp);
			}else{
				flywheel_right_setBrushless(speed);//左边调整
				flywheel_right_setPitch(pitch);
				flywheel_right_setYaw(yaw);
				//flywheel_right_setTurn(turn);
				//flywheel_right_setJmp(jmp);
			}
		}
		
    }else if (strcmp(argv[1], "print")==0)
    {
		if(strcmp(argv[2], "l") == 0)
			print_launch_list(now_pos->d[target].launch_ptr->link);//左边打印
		else
			print_launch_list(now_pos->r[target].launch_ptr->link);//右边打印
    }else if (strcmp(argv[1], "add")==0)
    {
        if(argc < 9){
            USART_SendString(CMD_USARTx,"msg: Error!please enter:\n");
            USART_SendString(CMD_USARTx,"msg:   launch add <l or r> <no> <pitch> <turn> <speed> <yaw> <jmp>\n");
            return;
        }
        no = atoi(argv[3]);
        pitch = atof(argv[4]);
        turn = atof(argv[5]);
        speed = atof(argv[6]);
        yaw = atof(argv[7]);
		jmp = atof(argv[8]);
        data = (Launch_data *) malloc(sizeof(Launch_data));
        data->pitch = pitch;
        data->turn = turn;
        data->speed = speed;
        data->yaw = yaw;
		data->jmp = jmp;
		
		if(strcmp(argv[2], "l") == 0)
		{
				if((erro_no = list_insert(&now_pos->d[target].launch_ptr, no, data)) <= 0){
				USART_SendString(CMD_USARTx,"msg: Error:%d\n",erro_no);
			}else
				now_pos->d[target].launch_num++;
			print_launch_list(now_pos->d[target].launch_ptr->link);
		}
		else
		{
			if((erro_no = list_insert(&now_pos->r[target].launch_ptr, no, data)) <= 0){
            USART_SendString(CMD_USARTx,"msg: Error:%d\n",erro_no);
			}else
				now_pos->r[target].launch_num++;
			print_launch_list(now_pos->r[target].launch_ptr->link);
		}
    }else if (strcmp(argv[1], "modi")==0)
    {
        if(argc < 9){
            USART_SendString(CMD_USARTx,"msg: Error!please enter:\n");
            USART_SendString(CMD_USARTx,"msg:   launch modi <l or r> <no> <pitch> <turn> <speed> <yaw> <jmp>\n");
            return;
        }
        no = atoi(argv[3]);
		if(strcmp(argv[2], "l") == 0)
			ptr = list_locate(&now_pos->d[target].launch_ptr, no);
		else
			ptr = list_locate(&now_pos->r[target].launch_ptr, no);
        
        if (ptr == NULL)
        {
            USART_SendString(CMD_USARTx, "msg: Error\n");
            return;
        }
        data = ptr->data;
        data->pitch = atof(argv[4]);
        data->turn = atof(argv[5]);
        data->speed = atof(argv[6]);
        data->yaw = atof(argv[7]);
		data->jmp = atof(argv[8]);
        if(strcmp(argv[2], "l") == 0)
			print_launch_list(now_pos->d[target].launch_ptr->link);//左边打印
		else
			print_launch_list(now_pos->r[target].launch_ptr->link);//右边打印
    }else if (strcmp(argv[1], "jmp")==0)
    {
        no = atoi(argv[3]);
        no0 = atoi(argv[4]);
        if(strcmp(argv[2], "l") == 0)
			ptr = list_locate(&now_pos->d[target].launch_ptr, no);
		else
			ptr = list_locate(&now_pos->r[target].launch_ptr, no);
        if (ptr != NULL)
        {
			if(strcmp(argv[2], "l") == 0)
				node_move(&now_pos->d[target].launch_ptr, no0, ptr);//左边jmp
			else
				node_move(&now_pos->r[target].launch_ptr, no0, ptr);//右边jmp
            
        }else{
            USART_SendString(CMD_USARTx, "msg: Error\n");
        }
        if(strcmp(argv[2], "l") == 0)
			print_launch_list(now_pos->d[target].launch_ptr->link);//左边打印
		else
			print_launch_list(now_pos->r[target].launch_ptr->link);//右边打印
    }else if (strcmp(argv[1], "del")==0)
    {
        no = atoi(argv[3]);
        if(strcmp(argv[2], "l") == 0)
			ptr = list_locate(&now_pos->d[target].launch_ptr, no);
		else
			ptr = list_locate(&now_pos->r[target].launch_ptr, no);
        if (ptr != NULL)
        {
            data = ptr->data;
            free(data);
			
			if(strcmp(argv[2], "l") == 0)
			{
				if(list_remove_num(&now_pos->d[target].launch_ptr,no))
				{	
					now_pos->d[target].launch_num-=1;
					USART_SendString(CMD_USARTx,"msg: del success\n");
				}
			}
			else
			{
				if(list_remove_num(&now_pos->r[target].launch_ptr,no))
				{	
					now_pos->r[target].launch_num-=1;
					USART_SendString(CMD_USARTx,"msg: del success\n");
				}
			}
        }
        if(strcmp(argv[2], "l") == 0)
			print_launch_list(now_pos->d[target].launch_ptr->link);//左边打印
		else
			print_launch_list(now_pos->r[target].launch_ptr->link);//右边打印
    }else if (strcmp(argv[1], "pop")==0)
    {
        if(strcmp(argv[2], "l") == 0)
			ptr = list_locate(&now_pos->d[target].launch_ptr, no);
		else
			ptr = list_locate(&now_pos->r[target].launch_ptr, no);
        if (ptr != NULL)
        {
            data = ptr->data;
            free(data);
			
			if(strcmp(argv[2], "l") == 0)
			{
				if(list_remove_num(&now_pos->d[target].launch_ptr, now_pos->d[target].launch_num))
				{	
					now_pos->d[target].launch_num-=1;
					USART_SendString(CMD_USARTx,"msg: pop success\n");
				}
			}
			else
			{
				if(list_remove_num(&now_pos->r[target].launch_ptr, now_pos->d[target].launch_num))
				{	
					now_pos->r[target].launch_num-=1;
					USART_SendString(CMD_USARTx,"msg: pop success\n");
				}
			}
        }
        if(strcmp(argv[2], "l") == 0)
			print_launch_list(now_pos->d[target].launch_ptr->link);//左边打印
		else
			print_launch_list(now_pos->r[target].launch_ptr->link);//右边打印
    }else if(strcmp(argv[1],"clear") == 0)
	{
		
		if(strcmp(argv[2], "l") == 0)
		{
			clear_launch(&now_pos->d[target].launch_ptr);
			now_pos->d[target].launch_num = 0;
		}
		else
		{
			clear_launch(&now_pos->r[target].launch_ptr);
			now_pos->r[target].launch_num = 0;
		}
		
	}else if(strcmp(argv[1],"target") == 0)
	{
		if (argc == 2)
			USART_SendString(CMD_USARTx,"target:%d\n",target);
		else{
			no = atoi(argv[2]);
			target = no;
			USART_SendString(CMD_USARTx,"target:%d\n",target);
		}
	}else if(argc == 8) {
	//发射n个
		pitch = atof(argv[2]);
		turn = atof(argv[3]);
		speed = atof(argv[4]);
		yaw = atof(argv[5]);
		jmp = atof(argv[6]);
		//调整姿态
		if(strcmp(argv[1], "l") == 0)
		{
			flywheel_left_flyn(atoi(argv[8]),speed,pitch,yaw);
		}else{
			flywheel_right_flyn(atoi(argv[8]),speed,pitch,yaw);
		}
	}
}
