#ifndef HARDWARE_H
#define HARDWARE_H
#include "MaTypes.h"
#include "common_lock_mfrc.h"  
#include "m_uart.h"


typedef enum{
    KEY_LED_0, KEY_LED_1, KEY_LED_2, KEY_LED_3,  //���ְ���
    KEY_LED_4, KEY_LED_5, KEY_LED_6, KEY_LED_7,
    KEY_LED_8, KEY_LED_9,
    KEY_LED_STAR,                                //*����
    KEY_LED_HASH,                                //#����
	  KEY_LED_TT,
	  KEY_LED_NONE = 0xff,                         //�ް���
}KeyLed_t;                                       //����/LED����

#define SCREEN_BYTES         (128 * 64 / 8)      //��ռ�õ���ʾ�����С
typedef enum{
    VOICE_MUTE,                                  //������
    VOICE_BEEP,                                  //������
    VOICE_WELCOME,                               //"��ӭʹ�ý��ԭ�Ӹ߰�ȫ�Զ�������"
}Voice_t;

//===========================Ӳ������ӿ�=======================================//
u32 SliderGetTouch(void);                        //��ȡ����Touch״̬
u32 SliderGetHallOpened(void);                   //��ȡ���ǿ�����״̬
u32 SliderGetHallClosed(void);                   //��ȡ���ǹػ���״̬
KeyLed_t FrontGetKey(void);                      //��ȡǰ��嵱ǰ����
u32 FrontGetConfigKey(void);                     //��ȡ���ü�״̬
u32 FrontGetKeyTouch(void);                      //��ȡkey����״̬
u32 FrontGetFingerTouch(void);                   //��ȡָ�ƴ���״̬
s32 FrontGetMatchFinger(void);                   //��ȡƥ���ָ��
s32 FrontGetRfid(void);                          //��ȡˢ������

//===========================Ӳ���������ӿ�====================================//
typedef struct{
    //���ǲ��ֵ�����
    //u32 slider_touch;                            //����Touch:0=δ����,1=����
    //u32 slider_hall_opened;                      //���ǿ���λ����:0=δ��λ,1=��λ
    //u32 slider_hall_closed;                      //���ǹص�λ����:0=δ��λ,1=��λ
    //ǰ��岿�ֵ�����
    KeyLed_t  key;                                			//��ǰ���µļ�
    u32 config_key;                              //���ü�:0=δ����,1=����
    u32 finger_touch;                            //ָ�ƴ���:0=δ����,1=����
	  u32 key_touch;                                 //��������:0=δ����1=����
    s32 finger;                                  //���µ�ָ�ƺ�:-1=��ָ��
    s32 rfid;                                    //ˢ���Ŀ���:-1=��ˢ��
}HardwareInput_t;
extern volatile HardwareInput_t g_hardware_input;         //ȫ��Ӳ���������ṹ

//===========================Ӳ������ӿ�=======================================//
void SliderSetMoto(u32 moto);                    //���û��ǵ��:0=����,1=��ת,2=��ת
void VoicePlay(Voice_t voice);                   //��������
void FrontSetLedOn(u32 leds);                    //����LED
void FrontSetLedOff(u32 leds);                   //Ϩ��LED
void FingerSetLed(u32 on_off);                   //����ָ��LED����
void ScreenClear(void);                          //����
void ScreenShowLogo(void);                       //����ʾLogo
void ScreenPrintLine(u8 line, u8 *str);          //��ӡ����0~3��֮һ,ÿ�δ�ӡ����

//===========================Ӳ���������ӿ�====================================//
typedef struct{
    //���ǲ��ֵ����
    //u32 slider_moto;                             //����Moto:0=����,1=��ת,2=��ת
    //ǰ��岿�ֵ����
    u8  screen[SCREEN_BYTES];                    //����ʾ
    Voice_t voice;                               //��������
    u32 led;                                     //�����ϵ�LED��D0~D9=���ּ�0~9λ�õ�LED,D10=*��λ�õ�LED,D11=#��λ�õ�LED
    u32 finger_led;                              //ָ��LED:0=��,1=��ɫ,2-��ɫ
}HardwareOutput_t;
extern HardwareOutput_t g_hardware_output;       //ȫ��Ӳ���������ṹ


typedef struct 
{
	uint32_t start_time;
  uint32_t end_time;
	uint32_t time_over;
	uint8_t  time_over_flag;
}Timeout;

extern Timeout gtTimeout_M;




void HardwareInputInit(void);
void GetStartTime_M(uint32_t timeover);
uint8_t GetCurrentTimeCnt_M(void);

#if 0
//===========================Ӳ���ڲ��ӿ�=======================================//
u32 FrontGetStamp(void);                         //��ȡʱ���
u32 FrontCalcElapsed(u32 stamp);                 //��ȡ��ǰ��ʱ����ļ��ʱ��
u32 FrontGetDatetime(void);                      //��ȡ����ʱ��
void FrontSetDatetime(u32 dt);                   //��������ʱ��
void FrontDatetimeToLine(u32 dt, u8 *str);       //������ʱ��ת��Ϊ�ַ���

//===========================Ӳ���ڲ�����ӿ�====================================//
typedef struct{
    u32 tick;                                    //��ʱ������(��λms)
    QDateTime dt_start;                          //����ʼ����ʱ��
    QDateTime dt_base;                           //����RTC����ʱ��
}HardwareInner_t;
extern HardwareInner_t g_hardware_inner;         //ȫ��Ӳ���ڲ�����ṹ
#endif
#endif // HARDWARE_H
