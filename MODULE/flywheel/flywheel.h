#ifndef _FLYWHEEL_H_
#define _FLYWHEEL_H_
#include "sys.h"
#include <stdbool.h>
#include "configuration.h"
#include "motor_brushless.h"

/** �ڸ�ģ���е�.h�ļ�ע����ָ��ÿ������Ӧ�÷������Դ�����£�
  * ȷ����Դ����󣬽������д��configuration.h
  * #ifndef BRUSHLESS
  *
  * #define BRUSHLESS
  * #define BRUSHLESS_TIM 		TIMx			//��ʱ��
  * #define BRUSHLESS_OC 		1				//ͨ��
  * #define BRUSHLESS_GPIOx 	GPIOx			//GPIO
  *	#define BRUSHLESS_PIN GPIO_	Pin_x			//�ܽ�
  * #define BRUSHLESS_PINSOURCE GPIO_PinSourcex //���ùܽ�
  * #define BRUSHLESS_AF 		GPIO_AF_TIMx	//�ܽŸ��ù���
  * #endif
  *
  * ��һ���궨�彫���ð�����������������
  
  */
#ifndef BRUSHLESS
#error δ������ˢ
#endif


#endif
