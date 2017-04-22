#include "sys.h"
#include "TIM.h"
#include "step.h"
#include "configuration.h"

static int step1_want;
static int step1_count;
static int step2_want;
static int step2_count;

void Step1_Init(void)
{
	GPIO_Configuration(GPIO_Pin_4,GPIO_Mode_OUT,GPIO_OType_PP,GPIO_Speed_50MHz,GPIO_PuPd_NOPULL,GPIOF);
	//GPIO_Configuration(GPIO_Pin_2,GPIO_Mode_OUT,GPIO_OType_PP,GPIO_Speed_50MHz,GPIO_PuPd_NOPULL,GPIOF);
	TIM13_Init();
	step1_count = 0;
	step1_want = 0;
	PFout(4) = 1;
	//PFout(2) = 0;
}

void TIM8_UP_TIM13_IRQHandler(void){
	if (TIM_GetITStatus(TIM13, TIM_IT_Update) != RESET){
		if(step1_want!=step1_count){
			if(PFout(4))
				step1_count++;
			else
				step1_count--;
		}
		TIM_ClearITPendingBit(TIM13,TIM_FLAG_Update);
	}
}

void Step1_dir(u8 dir)
{
	PFout(4) = dir;
}

int Step1_get_count(void)
{
	return step1_count;
}

void Step1_moveto(int count)
{
	step1_want = count;
}

void Step1_set_speed(int speed)
{
	TIM_SetAutoreload(TIM13,1000000/speed - 1);
	TIM_SetCompare1(TIM13,0.5*1000000/speed - 1);
}

void Step_check(void)
{
	static u8 state1 = 0;
	static u8 state2 = 0;
	if(PFout(2) == 0){
		if(step1_want>step1_count)
		{
			if(state1 == 0){
				PFout(4) = 1;
				PDout(1) = 1;
				TIM13->CCER |= TIM_CCER_CC1E;
				state1 = 1;
			}
		}else if(step1_want<step1_count)
		{
			if(state1 == 0){
				PFout(4) = 0;
				PDout(1) = 0;
				TIM13->CCER |= TIM_CCER_CC1E;
				state1 = 1;
			}
		}
		else
			if(state1 == 1)
			{
				TIM13->CCER &= ~TIM_CCER_CC1E;
				state1 = 0;
			}
	}
	
	/*if(PFout(3) == 0){
		if(step2_want>step2_count)
		{
			if(state2 == 0){
				PFout(5) = 0;
				PDout(4) = 0;
				TIM14->CCER |= TIM_CCER_CC1E;
				state2 = 1;
			}
		}else if(step2_want<step2_count)
		{
			if(state2 == 0){
				PFout(5) = 1;
				PDout(4) = 1;
				TIM14->CCER |= TIM_CCER_CC1E;
				state2 = 1;
			}
		}
		else
			if(state2 == 1)
			{
				TIM14->CCER &= ~TIM_CCER_CC1E;
				state2 = 0;
			}
	}*/
}

void Step1_stop(){
	step1_want = step1_count;
	TIM_Cmd(TIM13, DISABLE);
	TIM13->CCER &= ~TIM_CCER_CC1E;
}

void Step1_enable(){
	PFout(2) = 0;
}

bool Step1_state(){
	if(step1_want == step1_count)
		return true;
	else return false;
}

bool Step2_state(){
	if(step2_want == step2_count)
		return true;
	else return false;
}
void Step2_Init(void)
{
	GPIO_Configuration(GPIO_Pin_5,GPIO_Mode_OUT,GPIO_OType_PP,GPIO_Speed_50MHz,GPIO_PuPd_NOPULL,GPIOF);
	GPIO_Configuration(GPIO_Pin_3,GPIO_Mode_OUT,GPIO_OType_PP,GPIO_Speed_50MHz,GPIO_PuPd_NOPULL,GPIOF);
	step2_count = 0;
	step2_want = 0;
	PFout(5) = 0;
	PFout(3) = 0;
}

void TIM8_TRG_COM_TIM14_IRQHandler(){
	if (TIM_GetITStatus(TIM14, TIM_IT_Update) != RESET){
		if(step2_want!=step2_count){
			if(PFout(5))
				step2_count--;
			else
				step2_count++;
		}
		TIM_ClearITPendingBit(TIM14,TIM_FLAG_Update);
	}
}

void Step2_dir(u8 dir)
{
	PFout(5) = dir;
}

int Step2_get_count(void)
{
	return step2_count;
}

void Step2_moveto(int count)
{
	step2_want = count;
}

void Step2_set_speed(int speed)
{
	TIM_SetAutoreload(TIM14,1000000/speed - 1);
	TIM_SetCompare1(TIM14,0.5*1000000/speed - 1);
}

void Step2_stop(){
	step2_want = step2_count;
	TIM14->CCER &= ~TIM_CCER_CC1E;
}

void Step2_enable(){
	PFout(3) = 0;
}

