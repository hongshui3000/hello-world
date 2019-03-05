#ifndef USER_H
#define USER_H
#include "MaTypes.h"

#define MAX_RFID_NUM         200                 //���RFID��

typedef enum{
    VERIFY_MODE_NORMAL_USER,                     //һ���û�
    VERIFY_MODE_ADMIN_USER,                      //����Ա�û�
}VerifyMode_t;                                   //��֤ģʽ����

typedef struct{
    u32 password_admin_lo;                       //����Ա�����8λ
    u32 password_admin_hi;                       //����Ա�����4λ
    u32 password_unlock_lo;                      //���������8λ
    u32 password_unlock_hi;                      //���������4λ
    VerifyMode_t verify_mode;                    //����ģʽ
 
	  u32 admin_write_index;                       //д�����Ա���õ�λ��
	  
    u32 config_write_index;                      //д�����û����õ�λ��
    u32 rfid_write_index;                        //д����RFID��λ��,ͬʱҲ�ǲ���RFID�Ľ���λ��
	  u32 password_num_len;
		u8  passwordnum[16];
}UserConfig_t;                                   //�û���������

//===========================�û�����ӿ�=======================================//
extern volatile UserConfig_t g_user_config;               //ȫ���û�����

void UserInit(void);                             //�û������ʼ��
void AdminSaveConfig(void);                      //�������Ա����
void UserSaveConfig(void);                       //�����û�����
s32  UserFindRfid(u32 rfid);                     //���ҿ���,���ش��ڵ���0������,����-1��ʾδ�ҵ�
s32  UserAddRfid(u32 rfid);                      //���һ������,����0��ʾ�ɹ�,����-1��ʾRFID��
void UserDeleteAllRfid(void);                    //ɾ�����п���
void UserDeleteAllKeyUser(void);
void UserDeleteAllAdminUser(void);


u8 UserFindKeyUser(void);
u8  UserFindAdmin(void);

#endif // USER_H
