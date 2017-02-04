#include "cmd_func.h"
#include "cmd.h"
#include "stdlib.h"
#include "step.h"
#include "encoder.h"
#include "configuration.h"
#include "string.h"
#include "delay.h"
#include "math.h"
#include "param.h"

extern s8 ptrS,ptrB;
extern enum {car_stop, car_running, car_ready} car_state;//车的运动状态
extern Param * param;
extern bool g_stop_flag;
extern float pos_x;
extern float pos_y;
extern float angle;
extern int WantSpeed;
extern int truespeed;
extern float Move_radium,Angle_radium;
extern int Speed_min,Speed_max,Angle_speed,Move_speed;

u8 target=1;       				//目标0-6

static list_node * now_pos_ptr;
static Pos_data * now_pos;     //当前点的数据指针
static float motor_v;

extern float pur_pitch;
extern float pur_roll;
extern SwitchTIM encoder;
extern bool pitch_flag,roll_flag;
extern bool hit_flag;

void cmd_reboot_func(int argc,char *argv[]){
    NVIC_SystemReset();
}

void cmd_stop_func(int argc,char *argv[]){
    if(argc == 1){
        g_stop_flag = !g_stop_flag;
    }else if(argc == 2){
        g_stop_flag = atoi(argv[1]);
    }
}

void cmd_hello_func(int argc,char *argv[]){
    USART_SendString(CMD_USARTx, "msg: Hello World\n");
}

void cmd_test_func(int argc,char *argv[]){
	if(argc == 1){
		hit_flag = true;
		OPEN_Hander = 0;
	}
	if(argc == 2){
		OPEN_Hander = 0;
		USART_SendString(MOTOR_USARTx,"5LA%d\r",(int)(atof(argv[1])*10000));
		//delay_ms(1000);
		USART_SendString(MOTOR_USARTx,"5M\r5M\r5M\r");
	}else if(argc == 3){
		if(strcmp(argv[1],"air") == 0)
		{
			if(strcmp(argv[2],"1") == 0){
				PGout(12) = !PGout(12);
			}else if(strcmp(argv[2],"2") == 0){
				PGout(11) = !PGout(11);
			}
		}else{
			
		}
	}else if(argc == 4){
		if(strcmp(argv[1],"step") == 0)
		{
			OPEN_Hander = 0;
			if(strcmp(argv[2],"1") == 0){
				Step1_moveto(atoi(argv[3]));
			}else if(strcmp(argv[2],"2") == 0){
				Step2_moveto(atoi(argv[3]));
			}
		}
	}
}

void cmd_pos_func(int argc,char *argv[])
{
    int no;
    int no0;
    int i;
    float x,y;
    Pos_data *data;
    list_node * ptr;
    int start_no,end_no;
    int erro_no;
    if (strcmp(argv[1], "now") == 0)
    {
        USART_SendString(CMD_USARTx, "x:%f y:%f\n", pos_x,pos_y);
    }else
    if(strcmp(argv[1],"add") == 0){
        if(argc < 5){
            USART_SendString(CMD_USARTx,"msg: Error!please enter:\n");
            USART_SendString(CMD_USARTx,"msg:    pos add <no> <x> <y>\n");
        }
        no = atoi(argv[2]);
        x = atof(argv[3]);
        y = atof(argv[4]);
        data = (Pos_data *)malloc(sizeof(Pos_data));
        data->x = x;
        data->y =  y;
        for (i = 0; i < 7; ++i)
        {
            data->d[i].launch_num = 0;
            list_init(&data->d[i].launch_ptr);
        }
        if((erro_no = list_insert(&param-> pos_ptr, no, data)) <= 0){
            USART_SendString(CMD_USARTx,"msg: Error:%d\n",erro_no);
        }else{
			param->pos_num++;
			now_pos_ptr = list_locate(&param->pos_ptr,no);
			now_pos = now_pos_ptr->data;
		}
		print_pos_list(param->pos_ptr->link);
    }else if(strcmp(argv[1],"print") == 0){
        print_pos_list(param->pos_ptr->link);
    }else if(strcmp(argv[1],"del") == 0){
        no = atoi(argv[2]);
        ptr = list_locate(&param->pos_ptr, no);
        if (ptr != NULL)
        {
            data = ptr->data;
            for (i = 0; i < 7; ++i)
            {
                clear_launch(&data->d[i].launch_ptr); 
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
        }
    }else if(strcmp(argv[1],"modi") == 0){
        no = atoi(argv[2]);
        if((data = local_pos(no)) == NULL){
            USART_SendString(CMD_USARTx,"msg: Not found point No:%d\n",no);
            return;
        }
        if(argc < 5){
            USART_SendString(CMD_USARTx,"msg: Error cmd format\n");
            return;
        }
         x = atof(argv[3]);
         y = atof(argv[4]);

        data->x =  x;
        data->y =  y;
        print_pos_list(param->pos_ptr->link);
    }else if (strcmp(argv[1], "jmp")==0)
    {
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
    float x, y, v;
    float yaw;
    list_node * ptr;
    if(strcmp(argv[1],"left")==0){
        OPEN_Hander = 1;
		ptrB=L1_KEY;
    }else if(strcmp(argv[1],"right")==0){
        OPEN_Hander = 1;
		ptrB=R1_KEY;
    }else if (strcmp(argv[1],"rotate")==0){
        v = atof(argv[2]);
        yaw = atof(argv[3]);
        //测试底盘电机转动到一定角度
    }else if (argc == 1)
    {
        now_pos_ptr = now_pos_ptr->link;
        now_pos = now_pos_ptr->data;
		x = now_pos->x;
		y = now_pos->y;
		END.X = x;
		END.Y = y;
		END.ANG = angle;
		OPEN_Hander = 0;
		car_state = car_ready;
        //跑到下一个点
    }else if (argc == 2){
        no = atoi(argv[1]);
        ptr = list_locate(&param->pos_ptr, no);
        now_pos = ptr->data;
        now_pos_ptr = ptr;
		x = now_pos->x;
		y = now_pos->y;
		END.X = x;
		END.Y = y;
		END.ANG = angle;
		OPEN_Hander = 0;
        //跑到指定的点去
    }else if (argc == 3){
        x = atof(argv[1]);
        y = atof(argv[2]);
        //跑到指定的位置
		END.X = x;
		END.Y = y;
		END.ANG = angle;
		OPEN_Hander = 0;
		car_state = car_ready;
    }
	
}

void cmd_switch_func(int argc,char *argv[])
{
    u8 state1,state2;
    state1 = GPIO_ReadInputDataBit(GPIOF, GPIO_Pin_11);
    state2 = GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_14);
    USART_SendString(CMD_USARTx, "roll_switch:%d pitch_switch:%d", state1, state2);
}
void cmd_param_func(int argc,char *argv[]){
	if (strcmp(argv[1],"speedmax")==0)
		Speed_max = atoi(argv[2]);
	else if (strcmp(argv[1],"speedmin")==0)
		Speed_min = atoi(argv[2]);
	else if (strcmp(argv[1],"movespeed")==0)
		Move_speed = atoi(argv[2]);
	else if (strcmp(argv[1],"moveradium")==0)
		Move_radium = atof(argv[2]);
	else if (strcmp(argv[1],"angleradium")==0)
		Angle_radium = atof(argv[2]);
	else if (strcmp(argv[1],"anglespeed")==0)
		Angle_speed = atoi(argv[2]);
}

void cmd_launch_func(int argc,char *argv[])
{
    int no, no0;
    int erro_no;
    static float pitch, roll, speed =7.7, yaw;
    Launch_data * data;
    list_node * ptr;
    if (argc == 1)
    {
		
        //如果没开无刷，那就开无刷，转一圈推飞盘
    }else if (strcmp(argv[1], "now")==0)
    {
        //发射参数
        USART_SendString(CMD_USARTx, "pitch:%f roll:%f speed:%d yaw:%.6f\n",
				-encoder.GetTim3/10000.f,encoder.GetTim4/10000.f,encoder.GetTim5,angle);
    }else if (strcmp(argv[1],"start")==0)
    {
       
    }else if (strcmp(argv[1],"stop")==0)
    {
		//WantSpeed = 0;
		TIM_SetCompare1(TIM8,7.7/100*1000000/50 - 1);
        
    }else if (strcmp(argv[1],"pushstop")==0)
    {
        
    }else if (strcmp(argv[1],"load")==0)
    {
        no = atoi(argv[2]);
        ptr = list_locate(&now_pos->d[target].launch_ptr, no);
        if (ptr == NULL)
        {
            USART_SendString(CMD_USARTx, "msg: Error\n");
            return;
        }
        data = ptr->data;
        pitch = data->pitch;
        roll = data->roll;
        speed = data->speed;
        yaw = data->yaw;
        pur_pitch = pitch;
		pitch_flag = true;
		pur_roll = roll;
		roll_flag = true;
		WantSpeed = speed;
		END.X = pos_x;
		END.Y = pos_y;
		END.ANG = yaw;
		OPEN_Hander = 0;
		car_state = car_running;
    }else if (strcmp(argv[1], "set")==0)
    {
		if(strcmp(argv[2], "pitch")==0)
		{
			OPEN_Hander = 0;
			pitch = atof(argv[3]);//0-100
			pur_pitch = pitch;
			pitch_flag = true;
		}else if(strcmp(argv[2], "roll")==0)
		{
			OPEN_Hander = 0;
			roll = atof(argv[3]);
			pur_roll = roll;
			roll_flag = true;
        }else if(strcmp(argv[2], "speed")==0)
		{
			speed = atof(argv[3]);
			TIM_SetCompare1(TIM9,speed/100*1000000/50 - 1);
		}else if(strcmp(argv[2], "yaw")==0)
		{
			yaw = atof(argv[3]);
			END.ANG = yaw;
			OPEN_Hander = 0;
			car_state = car_ready;
        }else if(argc == 6) {
		//直接调整
			pitch = atof(argv[2]);
			roll = atof(argv[3]);
			speed = atof(argv[4]);
			yaw = atof(argv[5]);
			pur_pitch = pitch;
			pitch_flag = true;
			pur_roll = roll;
			roll_flag = true;
			//Move_speed = speed;
			TIM_SetCompare1(TIM8,speed/100*1000000/50 - 1);
			END.ANG = yaw;
			OPEN_Hander = 0;
		}
		
    }else if (strcmp(argv[1], "print")==0)
    {
        print_launch_list(now_pos->d[target].launch_ptr->link);
    }else if (strcmp(argv[1], "add")==0)
    {
        if(argc < 7){
            USART_SendString(CMD_USARTx,"msg: Error!please enter:\n");
            USART_SendString(CMD_USARTx,"msg:   launch add <no> <pitch> <roll> <speed> <yaw>\n");
            return;
        }
        no = atoi(argv[2]);
        pitch = atof(argv[3]);
        roll = atof(argv[4]);
        speed = atof(argv[5]);
        yaw = atof(argv[6]);
        data = (Launch_data *) malloc(sizeof(Launch_data));
        data->pitch = pitch;
        data->roll = roll;
        data->speed = speed;
        data->yaw = yaw;
        if((erro_no = list_insert(&now_pos->d[target].launch_ptr, no, data)) <= 0){
            USART_SendString(CMD_USARTx,"msg: Error:%d\n",erro_no);
        }else
			now_pos->d[target].launch_num++;
        print_launch_list(now_pos->d[target].launch_ptr->link);
    }else if (strcmp(argv[1], "modi")==0)
    {
        if(argc < 7){
            USART_SendString(CMD_USARTx,"msg: Error!please enter:\n");
            USART_SendString(CMD_USARTx,"msg:   launch add <no> <pitch> <roll> <speed> <yaw>\n");
            return;
        }
        no = atoi(argv[2]);
        ptr = list_locate(&now_pos->d[target].launch_ptr, no);
        if (ptr == NULL)
        {
            USART_SendString(CMD_USARTx, "msg: Error\n");
            return;
        }
        data = ptr->data;
        data->pitch = atof(argv[3]);
        data->roll = atof(argv[4]);
        data->speed = atof(argv[5]);
        data->yaw = atof(argv[6]);
        print_launch_list(now_pos->d[target].launch_ptr->link);
    }else if (strcmp(argv[1], "jmp")==0)
    {
        no = atoi(argv[2]);
        no0 = atoi(argv[3]);
        ptr = list_locate(&now_pos->d[target].launch_ptr, no);
        if (ptr != NULL)
        {
            node_move(&now_pos->d[target].launch_ptr, no0, ptr);
        }else{
            USART_SendString(CMD_USARTx, "msg: Error\n");
        }
        print_launch_list(now_pos->d[target].launch_ptr->link);
    }else if (strcmp(argv[1], "del")==0)
    {
        no = atoi(argv[2]);
        ptr = list_locate(&now_pos->d[target].launch_ptr, no);
        if (ptr != NULL)
        {
            data = ptr->data;
            free(data);
            list_remove_num(&now_pos->d[target].launch_ptr,no);
            now_pos->d[target].launch_num-=1;
			USART_SendString(CMD_USARTx,"msg: del success\n");
        }
        print_launch_list(now_pos->d[target].launch_ptr->link);
    }else if (strcmp(argv[1], "pop")==0)
    {
        ptr = list_locate(&now_pos->d[target].launch_ptr, now_pos->d[target].launch_num);
        if (ptr != NULL)
        {
            data = ptr->data;
            free(data);
            list_remove_num(&now_pos->d[target].launch_ptr, now_pos->d[target].launch_num);
            now_pos->d[target].launch_num-=1;
			USART_SendString(CMD_USARTx,"msg: pop success\n");
        }
        print_launch_list(now_pos->d[target].launch_ptr->link);
    }else if(strcmp(argv[1],"clear") == 0)
	{
		clear_launch(&now_pos->d[target].launch_ptr);
		now_pos->d[target].launch_num = 0;
	}else if(strcmp(argv[1],"target") == 0)
	{
		if (argc == 2)
			USART_SendString(CMD_USARTx,"target:%d\n",target);
		else{
			no = atoi(argv[2]);
			target = no;
			USART_SendString(CMD_USARTx,"target:%d\n",target);
		}
	}
}
