#include "main.h" 
#include "sys.h"

//extern u8 cmd, sticks[4];
//extern u8 ptr;

//全局变量定义区
enum {car_stop, car_running, car_ready} car_state;//车的运动状态
enum {pitch_ready, pitch_up, pitch_wait} pitch_state;//俯仰角的状态
enum {load_ready, load_up, load_catch, load_down, load_turn, load_wait} load_state;//送料机构的状态
enum {hit_ready, hit_pull, hit_put, hit_push, hit_wait} hit_state;//击打机构的状态

bool hit_flag = false;
int load_count, hit_count;
float pull_pos = 10;

bool g_stop_flag = false;
float pos_x,pos_y;
float angle; 
int WantSpeed = 0;//飞轮目标速度

float Default_angle;
int Chassis_motor0 =0 , Chassis_motor1 =0 , Chassis_motor2 =0 ;
float CH_angle_M0 = -PI + 2*PI/3, CH_angle_M1 = -PI - 2*PI/3, CH_angle_M2 = -PI;

float Speed_Duty = 770;
float Speed_P = 0.0030, Speed_D = 0.3000 , Speed_I = 0.0f;

POSITION END, START;
SwitchTIM encoder;

#define MAXSPEED 970
#define MINSPEED 600

int Speed_error = 0;
int PRESpeed_error = 0;
int truespeed = 0;


//--------------暂时用来草稿-------------

float Move_radium, Angle_radium;
int Speed_min, Speed_max, Angle_speed, Move_speed;
int ms = 0;

void set_params(){
	Speed_min = 30;
	Speed_max = 6000;
	Move_radium = 0.00001;
	Angle_radium = 0.001;
	Angle_speed = 2400;
	Move_speed = 3500;
}

void TIM2_IRQHandler(void){
	int i;
	if( TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET ) 
	{
		ms++;
		
		/*truespeed =  TIM5->CNT - 6000;
		TIM5->CNT = 6000;
		
		PRESpeed_error = Speed_error;
		Speed_error = truespeed - WantSpeed;

		Speed_Duty -= (Speed_P*Speed_error + Speed_D*(Speed_error - PRESpeed_error));
	
		if(Speed_Duty >MAXSPEED)
			Speed_Duty = MAXSPEED ;
		if(Speed_Duty < MINSPEED)
			Speed_Duty = MINSPEED ;
		TIM_SetCompare1(TIM8,1000000/50*Speed_Duty/10000 - 1);
		//USART_SendString(bluetooth,"msg: TrueSpeed:%d  WantSpeed:%d duty:%f\n",truespeed,WantSpeed,Speed_Duty);
		if (wait_cnt>-1)
			if (++wait_cnt==50)
			{	USART_SendString(UART5,"Good to go\n");
				USART_ITConfig(UART5, USART_IT_RXNE, ENABLE);wait_cnt=-1;}
		TIM_ClearITPendingBit(TIM2,TIM_FLAG_Update);//必须清除中断标志位否则一直中断*/
		for (i=0;i<12;i++) 
		 {
			 if (b[i]->cnt>0) b[i]->cnt--;
			 if (b[i]->cnt==0)
				 b[i]->ispressed=false;
		 }
		 if(load_count)
			load_count--;
		 if(hit_count)
			hit_count--;
		TIM_ClearITPendingBit(TIM2, TIM_FLAG_Update);//必须清除中断标志位否则一直中断
	}	
}


//-----------------草稿结束------------------------

int main(void)
{   //system_stm32f4xx.c #define PLL_M=8 PLL_N=336 HSE -> SYSCLK 168MHZ
	// RCC->CFGR |= RCC_CFGR_PPRE2_DIV2; APHB1_CLK 84MHZ
	//AHB 168MHZ  APB2 84MHZ  APB1 42MHZ

	int hit_flag_count = 0;
	int temp_speed = 0;
	
	extern int TIM3_round, TIM4_round;
	int Hx, Hy;
	
	int g_vega_pos_x = 0, g_vega_pos_y= 0;   
	float g_vega_angle = 0;
	
	float errorAngle, error_X, error_Y, Serror_X, Serror_Y;
	float factor = 5;
	float anglebuf =0;
	int Xianding = 0;
	u8 POS_Mark = 0;
	float Pre_ErrorY = 0;
	
	static int TURN_speed;
	static int ChassisSpeed;
	
	rcc_config();
	gpio_config();
	delay_init(168);  //初始化延时函数
//	EXTI_config();
	nvic_config();
	
	TIM9_Init();
	TIM14_Init();
	TIM13_Init();
	Step1_Init();
	Step2_Init();
//	SPI2_Init();
	usart_init(&Hx, &Hy);
	cmd_init();
	param_init();
//	can_add_callback();

	can_init();
    vega_init(&g_vega_pos_x,&g_vega_pos_y,&g_vega_angle);
    vega_reset();
	delay_ms(1000);
	encoder_init(&encoder);
	SetUsed(4,1);
	delay_ms(1000);
	SetUsed(3,1);
	delay_ms(1000);
	SetUsed(5,2);
	delay_ms(1000);
	SetTime(5);
	TIM2_Init();
	USART_SendString(UART5,"msg: Let's go!\n");
	
	USART_SendString(MOTOR_USARTx,"5HO\r6HO\r");
	set_params();
	direction_angle = PI/2;
	START.X = g_vega_pos_x* 0.0001 * 0.81;
	START.Y = g_vega_pos_y* 0.0001 * 0.81;
	START.ANG = (g_vega_angle/180.f)*PI;
	OPEN_Hander = 1;
	
	END = START;
	car_state = car_stop;
	pitch_state = pitch_ready;
	load_state = load_ready;
	hit_state = hit_ready;
    while(1) 
	{
		if(g_stop_flag){//停止一切运动
			USART_SendString(MOTOR_USARTx,"0v0\r1v0\r2v0\r");
			USART_SendString(MOTOR_USARTx,"3v0\r4v0\r");
			USART_SendString(MOTOR_USARTx,"5v0\r6v0\r");
		}else{
			//if((encoder.GetTim5-temp_speed)!=0)
			//	USART_SendString(UART5,"msg: %d\n",encoder.GetTim5);
			temp_speed = encoder.GetTim5;
			bottons_check();
			sticks_check(Hx,Hy);
			pos_x = g_vega_pos_x* 0.0001 * 0.81;
			pos_y = g_vega_pos_y* 0.0001 * 0.81;
			angle = (g_vega_angle/180.f)*PI;
			//USART_SendString(UART5,"\n\nX=%f   Y=%f  angle=%f\n",pos_x, pos_y , angle);
			ChassisSpeed= Move_speed;
			Step_check();
			if(OPEN_Hander ==0){
				/**-------------------------自动部分--------------------------------**/		
				switch(pitch_state){
					case pitch_ready:
						if(hit_flag){
							pitch_state = pitch_up;
							hit_flag_count = 0;
							pitch_flag = true;
							pur_pitch = 5;
							//USART_SendString(bluetooth,"msg: pitch_up\r");
						}
						break;
					case pitch_up:
						if(pitch_flag == false){
							pitch_state = pitch_wait;
							//USART_SendString(bluetooth,"msg: pitch_wait\r");
							hit_flag_count++;
						}
						break;
					case pitch_wait:
						if(hit_flag_count == 3)
						{
							hit_flag = false;
							pitch_state = load_ready;
							//USART_SendString(bluetooth,"msg: pitch_ready\r");
						}
						break;
				}
				
				switch(load_state){
					case load_ready:
						if(hit_flag){
							load_state = load_up;
							PGout(12) = 1;
							load_count = 100;
							//USART_SendString(bluetooth,"msg: load_up\r");
						}
						break;
					case load_up:
						if(load_count == 0){
							load_state = load_catch;
							PGout(13) = 1;
							TIM_SetCompare2(TIM9,1800);
							load_count = 100;
							//USART_SendString(bluetooth,"msg: load_catch\r");
						}
						break;
					case load_catch:
						if(load_count == 0){
							load_state = load_down;
							PGout(12) = 0;
							load_count = 100;
							//USART_SendString(bluetooth,"msg: load_down\r");
						}
						break;
					case load_down:
						if(load_count == 0){
							load_state = load_turn;
							Step1_moveto(Step1_get_count()+4080);
							//USART_SendString(bluetooth,"msg: load_turn\r");
						}
						break;
					case load_turn:
						if(Step1_state()){
							load_state = load_wait;
							//USART_SendString(bluetooth,"msg: load_wait\r");
							hit_flag_count++;
						}
						break;
					case load_wait:
						if(hit_flag_count == 3)
						{
							load_state = load_ready;
							//USART_SendString(bluetooth,"msg: load_ready\r");
						}
						break;
				}
				
				switch(hit_state){
					case hit_ready:
						if(hit_flag && load_state == load_turn)
						{	
							Step2_moveto(3000);
							USART_SendString(MOTOR_USARTx,"5LA%d\r",(int)(0*10000));
							USART_SendString(MOTOR_USARTx,"5M\r5M\r5M\r");
							hit_count = 1000;
							hit_state = hit_pull;
							//USART_SendString(bluetooth,"msg: hit_pull\r");
						}
						break;
					case hit_pull:
						if(hit_count == 0 && Step2_state()){
							hit_state = hit_put;
							TIM_SetCompare2(TIM9,1060);
							hit_count = 100;
							USART_SendString(bluetooth,"msg: hit_put%d\n",Step2_get_count());
						}
						break;
					case hit_put:
						if(hit_count == 0 && pitch_state == pitch_wait)
						{
							PGout(13) = 0;
							hit_count = 1000;
							Step2_moveto(0);
							USART_SendString(MOTOR_USARTx,"5LA340000\r");
							USART_SendString(MOTOR_USARTx,"5M\r5M\r5M\r");
							hit_state = hit_push;
							//USART_SendString(bluetooth,"msg: hit_push\r");
						}
						break;
					case hit_push:
						if(hit_count == 0 && Step2_state())
						{
							hit_state = hit_wait;
							USART_SendString(bluetooth,"msg: hit_wait%d\n",Step2_get_count());
							hit_flag_count++;
						}
						break;
					case hit_wait:
						if(hit_flag_count == 3)
						{
							hit_state = hit_ready;
							//USART_SendString(bluetooth,"msg: hit_ready\r");
						}
						break;
				}
				
				if(car_state == car_ready){
					START.X = g_vega_pos_x* 0.0001 * 0.81;
					START.Y = g_vega_pos_y* 0.0001 * 0.81;
					START.ANG = (g_vega_angle/180.f)*PI;
					ms = 0;
					car_state = car_running;
				}
				if(car_state == car_running){
					errorAngle = angle - END.ANG;
					error_X = END.X - pos_x;
					error_Y = END.Y - pos_y;
					
					Serror_X = START.X - pos_x;
					Serror_Y = START.Y - pos_y;
					
					if(factor * factor * (powf(Serror_X,2)+powf(Serror_Y,2)) < (powf(error_X,2)+powf(error_Y,2))) {//加速
						if((powf(Serror_X,2)+powf(Serror_Y,2)) > 0.0009)
							ChassisSpeed = sqrt(powf(Serror_X,2)+powf(Serror_Y,2))*Move_speed * factor;
						else 
							ChassisSpeed = 0.03 * Move_speed * factor;
					}
					else {
						if((powf(error_X,2)+powf(error_Y,2)) > 1)
							ChassisSpeed = sqrt(powf(error_X,2)+powf(error_Y,2))*Move_speed;
						else if((powf(error_X,2)+powf(error_Y,2)) > 0.0025)
							ChassisSpeed = sqrt(sqrt(powf(error_X,2)+powf(error_Y,2))) * Move_speed;
						else
							ChassisSpeed = 89.44 * (powf(error_X,2)+powf(error_Y,2)) * Move_speed;
					}
					/*if((powf(error_X,2)+powf(error_Y,2)) > 1)
						ChassisSpeed = sqrt(powf(error_X,2)+powf(error_Y,2))*Move_speed;
					else if((powf(error_X,2)+powf(error_Y,2)) > 0.01)
						ChassisSpeed = sqrt(sqrt(powf(error_X,2)+powf(error_Y,2)))*Move_speed;
					else 
						ChassisSpeed = 31.6 * (powf(error_X,2)+powf(error_Y,2)) * Move_speed;
					*/
					if(ChassisSpeed>Speed_max)
						ChassisSpeed = Speed_max;
					
					if(errorAngle >= 0.05 && errorAngle < 2)
					{          //角度调整
						TURN_speed = -1*Angle_speed*errorAngle;
					}
					else if(errorAngle <=-0.05 && errorAngle > -2)
					{
						TURN_speed = -1*Angle_speed*errorAngle;
					}
					else if(errorAngle > Angle_radium && errorAngle < 0.05)
					{
						TURN_speed = -1*Angle_speed*errorAngle;
					}
					else if(errorAngle < -1*Angle_radium && errorAngle > -0.05)
					{
						TURN_speed = -1*Angle_speed*errorAngle;
					}
					else if(errorAngle>=2)
					{
						TURN_speed =-1*Angle_speed*2;
					}
					else if(errorAngle<=-2)
					{
						TURN_speed =-1*Angle_speed*(-2);
					}
					else
					{
						TURN_speed= 0;
					}
					
					if(powf(error_X,2)+powf(error_Y,2) <= Move_radium)
					{//已经到达
						ChassisSpeed = 0;
					}
					else
					{
						direction_angle = atan2(error_Y,error_X);
					}
			
					Chassis_motor0 = (ChassisSpeed * cos((CH_angle_M0 +angle) - direction_angle) + TURN_speed);//Y轴方向，这里direction_angle代表小车相对于场地坐标系的方向
					Chassis_motor1 = -1*(ChassisSpeed * cos((CH_angle_M1 +angle) - direction_angle) + TURN_speed);
					Chassis_motor2 = (ChassisSpeed * cos((CH_angle_M2 +angle) - direction_angle) + TURN_speed);
					
					if(fabs(Chassis_motor2) < 1 && fabs(Chassis_motor1) < 1 && fabs(Chassis_motor0) < 1)
					{
						USART_SendString(MOTOR_USARTx,"2v0\r1v0\r0v0\r");
						car_state = car_stop;
					}
					else
					{
						USART_SendString(MOTOR_USARTx,"1v%d\r2v%d\r0v%d\r",Chassis_motor1 , Chassis_motor2 , Chassis_motor0);
					}
				}
				else if(car_state == car_stop)
				{
					if(ms>200)
						USART_SendString(CMD_USARTx,"msg: %fs\n",ms*5.0/1000);
					ms = 0;
					/*pos_x = temp_x* 0.0001 * 0.81;
					pos_y = temp_y* 0.0001 * 0.81;
					angle = (temp_angle/180.f)*PI;*/
					//state = ready;
					//OPEN_Hander = 1;
				}
			}
			else if(OPEN_Hander ==1)
			{
				/********手柄部分***********/
				//USART_SendString(CMD_USARTx,"Ohi\n");
				errorAngle = angle;
				Xianding = 0;
				if (RU.ispressed)
				{ PGout(11) = !GPIO_ReadOutputDataBit(GPIOG,GPIO_Pin_11);
					USART_SendString(bluetooth,"msg: RU is pressed\n");
				}
				if (RR.ispressed) {
					PGout(12) = !GPIO_ReadOutputDataBit(GPIOG,GPIO_Pin_12);
					USART_SendString(bluetooth,"msg: RR is pressed\n");
				}
				if (RD.ispressed) {
					PGout(13) = !GPIO_ReadOutputDataBit(GPIOG,GPIO_Pin_13);
					USART_SendString(bluetooth,"msg: RD is pressed\n");
				}
				if (RL.ispressed) {
					if (TIM9->CCR2<1500) TIM_SetCompare2(TIM9,1800);
					else TIM_SetCompare2(TIM9,1060);
					USART_SendString(bluetooth,"msg: RL is pressed\n");
				}
				if (L2.ispressed) {
					if (LU.ispressed){
						TIM14->CCER |= TIM_CCER_CC1E;
						PFout(5) = 0;
					}else				
					if (LD.ispressed){
						PFout(5) = 1;
						TIM14->CCER |= TIM_CCER_CC1E;
					}else{ 
					//USART_SendString(bluetooth,"msg: 000!\n");
						TIM14->CCER &= ~TIM_CCER_CC1E;
					}Xianding=0;
					if (LR.ispressed) Xianding -=2000;					
					if (LL.ispressed) Xianding +=2000;
					if (Xianding !=0){
						//USART_SendString(bluetooth,"msg: %d\n",Xianding);
						USART_SendString(UART4,"5V%d\r",Xianding);
					}else{
						//USART_SendString(bluetooth,"msg: %d\n",Xianding);
						USART_SendString(UART4,"5V0\r");
					}
				}else{ 
					//USART_SendString(bluetooth,"msg: 000!\n");
						TIM14->CCER &= ~TIM_CCER_CC1E;
						USART_SendString(UART4,"5V0\r");
				}
				if (R2.ispressed){
					Xianding=0;
					if (LU.ispressed) Xianding +=200;					
					if (LD.ispressed) Xianding -=200;
					if (Xianding !=0){
						//USART_SendString(bluetooth,"msg: %d\n",Xianding);
						USART_SendString(UART4,"3V%d\r",Xianding);
					}else{
						USART_SendString(UART4,"3V0\r");
					}
					Xianding=0;
					if (LL.ispressed) Xianding +=200;					
					if (LR.ispressed) Xianding -=200;
					if (Xianding !=0){
						//USART_SendString(bluetooth,"msg: %d\n",Xianding);
						USART_SendString(UART4,"4V%d\r",Xianding);
					}else{
						USART_SendString(UART4,"4V0\r");
					}
				}else{
					USART_SendString(UART4,"3V0\r4v0\r");
				}
				Xianding=0;
				if (L1.ispressed) Xianding +=200;					
				if (R1.ispressed) Xianding -=200;
				if (Xianding !=0){
					//USART_SendString(bluetooth,"msg: %d\n",Xianding);
					USART_SendString(UART4,"6V%d\r",Xianding);
				}else{
					USART_SendString(UART4,"6V0\r");
				}
				ChassisSpeed = 1000;
				if (!L2.ispressed&&!R2.ispressed){
				if (LU.ispressed) direction_angle = -PI/2;
				else if (LD.ispressed) direction_angle = PI/2;
				else if (LL.ispressed) direction_angle = PI;
				else if (LR.ispressed) direction_angle = 0;
				else ChassisSpeed = 0;}else
				ChassisSpeed = 0;
				
				Chassis_motor0 = -1* (ChassisSpeed/7*3*cos((CH_angle_M0 + errorAngle) - direction_angle)+TURN_speed);
				Chassis_motor1 = (ChassisSpeed/7*3*cos((CH_angle_M1 + errorAngle) - direction_angle)+TURN_speed);
				Chassis_motor2 = -1* (ChassisSpeed/7*3*cos((CH_angle_M2 + errorAngle) - direction_angle)+TURN_speed);
				USART_SendString(MOTOR_USARTx,"1v%d\r2v%d\r0v%d\r",Chassis_motor1 , Chassis_motor2 , Chassis_motor0);
//				if (Chassis_motor0 != 0) USART_SendString(UART5,"msg: C0:%d\n",Chassis_motor0);
//				if (Chassis_motor1 != 0) USART_SendString(UART5,"msg: C1:%d\n",Chassis_motor1);
//				if (Chassis_motor2 != 0) USART_SendString(UART5,"msg: C2:%d\n",Chassis_motor2);
				if(OPEN_Hander == 1){
					END.X=pos_x;
					END.Y=pos_y;
					END.ANG=angle;
				}
			}
			
			if (pitch_flag)
			{
				pitch.now = -encoder.GetTim3 / 10000.f;
				pitch.d = pur_pitch - pitch.now;
				pitch.dd = pitch.d - pitch.d_last;
				pitch.d_last = pitch.d;
				if (fabs(pitch.d) < 0.01)
				{
					if (pitch.cnt > 10)
					{
						pitch_move(0);
						pitch.cnt = 0;
						pitch_flag=false;
					}
					else
					{
						pitch.cnt++;
						pitch_move(pitch.d * 1000 + pitch.dd * 800);
					}
				}
				else
				{
					pitch.cnt = 0;
					pitch_move(pitch.d * 1000 + pitch.dd * 800);
				}
			}
			if (roll_flag)
			{
				roll.now = encoder.GetTim4 / 10000.f;
				roll.d = pur_roll - roll.now;
				roll.dd = roll.d - roll.d_last;
				roll.d_last = roll.d;
				if (fabs(roll.d) < 0.01)
				{
					if (roll.cnt > 10)
					{
						roll_flag=false;
						roll_move(0);
					}
					else
					{
						roll.cnt++;
						roll_move(roll.d * 1000 + roll.dd * 800);
					}
				}
				else
				{
					roll.cnt=0;
					roll_move(roll.d * 1000 + roll.dd * 800);
				}
			}	
		}
	}
}
