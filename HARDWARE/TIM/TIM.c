#include "TIM.h"

TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
TIM_OCInitTypeDef  TIM_OCInitStructure;
TIM_ICInitTypeDef   TIM_ICInitStructure;

void TIM2_Init()        //0.005ms 定时
{
	/*TIM2*/
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	TIM_DeInit(TIM2);
	TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
	TIM_TimeBaseStructure.TIM_Prescaler = 42000 - 1;  //    84M/42000 =2000  0.5ms/count
	TIM_TimeBaseStructure.TIM_Period = 1;  
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;  //设置了时钟分割
    //计时器的时钟周期为 (4+1)*(71+1)/(72*10^6)=0.005ms   ?
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;// 向上  
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
	TIM_ClearFlag(TIM2, TIM_FLAG_Update);
	TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
	TIM_Cmd(TIM2, ENABLE);	
}

void TIM9_Init()  //无刷舵机PWM输出
{
	TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
	TIM_TimeBaseStructure.TIM_Prescaler = 168 - 1;  //    84M/84 =1M  1us/count
	TIM_TimeBaseStructure.TIM_Period = 20000 - 1;  
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;  //设置了时钟分割
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;// 向上  
	TIM_TimeBaseInit(TIM9, &TIM_TimeBaseStructure);
	
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; //选择定时器模式:TIM脉冲宽度调制模式1
 	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //比较输出使能
	TIM_OCInitStructure.TIM_Pulse = 7.7/100*1000000/50 - 1;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; //输出极性
	TIM_OC1Init(TIM9, &TIM_OCInitStructure);  //根据T指定的参数初始化外设TIM9 OC1
    TIM_OC1PreloadConfig(TIM9, TIM_OCPreload_Enable);  //使能TIM3在CCR1上的预装载寄存器
	TIM_OCInitStructure.TIM_Pulse = 5.2/100*1000000/50 - 1;
	TIM_OC2Init(TIM9, &TIM_OCInitStructure);  //根据T指定的参数初始化外设TIM9 OC2
    TIM_OC2PreloadConfig(TIM9, TIM_OCPreload_Enable);  //使能TIM3在CCR1上的预装载寄存器
    TIM_ARRPreloadConfig(TIM9,ENABLE);//ARPE使能 	
	TIM_Cmd(TIM9,ENABLE);
}

void TIM13_Init(void)
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM13, ENABLE);
	TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
	TIM_TimeBaseStructure.TIM_Prescaler = 84 - 1;  //    84M/84 =1M  1us/count
	TIM_TimeBaseStructure.TIM_Period = 1000 - 1;  
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;  //设置了时钟分割
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;// 向上  
	TIM_TimeBaseInit(TIM13, &TIM_TimeBaseStructure);
	
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; //选择定时器模式:TIM脉冲宽度调制模式1
 	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //比较输出使能
	TIM_OCInitStructure.TIM_Pulse = 0.5*(TIM13->ARR+1) - 1;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low; //输出极性
	TIM_OC1Init(TIM13, &TIM_OCInitStructure);  //根据T指定的参数初始化外设TIM13 OC1
    TIM_OC1PreloadConfig(TIM13, TIM_OCPreload_Enable);  //使能TIM13在CCR1上的预装载寄存器
    TIM_ARRPreloadConfig(TIM13,ENABLE);//ARPE使能 
	
	TIM_ClearFlag(TIM13, TIM_FLAG_Update);
	TIM_ITConfig(TIM13, TIM_IT_Update, ENABLE);
	TIM_Cmd(TIM13, ENABLE);	
	TIM13->CCER &= ~TIM_CCER_CC1E;
}

void TIM14_Init(void)
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM14, ENABLE);
	TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
	TIM_TimeBaseStructure.TIM_Prescaler = 84 - 1;  //    84M/84 =1M  1us/count
	TIM_TimeBaseStructure.TIM_Period = 250 - 1;  
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;  //设置了时钟分割
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;// 向上  
	TIM_TimeBaseInit(TIM14, &TIM_TimeBaseStructure);
	
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; //选择定时器模式:TIM脉冲宽度调制模式1
 	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //比较输出使能
	TIM_OCInitStructure.TIM_Pulse = 0.5*(TIM14->ARR+1) - 1;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low; //输出极性
	TIM_OC1Init(TIM14, &TIM_OCInitStructure);  //根据T指定的参数初始化外设TIM13 OC1
    TIM_OC1PreloadConfig(TIM14, TIM_OCPreload_Enable);  //使能TIM13在CCR1上的预装载寄存器
    TIM_ARRPreloadConfig(TIM14,ENABLE);//ARPE使能 
	
	TIM_ClearFlag(TIM14, TIM_FLAG_Update);
	TIM_ITConfig(TIM14, TIM_IT_Update, ENABLE);
	TIM_Cmd(TIM14, ENABLE);	
	TIM14->CCER &= ~TIM_CCER_CC1E;
}

