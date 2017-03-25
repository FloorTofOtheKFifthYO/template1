#ifndef _CLIENT_H_
#define _CLIENT_H_

#include "sys.h"

typedef struct
{
	u8 can_id;
	u16 pwm1;
	u16 pwm2;
	u8 relay[4];
}Client;

void Client_RESET(Client client);

/*

*���ܣ����ôӰ��pwm�ͼ̵���
*������Client client �ӻ��Ľṹ��
*		
*/
void Client_SET(Client client);

#endif