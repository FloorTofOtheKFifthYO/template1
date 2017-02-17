#ifndef _FLYWHEEL_H_
#define _FLYWHEEL_H_
#include "sys.h"
#include <stdbool.h>
#include "configuration.h"
#include "motor_brushless.h"
#include "putter.h"
#include "motor_usart.h"

/** �ڸ�ģ���е�.h�ļ�ע����ָ��ÿ������Ӧ�÷������Դ�����£�
  * ȷ����Դ����󣬽������д��configuration.h
  *
  * #define BRUSHLESS
  * #define BRUSHLESS_TIM 		TIMx			//��ʱ��
  * #define BRUSHLESS_OC 		1				//ͨ��
  * #define BRUSHLESS_GPIOx 	GPIOx			//GPIO
  *	#define BRUSHLESS_PIN GPIO_	Pin_x			//�ܽ�
  * #define BRUSHLESS_PINSOURCE GPIO_PinSourcex //���ùܽ�
  * #define BRUSHLESS_AF 		GPIO_AF_TIMx	//�ܽŸ��ù���
  *
  * ��һ���궨�彫���ð�����������������
  
  */
#ifndef BRUSHLESS
#error δ������ˢ
#endif


/*����ʵ�������configuration.h������
#define ROLL
#define ROLL_ID 		4
#define ROLL_SP			2000
#define ROLL_GETTIMX	4
*/
#ifndef ROLL
#error δ������Ƹ�
#endif

/*����ʵ��������תƽ̨

*/
#ifndef TURN
#error	δ������תƽ̨
#endif

#ifndef FLY
#error δ���ü̵���
#endif

typedef struct
{
	Brushless wheel;
	putter roll;
	motor_usart turn;
}Flywheel;

extern Flywheel flywheel;

/**
  * @brief  ��ʼ��
  *     
  * @note
  *
  * @param 
  *          
  * @retval 
  */
void flywheel_init();

/**
  * @brief  �ĸ�����
  *     
  * @note
  *
  * @param ������
  *          
  * @retval 
  */
void flywheel_setRoll(float roll);


/**
  * @brief  ����ˢ
  *     
  * @note
  *
  * @param ռ�ձ�
  *          
  * @retval 
  */
void flywheel_setBrushless(float duty);

	
/**
  * @brief  ����תƽ̨
  *     
  * @note
  *
  * @param ��תƽ̨
  *          
  * @retval 
  */
void flywheel_setTurn(float turn);

void flywheel_fly();

#endif
