
#ifndef _Filter
#define _Filter


#include <stdint.h>

#define dt 0.01     // dt��ȡֵΪkalman�˲�������ʱ��,�˴�Ϊ10ms;

//***********�����ĺ궨��**************
#ifdef DEBUD

#define Q_angle 0.001
#define Q_gyro 0.003
#define R_angle 0.5
#define C_0 1
#endif

//***********����ӿ�******************
//extern float Angle, Angle_dot; // �˲������Ǻ���Ǽ��ٶ�
extern void AngleCalcu(float angle_m,float gyro_m,float *p); // 
void ClearAngle(void);
uint8_t CheckDoorOpened(void);
uint8_t CheckDoorClosed(void);

#endif