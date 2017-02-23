#include "flywheel.h"

Flywheel flywheel;

static int turn_count = 0;
static int fly_count = 0;
static int fly_n = 0;

/**
  * @brief  初始化
  *     
  * @note
  *
  * @param 
  *          
  * @retval 
  */
void flywheel_init()
{
	flywheel.wheel.TIMx = BRUSHLESS_TIM;
	flywheel.wheel.GetTimx = 5;
	flywheel.wheel.init_duty = 0.077;
	flywheel.wheel.OC = BRUSHLESS_OC;
	Brushless_init(&(flywheel.wheel));
	
	flywheel.roll.motor.id = ROLL_ID;
	flywheel.roll.motor.rate = 1;
	flywheel.roll.motor.usartx = MOTOR_USARTx;
	flywheel.roll.GetTimx = ROLL_GETTIMX;
	flywheel.roll.SP = ROLL_SP;
	putter_init(&(flywheel.roll));

	flywheel.turn.usartx = MOTOR_USARTx;
	flywheel.turn.id = TURN_ID;
	flywheel.turn.rate = 1;
	
	flywheel.state = fly_ready;
	flywheel.fly_flag = false;
	
	USART_SendString(MOTOR_USARTx,"6HO\r");
}

/**
  * @brief  改俯仰角
  *     
  * @note
  *
  * @param 俯仰角
  *          
  * @retval 
  */
void flywheel_setRoll(float roll)
{
	putter_moveto(&(flywheel.roll),roll);
}

void flywheel_fly()
{
	FLY=!FLY;
}

/**
  * @brief  改无刷
  *     
  * @note
  *
  * @param 占空比
  *          
  * @retval 
  */
void flywheel_setBrushless(float duty)
{
	Brushless_duty(flywheel.wheel, duty);
}

/**
  * @brief  改旋转平台
  *     
  * @note
  *
  * @param 旋转平台
  *          
  * @retval 
  */
void flywheel_setTurn(float turn)
{
	motor_usart_abspos(flywheel.turn,(int)(turn*10000));
}

/**
  * @brief  tim2中调用，进行延时等操作
  *     
  * @note
  *
  * @param 旋转平台
  *          
  * @retval 
  */
void flywheel_TIM()
{
	if(fly_count != 0)
		fly_count--;
	if(turn_count != 0)
		turn_count--;
}

/**
  * @brief 完成n次飞
  *     
  * @note int n,	次数
		float duty, 无刷占空比
		float roll,	俯仰角
		float turn  转角
  *
  * @param 旋转平台
  *          
  * @retval 
  */
void flywheel_flyn(int n, float duty, float roll, float turn)
{
	fly_n = n;
	flywheel.pur_duty = duty;
	flywheel.pur_roll = roll;
	flywheel.pur_turn = turn;
	flywheel.fly_flag = true;
}

/**
  * @brief main大循环中检查
  *     
  * @note
  *
  * @param
  *          
  * @retval 
  */
void flywheel_main()
{
		switch(flywheel.state)
		{
			case fly_ready:
				if(flywheel.fly_flag)
				{
					Brushless_duty(flywheel.wheel,flywheel.pur_duty);
					putter_moveto(&flywheel.roll,flywheel.pur_roll);
					motor_usart_abspos(flywheel.turn,(int)(flywheel.pur_turn*10000));
					turn_count = 500;
					flywheel.state = fly_adj;
					FLY = 0;
				}
				break;
			case fly_adj:
				if(!flywheel.roll.putter_flag && turn_count == 0)
				{
					flywheel.state = fly;
					FLY=!FLY;
					fly_count = 200;
					fly_n--;
				}
				break;
			case fly:
				{
					if(fly_count<200)
					{
						FLY=!FLY;
						
					}
					else if(fly_count == 0)
					{
						if(fly_n == 0)
						{
							fly_count = 0;
							flywheel.state = fly_ready;
							flywheel.fly_flag = false;
						}else{
							FLY=!FLY;
							fly_count = 200;
							fly_n--;
						}
					}
				}
				break;
		}
		
		putter_main(flywheel.roll);
}
