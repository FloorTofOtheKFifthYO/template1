#include "flywheel.h"

Flywheel flywheel;

/**
  * @brief  ��ʼ��
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
	
}

/**
  * @brief  �ĸ�����
  *     
  * @note
  *
  * @param ������
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
  * @brief  ����ˢ
  *     
  * @note
  *
  * @param ռ�ձ�
  *          
  * @retval 
  */
void flywheel_setBrushless(float duty)
{
	Brushless_duty(flywheel.wheel, duty);
}

/**
  * @brief  ����תƽ̨
  *     
  * @note
  *
  * @param ��תƽ̨
  *          
  * @retval 
  */
void flywheel_setTurn(float turn)
{
	motor_usart_abspos(flywheel.turn,(int)(turn*10000));
}

