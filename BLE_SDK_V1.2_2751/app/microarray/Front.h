#ifndef FRONT_H
#define FRONT_H
#include "MaTypes.h"
#include "User.h"
#include "Hardware.h"

void FrontInit(void);                            //ǰ��״̬����ʼ��
void FrontTask(void);                            //ǰ��״̬��ѭ��
void ChangeNumtoString(void);
uint8_t CheckKey(u8 * checkbuffer,u8 buflen,u8 * password,u8 passwordlen);


 
//===========================ǰ��״̬��״̬�����ṹ================================//
typedef struct{
    KeyLed_t key_last;                           //���һ�ΰ���״̬
	  u8  password_buff[256];
	  u32 password_lo;                      //���������8λ
    u32 password_hi;                      //���������4λ
	  u32  password_len;
}ParamAwake_t; 
extern ParamAwake_t g_front_param;//����״̬�Ĳ����ṹ

#endif // FRONT_H
